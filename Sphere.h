#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

#include "EigenIncludes.h"

#define SPHERE "sphere"

class Sphere : public Geometry
{

public:
    Sphere(std::string& type, std::string name, float& ka, float& kd, float& ks, float& pc, Color& ac, Color& dc, Color& sc, Eigen::Vector3d& center, double& radius)
        : Geometry(type, name, ka, kd, ks, pc, ac, dc, sc), center(center), radius(radius)
    {
    }
    ~Sphere() {};

    inline const auto GetCenter() const { return center; }
    inline const auto GetRadius() const { return radius; }

    Eigen::Vector3d GetNormal(const Eigen::Vector3d& hit_location) const { return hit_location - center; }

    std::string ToString() const override
    {
        return Geometry::ToString();
    }

    friend std::ostream& operator << (std::ostream& os, const Sphere& s)
    {
        os << s.ToString()
            << "Center: (" << s.GetCenter().x() << ", " << s.GetCenter().y() << ", " << s.GetCenter().z() << ")\n"
            << "Radius: " << s.GetRadius() << '\n';

        return os;
    }
private:
    Eigen::Vector3d center;
    double radius;
};

#endif
