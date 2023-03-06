
#include "YuMath.h"


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
	double a = (p2 - p1).norm();
	double b = (p3 - p2).norm();
	double c = (p1 - p3).norm();

	// A = root[ s(s-a)(s-b)(s-c) ]
	double s = (a + b + c) * 0.5f;


	return std::sqrt(s * (s - a) * (s - b) * (s - c));
}


Vector3d Lerp(Vector3d from, Vector3d to, double t)
{
	return (1.0f - t) * from + t * to;
}

double Clamp(double val, double min, double max)
{
	return (val < min) ? min : (val > max) ? max : val;
}

Vector3d Reflect(const Vector3d& normal, const Vector3d& inverse)
{
	// inverse as in the inverse vector that hits the base of the normal vector
	return 2.0f * (normal * inverse.dot(normal)) - inverse;
}