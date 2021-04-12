#include "Domain.h"

template <class T>
struct Function {
    T *f;
    Domain *D;

    Function(T *f, Domain *d) {
        this -> f = f;
        this -> D = d;
    }
};
