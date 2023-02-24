#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Geometry.h"

#include "Eigen/Core"
#include "Eigen/Dense"

#define RECTANGLE "rectangle"

class Rectangle : public Geometry
{
public:
    Rectangle() {}

    Rectangle(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3, Eigen::Vector3d& p4)
        :p1(p1), p2(p2), p3(p3), p4(p4)
    {
        auto l1 = (p2 - p1).norm();
        auto l2 = (p4 - p1).norm();
        area = (p2 - p1).norm() * (p4 - p1).norm();

        normal = ((p2 - p1).cross(p3 - p1)).normalized();
    }

    Rectangle(std::string& type, std::string& comment, float& ka, float& kd, float& ks, float& pc, Color& ac, Color& dc, Color& sc, Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3, Eigen::Vector3d& p4)
        : Geometry(type, comment, ka, kd, ks, pc, ac, dc, sc), p1(p1), p2(p2), p3(p3), p4(p4) 
    {
        auto l1 = (p2 - p1).norm();
        auto l2 = (p4 - p1).norm();
        area = (p2 - p1).norm() * (p4 - p1).norm();

        normal = ((p2 - p1).cross(p3 - p1)).normalized();
    }

    virtual ~Rectangle() {}

    inline const auto& GetP1() const { return p1; }
    inline const auto& GetP2() const { return p2; }
    inline const auto& GetP3() const { return p3; }
    inline const auto& GetP4() const { return p4; }

    inline double GetArea() const { return area; }
    inline auto& GetNormal() const { return normal; }

    std::string ToString() const override
    {
        return Geometry::ToString();
    }

    friend std::ostream& operator << (std::ostream& os, const Rectangle& rect)
    {
        os << rect.ToString() 
            <<  "P1: (" << rect.GetP1().x() << ", " << rect.GetP1().y() << ", " << rect.GetP1().z() << ")\n"
            << "P2: (" << rect.GetP2().x() << ", " << rect.GetP2().y() << ", " << rect.GetP2().z() << ")\n"
            << "P3: (" << rect.GetP3().x() << ", " << rect.GetP3().y() << ", " << rect.GetP3().z() << ")\n"
            << "P4: (" << rect.GetP4().x() << ", " << rect.GetP4().y() << ", " << rect.GetP4().z() << ")\n";

        return os;
    }
    
private:
    Eigen::Vector3d p1, p2, p3, p4; // CCW with repect to normal
    double area = 0.0f;
    Eigen::Vector3d normal;
};

#endif
