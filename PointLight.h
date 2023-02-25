#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H


#include "Light.h"
#include <string>

#include "Eigen/Core"
#include "Eigen/Dense"

#define POINT_LIGHT "point"

class PointLight : public Light
{
private:
    Eigen::Vector3d center;
public:
    PointLight() = delete;
    PointLight(std::string type, Color id, Color is, Eigen::Vector3d center, bool use)
        : Light(type, id, is, use), center(center)
    {
    }

    ~PointLight()
    {
    }

    inline const auto& GetCenter() const { return center; }

    friend std::ostream& operator << (std::ostream& os, const PointLight& pl)
    {
        os << (Light)pl
            << "Center: (" << pl.center.x() << ", " << pl.center.y() << ", " << pl.center.z() << ")\n";
        return os;
    }
};

#endif
