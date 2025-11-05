# كود تجميعي مولد للغة العربية
# =============================

.data
باي: .word 0
الحد_الأقصى: .word 0
الحد_الأدنى: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "قيمة باي: "
str_1: .asciiz "الحد الأقصى: "

.text
.globl main
main:
    li $t0, 3.14159
    sw $t0, t0
    lw $t0, t0
    sw $t0, باي
    li $t0, 100
    sw $t0, t1
    lw $t0, t1
    sw $t0, الحد_الأقصى
    li $t0, 1
    sw $t0, t2
    lw $t0, t2
    sw $t0, الحد_الأدنى
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, باي
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
    lw $a0, الحد_الأقصى
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
