#include "CustomRandom.h"
#include "YuMath.h"

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
	return ((float)rand() / (float)RAND_MAX) * num * 2.0f - num;
}

// Returns: angle in radian
double CustomRandom::GenerateAngle(double angle)
{
	// Goes from 0 to angle.
	return ((float)rand() / (float)RAND_MAX) * angle * Deg2Rad;
}

