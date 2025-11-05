# كود تجميعي مولد للغة العربية
# =============================

.data
ع: .word 0
المجموع: .word 0
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
t16: .word 0
t17: .word 0
t18: .word 0
t19: .word 0
t20: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "حلقة كرر من 1 إلى 5:"
str_1: .asciiz "حلقة طالما:"
str_2: .asciiz "مجموع الأرقام من 1 إلى 10: "

.text
.globl main
main:
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t0
    lw $t0, t0
    sw $t0, ع
    L0:
    lw $t0, ع
    sw $t0, t1
    li $t0, 5
    sw $t0, t2
    lw $t1, t1
    lw $t2, t2
    # CMP t1, t2
    lw $t1, t1
    lw $t2, t2
    bgt $t1, $t2, L1
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t3
    lw $t0, ع
    sw $t0, t1
    lw $t1, t1
    lw $t2, t3
    add $t0, $t1, $t2
    sw $t0, t4
    lw $t0, t4
    sw $t0, ع
    j L0
    L1:
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t5
    lw $t0, t5
    sw $t0, ع
    L2:
    lw $t0, ع
    sw $t0, t6
    li $t0, 3
    sw $t0, t7
    li $t0, 0
    sw $t0, t8
    lw $t1, t6
    lw $t2, t7
    # CMP t6, t7
    lw $t1, t6
    lw $t2, t7
    ble $t1, $t2, L4
    j L5
    L4:
    li $t0, 1
    sw $t0, t8
    L5:
    lw $t0, t8
    beqz $t0, L3
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ع
    sw $t0, t9
    li $t0, 1
    sw $t0, t10
    lw $t1, t9
    lw $t2, t10
    add $t0, $t1, $t2
    sw $t0, t11
    lw $t0, t11
    sw $t0, ع
    j L2
    L3:
    li $t0, 0
    sw $t0, t12
    lw $t0, t12
    sw $t0, المجموع
    li $t0, 1
    sw $t0, t13
    lw $t0, t13
    sw $t0, ع
    L6:
    lw $t0, ع
    sw $t0, t14
    li $t0, 10
    sw $t0, t15
    lw $t1, t14
    lw $t2, t15
    # CMP t14, t15
    lw $t1, t14
    lw $t2, t15
    bgt $t1, $t2, L7
    lw $t0, المجموع
    sw $t0, t16
    lw $t0, ع
    sw $t0, t17
    lw $t1, t16
    lw $t2, t17
    add $t0, $t1, $t2
    sw $t0, t18
    lw $t0, t18
    sw $t0, المجموع
    li $t0, 1
    sw $t0, t19
    lw $t0, ع
    sw $t0, t14
    lw $t1, t14
    lw $t2, t19
    add $t0, $t1, $t2
    sw $t0, t20
    lw $t0, t20
    sw $t0, ع
    j L6
    L7:
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, المجموع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
