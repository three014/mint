#ifndef __STATE_H
#define __STATE_H

#include "mint.h"

struct status {
    enum tag {
        WAITING,
        READY,
        COMPLETE,
        ABORTED
    } tag;
    union type {
        mint_t handle;
        void *ret;
    } value;
};

#define STATUS_COMPLETE(retptr) ((struct status){ .tag = COMPLETE, .value.ret = (retptr) })
#define STATUS_READY ((struct status){ .tag = READY, .value = 0 })
#define STATUS_WAITING(mint_handle) ((struct status){ .tag = WAITING, .value.handle = (mint_handle) })
#define STATUS_ABORTED ((struct status){ .tag = ABORTED, .value = 0 })

#endif
