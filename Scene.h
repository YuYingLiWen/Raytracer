#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>
#include <iostream>

#include "Rectangle.h"
#include "Sphere.h"
#include "AreaLight.h"
#include "PointLight.h"
#include "Output.h"

class Scene
{
private: 

    // Change Geometry to shared pointer and the vector as simple pointers

    std::vector<Geometry*>* geometries = new std::vector<Geometry*>();
    std::vector<Light*>* lights = new std::vector<Light*>();
    Output* output = new Output();

public:

    Scene() {}
    ~Scene() 
    {
        for (Geometry* geo : *geometries) delete geo;
        for (Light* light : *lights) delete light;

        delete geometries;
        delete lights;
        delete output;
    } 

    const auto& GetGeometries() const { return geometries; }
    const auto& GetLights() const { return lights; }
    const auto& GetOuput() const { return output; }

    bool HasAreaLight() {
        for (Light* l: *lights)
        {
            if (l->GetType() == AREA_LIGHT) return true;
        }
        return false;
    }

    void PrintGeometries() 
    {
        size_t size = geometries->size();
        for (uint16_t i = 0; i < size; i++)
        {
            if (geometries->at(i)->GetType() == "rectangle")
                std::cout << *((Rectangle*)geometries->at(i)) << std::endl;
            else if (geometries->at(i)->GetType() == "sphere")
                std::cout << *((Sphere*)geometries->at(i)) << std::endl;
        }
    }

    void PrintLights() 
    {
        size_t size = lights->size();
        for (uint16_t i = 0; i < size; i++)
        {
            if (lights->at(i)->GetType() == "area")
                std::cout << *((AreaLight*)lights->at(i)) << std::endl;
            else if (lights->at(i)->GetType() == "point")
                std::cout << *((PointLight*)lights->at(i)) << std::endl;
        }
    }

    void PrintOutput()
    {
        std::cout << *output << std::endl;
    }


};

#endif
