# كود تجميعي مولد للغة العربية
# =============================

.data
موظف: .word 0
الشخص: .word 0
t0: .word 0
t1: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أحمد"

.text
.globl main
main:
    la $t0, str_0
    sw $t0, t0
    lw $t0, t0
    sw $t0, موظف.الاسم
    li $t0, 30
    sw $t0, t1
    lw $t0, t1
    sw $t0, موظف.العمر
    li $v0, 1
    lw $a0, موظف.الاسم
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, موظف.العمر
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
