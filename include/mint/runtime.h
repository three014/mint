#ifndef __RUNTIME_H
#define __RUNTIME_H

#include "../mint.h"
#include "memcache.h"
#include "queue.h"

#include <stdint.h>

/// Pins global runtime to the current thread, if not already done.
int rt_pin(void);
int rt_unpin(void);
bool owns_rt(void);

/// Returns the currently running coroutine, or M_NOT_RUNNING if
/// there is no currently running coroutine.
mint_t rt_current(void);
queue *rt_ready(void);
queue *rt_waiting(void);

/// Note: There is no reason at all for
/// the 'complete' list to be a doubly-linked list/queue.
/// I just don't want to create yet another list structure.
/// TODO: Create a brand new list structure for this.
queue *rt_complete(void);

cache *rt_cache(void);
void rt_set_current(mint_t current);



#endif
