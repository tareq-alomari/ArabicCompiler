# كود تجميعي مولد للغة العربية
# =============================

.data
س: .word 0
درجة: .word 0
ص: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "امتياز"
str_1: .asciiz "جيد جداً"
str_2: .asciiz "جيد"
str_3: .asciiz "مقبول"
str_4: .asciiz "راسب"
str_5: .asciiz "الشرط الأول صحيح"
str_6: .asciiz "الشرط الثاني صحيح"

.text
.globl main
main:
    li $t0, 85
    sw $t0, t0
    lw $t0, t0
    sw $t0, درجة
    li $t0, 10
    sw $t0, t1
    lw $t0, t1
    sw $t0, س
    li $t0, 20
    sw $t0, t2
    lw $t0, t2
    sw $t0, ص
    lw $t0, درجة
    sw $t0, t3
    li $t0, 90
    sw $t0, t4
    li $t0, 0
    sw $t0, t5
    lw $t1, t3
    lw $t2, t4
    # CMP t3, t4
    lw $t1, t3
    lw $t2, t4
    bge $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t5
    L3:
    lw $t0, t5
    beqz $t0, L0
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L1
    L0:
    lw $t0, درجة
    sw $t0, t6
    li $t0, 80
    sw $t0, t7
    li $t0, 0
    sw $t0, t8
    lw $t1, t6
    lw $t2, t7
    # CMP t6, t7
    lw $t1, t6
    lw $t2, t7
    bge $t1, $t2, L6
    j L7
    L6:
    li $t0, 1
    sw $t0, t8
    L7:
    lw $t0, t8
    beqz $t0, L4
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L5
    L4:
    lw $t0, درجة
    sw $t0, t9
    li $t0, 70
    sw $t0, t10
    li $t0, 0
    sw $t0, t11
    lw $t1, t9
    lw $t2, t10
    # CMP t9, t10
    lw $t1, t9
    lw $t2, t10
    bge $t1, $t2, L10
    j L11
    L10:
    li $t0, 1
    sw $t0, t11
    L11:
    lw $t0, t11
    beqz $t0, L8
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L9
    L8:
    lw $t0, درجة
    sw $t0, t12
    li $t0, 60
    sw $t0, t13
    li $t0, 0
    sw $t0, t14
    lw $t1, t12
    lw $t2, t13
    # CMP t12, t13
    lw $t1, t12
    lw $t2, t13
    bge $t1, $t2, L14
    j L15
    L14:
    li $t0, 1
    sw $t0, t14
    L15:
    lw $t0, t14
    beqz $t0, L12
    li $v0, 4
    la $a0, str_3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L13
    L12:
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L13:
    L9:
    L5:
    L1:
    lw $t0, س
    sw $t0, t15
    li $t0, 5
    sw $t0, t16
    lw $t0, ص
    sw $t0, t17
    # AND t18, t16, t17
    li $t0, 0
    sw $t0, t19
    lw $t1, t15
    lw $t2, t18
    # CMP t15, t18
    lw $t1, t15
    lw $t2, t18
    bgt $t1, $t2, L18
    j L19
    L18:
    li $t0, 1
    sw $t0, t19
    L19:
    li $t0, 30
    sw $t0, t20
    li $t0, 0
    sw $t0, t21
    lw $t1, t19
    lw $t2, t20
    # CMP t19, t20
    lw $t1, t19
    lw $t2, t20
    blt $t1, $t2, L20
    j L21
    L20:
    li $t0, 1
    sw $t0, t21
    L21:
    lw $t0, t21
    beqz $t0, L16
    li $v0, 4
    la $a0, str_5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L16:
    lw $t0, س
    sw $t0, t22
    li $t0, 10
    sw $t0, t23
    lw $t0, ص
    sw $t0, t24
    # OR t25, t23, t24
    li $t0, 0
    sw $t0, t26
    lw $t1, t22
    lw $t2, t25
    # CMP t22, t25
    lw $t1, t22
    lw $t2, t25
    beq $t1, $t2, L24
    j L25
    L24:
    li $t0, 1
    sw $t0, t26
    L25:
    li $t0, 15
    sw $t0, t27
    li $t0, 0
    sw $t0, t28
    lw $t1, t26
    lw $t2, t27
    # CMP t26, t27
    lw $t1, t26
    lw $t2, t27
    beq $t1, $t2, L26
    j L27
    L26:
    li $t0, 1
    sw $t0, t28
    L27:
    lw $t0, t28
    beqz $t0, L22
    li $v0, 4
    la $a0, str_6
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L22:
    li $v0, 10
    syscall
