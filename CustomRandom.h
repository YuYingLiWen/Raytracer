#ifndef RANDOM_H
#define RANDOM_H

class CustomRandom
{
private:
	CustomRandom();
public:

	static CustomRandom& GetInstance();

	double Generate();
	double Generate(double num);
};


#endif // !RANDOM_H
