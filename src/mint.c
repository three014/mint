#include "mint.h"
#include "mint/coroutine.h"
#include "mint/memcache.h"
#include "mint/runtime.h"
#include "mint/queue.h"
#include "mint/status.h"

#include <assert.h>

int
mint_pin(void) {
    return rt_pin();
}

int
mint_unpin(void) {
    return rt_unpin();
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
    cr_set(new_cr, routine, args);

    // Check if we're in a coroutine,
    // aka if the runtime is currently
    // running a coroutine
    struct coroutine *curr_cr;
    mint_t curr = rt_current();
    if (curr != M_ROOT) {
        curr_cr = cr_from_handle(curr);
    } else {
        curr_cr = cache_pop_else_alloc(_c);
        if (curr_cr == NULL) {
            err = M_ALLOC_FAIL;
            goto cache_new;
        }
        cr_set(curr_cr, NULL, NULL);

        // Set current coroutine to curr
        rt_set_current(curr_cr->self);
    }

    // Link curr and new coroutines
    curr_cr->status = STATUS_WAITING(new_cr->self);
    new_cr->parent = curr_cr->self;

    // Add both coroutines to their respective queues
    queue_link(rt_ready(), new_cr);
    queue_link(rt_waiting(), curr_cr);

    // And away we go!!!
    mint_yield();

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

    struct coroutine *curr_cr = cr_from_handle(curr);
    struct coroutine *next_cr = queue_rotate_left(rt_ready());

finally:
    return err;
}
