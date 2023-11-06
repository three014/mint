.section .text
.globl ctx_switch

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