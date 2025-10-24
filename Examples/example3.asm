# كود تجميعي مولد للغة العربية
# =============================

.data
المجموع: .word 0
العداد: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "مجموع الأعداد من 1 إلى 10 هو: "

.text
.globl main
main:
    li $t0, 0
    sw $t0, t0
    lw $t0, t0
    sw $t0, العداد
    li $t0, 0
    sw $t0, t1
    lw $t0, t1
    sw $t0, المجموع
    li $t0, 0
    sw $t0, t2
    lw $t0, t2
    sw $t0, المجموع
    li $t0, 1
    sw $t0, t3
    lw $t0, t3
    sw $t0, العداد
    L0:
    li $t0, العداد
    sw $t0, t4
    li $t0, 10
    sw $t0, t5
    li $t0, 0
    sw $t0, t6
    # CMP t4, t5
    # JLE L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t6
    L3:
    lw $t0, t6
    beqz $t0, L1
    li $t0, المجموع
    sw $t0, t7
    li $t0, العداد
    sw $t0, t8
    lw $t1, t7
    lw $t2, t8
    add $t0, $t1, $t2
    sw $t0, t9
    lw $t0, t9
    sw $t0, المجموع
    li $t0, العداد
    sw $t0, t10
    li $t0, 1
    sw $t0, t11
    lw $t1, t10
    lw $t2, t11
    add $t0, $t1, $t2
    sw $t0, t12
    lw $t0, t12
    sw $t0, العداد
    j L0
    L1:
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, المجموع
    sw $t0, t13
    li $v0, 1
    lw $a0, t13
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
