# Using BLAS to do fast matrix math 
- Very basic demonstration of installing and using the OpenBLAS set of fast matrix routines.
- A next iteration on this would be installing LAPACK and doing matrix decompositions.

## Examples
- Physical simulation code
- ML programs 

## Facts
- The optimized routines in OpenBLAS and similar alternatives are much more optimized for modern CPUs than writing the code by hand yourself. This makes them many times faster.

## Download, Compile, Install OpenBLAS 
```
git clone https://github.com/xianyi/OpenBLAS.git  
cd OpenBLAS
make FC=gfortran
make install PREFIX=/usr/local/src/OpenBLAS
```

Note that on Linux `make install` simply copies the executable files and headers to a location. There is no update to the registry or anything like that.

## Setup Environment Variables 
- Not explicitly necessary but handy for using the library.

Add this to .bashrc:
```
export BLAS_INC=/usr/local/src/OpenBLAS/include/
export BLAS_LIB=/usr/local/src/OpenBLAS/lib/
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/src/OpenBLAS/lib
```
