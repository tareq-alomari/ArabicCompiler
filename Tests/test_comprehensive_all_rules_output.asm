# كود تجميعي مولد للغة العربية
# =============================

.data
الأرقام: .word 0
باي: .word 0
الحد_الأقصى: .word 0
س: .word 0
ن: .word 0
ص: .word 0
نص: .word 0
علم: .word 0
الشخص: .word 0
الموظف: .word 0
النتيجة: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "مرحبا من الإجراء"
str_1: .asciiz "المجموع: "
str_2: .asciiz "مرحبا"
str_3: .asciiz "=== اختبار التعليمات الأساسية ==="
str_4: .asciiz "س = "
str_5: .asciiz "=== اختبار جمل الشرط ==="
str_6: .asciiz "س أكبر من 5"
str_7: .asciiz "س أصغر من أو يساوي 5"
str_8: .asciiz "=== اختبار حلقات التكرار ==="
str_9: .asciiz "التكرار: "
str_10: .asciiz "=== اختبار التعبيرات ==="
str_11: .asciiz "س + ص = "
str_12: .asciiz "س * ص = "
str_13: .asciiz "=== اختبار القيم والثوابت ==="
str_14: .asciiz "باي = "
str_15: .asciiz "=== استدعاء الإجراءات ==="

.text
.globl main
main:
    li $t0, 3.14159
    sw $t0, t0
    lw $t0, t0
    sw $t0, باي
    li $t0, 100
    sw $t0, t1
    lw $t0, t1
    sw $t0, الحد_الأقصى
    proc_الطباعة_البسيطة:
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
    lw $a0, النتيجة
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    # UNKNOWN
    li $t0, 10
    sw $t0, t2
    lw $t0, t2
    sw $t0, س
    li $t0, 20
    sw $t0, t3
    lw $t0, t3
    sw $t0, ص
    li $t0, 3.14
    sw $t0, t4
    lw $t0, t4
    sw $t0, ن
    la $t0, str_2
    sw $t0, t5
    lw $t0, t5
    sw $t0, نص
    lw $t0, صح
    sw $t0, t6
    lw $t0, t6
    sw $t0, علم
    li $v0, 4
    la $a0, str_3
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
    li $v0, 1
    lw $a0, س
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
    lw $t0, س
    sw $t0, t7
    li $t0, 5
    sw $t0, t8
    li $t0, 0
    sw $t0, t9
    lw $t1, t7
    lw $t2, t8
    # CMP t7, t8
    lw $t1, t7
    lw $t2, t8
    bgt $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t9
    L3:
    lw $t0, t9
    beqz $t0, L0
    li $v0, 4
    la $a0, str_6
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L1
    L0:
    li $v0, 4
    la $a0, str_7
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L1:
    li $v0, 4
    la $a0, str_8
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t10
    lw $t0, t10
    sw $t0, ع
    L4:
    lw $t0, ع
    sw $t0, t11
    li $t0, 3
    sw $t0, t12
    lw $t1, t11
    lw $t2, t12
    # CMP t11, t12
    lw $t1, t11
    lw $t2, t12
    bgt $t1, $t2, L5
    li $v0, 4
    la $a0, str_9
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, ع
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t13
    lw $t0, ع
    sw $t0, t11
    lw $t1, t11
    lw $t2, t13
    add $t0, $t1, $t2
    sw $t0, t14
    lw $t0, t14
    sw $t0, ع
    j L4
    L5:
    li $v0, 4
    la $a0, str_10
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_11
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, س
    sw $t0, t15
    lw $t0, ص
    sw $t0, t16
    lw $t1, t15
    lw $t2, t16
    add $t0, $t1, $t2
    sw $t0, t17
    li $v0, 1
    lw $a0, t17
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_12
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, س
    sw $t0, t18
    lw $t0, ص
    sw $t0, t19
    lw $t1, t18
    lw $t2, t19
    mul $t0, $t1, $t2
    sw $t0, t20
    li $v0, 1
    lw $a0, t20
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_13
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_14
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, باي
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 4
    la $a0, str_15
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    # UNKNOWN
    li $t0, 5
    sw $t0, t21
    lw $t0, t21
    sw $t0, param_0
    li $t0, 3
    sw $t0, t22
    lw $t0, t22
    sw $t0, param_1
    # UNKNOWN
    li $v0, 10
    syscall
