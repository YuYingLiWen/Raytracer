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

	Camera(const Output& output, float resolution_factor = 1.0f) 
	{
		fov = output.fov;
		look_at = output.look_at;
		up = output.up;
		origin = output.center;
		right = up.cross(look_at);
		height = output.size.y() * resolution_factor;
		width = output.size.x() * resolution_factor;
		aspect_ratio = (double)width / (double)height;
		origin_lookat = origin + look_at;

		half_image = std::tan(Deg2Rad * fov * 0.5f);
		pixel_center = half_image / height;

		grid_size = output.rays_per_pixel != nullptr ? output.rays_per_pixel->x() : 1;
		sample_size = output.rays_per_pixel != nullptr ? output.rays_per_pixel->y() : 1;

		ambient_intensity = output.GetAmbientIntensity();
	}

	~Camera()
	{
		delete ppm_buffer;
	}

	Ray MakeRay(Vector3d& destination) const 
	{
		return Ray(origin, destination - origin);
	}

	std::vector<Color>& GetOutputBuffer()
	{
		if (!ppm_buffer) ppm_buffer = new std::vector<Color>((size_t)width * (size_t)height);

		return *ppm_buffer;
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
	
	auto GridSize() const { return grid_size; }
	auto SampleSize() const { return sample_size; }
	auto AmbientIntensity() const { return ambient_intensity; }

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

	uint16_t grid_size;
	uint16_t sample_size;

	Color ambient_intensity;
	std::vector<Color>* ppm_buffer = nullptr;
};

#endif // !CAMERA_H
