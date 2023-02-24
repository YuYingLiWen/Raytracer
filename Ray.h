#ifndef RAY_H
#define RAY_H

#include "Eigen/Core"
#include "Eigen/Dense"

#include "Geometry.h"


class Ray
{
public:

    Ray() = delete;
    Ray(Eigen::Vector3d origin, Eigen::Vector3d direction)
        :origin(origin), direction(direction)
    {
    }

    ~Ray() {
        delete hit_coor;
    };


    inline Eigen::Vector3d GetOrigin() const { return origin; }
    
    inline Eigen::Vector3d GetDirection() const {
        return direction; 
    }

    // If hit nothing distance == INIFINITY
    inline double GetHitDistance() const 
    { 
        if (!hit_coor) return INFINITY;

        return (*hit_coor - origin).norm(); 
    }

    // From origin of this ray to a point
    inline double GetDistance(const Eigen::Vector3d& point) const { return (point - origin).norm(); }

    
    inline Eigen::Vector3d GetPoint(double distance) const { return distance * direction + origin; }

    bool IsCloser(Eigen::Vector3d& point)
    {
        if (!hit_coor) return true;
        return (GetDistance(point) < (*hit_coor - origin).norm());
    }

    void SetClosestHit(Eigen::Vector3d& point, Geometry& hit_obj)
    {
        delete hit_coor;

        if (!hit_coor) hit_coor = &point;
        else hit_coor = &point;

        this->hit_obj = &hit_obj;
    }

private:
    Eigen::Vector3d origin;
    Eigen::Vector3d direction;

public:
    Geometry* hit_obj = nullptr;
    Eigen::Vector3d* hit_coor = nullptr;
};


#endif
