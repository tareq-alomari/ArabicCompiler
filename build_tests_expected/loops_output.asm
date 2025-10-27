# كود تجميعي مولد للغة العربية
# =============================

.data
عداد: .word 0
عدد: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
t6: .word 0
t7: .word 0
t8: .word 0
t9: .word 0
t10: .word 0
t11: .word 0
t12: .word 0
t13: .word 0
t14: .word 0
t15: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "عداد = "
str_1: .asciiz "العدد: "
str_2: .asciiz "انتهت الحلقات"

.text
.globl main
main:
    li $t0, 1
    sw $t0, t0
    lw $t0, t0
    sw $t0, عداد
    li $t0, 1
    sw $t0, t1
    lw $t0, t1
    sw $t0, عدد
    L0:
    lw $t0, عداد
    sw $t0, t2
    li $t0, 5
    sw $t0, t3
    li $t0, 0
    sw $t0, t4
    lw $t1, t2
    lw $t2, t3
    # CMP t2, t3
    lw $t1, t2
    lw $t2, t3
    ble $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t4
    L3:
    lw $t0, t4
    beqz $t0, L1
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, عداد
    sw $t0, t5
    li $v0, 1
    lw $a0, t5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, عداد
    sw $t0, t6
    li $t0, 1
    sw $t0, t7
    lw $t1, t6
    lw $t2, t7
    add $t0, $t1, $t2
    sw $t0, t8
    lw $t0, t8
    sw $t0, عداد
    j L0
    L1:
    L4:
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, عدد
    sw $t0, t9
    li $v0, 1
    lw $a0, t9
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, عدد
    sw $t0, t10
    li $t0, 1
    sw $t0, t11
    lw $t1, t10
    lw $t2, t11
    add $t0, $t1, $t2
    sw $t0, t12
    lw $t0, t12
    sw $t0, عدد
    lw $t0, عدد
    sw $t0, t13
    li $t0, 3
    sw $t0, t14
    li $t0, 0
    sw $t0, t15
    lw $t1, t13
    lw $t2, t14
    # CMP t13, t14
    lw $t1, t13
    lw $t2, t14
    bgt $t1, $t2, L5
    j L6
    L5:
    li $t0, 1
    sw $t0, t15
    L6:
    lw $t0, t15
    beqz $t0, L4
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
