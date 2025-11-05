# كود تجميعي مولد للغة العربية
# =============================

.data
رقم_صحيح: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    li $v0, 1
    lw $a0, رقم_صحيح
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, رقم_حقيقي
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, نص
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, منطقي
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
