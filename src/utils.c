#include "mint/utils.h"

#include <stdint.h>

uintptr_t
get_rbp(void) {
    uintptr_t ret;
    asm inline ("movq %%rbp, %0;" : "=r" (ret));
    return ret;
}
