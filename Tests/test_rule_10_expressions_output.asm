# كود تجميعي مولد للغة العربية
# =============================

.data
أ: .word 0
ب: .word 0
ج: .word 0
د: .word 0
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
t21: .word 0
t22: .word 0
t23: .word 0
t24: .word 0
t25: .word 0
t26: .word 0
t27: .word 0
t28: .word 0
t29: .word 0
t30: .word 0
t31: .word 0
t32: .word 0
t33: .word 0
t34: .word 0
t35: .word 0
t36: .word 0
t37: .word 0
t38: .word 0
t39: .word 0
t40: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أ + ب = "
str_1: .asciiz "أ - ب = "
str_2: .asciiz "أ * ب = "
str_3: .asciiz "ب / أ = "
str_4: .asciiz "ب % أ = "
str_5: .asciiz "أ > ب: "
str_6: .asciiz "أ < ب: "
str_7: .asciiz "أ == أ: "
str_8: .asciiz "أ != ب: "
str_9: .asciiz "(أ > 5) && (ب > 15): "
str_10: .asciiz "(أ > 15) || (ب > 15): "

.text
.globl main
main:
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t0
    lw $t0, ب
    sw $t0, t1
    lw $t1, t0
    lw $t2, t1
    add $t0, $t1, $t2
    sw $t0, t2
    li $v0, 1
    lw $a0, t2
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
    lw $t0, أ
    sw $t0, t3
    lw $t0, ب
    sw $t0, t4
    lw $t1, t3
    lw $t2, t4
    sub $t0, $t1, $t2
    sw $t0, t5
    li $v0, 1
    lw $a0, t5
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
    lw $t0, أ
    sw $t0, t6
    lw $t0, ب
    sw $t0, t7
    lw $t1, t6
    lw $t2, t7
    mul $t0, $t1, $t2
    sw $t0, t8
    li $v0, 1
    lw $a0, t8
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
    lw $t0, ب
    sw $t0, t9
    lw $t0, أ
    sw $t0, t10
    lw $t1, t9
    lw $t2, t10
    div $t0, $t1, $t2
    sw $t0, t11
    li $v0, 1
    lw $a0, t11
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, ب
    sw $t0, t12
    lw $t0, أ
    sw $t0, t13
    # MOD t14, t12, t13
    li $v0, 1
    lw $a0, t14
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t15
    lw $t0, ب
    sw $t0, t16
    li $t0, 0
    sw $t0, t17
    lw $t1, t15
    lw $t2, t16
    # CMP t15, t16
    lw $t1, t15
    lw $t2, t16
    bgt $t1, $t2, L0
    j L1
    L0:
    li $t0, 1
    sw $t0, t17
    L1:
    li $v0, 1
    lw $a0, t17
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_6
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t18
    lw $t0, ب
    sw $t0, t19
    li $t0, 0
    sw $t0, t20
    lw $t1, t18
    lw $t2, t19
    # CMP t18, t19
    lw $t1, t18
    lw $t2, t19
    blt $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t20
    L3:
    li $v0, 1
    lw $a0, t20
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_7
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t21
    lw $t0, أ
    sw $t0, t22
    li $t0, 0
    sw $t0, t23
    lw $t1, t21
    lw $t2, t22
    # CMP t21, t22
    lw $t1, t21
    lw $t2, t22
    beq $t1, $t2, L4
    j L5
    L4:
    li $t0, 1
    sw $t0, t23
    L5:
    li $v0, 1
    lw $a0, t23
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_8
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t24
    lw $t0, ب
    sw $t0, t25
    li $t0, 0
    sw $t0, t26
    lw $t1, t24
    lw $t2, t25
    # CMP t24, t25
    lw $t1, t24
    lw $t2, t25
    bne $t1, $t2, L6
    j L7
    L6:
    li $t0, 1
    sw $t0, t26
    L7:
    li $v0, 1
    lw $a0, t26
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_9
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t27
    li $t0, 5
    sw $t0, t28
    li $t0, 0
    sw $t0, t29
    lw $t1, t27
    lw $t2, t28
    # CMP t27, t28
    lw $t1, t27
    lw $t2, t28
    bgt $t1, $t2, L8
    j L9
    L8:
    li $t0, 1
    sw $t0, t29
    L9:
    lw $t0, ب
    sw $t0, t30
    li $t0, 15
    sw $t0, t31
    li $t0, 0
    sw $t0, t32
    lw $t1, t30
    lw $t2, t31
    # CMP t30, t31
    lw $t1, t30
    lw $t2, t31
    bgt $t1, $t2, L10
    j L11
    L10:
    li $t0, 1
    sw $t0, t32
    L11:
    # AND t33, t29, t32
    li $v0, 1
    lw $a0, t33
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_10
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, أ
    sw $t0, t34
    li $t0, 15
    sw $t0, t35
    li $t0, 0
    sw $t0, t36
    lw $t1, t34
    lw $t2, t35
    # CMP t34, t35
    lw $t1, t34
    lw $t2, t35
    bgt $t1, $t2, L12
    j L13
    L12:
    li $t0, 1
    sw $t0, t36
    L13:
    lw $t0, ب
    sw $t0, t37
    li $t0, 15
    sw $t0, t38
    li $t0, 0
    sw $t0, t39
    lw $t1, t37
    lw $t2, t38
    # CMP t37, t38
    lw $t1, t37
    lw $t2, t38
    bgt $t1, $t2, L14
    j L15
    L14:
    li $t0, 1
    sw $t0, t39
    L15:
    # OR t40, t36, t39
    li $v0, 1
    lw $a0, t40
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
