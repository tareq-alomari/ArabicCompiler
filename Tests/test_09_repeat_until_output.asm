# كود تجميعي مولد للغة العربية
# =============================

.data
ع: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    L0:
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ع
    sw $t0, t0
    li $t0, 1
    sw $t0, t1
    lw $t1, t0
    lw $t2, t1
    add $t0, $t1, $t2
    sw $t0, t2
    lw $t0, t2
    sw $t0, ع
    lw $t0, ع
    sw $t0, t3
    li $t0, 5
    sw $t0, t4
    li $t0, 0
    sw $t0, t5
    lw $t1, t3
    lw $t2, t4
    # CMP t3, t4
    lw $t1, t3
    lw $t2, t4
    bgt $t1, $t2, L1
    j L2
    L1:
    li $t0, 1
    sw $t0, t5
    L2:
    lw $t0, t5
    beqz $t0, L0
    li $v0, 10
    syscall
