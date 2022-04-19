#!/bin/bash

src="main.cpp"
parser="assembly_parser.cpp"
input="sample.s"
esrc="main"
eparser="parser"
pipeline="pipeline.txt"

if [ -n "$1" ]
then 
    input=$1
fi

echo "you are now opening the file: "$input

g++-11 $parser -o $eparser -O2
./$eparser $input

g++-11 $src -o $esrc -O2
./$esrc $input
