## Creating and linking to a shared library
<br></br>
### Create object file
```
clang++ -c -fPIC library.cpp
```
  
### Create shared library from object file
```
clang++ -shared -o library.so library.o
```
  
### Compile main.cpp and link it with the shared object
```
clang++ -o main main.cpp library.so
```
  
### Running ```ldd``` on main shows dependence on library.so
```
        linux-vdso.so.1 (0x00007fffa28e3000)
----->  library.so (0x00007fcdf5e2f000) 
        libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007fcdf5c4d000)
        libm.so.6 => /usr/lib/x86_64-linux-gnu/libm.so.6 (0x00007fcdf5afe000)
        libgcc_s.so.1 => /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007fcdf5ae3000)
        libc.so.6 => /usr/lib/x86_64-linux-gnu/libc.so.6 (0x00007fcdf58f1000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fcdf5e36000)
```
    
### Run executable when `library.so` is in different directory. 
- If shared library is in a different directory, use the **LD_LIBRARY_PATH** environment variable to tell the program where to look for the shared library, or, when compiling, use the **rpath** command line flag to tell clang where to look for the shared library.
- See *./run_from_diff_dir/* folder for bash script to run main when library.so is in a different directory.


     
<br></br>
## Common errors with shared libraries
**Compiling main.cpp** without telling compiler how to load the shared library:  
```
clang++ main.cpp
```
Produces  
```
/usr/bin/ld: /tmp/main-95c1a1.o: in function `main':
/home/bb/wrk/low-level/shared_library/main.cpp:9: undefined reference to `add(int, int)'
/usr/bin/ld: /home/bb/wrk/low-level/shared_library/main.cpp:10: undefined reference to `multiply(int, int)'
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```