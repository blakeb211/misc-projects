#!/bin/bash
echo "Compiling..."
clang++ blas.cpp  -o program -Wall -Wextra -pedantic -g -std=c++17 -I${BLAS_INC} -L${BLAS_LIB} -lopenblas 


