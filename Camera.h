#ifndef CAMERA_H
#define CAMERA_H

#include "Output.h"
#include "Ray.h"
#include "Eigen/Core"
#include "YuMath.h"

using namespace Eigen;

class Camera
{
private:
	Camera();
public:

	static Camera* GetInstance();

	void SetData(const Output& output, float resolution_factor);

	Camera(const Camera& other) = delete;

	void operator=(const Camera& other) = delete;

	~Camera();

	Ray MakeRay(Vector3d& destination) const;

	std::vector<Color>& GetOutputBuffer();

	uint16_t Height() const;
	uint16_t Width() const;					
	double AspectRatio() const;			
	double FOV() const;					
	Vector3d Position() const;
	Vector3d LookAt() const;				
	Vector3d Up() const;					
	Vector3d Right() const;					

public:
	uint16_t GridSize() const;		
	uint16_t SampleSize() const;	
	Vector3d OriginLookAt() const;	
	double ScaledPixel() const;	

	Color AmbientIntensity() const;		
	double PixelCenter() const;			
	double HalfImage() const;				
	uint8_t MaxBounce() const;				
	float ProbeTerminate() const;		
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
