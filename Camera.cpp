#include "Camera.h"



Camera::Camera() {};

Camera& Camera::GetInstance()
{
	static Camera instance;

	return instance;
}

void Camera::SetData(const Output& output, float resolution_factor = 1.0f)
{
	fov = output.fov;
	look_at = output.look_at;
	up = output.up;
	position = output.center;
	right = up.cross(look_at);
	height = (uint16_t)(output.GetHeight() * resolution_factor);
	width = (uint16_t)(output.GetWidth() * resolution_factor);
	aspect_ratio = (double)width / (double)height;
	origin_lookat = position + look_at;

	half_image = std::tan(Deg2Rad * fov * 0.5f);
	pixel_center = half_image / height;

	if (output.GetC() != nullptr)
	{
		grid_height = *output.GetA();
		grid_width = *output.GetB(); 
		sample_size = *output.GetC();
	}
	else 
	{
		if (output.GetB() != nullptr)
		{
			// Square
			grid_height = *output.GetA();
			grid_width = *output.GetA();
			sample_size = *output.GetB();
		}
		else if (output.GetA() != nullptr)
		{

			grid_height = 1;
			grid_width = 1;

			sample_size = *output.GetA();
		}
		else
		{
			grid_height = 1;
			grid_width = 1;
			sample_size = 1;
		}
	}


	scaled_pixel = half_image * aspect_ratio;
	ambient_intensity = output.GetAmbientIntensity();
	max_bounce = output.GetMaxBounce();
	probe_terminate = output.GetProbeTerminate();

	delete ppm_buffer;
	ppm_buffer = new std::vector<Color>((size_t)width * (size_t)height);
}

Camera::~Camera()
{
	delete ppm_buffer;
}

Ray Camera::MakeRay(Vector3d& destination) const
{
	return Ray(position, destination - position);
}

std::vector<Color>& Camera::GetOutputBuffer() { return *ppm_buffer; }


uint16_t Camera::Height() const { return height; }
uint16_t Camera::Width() const { return width; }
double Camera::AspectRatio() const { return aspect_ratio; }
double Camera::FOV() const { return fov; }
Vector3d Camera::Position() const { return position; }
Vector3d Camera::LookAt() const { return look_at; }
Vector3d Camera::Up() const { return up; }
Vector3d Camera::Right() const { return right; }

uint16_t Camera::GridHeight() const { return grid_height; }
uint16_t Camera::GridWidth() const { return grid_width; }

uint16_t Camera::SampleSize() const { return sample_size; }
Vector3d Camera::OriginLookAt() const { return origin_lookat; }
double Camera::ScaledPixel() const { return scaled_pixel; }

Color Camera::AmbientIntensity() const { return ambient_intensity; }
double Camera::PixelCenter() const { return pixel_center; }
double Camera::HalfImage() const { return half_image; }
uint8_t Camera::MaxBounce() const { return max_bounce; }
double Camera::ProbeTerminate() const { return probe_terminate; }
