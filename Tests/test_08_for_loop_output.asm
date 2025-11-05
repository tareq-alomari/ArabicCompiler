# كود تجميعي مولد للغة العربية
# =============================

.data
ع: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    li $t0, 1
    sw $t0, t0
    lw $t0, t0
    sw $t0, ع
    L0:
    lw $t0, ع
    sw $t0, t1
    li $t0, 5
    sw $t0, t2
    lw $t1, t1
    lw $t2, t2
    # CMP t1, t2
    lw $t1, t1
    lw $t2, t2
    bgt $t1, $t2, L1
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t3
    lw $t0, ع
    sw $t0, t1
    lw $t1, t1
    lw $t2, t3
    add $t0, $t1, $t2
    sw $t0, t4
    lw $t0, t4
    sw $t0, ع
    j L0
    L1:
    li $v0, 10
    syscall
