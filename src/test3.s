addi $1 $0 8
addi $2 $0 1
sub $1 $1 $2
addi $2 $2 -1
beqz $2 -2
add $1 $1 $2