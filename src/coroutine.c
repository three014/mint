#include "mint.h"
#include "mint/context.h"
#include "mint/coroutine.h"
#include "mint/runtime.h"
#include "mint/status.h"
#include "log.h"

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>

struct coroutine *
cr_alloc(void) {
    struct coroutine *cr = malloc(sizeof *cr);
    if (cr == NULL) {
        goto finally;
    }    

    void *stack = mmap(
        NULL, 
        __MINT_STACK_SIZE, 
        PROT_READ | PROT_WRITE, 
        MAP_GROWSDOWN | MAP_PRIVATE | MAP_ANONYMOUS, 
        -1, 
        0
    );
    if (stack == MAP_FAILED) {
        __logln_err_fmt("failed to map new stack: %s", strerror(errno));
        free(cr);
        goto finally;
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
cr_init_stack_old(struct coroutine *cr, void *(*routine)(void *args), void *args) {
    // We need to setup the stack now
    void **stack = cr->stack;

    __logln_dbg("Setting up the new stack");

    stack[0] = (void *)rt_canary();
    stack[-1] = end_coroutine; // Old rip, will call mint_return
    stack[-2] = 0; // TODO: Some sort of 'old rbp' address
                   //       Can it be null? Since we won't be returning
                   //       from mint_return
    stack[-3] = args; // Args that we wanna place in %rdi
    stack[-4] = routine; // Function pointer, place in any silly register
    stack[-5] = start_coroutine; // Old rip, will call function at func pointer

    // Therefore, first usable address will be stack[-5]
    // So we can go ahead and set the stack pointer
    cr->ctx.rsp = (uintptr_t)&stack[-5];
    cr->ctx.rbp = (uintptr_t)&stack[-2];
}

void
cr_init_stack(struct coroutine *cr, void *(*routine)(void *args), void *args) {
    uintptr_t *stack = cr->stack + __MINT_STACK_SIZE;

    __logln_dbg("Setting up the new stack");

    // Allows us to check if we're at the
    // initial coroutine function
    *(stack - 1) = rt_canary();

    // Assembly routine that ensures that
    // the coroutine's return value is in
    // %rdi to call mint_return
    *(stack - 2) = (uintptr_t)end_coroutine;

    // Base pointer
    *(stack - 3) = 0;

    // Will be popped into %rdi
    *(stack - 4) = (uintptr_t)args;

    // Will be popped into %rax
    *(stack - 5) = (uintptr_t)routine;

    // Will be popped into %rip (this is the
    // entry point of our coroutine)
    *(stack - 6) = (uintptr_t)start_coroutine;

    // We make sure the stack and base pointers
    // know where to find the starting routines
    cr->ctx.rsp = (uintptr_t)(stack - 6);
    cr->ctx.rbp = (uintptr_t)(stack - 3);
}

void cr_delete(struct coroutine *cr) {
    munmap(cr->stack, __MINT_STACK_SIZE);
    free(cr);
}
