# كود تجميعي مولد للغة العربية
# =============================

.data
النتيجة: .word 0
س: .word 0
ص: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "قيمة س: "
str_1: .asciiz "قيمة ص: "
str_2: .asciiz "النتيجة: "

.text
.globl main
main:
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, س
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
    li $v0, 1
    lw $a0, ص
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, س
    sw $t0, t0
    lw $t0, ص
    sw $t0, t1
    lw $t1, t0
    lw $t2, t1
    add $t0, $t1, $t2
    sw $t0, t2
    lw $t0, t2
    sw $t0, النتيجة
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, النتيجة
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
