# كود تجميعي مولد للغة العربية
# =============================

.data
أرقام: .word 0
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
newline: .asciiz "\n"
int_format: .asciiz "%d"
str_format: .asciiz "%s"

.text
.globl main
main:
    li $t0, 10
    sw $t0, t0
    li $t0, 0
    sw $t0, t1
    # STORE_INDEXED أرقام[t1], t0
    li $t0, 20
    sw $t0, t2
    li $t0, 1
    sw $t0, t3
    # STORE_INDEXED أرقام[t3], t2
    li $t0, 30
    sw $t0, t4
    li $t0, 2
    sw $t0, t5
    # STORE_INDEXED أرقام[t5], t4
    li $t0, 0
    sw $t0, t6
    # LOAD_INDEXED t7, أرقام[t6]
    li $v0, 1
    lw $a0, t7
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 1
    sw $t0, t8
    # LOAD_INDEXED t9, أرقام[t8]
    li $v0, 1
    lw $a0, t9
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t0, 2
    sw $t0, t10
    # LOAD_INDEXED t11, أرقام[t10]
    li $v0, 1
    lw $a0, t11
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
