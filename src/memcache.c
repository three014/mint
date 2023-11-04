#include "mint/memcache.h"
#include "mint/coroutine.h"

#include <stdlib.h>

struct cached_coroutine *
cache_pop(cache *cache) {
    struct cached_coroutine *pop = cache->head;
    if (pop != NULL) {
        cache->head = pop->next;
    } else {
        cache->head = NULL;
    }

    return pop;
}

struct coroutine *
coroutine_from_cache(
    struct cached_coroutine *cached_cr, 
    void *(*routine)(void *args), 
    void *args
) {
    struct coroutine *cr = cached_cr->cr;
    cr->stack = cached_cr->mapping;
    coroutine_set(cr, routine, args);
    cache_delete(cached_cr);
    
    return cr;
}

void cache_delete(struct cached_coroutine *cached_cr) {
    free(cached_cr);    
}
