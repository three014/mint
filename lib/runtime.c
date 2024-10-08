#include "mint.h"
#include "mint/coroutine.h"
#include "mint/runtime.h"
#include "mint/queue.h"
#include "mint/memcache.h"
#include <stdint.h>

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

struct runtime {
    pthread_t thread;
    mint_t current; 
    queue ready;
    queue waiting;
    queue complete;
    cache recycled_cr;
};

static struct runtime rt = {0};
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

mint_t
rt_current(void) {
    return rt.current;
}

queue *
rt_complete(void) {
    return &rt.complete;
}

queue *
rt_ready(void) {
    return &rt.ready;
}

queue *
rt_waiting(void) {
    return &rt.waiting;
}

cache *
rt_cache(void) {
    return &rt.recycled_cr;
}

bool
owns_rt(void) {
    return rt.thread == pthread_self();
}

void
rt_set_current(mint_t current) {
    rt.current = current;
}

int 
rt_pin(void) {
    int err = M_SUCCESS;
    pthread_t thread = rt.thread;
    pthread_t self = pthread_self();

    if (thread == 0) {
        int result = pthread_mutex_trylock(&mtx);
        if (result == 0 && rt.thread == 0) {
            // This thread is the owner
            rt.thread = self;
            pthread_mutex_unlock(&mtx);
        }
    }

    if (rt.thread != self) {
        err = M_NOT_OWNER;
    }

    return err;
}

int
rt_unpin(void) {
    pthread_t self = pthread_self();
    pthread_t thread = rt.thread;
    int err = M_NOT_OWNER;

    if (thread == self) {
        int result = pthread_mutex_lock(&mtx);
        if (result == 0) {
            if (rt.current == M_ROOT) {
                rt.thread = 0;
                err = M_SUCCESS;
            } else {
                err = M_BUSY;
            }
            pthread_mutex_unlock(&mtx);
        }
            
    }

    return err;
}
