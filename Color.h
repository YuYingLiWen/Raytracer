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
    Color(Vector3d value)
    {
        _r = (float)value.x() < 0.0f ? 0.0f : (float)value.x() > 1.0f ? 1.0f : (float)value.x();
        _g = (float)value.y() < 0.0f ? 0.0f : (float)value.y() > 1.0f ? 1.0f : (float)value.y();
        _b = (float)value.z() < 0.0f ? 0.0f : (float)value.z() > 1.0f ? 1.0f : (float)value.z();
    }

    // Expect values in 0.0f to 1.0f format
    Color(float r, float g, float b)
    {
        _r = r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r;
        _g = g < 0.0f ? 0.0f : g > 1.0f ? 1.0f : g;
        _b = b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b;
    }

    Color(float val)
    {
        val= val < 0.0f ? 0.0f : val > 1.0f ? 1.0f : val;

        _r =val ;
        _g =val ;
        _b =val ;
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
        return Color(_r * (float)other.x(), _g * (float)other.y(), _b * (float)other.z());
    }

    Color operator*(double val) const
    {
        return Color(_r * (float)val, _g * (float)val, _b * (float)val);
    }

    Color operator/(double val) const
    {
        return Color(_r / (float)val, _g / (float)val, _b / (float)val);
    }

    Color operator/(Eigen::Vector3d val) const
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