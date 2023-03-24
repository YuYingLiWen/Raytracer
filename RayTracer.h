
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "external/json.hpp" //#include "../external/json.hpp"

#include "Scene.h"
#include "Ray.h"
#include "Camera.h"
#include "YuMath.h" 

#include <cstdio>
#include <iostream>

#define PRINT(x) std::cout << ">> " << x << std::endl

#if _DEBUG
#define DEBUG_LOG(x) PRINT(x)
#else
#define DEBUG_LOG(x) //x
#endif


extern void JSONReadGeometries(std::vector<Geometry*>& scene_geo, nlohmann::json& geometries);
extern void JSONReadLights(std::vector<Light*>& scene_lights, nlohmann::json& lights);
extern void JSONReadOutput(Output& scene_output, nlohmann::json& output);


static const float RESOLUTION = 1.00f;

using namespace Eigen;
struct Hit;

class RayTracer
{
private:
    nlohmann::json json_file;
    Scene scene;

public:
    RayTracer() = delete;
    RayTracer(nlohmann::json json_file);

    ~RayTracer();

    /// Main function that starts the tracer.
    void run();

private: 
    /// Builds scene from json file
    void BuildScene();

    void SetupCamera();

    /// Starts tracing the scene
    void Trace();
    /// Save current scene data as .ppm file.
    void SaveToPPM();

    // Saves which closest object to ray origin is hit, or nothing is hit.
    bool Raycast(Ray& ray, double max_distance = DBL_MAX);

    // Returns an array of object that ray intersected with.
    std::vector<Hit> RaycastAll(const Ray& ray, double max_distance);

    // Saves the closest hit point on the sphere to ray
    bool IntersectCoor(const Ray& ray, Sphere& sphere, Vector3d& intersect);
    bool IntersectCoor(const Ray& ray, Rectangle& rect, Vector3d& intersect);

    Color CalculatePointLightDiffuse(const Vector3d& center, const Color& diffuse_intensity, const Ray& ray, bool& gl);

    Color GetDiffuseColor(const Ray& ray, bool gl);
    Color GetSpecularColor(const Ray& ray);

    Color GetAmbientColor(const Ray& ray);

    bool IsLightHidden(const Vector3d& light_center, const Ray& ray);

    double BlinnPhong(const Vector3d& normal, const Vector3d& towards_light, const Vector3d& towards_camera);

    void UseMSAA(const Vector3d& px, const Vector3d& py, Color& out_final_ambient, Color& out_final_diffuse);

    Vector3d GetNormal(const Ray& ray);


    void Helper_CalculatePointLightDiffuse(const Vector3d& center, const Color& diffuse_intensity, const Ray& ray, Color& diffuse, unsigned int& hit_count, bool& gl);
};


#endif
