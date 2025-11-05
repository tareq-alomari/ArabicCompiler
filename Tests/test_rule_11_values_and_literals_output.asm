# كود تجميعي مولد للغة العربية
# =============================

.data
باي: .word 0
رقم_حقيقي: .word 0
الحد_الأقصى: .word 0
الرسالة: .word 0
رقم_صحيح: .word 0
علم: .word 0
نص: .word 0
t0: .word 0
t1: .word 0
t2: .word 0
t3: .word 0
t4: .word 0
t5: .word 0
t6: .word 0
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"
str_0: .asciiz "مرحبا بك"
str_1: .asciiz "رقم صحيح: "
str_2: .asciiz "رقم حقيقي: "
str_3: .asciiz "السلام عليكم"
str_4: .asciiz "سلسلة رمزية: "
str_5: .asciiz "قيمة منطقية (صح): "
str_6: .asciiz "قيمة منطقية (خطأ): "
str_7: .asciiz "قيمة الثابت باي: "
str_8: .asciiz "قيمة الثابت الحد_الأقصى: "
str_9: .asciiz "قيمة الثابت الرسالة: "

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
    la $t0, str_0
    sw $t0, t2
    lw $t0, t2
    sw $t0, الرسالة
    li $v0, 4
    la $a0, str_1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, رقم_صحيح
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 3.14159
    sw $t0, t3
    lw $t0, t3
    sw $t0, رقم_حقيقي
    li $v0, 4
    la $a0, str_2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, رقم_حقيقي
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    la $t0, str_3
    sw $t0, t4
    lw $t0, t4
    sw $t0, نص
    li $v0, 4
    la $a0, str_4
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, نص
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, صح
    sw $t0, t5
    lw $t0, t5
    sw $t0, علم
    li $v0, 4
    la $a0, str_5
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, علم
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, خطأ
    sw $t0, t6
    lw $t0, t6
    sw $t0, علم
    li $v0, 4
    la $a0, str_6
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 1
    lw $a0, علم
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
    li $v0, 1
    lw $a0, باي
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
    li $v0, 1
    lw $a0, الحد_الأقصى
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
    li $v0, 1
    lw $a0, الرسالة
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
