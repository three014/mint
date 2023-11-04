#ifndef __COROUTINE_H
#define __COROUTINE_H

#include "status.h"

#ifndef __MINT_STACK_SIZE
#define __MINT_STACK_SIZE 0x200
#endif

struct context {
    uintptr_t rsp;
    uintptr_t rbp;
    uintptr_t rbx;
    uintptr_t r12;
    uintptr_t r13;
    uintptr_t r14;
    uintptr_t r15;
};

struct coroutine {
    mint_t self;
    mint_t depends_on;
    void *(*routine)(void *args);
    void *args;
    void *ret;
    void *stack;
    struct context ctx;
    struct status status;
    struct coroutine *next;
    struct coroutine *prev;
};

struct coroutine *cr_alloc(void);
void cr_set(struct coroutine *cr, void *(*routine)(void *args), void *args);

#endif
