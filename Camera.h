#ifndef CAMERA_H
#define CAMERA_H

#include "Output.h"
#include "Ray.h"
#include "EigenIncludes.h"
#include "YuMath.h"

using namespace Eigen;

class Camera
{
private:
	Camera();
public:

	static Camera& GetInstance();

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
	uint16_t GridHeight() const;
	uint16_t GridWidth() const;

	uint16_t SampleSize() const;	
	Vector3d OriginLookAt() const;	
	double ScaledPixel() const;	

	Color AmbientIntensity() const;		
	double PixelCenter() const;			
	double HalfImage() const;				
	uint8_t MaxBounce() const;				
	double ProbeTerminate() const;		
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

	unsigned int grid_height{};
	unsigned int grid_width{};
	unsigned int sample_size{};

	Color ambient_intensity;
	uint8_t max_bounce{};
	double probe_terminate{};

	std::vector<Color>* ppm_buffer = nullptr;
};


#endif // !CAMERA_H
