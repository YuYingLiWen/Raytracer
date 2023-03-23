#ifndef SCENE_H
#define SCENE_H

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

    std::vector<Geometry*> geometries;
    std::vector<Light*> lights;
    Output output;

public:

    Scene() {}
    ~Scene() {}

    auto& GetGeometries() { return geometries; }
    auto& GetLights() { return lights; }
    auto& GetOuput() { return output; }

    bool HasAreaLight() {
        for (Light* l: lights)
        {
            if (l->GetType().compare(AREA_LIGHT) == 0) return true;
        }
        return false;
    }

    //void PrintGeometries() 
    //{
    //    size_t size = geometries->size();
    //    for (uint16_t i = 0; i < size; i++)
    //    {
    //        if (geometries->at(i)->GetType() == "rectangle")
    //            std::cout << *((Rectangle*)geometries->at(i)) << std::endl;
    //        else if (geometries->at(i)->GetType() == "sphere")
    //            std::cout << *((Sphere*)geometries->at(i)) << std::endl;
    //    }
    //}

    //void PrintLights() 
    //{
    //    size_t size = lights->size();
    //    for (uint16_t i = 0; i < size; i++)
    //    {
    //        if (lights->at(i)->GetType() == "area")
    //            std::cout << *((AreaLight*)lights->at(i)) << std::endl;
    //        else if (lights->at(i)->GetType() == "point")
    //            std::cout << *((PointLight*)lights->at(i)) << std::endl;
    //    }
    //}

    //void PrintOutput()
    //{
    //    std::cout << *output << std::endl;
    //}


};

#endif
