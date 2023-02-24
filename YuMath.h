#ifndef YUMATH_H
#define YUMATH_H


// Last assert: Feb 17, 2023

#include <cmath>
#include <memory>

namespace YuMath
{
#define PI 3.14159265359f
#define Deg2Rad (PI / 180.0f)

	struct Tuple
	{
		double b_pos;
		double b_neg;
	};

	double Discriminant(double a, double b, double c) { return b * b - 4.0f * a * c; }

	std::shared_ptr<Tuple> Quadratic(double a, double b, double c)
	{
		double disc = Discriminant(a, b, c);

		if (disc < 0) return nullptr;
		if (a < 0) return nullptr;

		double pos = (-b + std::sqrt(disc)) / (2.0f * a);
		double neg = (-b - std::sqrt(disc)) / (2.0f * a);

		auto tuple = std::make_shared<Tuple>();
		tuple->b_pos = pos;
		tuple->b_neg = neg;

		return tuple;
	}

	unsigned int HitResultsNum(double a, double b, double c)
	{
		double dis = Discriminant(a, b, c);
		if (dis > 0) return 2;
		if (dis == 0) return 1;
		if (dis < 0) return 0;

		return -1; // Something went wrong
	}

	double HeronTriangle(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const Eigen::Vector3d& p3)
	{
		auto a = (p2 - p1).norm();
		auto b = (p3 - p2).norm();
		auto c = (p1 - p3).norm();

		// A = root[ s(s-a)(s-b)(s-c) ]
		auto s = (a + b + c) * 0.5f;


		return std::sqrt(s * (s - a) *(s - b) * (s-c));
	}

}
#endif

