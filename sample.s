addi $1 $0 8
addi $2 $0 8
sw $1 10($0)
lw $3 10($0)
sub $1 $1 $2
beqz $1 -2