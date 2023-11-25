#ifndef __MINT_H
#define __MINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uintptr_t mint_t;
typedef void *(*mroutine_t)(void *args);

#define M_SUCCESS     0x0
#define M_NOT_OWNER   0x1
#define M_NOT_RUNNING 0x2
#define M_ALLOC_FAIL  0x4
#define M_NOT_DIRECT  0x8

int  mint_create(mint_t *restrict handle, mroutine_t routine, void *restrict args);
int  mint_spawn(mint_t *restrict handle, mroutine_t routine, void *restrict args);
int  mint_await(mint_t handle, void **ret);

/// Transfers execution to the runtime,
/// so that other coroutines can have time
/// on the hardware thread. Does not move
/// coroutines between any of the queues,
/// and does not make any guarantees on when
/// this function will return to the calling
/// coroutine.
int  mint_yield(void);

/// Returns from the coroutine, and tells
/// the runtime to not allow this coroutine
/// to run again. Optionally, pass in a return
/// value so that the creator of this
/// coroutine can retrieve the value with
/// `mint_await`. Also, if the program is
/// in the main function of the coroutine, the
/// normal way of exiting the function, `return`
/// works the exact same way.
void mint_return(void *ret);

int  mint_shutdown(void);

int  mint_block_on(mroutine_t routine, void *args, void **ret);

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
