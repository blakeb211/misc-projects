/* Demonstration showing how to use println_resursive.h with built-in types and
 * user defined types. */
#include "println_recursive.h"
#include <iostream>
#include <string>

struct Point {
  float x, y;
};

std::ostream &operator<<(std::ostream &os, Point const &p) {
  os << "x:" << p.x << " y:" << p.y;
  return os;
}

Point operator*(Point const &a, int b) { return Point{a.x * b, a.y * b}; }

int main() {
  using std::cout, std::endl;
  /* BEGIN VARIABLES */
  int a = 10;
  float b = 10.22f;
  std::string s = "blake";
  Point center{11.1f, 0.5f};
  /* END VARIABLES */

  println();
  println(a, b);
  println(a, b, "line1", (double)a * b);
  println(a, b, "line2", (double)a * b, 99u);
  println(a, b, center * 3, center);

  return 0;
}
