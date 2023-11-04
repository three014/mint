#ifndef __QUEUE_H
#define __QUEUE_H

#include "coroutine.h"

typedef struct {
    size_t len;
    struct coroutine *curr;
} queue;

void queue_link(queue *queue, struct coroutine *cr);

#endif
