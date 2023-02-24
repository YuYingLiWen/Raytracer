#ifndef COLOR_H
#define COLOR_H

#include "Eigen/Core"

using namespace Eigen;

struct Color
{
private:
    Vector3f color;
public:
    /// @brief Default black
    Color() : color(0.0f, 0.0f, 0.0f) {}

    // Expect values in 0.0f to 1.0f format
    Color(float r, float g, float b)
    {
        color.x() = r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r;
        color.y() = g < 0.0f ? 0.0f : g > 1.0f ? 1.0f : g;
        color.z() = b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b;
    }

    ~Color() {}

    Color(const Color& other)
    {
        color.x() = other.R();
        color.y() = other.G();
        color.z() = other.B();
    }

    Color& operator=(const Color& other)
    {
        color.x() = other.R();
        color.y() = other.G();
        color.z() = other.B();
        return *this;
    }



    float R() const { return color.x(); }
    float G() const { return color.y(); }
    float B() const { return color.z(); }
    
    // Handamard product
    Color operator* (const Color& other) const
    {
        return Color(color.x() * other.R(), color.y() * other.G(), color.z() * other.B());
    }

    Color operator*(double val)
    {
        return Color(color.x() * (float)val, color.y() * (float)val, color.z() * (float)val);
    }

    Color operator/(double val)
    {
        return Color(color.x() / (float)val, color.y() / (float)val, color.z() / (float)val);
    }

    Color& operator/=(double val)
    {
        color.x() /= (float)val;
        color.y() /= (float)val;
        color.z() /= (float)val;
        return *this;
    }

    Color operator+(const Color& other)
    {
        return Color(color.x() + other.R(), color.y() + other.G(), color.z() + other.B());
    }

    Color& operator+=(const Color& other)
    {
        color.x() += other.R();
        color.y() += other.G();
        color.z() += other.B();
        return *this; 
    }


    std::string ToString() const
    {
        return "(" + std::to_string(color.x()) + ", " + std::to_string(color.y()) + ", " + std::to_string(color.z()) + ")";
    }

    friend std::ostream& operator << (std::ostream& os, const Color& color)
    {
        os << "Color (" << color.R() << ", " << color.G() << ", " << color.B() << ")";
        return os;
    }
};

#endif