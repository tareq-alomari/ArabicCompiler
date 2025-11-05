# كود تجميعي مولد للغة العربية
# =============================

.data
النتيجة: .word 0
t0: .word 0
t1: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "السلام عليكم ورحمة الله وبركاته"
str_1: .asciiz "مجموع "
str_2: .asciiz " و "
str_3: .asciiz " = "

.text
.globl main
main:
    proc_السلام:
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    # UNKNOWN
    proc_الجمع:
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, أ
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
    lw $a0, ب
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
    lw $a0, النتيجة
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    # UNKNOWN
    # UNKNOWN
    li $t0, 5
    sw $t0, t0
    lw $t0, t0
    sw $t0, param_0
    li $t0, 3
    sw $t0, t1
    lw $t0, t1
    sw $t0, param_1
    # UNKNOWN
    li $v0, 10
    syscall
