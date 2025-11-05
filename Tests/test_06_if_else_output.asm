# كود تجميعي مولد للغة العربية
# =============================

.data
س: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "كبير"
str_1: .asciiz "صغير"

.text
.globl main
main:
    lw $t0, س
    sw $t0, t0
    li $t0, 10
    sw $t0, t1
    li $t0, 0
    sw $t0, t2
    lw $t1, t0
    lw $t2, t1
    # CMP t0, t1
    lw $t1, t0
    lw $t2, t1
    bgt $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t2
    L3:
    lw $t0, t2
    beqz $t0, L0
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L1
    L0:
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L1:
    li $v0, 10
    syscall
