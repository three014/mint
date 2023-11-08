.section .text
.globl ctx_switch, start_coroutine, mint_return

# void ctx_switch(struct coroutine *curr, struct coroutine *next);
ctx_switch:
        movq %rsp, (%rdi)
        movq %rbp, 0x8(%rdi)
        movq %rbx, 0x10(%rdi)
        movq %r12, 0x18(%rdi)
        movq %r13, 0x20(%rdi)
        movq %r14, 0x28(%rdi)
        movq %r15, 0x30(%rdi)

        movq (%rsi), %rsp
        movq 0x8(%rsi), %rbp
        movq 0x10(%rsi), %rbx
        movq 0x18(%rsi), %r12
        movq 0x20(%rsi), %r13
        movq 0x28(%rsi), %r14
        movq 0x30(%rsi), %r15

        ret

# void start_coroutine(void);
start_coroutine:
        sub $0x18, %rsp
        
        # Pops the function pointer off the stack
        popq %rax

        # Pops the function argument `args` off the stack
        popq %rdi

        # Start the function for this coroutine
        call %rax

        # If we came back, then the coroutine has completed,
        # and if they specified a value to return, we'll move
        # it into %rdi so it can be the first argument to
        # `mint_return`.
        movq %rax, %rdi
        jmp mint_return
     
