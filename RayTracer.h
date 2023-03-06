
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "external/json.hpp"

#include "Scene.h"
#include "Ray.h"

#include <cstdio>
#include <iostream>

#define PRINT(x) std::cout << ">> " << x << std::endl

#if _DEBUG
#define DEBUG_LOG(x) PRINT(x)
#else
#define DEBUG_LOG(x) //x
#endif


extern void JSONReadGeometries(std::vector<Geometry*>* scene_geo, nlohmann::json& geometries);
extern void JSONReadLights(std::vector<Light*>* scene_lights, nlohmann::json& lights);
extern void JSONReadOutput(Output* scene_output, nlohmann::json& output);


using namespace Eigen;
struct Hit;

class RayTracer
{
private:
    nlohmann::json json_file;

    Scene* scene = nullptr;
    Camera* camera = nullptr;
public:
    RayTracer(nlohmann::json json_file)
        :json_file(json_file)
    {
        PRINT("JSON file acquired!");

        
    }

    ~RayTracer() 
    {
        delete scene;
        delete camera;
    }

    /// Main function that starts the tracer.
    void run()
    {
        // Each ray is a thread.
        // Count the number of rays to shoot
        // if count == size then it is done.
        // Save to file or wtv

        BuildScene();
        SetupCamera();
        Trace();
        SaveToPPM();
    }

    /// Builds scene from json file
    void BuildScene()
    {
        PRINT("Building scene...");

        scene = new Scene();

        nlohmann::json geo = json_file.at("geometry");
        nlohmann::json light = json_file.at("light");
        nlohmann::json output = json_file.at("output");

        JSONReadGeometries(scene->GetGeometries(), geo);
        JSONReadLights(scene->GetLights(), light);
        JSONReadOutput(scene->GetOuput(), output);

        //#if _DEBUG
        //        scene->PrintGeometries();
        //        scene->PrintLights();
        //        scene->PrintOutput();
        //#endif
    }

    void SetupCamera() 
    { 
        PRINT("Setting the camera...");
        camera = new Camera(*scene->GetOuput(), 1.0f); 
    }

    /// Starts tracing the scene
    void Trace();
    /// Save current scene data as .ppm file.
    void SaveToPPM();
    
    // Saves which closest object to ray origin is hit, or nothing is hit.
    bool Raycast(Ray& ray, double max_distance);

    // Returns an array of object that ray intersected with.
    std::shared_ptr<std::vector<Hit>> RaycastAll(const Ray& ray, double max_distance);

    // Saves the closest hit point on the sphere to ray
    bool IntersectCoor(const Ray& ray, Sphere& sphere, Vector3d& intersect);
    bool IntersectCoor(const Ray& ray, Rectangle& rect, Vector3d& intersect);

    // Returns integral of intensities of Color for each RGB channel
    void CalculateDiffuse(const Vector3d& normal,  Ray& ray);
    void CalculateSpecular(const Vector3d& normal, Ray& ray);

    Color CalculatePointLightDiffuse(const Vector3d& normal, const Vector3d& center, const Color& diffuse_intensity,  Ray& ray);

    void GetDiffuseColor(Ray& ray);
    void GetSpecularColor(Ray& ray);

    void GetAmbientColor(Ray& ray);

    void UseMSAA(Camera& camera, Vector3d& px, Vector3d& py, Color& out_final_ambient, Color& out_final_diffuse, Color& out_final_specular, bool use_specular);
};


#endif
