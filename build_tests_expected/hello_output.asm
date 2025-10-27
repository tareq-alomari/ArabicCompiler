# كود تجميعي مولد للغة العربية
# =============================

.data
عمر: .word 0
اسم: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
t6: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أحمد"
str_1: .asciiz "مرحباً بك "
str_2: .asciiz "عمرك هو: "
str_3: .asciiz "أنت بالغ"
str_4: .asciiz "أنت قاصر"
str_5: .asciiz "نهاية البرنامج"

.text
.globl main
main:
    la $t0, str_0
    sw $t0, t0
    lw $t0, t0
    sw $t0, اسم
    li $t0, 25
    sw $t0, t1
    lw $t0, t1
    sw $t0, عمر
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, اسم
    sw $t0, t2
    li $v0, 1
    lw $a0, t2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, عمر
    sw $t0, t3
    li $v0, 1
    lw $a0, t3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, عمر
    sw $t0, t4
    li $t0, 18
    sw $t0, t5
    li $t0, 0
    sw $t0, t6
    lw $t1, t4
    lw $t2, t5
    # CMP t4, t5
    lw $t1, t4
    lw $t2, t5
    bgt $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t6
    L3:
    lw $t0, t6
    beqz $t0, L0
    li $v0, 4
    la $a0, str_3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L1
    L0:
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L1:
    li $v0, 4
    la $a0, str_5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
