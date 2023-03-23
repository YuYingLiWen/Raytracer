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
	double GenerateAngle(double angle);
};


#endif // !RANDOM_H
