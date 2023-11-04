#include "mint.h"
#include "mint/coroutine.h"
#include "mint/memcache.h"
#include "mint/runtime.h"

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
    int err = 0;

    // Check if we have the runtime
    if (!owns_rt()) {
        err = M_NOT_OWNER;
        goto finally;
    }

    // Check if we have a spare coroutine object
    // we can use, or else make a new one
    cache *_c = rt_cache();
    struct cached_coroutine *cache = cache_pop(_c);
    struct coroutine *cr;
    if (cache != NULL) {
        cr = cache_into_cr(cache, routine, args);
    } else {
        cr = cr_alloc();
        cr_set(cr, routine, args);
    }



finally:
    return err;
}
