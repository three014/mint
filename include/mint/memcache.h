#ifndef __MEMCACHE_H
#define __MEMCACHE_H

#include "coroutine.h"

#include <stddef.h>

struct cached_coroutine {
    void *mapping;
    struct coroutine *cr;
    struct cached_coroutine *next;
};

typedef struct {
    struct cached_coroutine *head;
} cache;

struct cached_coroutine *cache_pop(cache *cache);
struct coroutine *cache_into_cr(struct cached_coroutine *cached_cr, void *(*routine)(void *args), void *args);
void cache_delete(struct cached_coroutine *cached_cr);

#endif
