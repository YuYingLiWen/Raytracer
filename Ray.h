#ifndef RAY_H
#define RAY_H


#include "Eigen/Core"
#include "Eigen/Dense"

#include "Geometry.h"
#include <cfloat>

#include <iostream>
using namespace Eigen;
class Ray
{
public:

    Ray() {
    };
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
        if (hit_coor.hasNaN()) return INFINITY;

        return (hit_coor - origin).norm(); 
    }

    // From origin of this ray to a point
    double GetDistance(const Vector3d& point) const 
    { 
        if (point.hasNaN()) return DBL_MAX;
        return (point - origin).norm(); 
    }

    
    Vector3d GetPoint(double& distance) const { return distance * direction + origin; }

    bool IsCloser(const Vector3d& point)
    {
        if (hit_coor.hasNaN()) return true;
        return (GetDistance(point) < (hit_coor - origin).norm());
    }

    void SetClosestHit(const Vector3d& point, Geometry& hit_obj)
    {
        hit_coor = point;
        this->hit_obj = &hit_obj;
    }

    const Vector3d& GetHitCoor() const
    {
        return hit_coor;
    }

private:
    Vector3d hit_coor = Vector3d(NAN,NAN,NAN);

public:
    Geometry* hit_obj = nullptr;

    
public:
    Vector3d origin;
    Vector3d direction;

    Color diffuse;
};


#endif
