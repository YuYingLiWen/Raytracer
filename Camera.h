#ifndef CAMERA_H
#define CAMERA_H

#include "Output.h"
#include "Ray.h"
#include "Eigen/Core"

using namespace Eigen;

#define PI 3.14159265359f
#define Deg2Rad (PI / 180.0f)

class Camera 
{
public:
	Camera() = delete;

	Camera(const Output& output) 
	{
		fov = output.fov;
		look_at = output.look_at;
		up = output.up;
		origin = output.center;
		right = up.cross(look_at);
		height = output.size.y();
		width = output.size.x();
		aspect_ratio = (double)width / (double)height;
		origin_lookat = origin + look_at;

		half_image = std::tan(Deg2Rad * fov * 0.5f);
		pixel_center = half_image / height;
	}

	Ray MakeRay(Vector3d& destination) const 
	{
		return Ray(origin, destination - origin);
	}

	auto Height() const { return height; }
	auto Width() const { return width; }
	auto AspectRatio() const { return aspect_ratio; }
	auto FOV() const { return fov; }
	auto Origin() const { return origin; }
	auto LookAt() const { return look_at; }
	auto Up() const { return up; }
	auto Right() const { return right; }
	auto OriginLookAt() const { return origin_lookat; }
	auto PixelCenter() const { return pixel_center; }
	auto HalfImage() const { return half_image; }

	auto ScaledPixel() const { return half_image * aspect_ratio; }


private:
	double fov;
	uint16_t height;
	uint16_t width;
	double aspect_ratio;
	double pixel_center;
	double half_image; 

	Vector3d origin;
	Vector3d look_at;
	Vector3d up;
	Vector3d right;
	Vector3d origin_lookat;
};

#endif // !CAMERA_H
