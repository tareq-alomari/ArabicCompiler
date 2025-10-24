# كود تجميعي مولد للغة العربية
# =============================

.data
ن: .word 0
س: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
t6: .word 0
t7: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أدخل رقمًا: "
str_1: .asciiz "الناتج: "

.text
.globl main
main:
    li $t0, 0
    sw $t0, t0
    lw $t0, t0
    sw $t0, س
    li $t0, 0
    sw $t0, t1
    lw $t0, t1
    sw $t0, ن
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 5
    syscall
    sw $v0, س
    li $t0, س
    sw $t0, t2
    li $t0, 2
    sw $t0, t3
    lw $t1, t2
    lw $t2, t3
    mul $t0, $t1, $t2
    sw $t0, t4
    li $t0, 5
    sw $t0, t5
    lw $t1, t4
    lw $t2, t5
    add $t0, $t1, $t2
    sw $t0, t6
    lw $t0, t6
    sw $t0, ن
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, ن
    sw $t0, t7
    li $v0, 1
    lw $a0, t7
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
