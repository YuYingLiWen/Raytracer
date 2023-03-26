#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <vector>

#include "EigenIncludes.h"

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

    bool global_illum;
    uint8_t max_bounce;
    double probe_terminate; 
    bool antialiasing;

    unsigned int* grid_a = nullptr; // a
    unsigned int* grid_b = nullptr; // b
    unsigned int* grid_c = nullptr; // c
};


class Output
{
public:
    Output() 
    {

    };

    ~Output() 
    { 
        delete grid_a;
        delete grid_b;
        delete grid_c;
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
        anti_aliase = data.antialiasing;
        
        max_bounce = data.max_bounce;
        probe_terminate = data.probe_terminate;

        grid_a = data.grid_a;
        grid_b = data.grid_b;
        grid_c = data.grid_c;
    }

    inline const auto& GetFileName() const { return file_name; }
    inline const auto& GetSize() const { return size; }
    inline const auto& GetWidth() const { return size.x(); }
    inline const auto& GetHeight() const { return size.y(); }



    inline const auto& GetAmbientIntensity() const { return ai; }
    inline const auto& GetBgColor() const { return bkc; }

    inline bool HasGlobalIllumination() const
    { 
        return global_illum;
    }
    
    inline auto GetGlobalIllum() const { return global_illum; }
    inline auto GetMaxBounce() const { return max_bounce; }
    inline auto GetProbeTerminate() const { return probe_terminate; }
    inline auto AntiAliase() const { return anti_aliase; }

    inline auto GetMaxRayBounce() const { return max_bounce; }

    inline unsigned int* GetA() const { return grid_a; }
    inline unsigned int* GetB() const { return grid_b; }
    inline unsigned int* GetC() const { return grid_c; }

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
            << "Global Illumination: " << (out.global_illum == 1 ? "True" : "False") << '\n'
            << "Rays per pixels: (" 
                      << (out.grid_a != nullptr ? std::to_string(*out.grid_a): "N/A" )
                + ", " + (out.grid_b != nullptr ? std::to_string(*out.grid_b): "N/A" )
                + ", " + (out.grid_c != nullptr ? std::to_string(*out.grid_c): "N/A" )
                + ")" << '\n'
            << "Max bounce: " <<  std::to_string(out.max_bounce) << '\n'
            << "Probe Termination: " << std::to_string(out.probe_terminate) << '\n';
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

    bool global_illum{};
    
    unsigned int* grid_a = nullptr;
    unsigned int* grid_b = nullptr;
    unsigned int* grid_c = nullptr;


    unsigned int max_bounce{};
    double probe_terminate{};
    bool contains_area_light = false;
    bool anti_aliase = false;
};

#endif
