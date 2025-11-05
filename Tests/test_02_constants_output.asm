# كود تجميعي مولد للغة العربية
# =============================

.data
الحد_الأقصى: .word 0
س: .word 0
t0: .word 0
t1: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أحمد"

.text
.globl main
main:
    li $t0, 100
    sw $t0, t0
    lw $t0, t0
    sw $t0, الحد_الأقصى
    la $t0, str_0
    sw $t0, t1
    lw $t0, t1
    sw $t0, الاسم
    li $v0, 1
    lw $a0, س
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
