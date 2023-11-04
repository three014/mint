#include "mint/coroutine.h"
#include "mint/status.h"

#include <sys/mman.h>
#include <stdlib.h>

struct coroutine *
coroutine_alloc(void) {
    struct coroutine *cr = NULL;
    void *stack = mmap(
        NULL, 
        __MINT_STACK_SIZE, 
        PROT_READ | PROT_WRITE, 
        MAP_GROWSDOWN | MAP_PRIVATE | MAP_ANONYMOUS, 
        -1, 
        0
    );
    if (stack == MAP_FAILED) {
        // BIG ERROR
        // Check errno
        goto finally;
    }

    cr = malloc(sizeof *cr);
    if (cr == NULL) {
        if (munmap(stack, __MINT_STACK_SIZE) == -1) {
            // Check errno
        }
    }

    cr->stack = stack;

finally:
    return cr;
}

void
coroutine_set(struct coroutine *cr, void *(*routine)(void *args), void *args) {
    // Stack was already set in alloc
    cr->routine = routine;
    cr->args = args;
    cr->next = NULL;
    cr->prev = NULL;
    cr->ret = NULL;
    cr->depends_on = 0;
    cr->self = (mint_t)cr->stack;
    cr->status = STATUS_READY;
}

