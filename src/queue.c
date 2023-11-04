#include "mint/queue.h"
#include "mint/coroutine.h"

void queue_link(queue *queue, struct coroutine *cr) {
    struct coroutine *curr;
    switch (queue->len) {
    case 0:
        queue->curr = cr;
        break;
    case 1:
        curr = queue->curr;

        // Link two nodes together
        curr->next = cr;
        curr->prev = cr;
        cr->next = curr;
        cr->prev = curr;
        break;
    default:
        curr = queue->curr;
        struct coroutine *next;

        // Open up a hole for new
        // node to fit, then slide
        // in the node!
        next = curr->next;
        curr->next = cr;
        next->prev = cr;
        cr->next = next;
        cr->prev = curr;
        break;
    }

    queue->len++;
}

struct coroutine *queue_rotate_left(queue *queue) {
    struct coroutine *next;
    switch (queue->len) {
    case 0:
    case 1:
        next = queue->curr;
        break;
    default:
        queue->curr = queue->curr->next;
        next = queue->curr;
        break;
    }

    return next;
}
