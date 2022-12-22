#ifndef PRINTLN_RECURSIVE_H
#define PRINTLN_RECURSIVE_H

#include <iostream>

void println() { std::cout << "\n"; };
template <class H> void println(H const &head) { std::cout << head << "\n"; };
template <class H, class... P1toN>
void println(H const &head, P1toN const &... rest) {
  std::cout << head << " ";
  println(rest...);
}

#endif