#ifndef RANDOM_H

#include <random>

class Random
{
public:
	Random() 
	{
		std::srand((unsigned int)std::time(nullptr)); // seeds the RNG
	}

	double Generate(double num) 
	{ 
		double result = ((float)rand() / (float)RAND_MAX) * num * 2.0f - num;
		return result; 
	}
};


#endif // !RANDOM_H
