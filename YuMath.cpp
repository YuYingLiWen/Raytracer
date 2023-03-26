
#include "YuMath.h"

namespace YuMath
{
	double Discriminant(double a, double b, double c) { return b * b - 4.0f * a * c; }

	std::shared_ptr<Tuple> Quadratic(double a, double b, double c)
	{
		double disc = Discriminant(a, b, c);

		if (disc < 0) return nullptr;
		if (a < 0) return nullptr;

		double root_disc = std::sqrt(disc);

		auto tuple = std::make_shared<Tuple>();
		tuple->b_pos = (-b + root_disc) / (2.0f * a);
		tuple->b_neg = (-b - root_disc) / (2.0f * a);

		return tuple;
	}

	std::shared_ptr<Tuple> Quadratic(double a, double b, double c, double discriminant)
	{
		if (discriminant < 0) return nullptr;
		if (a < 0) return nullptr;

		double root_disc = std::sqrt(discriminant);

		auto tuple = std::make_shared<Tuple>();
		tuple->b_pos = (-b + root_disc) / (2.0f * a);
		tuple->b_neg = (-b - root_disc) / (2.0f * a);

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

	double TriangleArea(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3)
	{
		return ((p1 - p2).cross(p3 - p2)).norm() * 0.5f;
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

	Vector3d ReflectRand(const Vector3d& normal, const Vector3d& inverse, const float rand_num)
	{
		//Note that rand_num needs to be between 
		// inverse as in the inverse vector that hits the base of the normal vector
		return 2.0f * (normal * inverse.dot(normal) * rand_num) - inverse;
	}

	Vector3d RandomDir(const Vector3d& normal)
	{
		//Note that rand_num needs to be between 
		// inverse as in the inverse vector that hits the base of the normal vector

		double tetha = CustomRandom::GetInstance().GenerateAngle(360.0f);
		double phi = CustomRandom::GetInstance().GenerateAngle(360.0f);

		Vector3d rand_vector(
			std::sin(tetha) * std::cos(phi),
			std::sin(tetha) * std::sin(phi),
			std::cos(tetha)
		);

		return (normal.dot(rand_vector) < 0 ? -rand_vector : rand_vector);
	}
}
