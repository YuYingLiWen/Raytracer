#ifndef AREA_LIGHT_H
#define AREA_LIGHT_H

#include "Light.h"
#include "Rectangle.h"

#define AREA_LIGHT "area"

class AreaLight : public Light
{
private:
    Rectangle rectangle;
public:
    AreaLight() = delete;
    AreaLight(std::string type, Color id, Color is, Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3, Eigen::Vector3d& p4)
        : Light(type, id, is), rectangle(p1,p2,p3,p4)
    {

    }

    ~AreaLight()
    {
    }

    inline const auto& GetP1() const { return rectangle.GetP1(); }
    inline const auto& GetP2() const { return rectangle.GetP2(); }
    inline const auto& GetP3() const { return rectangle.GetP3(); }
    inline const auto& GetP4() const { return rectangle.GetP4(); }

    inline const auto& GetRectangle() const { return rectangle; }


    friend std::ostream& operator << (std::ostream& os, const AreaLight& al)
    {
        os << (Light)al
            << "P1: (" << al.GetP1().x() << ", " << al.GetP1().y() << ", " << al.GetP1().z() << ")\n"
            << "P2: (" << al.GetP2().x() << ", " << al.GetP2().y() << ", " << al.GetP2().z() << ")\n"
            << "P3: (" << al.GetP3().x() << ", " << al.GetP3().y() << ", " << al.GetP3().z() << ")\n"
            << "P4: (" << al.GetP4().x() << ", " << al.GetP4().y() << ", " << al.GetP4().z() << ")\n";
        return os;
    }
};

#endif
