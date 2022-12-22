#include <iostream>
#include <string>
/***********************************************************************/
/*                          Helper Functions                           */
/***********************************************************************/
bool check_error(int value, int expected, std::string errMsg) {
  if (value < expected) {
    std::cout << "error. " << errMsg << std::endl;
    return true; 
  }
  return false;
}

void print_bytes_receieved(int numBytes, std::string buffer) {
  printf("%d bytes of data were received\n", numBytes);
  printf("Data received:\n");
  printf("-------------------------------\n");
  printf("%s\n", buffer.data());
  printf("-------------------------------\n");
}
