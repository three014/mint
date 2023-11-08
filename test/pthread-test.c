#include "log.h"

#include <stdio.h>
#include <pthread.h>

void *hello(void *args) {
    __logln_info("Entered hello function!");

    return NULL;
}

int main(int argc, char *argv[]) {

    pthread_t h;
    pthread_create(&h, NULL, hello, NULL);

    pthread_join(h, NULL);
    
    return 0;
}
