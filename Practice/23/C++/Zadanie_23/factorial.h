
#ifndef _FACTORIAL_H_
#define _FACTORIAL_H_

double factorial(int n) {
    return (n < 3) ? ((n < 1) ? 1 : n) : n * factorial(n - 1);
}

#endif
