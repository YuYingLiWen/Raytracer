
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "external/json.hpp"

#include "Scene.h"
#include "Ray.h"

#include <cstdio>
#include <iostream>


#define DEBUG_LOG(x) std::cout << ">> " << x << std::endl

extern void JSONReadGeometries(std::vector<Geometry*>* scene_geo, nlohmann::json& geometries);
extern void JSONReadLights(std::vector<Light*>* scene_lights, nlohmann::json& lights);
extern void JSONReadOutput(Output* scene_output, nlohmann::json& output);


using namespace Eigen;

class RayTracer
{
private:
    nlohmann::json json_file;

    Scene* scene = nullptr;

public:
    RayTracer(nlohmann::json json_file)
        :json_file(json_file)
    {
        DEBUG_LOG("JSON file acquired!");
    }

    ~RayTracer() 
    {
        delete scene;
    }

    /// Main function that starts the tracer.
    void run()
    {
        // Each ray is a thread.
        // Count the number of rays to shoot
        // if count == size then it is done.
        // Save to file or wtv

        BuildScene();
        Trace();
        SaveToPPM();
    }

    /// Builds scene from json file
    void BuildScene()
    {
        DEBUG_LOG("Building scene...");

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
    /// Starts tracing the scene
    void Trace();
    /// Save current scene data as .ppm file.
    void SaveToPPM();
    
    // Saves which closest object to ray origin is hit, or nothing is hit.
    bool Raycast(Ray& ray);

    // Returns an array of object that ray intersected with.
    std::vector<Geometry*> RaycastAll(const Ray& ray);

    bool IsHit(const Ray& ray, const Sphere& sphere);

    bool IsHit(const Ray& ray, const Rectangle& rect);

    // Saves the closest hit point on the sphere to ray
    bool IntersectCoor(const Ray& ray, const Sphere& sphere, Vector3d& intersect);
    bool IntersectCoor(const Ray& ray, const Rectangle& rect, Vector3d& intersect); 

    // Returns integral of intensities of Color for each RGB channel
    Color GetShading(const Vector3d& normal, const Vector3d& hit_coor);

    void GetDiffuseColor(Ray& ray);
};


#endif
