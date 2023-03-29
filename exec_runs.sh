#!/bin/bash

alpha=0.01
file="chry.txt"
min_k=3
max_k=10
threshold=0.8

run=1
for i in $(seq $min_k $max_k)
do
    echo $i
    bin/cpm -k $i -a $alpha -f "example/${file}" -o "runs/${cnt}.run" -t $threshold
    cnt=$((cnt+1))
done




