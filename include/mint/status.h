#ifndef __STATE_H
#define __STATE_H

#include "mint.h"

struct status {
    enum tag {
        WAITING,
        READY,
        COMPLETE
    } tag;
    union type {
        mint_t handle;
        void *ret;
    } value;
};

#define STATUS_COMPLETE(retptr) ((struct status){ .tag = COMPLETE, .value.ret = retptr })
#define STATUS_READY ((struct status){ .tag = READY, .value = 0 })
#define STATUS_WAITING(handle) ((struct status){ .tag = WAITING, .value.handle = handle })

#endif
