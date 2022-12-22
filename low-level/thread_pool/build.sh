#!/bin/bash

for FNAME in threadpool0 threadpool1 threadpool2
do
clang++ -o $FNAME -Wall -Wextra -pedantic -g -std=c++17 $FNAME.cpp -lpthread
done


