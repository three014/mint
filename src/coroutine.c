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

#define __DBG_STACK 16

void cr_dbg(struct coroutine *cr) {
    __logln_dbg_fmt("coroutine 0x%lx", cr->self);
    __logln_dbg_fmt("\tParent: 0x%lx", cr->parent);
    __logln_dbg_fmt("\tStatus: %s", (cr->status.tag == READY) ? "Ready" : "Probably Waiting");
    __logln_dbg_fmt("\t", "Context:");
    __logln_dbg_fmt("\t\trsp: 0x%lx", cr->ctx.rsp);
    __logln_dbg_fmt("\t\trbp: 0x%lx", cr->ctx.rbp);
    __logln_dbg_fmt("\tStack trace (bottom of stack at %p):", cr->stack);
    uintptr_t *stack = __MINT_TOP_OF_STACK(cr->stack);
    for (size_t i = 0; i < __DBG_STACK; i++) {
        __logln_dbg_fmt("\t\t Address %p: 0x%lx", stack, *stack);
        stack--;
    }
}

struct coroutine *
cr_alloc(void) {
    struct coroutine *cr = malloc(sizeof *cr);
    if (cr == NULL) {
        goto finally;
    }    

    void *stack = mmap(NULL, __MINT_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_STACK | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    //void *stack = malloc(__MINT_STACK_SIZE);
    if (stack == NULL) {
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
    cr->parent = M_ROOT;
    cr->self = (mint_t)cr;
    cr->status = STATUS_READY;
}

void
cr_init_stack(struct coroutine *cr, void *(*routine)(void *args), void *args) {
    uintptr_t *stack = __MINT_TOP_OF_STACK(cr->stack);

    __logln_dbg_fmt("Setting up the new ", "stack");

    // Assembly routine that ensures that
    // the coroutine's return value is in
    // %rdi to call mint_return
    *stack = (uintptr_t)start_coroutine;

    // Will be popped into %rdi
    *(stack - 1) = (uintptr_t)args;

    // Will be popped into %rax
    *(stack - 2) = (uintptr_t)routine;

    // We make sure the stack pointer
    // knows where to find the starting routines
    cr->ctx.rsp = (uintptr_t)stack;
    cr->ctx.rbp = 1;
}

void cr_delete(struct coroutine *cr) {
    munmap(cr->stack, __MINT_STACK_SIZE);
    //free(cr->stack);
    free(cr);
}
