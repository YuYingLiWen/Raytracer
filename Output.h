#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <vector>

#include "Eigen/Core"
#include "Eigen/Dense"

#include "Color.h"

struct OutputExtra
{
    OutputExtra() 
    {
    }

    OutputExtra(bool global_illum, Eigen::Vector2i rays_per_pixel, unsigned int max_bounce, double probe_terminate)
    {
        this->global_illum = new bool(global_illum);
        this->rays_per_pixel = new Eigen::Vector2i(rays_per_pixel);
        this->max_bounce = new unsigned int(max_bounce);
        this->probe_terminate = new double(probe_terminate);
    }

    ~OutputExtra()
    {
        delete global_illum;
        delete rays_per_pixel;
        delete max_bounce;
        delete probe_terminate;
    }

    friend std::ostream& operator << (std::ostream& os, const OutputExtra& extra)
    {
        os << "Global Illumination: " << (((extra.global_illum == nullptr) ? "N/A" : (*extra.global_illum == 1) ? "True" : "False")) << '\n'
            << "Rays per pixels: (" << ((extra.rays_per_pixel == nullptr) ? "N/A" : 
                (std::to_string(extra.rays_per_pixel->x()) + ", " + std::to_string(extra.rays_per_pixel->y()) + ")")) << '\n'
            << "Max bounce: " << ((extra.max_bounce == nullptr) ? "N/A" : std::to_string(*extra.max_bounce)) << '\n'
            << "Probe Termination: " << ((extra.probe_terminate == nullptr) ? "N/A" : std::to_string(*extra.probe_terminate)) << '\n';

        return os;
    }

    bool* global_illum = nullptr;
    Eigen::Vector2i* rays_per_pixel = nullptr;
    unsigned int* max_bounce = nullptr;
    double* probe_terminate = nullptr; //?? wats this thing??

};


/// The "camera"
class Output
{
public:
    Output() 
    {
    };

    ~Output() 
    { 
        delete extra;
        delete ppm_buffer;
    };
    
    void Set(std::string& file_name, Eigen::Vector2i& size, Eigen::Vector3d& up, Eigen::Vector3d& look_at, Eigen::Vector3d& center, float& fov, Color& ai, Color& bkc)
    {
        this->file_name = file_name;
        this->up = up;
        this->look_at = look_at;
        this->ai = ai;
        this->bkc = bkc;
        this->size = size;
        this->center = center;
        this->fov = fov;
    }

    void Set(std::string& file_name, Eigen::Vector2i& size, Eigen::Vector3d& up, Eigen::Vector3d& look_at, Eigen::Vector3d& center, float& fov, Color& ai, Color& bkc, OutputExtra* output_extra)
    {
        this->file_name = file_name;
        this->up = up;
        this->look_at = look_at;
        this->ai = ai;
        this->bkc = bkc;
        this->size = size;
        this->center = center;
        this->fov = fov;

        this->extra = output_extra;
    }

    inline const auto& GetFileName() const { return file_name; }
    inline const auto& GetCenter() const { return center; }
    inline const auto& GetFOV() const { return fov; }
    inline const auto& GetSize() const { return size; }
    inline const auto& GetHeight() const { return size.y(); }
    inline const auto& GetWidth() const { return size.x(); }
    inline const auto& GetUp() const { return up; }
    inline const auto& GetLookAt() const { return look_at; }
    inline const auto& GetAmbientIntensity() const { return ai; }
    inline const auto& GetBgColor() const { return bkc; }
    inline const OutputExtra* GetExtra() const { return extra; }
    std::vector<Color>& GetBuffer() 
    {
        if (!ppm_buffer) ppm_buffer = new std::vector<Color>((size_t)size.x() * (size_t)size.y());

        return *ppm_buffer; 
    }


    inline void SetFileName(std::string& file_name) { this->file_name = file_name; }
    inline void SetCenter(Eigen::Vector3d& center) { this->center = center; }
    inline void SetFOV(float& fov) { this->fov = fov; }
    inline void SetSize(Eigen::Vector2i& size) { this->size = size; }
    inline void SetUp(Eigen::Vector3d& up) { this->up = up; }
    inline void SetLookAt(Eigen::Vector3d& look_at) { this->look_at = look_at; }
    inline void SetAmbientIntensity(Color& ambient_intensity) { this->ai = ambient_intensity; }
    inline void SetBackgroundColor(Color& bg_color) { this->bkc = bg_color; }
    inline void GetExtra(OutputExtra* extra) { this->extra = extra; }


    friend std::ostream& operator << (std::ostream& os, const Output& out)
    {
        os << "File name: " << out.GetFileName() << '\n'
            << "Size: [" << out.GetSize().x() << "," << out.GetSize().y() << "]\n"
            << "Up: [" << out.GetUp().x() << "," << out.GetUp().y() << "," << out.GetUp().z() << "]\n"
            << "Look At: [" << out.GetLookAt().x() << "," << out.GetLookAt().y() << "," << out.GetLookAt().z() << "]\n"
            << "Center: [" << out.GetCenter().x() << "," << out.GetCenter().y() << "," << out.GetCenter().z() << "]\n"
            << "FOV: " << out.GetFOV() << '\n'
            << "Ambient Intensity: " << out.GetAmbientIntensity() << '\n'
            << "Background Color: " << out.GetBgColor() << '\n';

        if (out.extra)
            os << *out.GetExtra();
        else
            os << "No Extra Data." << '\n';

        return os;
    }

private:
    std::string file_name;
    Eigen::Vector2i size; // Image Resolution
    Eigen::Vector3d up; // up vector - may not be normalized
    Eigen::Vector3d look_at; // look at vector - may not be normalized
    Eigen::Vector3d center;
    Color ai; // ambient intensity
    Color bkc; // background color 
    float fov = 0.0f;

    OutputExtra* extra = nullptr;

    std::vector<Color>* ppm_buffer = nullptr;
};

#endif
