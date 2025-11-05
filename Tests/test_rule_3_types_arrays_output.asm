# كود تجميعي مولد للغة العربية
# =============================

.data
الأرقام: .word 0
الأسعار: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "الرقم الأول: "
str_1: .asciiz "السعر الأول: "

.text
.globl main
main:
    li $t0, 10
    sw $t0, t0
    li $t0, 0
    sw $t0, t1
    # STORE_INDEXED الأرقام[t1], t0
    li $t0, 20
    sw $t0, t2
    li $t0, 1
    sw $t0, t3
    # STORE_INDEXED الأرقام[t3], t2
    li $t0, 30
    sw $t0, t4
    li $t0, 2
    sw $t0, t5
    # STORE_INDEXED الأرقام[t5], t4
    li $t0, 99.99
    sw $t0, t6
    li $t0, 0
    sw $t0, t7
    # STORE_INDEXED الأسعار[t7], t6
    li $t0, 149.50
    sw $t0, t8
    li $t0, 1
    sw $t0, t9
    # STORE_INDEXED الأسعار[t9], t8
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 0
    sw $t0, t10
    # LOAD_INDEXED t11, الأرقام[t10]
    li $v0, 1
    lw $a0, t11
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 0
    sw $t0, t12
    # LOAD_INDEXED t13, الأسعار[t12]
    li $v0, 1
    lw $a0, t13
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
