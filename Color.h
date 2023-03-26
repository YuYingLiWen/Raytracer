#ifndef COLOR_H
#define COLOR_H

#include "EigenIncludes.h"

using namespace Eigen;

struct Color
{
    float r, g, b;

    ///Default this->b lack
    Color() : r(0.0f), g(0.0f), b(0.0f) {}

    Color(Vector3d value)
    {
        this->r = (float)value.x();
        this->g = (float)value.y();
        this->b = (float)value.z();
    }

    Color(float r, float g, float b )
    {
        this->r  = r;
        this->g  = g;
        this->b  = b;
    }

    Color(float val)
    {
        this->r  =val ;
        this->g  =val ;
        this->b  =val ;
    }

    ~Color() {}

    static Color Black() { return Color(); }
    static Color Red() { return Color(1, 0, 0); }
    static Color Green() { return Color(0, 1, 0); }
    static Color Blue() { return Color(0, 0, 1); }
    static Color White() { return Color(1, 1, 1); }
    static Color Magenta() { return Color(1, 0, 1); }

    Color Clamp()
    {
        float r = this->r < 0.0f ? 0.0f : this->r > 1.0f ? 1.0f : this->r;
        float g = this->g < 0.0f ? 0.0f : this->g > 1.0f ? 1.0f : this->g;
        float b = this->b < 0.0f ? 0.0f : this->b > 1.0f ? 1.0f : this->b;

        return Color{r,g,b};
    }

    Color(const Color& other)
    {
        this->r = other.r;
        this->g = other.g;
        this->b = other.b;
    }

    Color& operator=(const Color& other)
    {
        this->r = other.r;
        this->g = other.g;
        this->b = other.b;
        return *this;
    }
    
    bool operator== (const Color& other)
    {
        return 
            (this->r  == other.r) &&
            (this->g  == other.g) &&
            (this->b  == other.b);
    }
    
    Color& operator*= (const Color& other)
    {
        this->r *= other.r;
        this->g *= other.g;
        this->b *= other.b;
        return *this;
    }

    // Handamard product
    Color operator* (const Color& other) const
    {
        return Color(this->r * other.r, this->g * other.g, this->b * other.b);
    }

    Color operator* (const Vector3d& other) const
    {
        return Color(this->r * (float)other.x(), this->g * (float)other.y(), this->b * (float)other.z());
    }

    Color operator*(double val) const
    {
        return Color(this->r * (float)val, this->g * (float)val, this->b * (float)val);
    }

    Color operator/(double val) const
    {
        return Color(this->r  / (float)val, this->g / (float)val, this->b / (float)val);
    }

    Color operator/(Eigen::Vector3d val) const
    {
        return Color(this->r / (float)val.x(), this->g / (float)val.y(), this->b / (float)val.z());
    }

    Color& operator/=(double val)
    {
        this->r /= (float)val;
        this->g /= (float)val;
        this->b /= (float)val;
        return *this;
    }

    Color operator+(const Color& other)
    {
        return Color(this->r + other.r, this->g + other.g, this->b + other.b);
    }

    Color& operator+=(const Color& other)
    {
        this->r += other.r;
        this->g += other.g;
        this->b += other.b;
        return *this; 
    }

    std::string ToString() const
    {
        return "(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")";
    }

    friend std::ostream& operator << (std::ostream& os, const Color& color)
    {
        os << "Color (" << color.r << ", " << color.g << ", " << color.b << ")";
        return os;
    }
};

#endif