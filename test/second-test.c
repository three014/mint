#include "mint.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define N 4
#define BUF_SIZE 0x100

void *silly(void *args);
void *hello(void *args);
void get_user_input(void);
void *entry(void *args);

int main(void) {
    int err = EXIT_SUCCESS;
    mint_pin();

    err = mint_block_on(entry, NULL, NULL);

    mint_unpin();
    return err;
}

void *entry(void *args) {
    (void)args;

    printf("Entered 'entry'\n");

    mint_t silly_handles[N] = {0};
    int silly_args[N] = {0};
    for (int i = 0; i < N; i++) {
        silly_args[i] = i;
        mint_spawn(silly_handles + i, silly, silly_args + i);
    }

    mint_t hello_handle;
    mint_spawn(&hello_handle, hello, NULL);

    for (int i = 0; i < N; i++) {
        mint_await(silly_handles[i], NULL);
    }

    mint_await(hello_handle, NULL);


    return NULL;
}

void *silly(void *args) {
    int s = *(int *)args;
    mint_yield();
    printf("%d is so silly!\n", s);
    return NULL;
}

void *hello(void *args) {
    (void)args;

    printf("Entered 'hello'\n");

    mint_yield();
    get_user_input();

    return NULL;
}

void get_user_input(void) {
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    char buf[BUF_SIZE] = {0};
    ssize_t num_bytes = 0;
    while ((num_bytes = read(STDIN_FILENO, buf, BUF_SIZE - 1)) != 0) {
        if (num_bytes > 1) {
            printf("User input: %.*s\n", (int)num_bytes, buf);
            break;
        } else if (errno == EAGAIN) {
            mint_yield();
        } else {
            perror("Error");
            break;
        }
    }
}
