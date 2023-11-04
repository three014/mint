#include "mint.h"
#include "mint/coroutine.h"
#include "mint/memcache.h"
#include "mint/runtime.h"
#include "mint/queue.h"

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
    struct coroutine *new_cr = cache_pop(_c);
    if (new_cr == NULL) {
        new_cr = cr_alloc();
        if (new_cr == NULL) {
            goto finally;
        }
    }

    // Set coroutine to use
    // user-specified arguments
    cr_set(new_cr, routine, args);

    
    // Check if we're in a coroutine,
    // aka if the runtime is currently
    // running a coroutine
    struct context *ctx;
    mint_t curr = rt_current();
    if (curr != 0) {
        struct coroutine *curr_cr = cr_from_handle(curr);
        ctx = &curr_cr->ctx;
    } else {
        ctx = ctx_alloc();
        if (ctx == NULL) {
            goto ctx_failed;
        }
    }

    

    // Set runtime to new coroutine
    rt_set_current(new_cr->self);
  
    // Link with the ready queue
    queue *_q = rt_ready();
    queue_link(_q, new_cr);


    // We did it! We can leave!!
    goto finally;

ctx_failed:
    free(ctx);

finally:
    return err;
}
