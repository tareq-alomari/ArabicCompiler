# كود تجميعي مولد للغة العربية
# =============================

.data
رقم1: .word 0
رقم2: .word 0
ناتج: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
t6: .word 0
t7: .word 0
t8: .word 0
t9: .word 0
t10: .word 0
t11: .word 0
t12: .word 0
t13: .word 0
t14: .word 0
t15: .word 0
t16: .word 0
t17: .word 0
t18: .word 0
t19: .word 0
t20: .word 0
t21: .word 0
t22: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "الجمع: "
str_1: .asciiz "الطرح: "
str_2: .asciiz "الضرب: "
str_3: .asciiz "القسمة: "
str_4: .asciiz "الباقي: "

.text
.globl main
main:
    li $t0, 15
    sw $t0, t0
    lw $t0, t0
    sw $t0, رقم1
    li $t0, 3
    sw $t0, t1
    lw $t0, t1
    sw $t0, رقم2
    li $t0, 0
    sw $t0, t2
    lw $t0, t2
    sw $t0, ناتج
    lw $t0, رقم1
    sw $t0, t3
    lw $t0, رقم2
    sw $t0, t4
    lw $t1, t3
    lw $t2, t4
    add $t0, $t1, $t2
    sw $t0, t5
    lw $t0, t5
    sw $t0, ناتج
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ناتج
    sw $t0, t6
    li $v0, 1
    lw $a0, t6
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, رقم1
    sw $t0, t7
    lw $t0, رقم2
    sw $t0, t8
    lw $t1, t7
    lw $t2, t8
    sub $t0, $t1, $t2
    sw $t0, t9
    lw $t0, t9
    sw $t0, ناتج
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ناتج
    sw $t0, t10
    li $v0, 1
    lw $a0, t10
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, رقم1
    sw $t0, t11
    lw $t0, رقم2
    sw $t0, t12
    lw $t1, t11
    lw $t2, t12
    mul $t0, $t1, $t2
    sw $t0, t13
    lw $t0, t13
    sw $t0, ناتج
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ناتج
    sw $t0, t14
    li $v0, 1
    lw $a0, t14
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, رقم1
    sw $t0, t15
    lw $t0, رقم2
    sw $t0, t16
    lw $t1, t15
    lw $t2, t16
    div $t0, $t1, $t2
    sw $t0, t17
    lw $t0, t17
    sw $t0, ناتج
    li $v0, 4
    la $a0, str_3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ناتج
    sw $t0, t18
    li $v0, 1
    lw $a0, t18
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, رقم1
    sw $t0, t19
    lw $t0, رقم2
    sw $t0, t20
    # MOD t21, t19, t20
    lw $t0, t21
    sw $t0, ناتج
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ناتج
    sw $t0, t22
    li $v0, 1
    lw $a0, t22
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
