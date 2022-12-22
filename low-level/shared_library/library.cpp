/* Create a shared object (dll on windows, .so on linux).
   It is pre-written code for your other programs to use (e.g. LibC is called my
   most programs).

    Benefits
    - Reduce duplicate code creation and disk space.
    - Ram usage reduction - multiple programs can call code from same library.
    - "Hot loading" of code into a program.

*/

int add(int a, int b) { return a + b; }

int multiply(int a, int b) { return a * b; }
