#ifndef __COROUTINE_H
#define __COROUTINE_H

#include "status.h"
#include "context.h"

#include <stdlib.h>

#ifndef __MINT_STACK_SIZE
#define __MINT_STACK_SIZE 0x200
#endif

#define M_ROOT 0

struct coroutine {
    mint_t self;
    mint_t parent;
    void *(*routine)(void *args);
    void *args;
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
void cr_set(struct coroutine *cr, void *(*routine)(void *args), void *args);
void cr_init_stack(struct coroutine *cr);
void cr_delete(struct coroutine *cr);


#endif
