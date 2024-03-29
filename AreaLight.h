#ifndef AREA_LIGHT_H
#define AREA_LIGHT_H

#include "Light.h"
#include "Rectangle.h"
#include "YuMath.h"

#define AREA_LIGHT "area"
#include <cmath>



using namespace Eigen;

class AreaLight : public Light
{

public:
    AreaLight() = delete;
    AreaLight(std::string type, Color id, Color is, Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3, Eigen::Vector3d& p4, bool use_center, unsigned int n)
        : Light(type, id, is), rectangle(p1,p2,p3,p4), use_center(use_center)
    {
        if (use_center)
        {
            //diagonals
            //1 & 3 => y = ax + b 
            //2 & 4 => y = dx + c

            Vector3d a = (p1 - p3); // b = 1
            Vector3d d = (p2 - p4); // c = 2

            double x = (p2 - p1).norm() / (a - d).norm();

            center = a * x + p3;
        }
        else // Use the full light
        {
            double lerp1 = 0.0f;
            while (lerp1 < 1.0f)
            {
                lerp1 = YuMath::Clamp(lerp1 + (1.0f / n), 0.0f, 1.0f);

                Vector3d l1 = YuMath::Lerp(p3, p4, lerp1);
                Vector3d l2 = YuMath::Lerp(p2, p1, lerp1);

                double lerp2 = 0.0f;

                while (lerp2 < 1.0f)
                {
                    hits_points.push_back(YuMath::Lerp(l1, l2, lerp2));

                    lerp2 = YuMath::Clamp(lerp2 + (1.0f / n), 0.0f, 1.0f);
                }
            }
        }
    }

    ~AreaLight()
    {
    }

    inline const auto& GetP1() const { return rectangle.GetP1(); }
    inline const auto& GetP2() const { return rectangle.GetP2(); }
    inline const auto& GetP3() const { return rectangle.GetP3(); }
    inline const auto& GetP4() const { return rectangle.GetP4(); }

    inline auto& GetRectangle() { return rectangle; }
    inline bool GetUseCenter() const { return use_center; }
    inline auto& GetCenter() const { return center; }
    inline auto& GetHitPoints() { return hits_points; }



    friend std::ostream& operator << (std::ostream& os, const AreaLight& al)
    {
        os << (Light)al
            << "P1: (" << al.GetP1().x() << ", " << al.GetP1().y() << ", " << al.GetP1().z() << ")\n"
            << "P2: (" << al.GetP2().x() << ", " << al.GetP2().y() << ", " << al.GetP2().z() << ")\n"
            << "P3: (" << al.GetP3().x() << ", " << al.GetP3().y() << ", " << al.GetP3().z() << ")\n"
            << "P4: (" << al.GetP4().x() << ", " << al.GetP4().y() << ", " << al.GetP4().z() << ")\n";
        return os;
    }
private:
    Rectangle rectangle;
    bool use_center = false;
    Vector3d center;
    std::vector<Vector3d> hits_points;
};

#endif
