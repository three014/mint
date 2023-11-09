#include "mint.h"
#include "mint/context.h"
#include "mint/coroutine.h"
#include "mint/memcache.h"
#include "mint/runtime.h"
#include "mint/queue.h"
#include "mint/status.h"
#include "log.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void __mint_yield(void);

int
mint_pin(void) {
    return rt_pin();
}

int
mint_unpin(void) {
    return rt_unpin();
}

int
mint_spawn(
    mint_t *restrict handle, 
    void *(*routine)(void *args), 
    void *args
) {
    int err = M_SUCCESS;

    // Check if we have the runtime
    if (!owns_rt()) {
        err = M_NOT_OWNER;
        goto finally;
    }

    // Check if we have a spare coroutine object
    // we can use, or else make a new one
    cache *_c = rt_cache();
    struct coroutine *new_cr = cache_pop_else_alloc(_c);
    if (new_cr == NULL) {
        err = M_ALLOC_FAIL;
        goto finally;
    }

    // Set coroutine to use
    // user-specified arguments
    cr_set(new_cr);
    cr_init_stack(new_cr, routine, args);

    if (rt_current() == M_ROOT) {
        err = M_NOT_RUNNING;
        cache_push(_c, new_cr);
        goto finally;
    }

    queue_link(rt_ready(), new_cr);

    cr_dbg(new_cr);

    __mint_yield();

    // Give caller the handle
    *handle = new_cr->self;

finally:
    return err;
}

int 
mint_block_on(
    void *(*routine)(void *args), 
    void *args, 
    void **ret
) {
    int err = M_SUCCESS;

    // Check if we have the runtime
    if (!owns_rt()) {
        err = M_NOT_OWNER;
        goto finally;
    }

    // Check if we have a spare coroutine object
    // we can use, or else make a new one
    cache *_c = rt_cache();
    struct coroutine *new_cr = cache_pop_else_alloc(_c);
    if (new_cr == NULL) {
        err = M_ALLOC_FAIL;
        goto finally;
    }

    // Set coroutine to use
    // user-specified arguments
    cr_set(new_cr);
    cr_init_stack(new_cr, routine, args);

    __logln_dbg_fmt("Created stack and coroutine for %lx", new_cr->self);

    // Check if we're in a coroutine,
    // aka if the runtime is currently
    // running a coroutine
    struct coroutine *curr_cr;
    mint_t curr = rt_current();
    if (curr != M_ROOT) {
        curr_cr = cr_from_handle(curr);
        __logln_dbg("Retrieved currently running coroutine");
    } else {
        curr_cr = cache_pop_else_alloc(_c);
        if (curr_cr == NULL) {
            err = M_ALLOC_FAIL;
            goto cache_new;
        }
        cr_set(curr_cr);

        // Set current coroutine to curr
        rt_set_current(curr_cr->self);

        __logln_dbg("Created a new coroutine to represent non-green function");
    }

    // Link curr and new coroutines
    curr_cr->status = STATUS_WAITING(new_cr->self);
    new_cr->parent = curr_cr->self;

    // Add both coroutines to their respective queues
    queue_link(rt_ready(), new_cr);
    queue_link(rt_waiting(), curr_cr);

    cr_dbg(new_cr);

    // And away we go!!!
    __mint_yield();

    // After here, new_cr should be complete, and it
    // should be waiting in the completed list.
    assert(new_cr->status.tag == COMPLETE);
    queue_unlink(rt_complete(), new_cr);

    // We save the coroutine's return value
    if (ret != NULL) {
        *ret = new_cr->status.value.ret;
    }

    // Now we can say that we're done as well
    // If this call came from a regular function,
    // then we need to clean up our state.
    if (curr_cr->parent == M_ROOT) {
        // Coroutine object will be in ready queue,
        // since that's the only way we'd be able to
        // return from the yield
        queue_unlink(rt_ready(), curr_cr);
        curr_cr->status = STATUS_COMPLETE(NULL);

        // Runtime is no longer running anything
        rt_set_current(M_ROOT);

        // We are done with this coroutine,
        // send to the cache!
        cache_push(_c, curr_cr);
    }

cache_new:
    cache_push(_c, new_cr);

finally:
    return err;
}

int
mint_await(mint_t handle, void **ret) {
    int err = M_SUCCESS;

    if (!owns_rt()) {
        err = M_NOT_OWNER;
        goto finally;
    }

    struct coroutine *handle_cr = cr_from_handle(handle);
    mint_t curr = rt_current();
    if (curr == M_ROOT) {
        err = M_NOT_RUNNING;
        goto finally;
    }
    if (handle_cr->status.tag == COMPLETE) {
        goto ret;
    }
    
    struct coroutine *curr_cr = cr_from_handle(curr);
    queue_unlink(rt_ready(), curr_cr);
    curr_cr->status = STATUS_WAITING(handle_cr->self);
    queue_link(rt_waiting(), curr_cr); 
    handle_cr->parent = curr_cr->self;

    __mint_yield();

    assert(handle_cr->status.tag == COMPLETE);

ret:
    queue_unlink(rt_complete(), handle_cr);
    if (ret != NULL) {
        *ret = handle_cr->status.value.ret;
    }

    cache_push(rt_cache(), handle_cr);

finally:
    return err;
}

int
mint_yield(void) {
    int err = M_SUCCESS;

    if (!owns_rt()) {
        err = M_NOT_OWNER;
        goto finally;
    }

    mint_t curr = rt_current();
    if (curr == M_ROOT) {
        err = M_NOT_RUNNING;
        goto finally;
    }

    //__logln_dbg_fmt("Entered yield from %lx", curr);

    __mint_yield();

finally:
    return err;
}

void
__mint_yield(void) {
    struct coroutine *curr_cr = cr_from_handle(rt_current());
    struct coroutine *next_cr = queue_rotate_left(rt_ready());

    rt_set_current(next_cr->self);

    //__logln_dbg("About to switch!");
    ctx_switch(&curr_cr->ctx, &next_cr->ctx);
}

void
mint_return(void *ret) {
    mint_t curr = rt_current();
    if (curr == M_ROOT) {
        // If we got to here, then
        // we're really hoping this
        // call wasn't made from a
        // coroutine, otherwise
        // this return would crash or
        // corrupt the program??
        abort();
    }

    struct coroutine *curr_cr = cr_from_handle(curr);

    //cr_dbg(curr_cr);

    // Move current coroutine to 'Complete' list
    queue_unlink(rt_ready(), curr_cr);
    curr_cr->status = STATUS_COMPLETE(ret);
    queue_link(rt_complete(), curr_cr);

    // If there's a parent, move to 'Ready' queue
    mint_t parent = curr_cr->parent;
    if (parent != M_ROOT) {
        struct coroutine *parent_cr = cr_from_handle(parent);
        queue_unlink(rt_waiting(), parent_cr);
        parent_cr->status = STATUS_READY;
        queue_link(rt_ready(), parent_cr);
    }

    __mint_yield();
}
