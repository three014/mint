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
cr_set(struct coroutine *cr) {
    // Stack was already set in alloc
    cr->next = NULL;
    cr->prev = NULL;
    cr->parent = 0;
    cr->self = (mint_t)cr;
    cr->status = STATUS_READY;
}

void
cr_init_stack(struct coroutine *cr, void *(*routine)(void *args), void *args) {
    // We need to setup the stack now
    void **stack = cr->stack;

    stack[0] = end_coroutine; // Old rip, will call mint_return
    stack[-1] = 0; // TODO: Some sort of 'old rbp' address
                   //       Can it be null? Since we won't be returning
                   //       from mint_return
    stack[-2] = args; // Args that we wanna place in %rdi
    stack[-3] = routine; // Function pointer, place in any silly register
    stack[-4] = start_coroutine; // Old rip, will call function at func pointer

    // Therefore, first usable address will be stack[-4]
    // So we can go ahead and set the stack pointer
    cr->ctx.rsp = (uintptr_t)&stack[-4];
}

void cr_delete(struct coroutine *cr) {
    munmap(cr->stack, __MINT_STACK_SIZE);
    free(cr);
}
