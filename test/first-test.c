#include "mint.h"
#include "log.h"

#include <stdio.h>

void *hello(void *args) {
    __logln_info("Entered 'hello' coroutine");
    for (int i = 0; i < 10; i++) {
        mint_yield();
        __logln_info_fmt("Hello number %d!", i);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    mint_pin();
    __logln_dbg("Pinned runtime to current thread, about to block");

    int result = mint_block_on(hello, NULL, NULL);
    __logln_dbg("Completed block");

    mint_unpin();
    
    return result;
}
