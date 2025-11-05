# كود تجميعي مولد للغة العربية
# =============================

.data
الحد_الأقصى: .word 0
الشخص: .word 0
النتيجة: .word 0
س: .word 0
ع: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أحمد"
str_1: .asciiz "مرحبا"
str_2: .asciiz "علي"
str_3: .asciiz "كبير"
str_4: .asciiz "صغير"
str_5: .asciiz "تكرار"

.text
.globl main
main:
    li $t0, 100
    sw $t0, t0
    lw $t0, t0
    sw $t0, الحد_الأقصى
    proc_الجمع:
    li $v0, 1
    lw $a0, النتيجة
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    # UNKNOWN
    la $t0, str_0
    sw $t0, t1
    lw $t0, t1
    sw $t0, الاسم
    li $t0, 10
    sw $t0, t2
    lw $t0, t2
    sw $t0, س
    li $t0, 3.14
    sw $t0, t3
    lw $t0, t3
    sw $t0, ص
    la $t0, str_1
    sw $t0, t4
    lw $t0, t4
    sw $t0, نص
    lw $t0, صح
    sw $t0, t5
    lw $t0, t5
    sw $t0, منطقي
    li $t0, 5
    sw $t0, t6
    li $t0, 0
    sw $t0, t7
    # STORE_INDEXED أرقام[t7], t6
    la $t0, str_2
    sw $t0, t8
    lw $t0, t8
    sw $t0, موظف.الاسم
    lw $t0, س
    sw $t0, t9
    li $t0, 5
    sw $t0, t10
    li $t0, 0
    sw $t0, t11
    lw $t1, t9
    lw $t2, t10
    # CMP t9, t10
    lw $t1, t9
    lw $t2, t10
    bgt $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t11
    L3:
    lw $t0, t11
    beqz $t0, L0
    li $v0, 4
    la $a0, str_3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L1
    L0:
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L1:
    L4:
    lw $t0, س
    sw $t0, t12
    li $t0, 20
    sw $t0, t13
    li $t0, 0
    sw $t0, t14
    lw $t1, t12
    lw $t2, t13
    # CMP t12, t13
    lw $t1, t12
    lw $t2, t13
    ble $t1, $t2, L6
    j L7
    L6:
    li $t0, 1
    sw $t0, t14
    L7:
    lw $t0, t14
    beqz $t0, L5
    li $v0, 1
    lw $a0, س
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, س
    sw $t0, t15
    li $t0, 1
    sw $t0, t16
    lw $t1, t15
    lw $t2, t16
    add $t0, $t1, $t2
    sw $t0, t17
    lw $t0, t17
    sw $t0, س
    j L4
    L5:
    li $t0, 1
    sw $t0, t18
    lw $t0, t18
    sw $t0, ع
    L8:
    lw $t0, ع
    sw $t0, t19
    li $t0, 5
    sw $t0, t20
    lw $t1, t19
    lw $t2, t20
    # CMP t19, t20
    lw $t1, t19
    lw $t2, t20
    bgt $t1, $t2, L9
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t21
    lw $t0, ع
    sw $t0, t19
    lw $t1, t19
    lw $t2, t21
    add $t0, $t1, $t2
    sw $t0, t22
    lw $t0, t22
    sw $t0, ع
    j L8
    L9:
    L10:
    li $v0, 4
    la $a0, str_5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, خطأ
    sw $t0, t23
    lw $t0, t23
    beqz $t0, L10
    li $t0, 3
    sw $t0, t24
    lw $t0, t24
    sw $t0, param_0
    li $t0, 7
    sw $t0, t25
    lw $t0, t25
    sw $t0, param_1
    # UNKNOWN
    li $v0, 10
    syscall
