# كود تجميعي مولد للغة العربية
# =============================

.data
الموظف: .word 0
الشخص: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أحمد"
str_1: .asciiz "اسم الموظف: "
str_2: .asciiz "عمر الموظف: "
str_3: .asciiz "راتب الموظف: "

.text
.globl main
main:
    la $t0, str_0
    sw $t0, t0
    lw $t0, t0
    sw $t0, الموظف.الاسم
    li $t0, 30
    sw $t0, t1
    lw $t0, t1
    sw $t0, الموظف.العمر
    li $t0, 5000.0
    sw $t0, t2
    lw $t0, t2
    sw $t0, الموظف.الراتب
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, الموظف.الاسم
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
    li $v0, 1
    lw $a0, الموظف.العمر
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, الموظف.الراتب
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
