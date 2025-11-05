# كود تجميعي مولد للغة العربية
# =============================

.data
أ: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
t6: .word 0
t7: .word 0
t8: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    li $v0, 1
    lw $a0, ج
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t0
    lw $t0, ب
    sw $t0, t1
    lw $t1, t0
    lw $t2, t1
    sub $t0, $t1, $t2
    sw $t0, t2
    lw $t0, t2
    sw $t0, ج
    li $v0, 1
    lw $a0, ج
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t3
    lw $t0, ب
    sw $t0, t4
    lw $t1, t3
    lw $t2, t4
    mul $t0, $t1, $t2
    sw $t0, t5
    lw $t0, t5
    sw $t0, ج
    li $v0, 1
    lw $a0, ج
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t6
    lw $t0, ب
    sw $t0, t7
    lw $t1, t6
    lw $t2, t7
    div $t0, $t1, $t2
    sw $t0, t8
    lw $t0, t8
    sw $t0, ج
    li $v0, 1
    lw $a0, ج
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
