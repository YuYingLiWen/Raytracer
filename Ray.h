#ifndef RAY_H
#define RAY_H

#include "Eigen/Core"
#include "Eigen/Dense"

#include "Geometry.h"

using namespace Eigen;
class Ray
{
public:

    Ray() = delete;
    Ray(Vector3d origin, Vector3d direction)
        :origin(origin), direction(direction)
    {
    }

    ~Ray() {

    };


    Vector3d GetOrigin() const { return origin; }
    
    Vector3d GetDirection() const {
        return direction; 
    }

    // If hit nothing distance == INIFINITY
    double GetHitDistance() const 
    { 
        if (!hit_coor) return INFINITY;

        return (*hit_coor - origin).norm(); 
    }

    // From origin of this ray to a point
    double GetDistance(const std::shared_ptr<Vector3d> point) const 
    { 
        if (point == nullptr) return DBL_MAX;
        return (*point - origin).norm(); 
    }

    
    Vector3d GetPoint(double& distance) const { return distance * direction + origin; }

    bool IsCloser(std::shared_ptr<Vector3d> point)
    {
        if (!hit_coor) return true;
        return (GetDistance(point) < (*hit_coor - origin).norm());
    }

    void SetClosestHit(std::shared_ptr<Vector3d> point, Geometry& hit_obj)
    {
        if (!hit_coor) hit_coor = point;
        else hit_coor = point;

        this->hit_obj = &hit_obj;
    }

private:
    Vector3d origin;
    Vector3d direction;

public:
    Geometry* hit_obj = nullptr;
    std::shared_ptr<Vector3d> hit_coor = nullptr;
    
public:
    Color diffuse;
    Color specular;
    Color ambient;
};


#endif
