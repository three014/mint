#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <stdint.h>

struct context {
    uintptr_t rsp;
    uintptr_t rbp;
    uintptr_t rbx;
    uintptr_t r12;
    uintptr_t r13;
    uintptr_t r14;
    uintptr_t r15;
};

void ctx_switch(struct context *curr, struct context *new);
void start_coroutine(void);
void end_coroutine(void);

#endif
