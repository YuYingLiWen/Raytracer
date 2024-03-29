#ifndef YUMATH_H
#define YUMATH_H


// Last assert: Feb 17, 2023

#include <cmath>
#include <memory>

#include "EigenIncludes.h"

#include "CustomRandom.h"

#ifndef PI
#define PI 3.14159265359f
#endif // !PI

#ifndef Deg2Rad
#define Deg2Rad 0.01745329251994f //(PI / 180.0f) from the above pi
#endif // !Deg2Rad

namespace YuMath
{
	using namespace Eigen;

	struct Tuple
	{
		double b_pos;
		double b_neg;
	};

	double Discriminant(double a, double b, double c);

	std::shared_ptr<Tuple> Quadratic(double a, double b, double c);
	std::shared_ptr<Tuple> Quadratic(double a, double b, double c, double discriminant);

	unsigned int HitResultsNum(double a, double b, double c);

	double TriangleArea(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3);

	Vector3d Lerp(Vector3d from, Vector3d to, double t);

	double Clamp(double val, double min, double max);

	Vector3d Reflect(const Vector3d& normal, const Vector3d& inverse);

	Vector3d ReflectRand(const Vector3d& normal, const Vector3d& inverse, const float rand_num);

	Vector3d RandomDir(const Vector3d& normal);
}

#endif

