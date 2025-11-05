# كود تجميعي مولد للغة العربية
# =============================

.data
ع: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    L0:
    lw $t0, ع
    sw $t0, t0
    li $t0, 5
    sw $t0, t1
    li $t0, 0
    sw $t0, t2
    lw $t1, t0
    lw $t2, t1
    # CMP t0, t1
    lw $t1, t0
    lw $t2, t1
    ble $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t2
    L3:
    lw $t0, t2
    beqz $t0, L1
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ع
    sw $t0, t3
    li $t0, 1
    sw $t0, t4
    lw $t1, t3
    lw $t2, t4
    add $t0, $t1, $t2
    sw $t0, t5
    lw $t0, t5
    sw $t0, ع
    j L0
    L1:
    li $v0, 10
    syscall
