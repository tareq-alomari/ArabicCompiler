# كود تجميعي مولد للغة العربية
# =============================

.data
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "مرحبا بك في مترجم اللغة العربية!"
str_1: .asciiz "هذا برنامج أساسي بسيط"

.text
.globl main
main:
    li $v0, 4
    la $a0, str_0
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
    li $v0, 10
    syscall
