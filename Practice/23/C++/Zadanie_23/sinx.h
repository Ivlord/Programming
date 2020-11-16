#ifndef _FACTORIAL_H_
	#include "factorial.h"
#endif


#ifndef SINX_H
#define SINX_H

double sinx(double x, int k)
{
	double res = 0;
	double xx = x; // чтобы не использовать pow(a,b)
	int sig = 1;

	for (short i = 0; i < k; i++)
	{
		res += sig * xx / factorial(2 * i + 1);
		xx = xx * x * x;
		sig = -sig;
	}
	return res;
}

#endif