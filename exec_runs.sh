#!/bin/bash

alpha=0.01
file="teste.txt"
min_k=3
max_k=10


run=1
for i in $(seq $min_k $max_k)
do
    echo $i
    bin/cpm -k $i -a $alpha -f "example/${file}" -o "runs/${cnt}.run"
    cnt=$((cnt+1))
done




