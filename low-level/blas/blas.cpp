#include "/usr/local/src/OpenBLAS/include/cblas.h"
// Location specified explicitly because another blas implementation
// is install on the system and I want my editor to pull in the 
// correct header when it gives me code completion.
#include <iostream>
using std::cout, std::endl;

int main() {
  double x[] = {1.0, 2.0, 3.0};
  double coeff = 4.323;
  int one = 1;
  int n = 3;
  // Direct call to cblas
  cblas_dscal(n, coeff, x, one);
  for (int i = 0; i < n; i++)
    cout << " " << x[i];
  cout << endl;
  return 0;
}