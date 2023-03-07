#ifndef CAMERA_H
#define CAMERA_H

#include "Output.h"
#include "Ray.h"
#include "Eigen/Core"
#include "YuMath.h"

using namespace Eigen;

#define PI 3.14159265359f
#define Deg2Rad (PI / 180.0f)

class Camera
{
private:
	Camera() {};
public:

	static Camera* GetInstance()
	{
		if (instance == nullptr) instance = new Camera();

		return instance;
	}

	void SetData(const Output& output, float resolution_factor = 1.0f)
	{
		fov = output.fov;
		look_at = output.look_at;
		up = output.up;
		position = output.center;
		right = up.cross(look_at);
		height = output.size.y() * resolution_factor;
		width = output.size.x() * resolution_factor;
		height = (uint16_t)(output.size.y() * resolution_factor);
		width = (uint16_t)(output.size.x() * resolution_factor);
		aspect_ratio = (double)width / (double)height;
		origin_lookat = position + look_at;

		half_image = std::tan(Deg2Rad * fov * 0.5f);
		pixel_center = half_image / height;

		grid_size = output.rays_per_pixel != nullptr ? output.rays_per_pixel->x() : 1;
		sample_size = output.rays_per_pixel != nullptr ? output.rays_per_pixel->y() : 1;


		scaled_pixel = half_image * aspect_ratio;
		ambient_intensity = output.GetAmbientIntensity();
		max_bounce = (uint8_t)output.max_bounce;
		ambient_intensity = output.ai;
		max_bounce = output.max_bounce != nullptr ? *output.max_bounce : 0;
		probe_terminate = output.probe_terminate != nullptr ? *output.probe_terminate : 0.0f;

		ppm_buffer = new std::vector<Color>((size_t)width * (size_t)height);
	}

	Camera(const Camera& other) = delete;

	void operator=(const Camera& other) = delete;

	~Camera()
	{
		delete ppm_buffer;
		delete instance;
	}

	Ray MakeRay(Vector3d& destination) const
	{
		return Ray(position, destination - position);
	}

	std::vector<Color>& GetOutputBuffer() { return *ppm_buffer; }

	auto Height() const { return height; }
	auto Width() const { return width; }
	auto AspectRatio() const { return aspect_ratio; }
	auto FOV() const { return fov; }
	auto Position() const { return position; }
	auto LookAt() const { return look_at; }
	auto Up() const { return up; }
	auto Right() const { return right; }

public:
	auto GridSize() const { return grid_size; }
	auto SampleSize() const { return sample_size; }
	auto OriginLookAt() const { return origin_lookat; }
	auto ScaledPixel() const { return scaled_pixel; }

	auto AmbientIntensity() const { return ambient_intensity; }
	auto PixelCenter() const { return pixel_center; }
	auto HalfImage() const { return half_image; }
	auto MaxBounce() const { return max_bounce; }
	auto ProbeTerminate() const { return probe_terminate; }
private:

	static Camera* instance;

	double fov{};
	uint16_t height{};
	uint16_t width{};
	double aspect_ratio{};
	double pixel_center{};
	double half_image{};
	double scaled_pixel{};

	Vector3d position;
	Vector3d look_at;
	Vector3d up;
	Vector3d right;
	Vector3d origin_lookat;

	uint16_t grid_size{};
	uint16_t sample_size{};

	Color ambient_intensity;
	uint8_t max_bounce{};
	float probe_terminate{};

	std::vector<Color>* ppm_buffer = nullptr;
};


#endif // !CAMERA_H
