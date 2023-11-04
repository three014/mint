#ifndef __MEMCACHE_H
#define __MEMCACHE_H

#include "coroutine.h"

#include <stddef.h>

typedef struct {
    struct coroutine *head;
} cache;

void cache_push(cache *cache, struct coroutine *cached_cr);
struct coroutine *cache_pop(cache *cache);
struct coroutine *cache_pop_else_alloc(cache *cache);

#endif
