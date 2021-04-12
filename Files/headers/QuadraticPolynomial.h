#include <iostream>
#include <unordered_map>

#include "Coefficients.h"

class QuadraticPolynomial {
    Coefficients *coefficients;
    std :: unordered_map <long double, long double> cache;
public:

    QuadraticPolynomial (Coefficients *coefficients) {
        this -> coefficients = coefficients;
    }

    /* Get the value of the quadratic polynomial in some point x */
    long double calculate(double x) {
        if (this -> cache.find(x) != this -> cache.end())
            return this -> cache[x];

        return this -> cache[x] = (coefficients -> a) * x * x + (coefficients -> b) * x + (coefficients -> c);
    }
};
