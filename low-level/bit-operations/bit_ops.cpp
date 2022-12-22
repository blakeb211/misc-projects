#include <iostream>
#include <bitset>
#include "nr3.h"
using namespace std;

namespace Erf {
    double erf();
    double erfc();
};

int main()
{
    cout << 10 << " " << 21 << endl;
    cout << bitset<8>(10) << endl;
    cout << bitset<8>(21) << endl;

    for (int i = 0; i < 8; i++)
    {
        cout << i << " " << (1 << i) << " " << bitset<8>(1 << i) << endl;
    }

    // construct a mask of n bits
    cout << bitset<8>((1 << 8) - 1) << endl;

    cout << Erf::erf() << endl;
    cout << Erf::erfc() << endl;
    return 0;
}

double Erf::erf() { return 1; }
double Erf::erfc() { return 2; }