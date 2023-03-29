#!/bin/bash

alpha=0.01
file="chry.txt"
min_k=3
max_k=10
threshold=0.8
run=1
for i in $(seq $min_k $max_k)
do
    printf "run : $run"
    bin/cpm -k $i -a $alpha -f "example/${file}" -o "runs/${run}.run" -t $threshold
    run=$((run+1))
done




