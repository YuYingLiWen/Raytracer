#include "CustomRandom.h"

#include <random>


CustomRandom::CustomRandom()
{
	std::srand((unsigned int)std::time(nullptr)); // seeds the RNG
}

CustomRandom& CustomRandom::GetInstance()
{
	static CustomRandom instance;

	return instance;
}

double CustomRandom::Generate()
{
	return (float)rand() / (float)RAND_MAX;
}

double CustomRandom::Generate(double num)
{
	double result = ((float)rand() / (float)RAND_MAX) * num * 2.0f - num;
	return result;
}

