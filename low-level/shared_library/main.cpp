/* Call functions from our library */

#include <iostream>
// Need function definitions for the functions we are calling from our shared library.
int add(int, int);
int multiply(int, int);

int main() {
    std::cout << add(2,40) << std::endl;
    std::cout << multiply(21,2) << std::endl;
  return 0;
}

