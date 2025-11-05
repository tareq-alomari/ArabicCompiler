# كود تجميعي مولد للغة العربية
# =============================

.data
النتيجة: .word 0
t0: .word 0
t1: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    proc_الجمع:
    li $v0, 1
    lw $a0, النتيجة
    syscall
    li $v0, 4
    la $a0, newline
    syscall
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
