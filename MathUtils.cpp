#include "MathUtils.h"

#include <cmath>

double SimpleRandom(double a, double b)
{
    constexpr double k = 0.0001;
    int f = rand() % 10000;
    int l = rand() % 10001;
    return a + (k*f + k*k*l)*(b - a);
}
