#ifndef __MINT_H
#define __MINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uintptr_t mint_t;

#define M_SUCCESS     0x0
#define M_NOT_OWNER   0x1
#define M_NOT_RUNNING 0x2

int  mint_create(mint_t *handle, void *(*routine)(void *args), void *args);
int  mint_spawn(mint_t *handle, void *(*routine)(void *args), void *args);
int  mint_await(mint_t handle, void **ret);
int  mint_yield(void);
void mint_return(void *ret);

int  mint_shutdown(void);

int  mint_block_on(void *(*routine)(void *args), void *args, void **ret);

/// Pins the runtime to the current
/// thread so that this thread can
/// add coroutines to the runtime.
/// returns M_NOT_OWNER if another
/// thread has ownership already.
int  mint_pin(void);

/// Unpins the runtime so that
/// another OS thread can use the
/// runtime. Returns M_NOT_OWNER if
/// the runtime was not pinned to
/// the current thread.
int  mint_unpin(void);

#endif