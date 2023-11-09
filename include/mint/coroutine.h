#ifndef __COROUTINE_H
#define __COROUTINE_H

#include "status.h"
#include "context.h"

#include <stdlib.h>

#ifndef __MINT_STACK_SIZE
#define __MINT_STACK_SIZE (0x6000)
#endif

#define __MINT_TOP_OF_STACK(addr) (addr + (__MINT_STACK_SIZE / sizeof(uintptr_t)) - sizeof(uintptr_t))

#define M_ROOT 0

struct coroutine {
    mint_t self;
    mint_t parent;
    void *stack;
    struct context ctx;
    struct status status;
    struct coroutine *next;
    struct coroutine *prev;
};

inline struct coroutine *cr_from_handle(mint_t handle) {
    return (struct coroutine *)handle;
}

struct coroutine *cr_alloc(void);
void cr_set(struct coroutine *cr);
void cr_dbg(struct coroutine *cr);
void cr_init_stack(struct coroutine *cr, void *(*routine)(void *args), void *args);
void cr_delete(struct coroutine *cr);


#endif
