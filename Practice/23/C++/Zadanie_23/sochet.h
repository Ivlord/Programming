#pragma once
#ifndef _FACTORIAL_H_
	#include "factorial.h"
#endif

int sochet(int n, int k)
{
	return (factorial(n)) / ((factorial(k)) * (factorial(n-k)));
}