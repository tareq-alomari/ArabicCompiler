# كود تجميعي مولد للغة العربية
# =============================

.data
العمر: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أدخل عمرك: "
str_1: .asciiz "أنت بالغ"
str_2: .asciiz "أنت قاصر"

.text
.globl main
main:
    li $t0, 0
    sw $t0, t0
    lw $t0, t0
    sw $t0, العمر
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 5
    syscall
    sw $v0, العمر
    lw $t0, العمر
    sw $t0, t1
    li $t0, 18
    sw $t0, t2
    li $t0, 0
    sw $t0, t3
    # CMP t1, t2
    lw $t1, t1
    lw $t2, t2
    bge $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t3
    L3:
    lw $t0, t3
    beqz $t0, L0
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L1
    L0:
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L1:
    li $v0, 10
    syscall
