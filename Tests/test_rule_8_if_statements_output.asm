# كود تجميعي مولد للغة العربية
# =============================

.data
العمر: .word 0
الراتب: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "أنت بالغ"
str_1: .asciiz "راتب مرتفع"
str_2: .asciiz "راتب عادي"
str_3: .asciiz "أنت طفل"
str_4: .asciiz "أنت مراهق"
str_5: .asciiz "أنت كبير السن"

.text
.globl main
main:
    lw $t0, العمر
    sw $t0, t0
    li $t0, 18
    sw $t0, t1
    li $t0, 0
    sw $t0, t2
    lw $t1, t0
    lw $t2, t1
    # CMP t0, t1
    lw $t1, t0
    lw $t2, t1
    bgt $t1, $t2, L2
    j L3
    L2:
    li $t0, 1
    sw $t0, t2
    L3:
    lw $t0, t2
    beqz $t0, L0
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L0:
    lw $t0, الراتب
    sw $t0, t3
    li $t0, 10000.0
    sw $t0, t4
    li $t0, 0
    sw $t0, t5
    lw $t1, t3
    lw $t2, t4
    # CMP t3, t4
    lw $t1, t3
    lw $t2, t4
    bgt $t1, $t2, L6
    j L7
    L6:
    li $t0, 1
    sw $t0, t5
    L7:
    lw $t0, t5
    beqz $t0, L4
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L5
    L4:
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L5:
    lw $t0, العمر
    sw $t0, t6
    li $t0, 13
    sw $t0, t7
    li $t0, 0
    sw $t0, t8
    lw $t1, t6
    lw $t2, t7
    # CMP t6, t7
    lw $t1, t6
    lw $t2, t7
    blt $t1, $t2, L10
    j L11
    L10:
    li $t0, 1
    sw $t0, t8
    L11:
    lw $t0, t8
    beqz $t0, L8
    li $v0, 4
    la $a0, str_3
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L9
    L8:
    lw $t0, العمر
    sw $t0, t9
    li $t0, 18
    sw $t0, t10
    li $t0, 0
    sw $t0, t11
    lw $t1, t9
    lw $t2, t10
    # CMP t9, t10
    lw $t1, t9
    lw $t2, t10
    blt $t1, $t2, L14
    j L15
    L14:
    li $t0, 1
    sw $t0, t11
    L15:
    lw $t0, t11
    beqz $t0, L12
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L13
    L12:
    lw $t0, العمر
    sw $t0, t12
    li $t0, 65
    sw $t0, t13
    li $t0, 0
    sw $t0, t14
    lw $t1, t12
    lw $t2, t13
    # CMP t12, t13
    lw $t1, t12
    lw $t2, t13
    blt $t1, $t2, L18
    j L19
    L18:
    li $t0, 1
    sw $t0, t14
    L19:
    lw $t0, t14
    beqz $t0, L16
    li $v0, 4
    la $a0, str_0
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    j L17
    L16:
    li $v0, 4
    la $a0, str_5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    L17:
    L13:
    L9:
    li $v0, 10
    syscall
