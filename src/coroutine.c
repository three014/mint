#include "mint/coroutine.h"
#include "mint.h"
#include "mint/status.h"

#include <sys/mman.h>
#include <stdlib.h>

struct coroutine *
cr_alloc(void) {
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
cr_set(struct coroutine *cr, void *(*routine)(void *args), void *args) {
    // Stack was already set in alloc
    cr->routine = routine;
    cr->args = args;
    cr->next = NULL;
    cr->prev = NULL;
    cr->parent = 0;
    cr->self = (mint_t)cr;
    cr->status = STATUS_READY;
}

void
cr_init_stack(struct coroutine *cr) {
    // We need to setup the stack now
    void **stack = cr->stack;

    // Top of the stack is the return address for mint_return, which
    // all coroutines return to upon completion
    stack[0] = mint_return; // Old rip
    stack[-1] = 0; // TODO: Some sort of 'old rbp' address
                   //       Can it be null? Since we won't be returning
                   //       from `mint_return`
    stack[-2] = cr->routine; // Old rip
    stack[-3] = &stack[-1]; // Old rbp

    // Therefore, first usable address will be stack[-3]
    // So we can go ahead and set the stack pointer
    cr->ctx.rsp = (uintptr_t)&stack[-3];
}

void cr_delete(struct coroutine *cr) {
    munmap(cr->stack, __MINT_STACK_SIZE);
    free(cr);
}
