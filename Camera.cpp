#include "Camera.h"


Camera* Camera::instance = nullptr;

Camera::Camera() {};

Camera* Camera::GetInstance()
{
	if (instance == nullptr) instance = new Camera();

	return instance;
}

void Camera::SetData(const Output& output, float resolution_factor = 1.0f)
{
	fov = output.fov;
	look_at = output.look_at;
	up = output.up;
	position = output.center;
	right = up.cross(look_at);
	height = (uint16_t)(output.size.y() * resolution_factor);
	width = (uint16_t)(output.size.x() * resolution_factor);
	aspect_ratio = (double)width / (double)height;
	origin_lookat = position + look_at;

	half_image = std::tan(Deg2Rad * fov * 0.5f);
	pixel_center = half_image / height;

	grid_size = output.rays_per_pixel != nullptr ? output.rays_per_pixel->x() : 1;
	sample_size = output.rays_per_pixel != nullptr ? output.rays_per_pixel->y() : 1;

	scaled_pixel = half_image * aspect_ratio;
	ambient_intensity = output.ai;
	max_bounce = output.max_bounce != nullptr ? *output.max_bounce : 0;
	probe_terminate = output.probe_terminate != nullptr ? *output.probe_terminate : 0.0f;

	ppm_buffer = new std::vector<Color>((size_t)width * (size_t)height);
}

Camera::~Camera()
{
	delete ppm_buffer;
	delete instance;
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

uint16_t Camera::GridSize() const { return grid_size; }
uint16_t Camera::SampleSize() const { return sample_size; }
Vector3d Camera::OriginLookAt() const { return origin_lookat; }
double Camera::ScaledPixel() const { return scaled_pixel; }

Color Camera::AmbientIntensity() const { return ambient_intensity; }
double Camera::PixelCenter() const { return pixel_center; }
double Camera::HalfImage() const { return half_image; }
uint8_t Camera::MaxBounce() const { return max_bounce; }
double Camera::ProbeTerminate() const { return probe_terminate; }
