#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <vector>

#include "Eigen/Core"
#include "Eigen/Dense"

#include "Color.h"

struct OutputData
{
    std::string file_name;
    Eigen::Vector2i size; // Image Resolution
    Eigen::Vector3d up; // up vector - may not be normalized
    Eigen::Vector3d look_at; // look at vector - may not be normalized
    Eigen::Vector3d center;
    Color ai; // ambient intensity
    Color bkc; // background color 
    float fov = 0.0f;

    bool* global_illum = nullptr;
    Eigen::Vector2i* rays_per_pixel = nullptr;
    uint8_t* max_bounce = nullptr;
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
        delete global_illum;
        delete rays_per_pixel;
        delete max_bounce;
        delete probe_terminate;
    };
    
    void Set(const OutputData& data)
    {
        file_name = data.file_name;
        up = data.up;
        look_at = data.look_at;
        ai = data.ai;
        bkc = data.bkc;
        size = data.size;
        center = data.center;
        fov = data.fov;

        global_illum = data.global_illum;
        rays_per_pixel = data.rays_per_pixel;
        max_bounce = data.max_bounce;
        probe_terminate = data.probe_terminate;
    }

    inline const auto& GetFileName() const { return file_name; }
    inline const auto& GetSize() const { return size; }


    inline const auto& GetAmbientIntensity() const { return ai; }
    inline const auto& GetBgColor() const { return bkc; }

    inline bool HasGlobalIllumination() const
    { 
        if (global_illum) return *global_illum;
        
        return false;
    }
    
    inline auto GetGlobalIllum() { return global_illum; }
    inline auto GetMaxBounce() { return max_bounce; }
    inline auto GetProbeTerminate() { return probe_terminate; } //?? wats this thing??

    // Might need modification due to info from assignment docs
    inline auto GetRaysPerPixel() { return rays_per_pixel; }
    inline auto GetMaxRayBounce() { return max_bounce != nullptr ? max_bounce : 0; }

    inline uint16_t GetRaySampleSize() { return rays_per_pixel != nullptr ? rays_per_pixel->x(): 1; }
    inline uint16_t GetGridSize() { return rays_per_pixel != nullptr ? rays_per_pixel->y() : 1; }

    friend std::ostream& operator << (std::ostream& os, const Output& out)
    {
        os << "File name: " << out.GetFileName() << '\n'
            << "Size: [" << out.GetSize().x() << "," << out.GetSize().y() << "]\n"
            << "Up: [" << out.up.x() << "," << out.up.y() << "," << out.up.z() << "]\n"
            << "Look At: [" << out.look_at.x() << "," << out.look_at.y() << "," << out.look_at.z() << "]\n"
            << "Center: [" << out.center.x() << "," << out.center.y() << "," << out.center.z() << "]\n"
            << "FOV: " << out.fov << '\n'
            << "Ambient Intensity: " << out.GetAmbientIntensity() << '\n'
            << "Background Color: " << out.GetBgColor() << '\n'
            << "Global Illumination: " << (((out.global_illum == nullptr) ? "N/A" : (*out.global_illum == 1) ? "True" : "False")) << '\n'
            << "Rays per pixels: (" << ((out.rays_per_pixel == nullptr) ? "N/A" :
                (std::to_string(out.rays_per_pixel->x()) + ", " + std::to_string(out.rays_per_pixel->y()) + ")")) << '\n'
            << "Max bounce: " << ((out.max_bounce == nullptr) ? "N/A" : std::to_string(*out.max_bounce)) << '\n'
            << "Probe Termination: " << ((out.probe_terminate == nullptr) ? "N/A" : std::to_string(*out.probe_terminate)) << '\n';
        return os;
    }

    friend class Camera;

private:
    std::string file_name;
    Eigen::Vector2i size; // Image Resolution
    Eigen::Vector3d up; // up vector - may not be normalized
    Eigen::Vector3d look_at; // look at vector - may not be normalized
    Eigen::Vector3d center;
    Color ai; // ambient intensity
    Color bkc; // background color 
    float fov = 0.0f;

    bool* global_illum = nullptr;
    Eigen::Vector2i* rays_per_pixel = nullptr;
    uint8_t* max_bounce = nullptr;
    double* probe_terminate = nullptr; //?? wats this thing??
    bool contains_area_light = false;
};

#endif
