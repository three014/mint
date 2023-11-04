#ifndef __QUEUE_H
#define __QUEUE_H

#include "coroutine.h"

typedef struct {
    size_t len;
    struct coroutine *curr;
} queue;

void queue_unlink(queue *queue, struct coroutine *cr);
void queue_link(queue *queue, struct coroutine *cr);
struct coroutine *queue_rotate_left(queue *queue);

#endif
