#ifndef COLOR_H
#define COLOR_H

#include "Eigen/Core"

using namespace Eigen;

struct Color
{
private:
    float _r, _g, _b;
public:
    /// @brief Default black
    Color() : _r(0.0f), _g(0.0f), _b(0.0f) {}

    // Expect values in 0.0f to 1.0f format
    Color(float r, float g, float b)
    {
        _r = r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r;
        _g = g < 0.0f ? 0.0f : g > 1.0f ? 1.0f : g;
        _b = b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b;
    }

    ~Color() {}

    Color(const Color& other)
    {
        _r = other.R();
        _g = other.G();
        _b = other.B();
    }

    Color& operator=(const Color& other)
    {
        _r = other.R();
        _g = other.G();
        _b = other.B();
        return *this;
    }



    float R() const { return _r; }
    float G() const { return _g; }
    float B() const { return _b; }
    
    
    Color& operator*= (const Color& other)
    {
        _r *= other.R();
        _g *= other.G();
        _b *= other.B();
        return *this;
    }

    // Handamard product
    Color operator* (const Color& other) const
    {
        return Color(_r * other.R(), _g * other.G(), _b * other.B());
    }

    Color operator* (const Vector3d& other) const
    {
        return Color(_r * other.x(), _g * other.y(), _b * other.z());
    }

    Color operator*(double val)
    {
        return Color(_r * (float)val, _g * (float)val, _b * (float)val);
    }

    Color operator/(double val)
    {
        return Color(_r / (float)val, _g / (float)val, _b / (float)val);
    }

    Color operator/(Eigen::Vector3d val)
    {
        return Color(_r / (float)val.x(), _g / (float)val.y(), _b / (float)val.z());
    }

    Color& operator/=(double val)
    {
        _r /= (float)val;
        _g /= (float)val;
        _b /= (float)val;
        return *this;
    }

    Color operator+(const Color& other)
    {
        return Color(_r + other.R(), _g + other.G(), _b + other.B());
    }

    Color& operator+=(const Color& other)
    {
        _r += other.R();
        _g += other.G();
        _b += other.B();
        return *this; 
    }


    std::string ToString() const
    {
        return "(" + std::to_string(_r) + ", " + std::to_string(_g) + ", " + std::to_string(_b) + ")";
    }

    friend std::ostream& operator << (std::ostream& os, const Color& color)
    {
        os << "Color (" << color.R() << ", " << color.G() << ", " << color.B() << ")";
        return os;
    }
};

#endif