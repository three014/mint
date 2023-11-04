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

        // Set curr to new node
        queue->curr = curr->next;
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

        // Set curr to new node
        queue->curr = curr->next;
        break;
    }

    queue->len++;
}
