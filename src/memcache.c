#include "mint/memcache.h"
#include "mint/coroutine.h"

struct coroutine *
cache_pop(cache *cache) {
    struct coroutine *pop = cache->head;
    if (pop != NULL) {
        cache->head = pop->next;
    } else {
        cache->head = NULL;
    }

    return pop;
}


void
cache_push(cache *cache, struct coroutine *cached_cr) {
    struct coroutine *head = cache->head;
    cache->head = cached_cr;
    cached_cr->next = head;
}

struct coroutine *
cache_pop_else_alloc(cache *cache) {
    struct coroutine *pop = cache_pop(cache);
    if (pop == NULL) {
        pop = cr_alloc();
    }

    return pop;
}
