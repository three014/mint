#include "mint.h"
#include "log.h"

#include <stdio.h>

void *hello(void *args) {

    mint_return(NULL);
    __logln_info("Entered 'hello' coroutine");
    int a = *(int *)args;
    for (int i = 0; i < 10; i++) {
        mint_yield();
        __logln_info_fmt("Hello number %d!", i);
    }

    __logln_info_fmt("Got %d from args", a);

    return NULL;
}

int main(int argc, char *argv[]) {
    mint_pin();
    __logln_dbg("Pinned runtime to current thread, about to block");

    int a = 9;

    __logln_dbg_fmt("Address of 'hello': %p", hello);
    __logln_dbg_fmt("Address of 'a': %p", &a);
    int result = mint_block_on(hello, &a, NULL);
    __logln_dbg("Completed block");

    mint_unpin();
    
    return result;
}
