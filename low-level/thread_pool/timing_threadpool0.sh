#!/bin/bash

echo "5 threads"
echo "---------"
(time ./threadpool0 5 1>out.txt) 2> tmp.txt
cat tmp.txt | tail -n 3

echo 

echo "1 thread"
echo "---------"
(time ./threadpool0 1 1>out.txt) 2> tmp.txt
cat tmp.txt | tail -n 3

rm out.txt
rm tmp.txt
