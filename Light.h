#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include "Color.h"

class Light
{
protected:
    bool use = true;
    std::string type;
    Color id, is; // Intensity diffuse, itensity specular
public:
    Light()
    {
    }

    Light(std::string type, Color id, Color is, bool use)
        : type(type), id(id), is(is), use(use)
    {
    }
    virtual ~Light()
    {
    }

    inline const auto& GetType() const { return type; }
    inline auto GetDiffuseIntensity() const { return id; }
    inline auto GetSpecularIntensity() const { return is; }
    inline auto GetUse() const { return use; }

    friend std::ostream& operator << (std::ostream& os, const Light& l)
    {
        os << "Type: " << l.GetType() << '\n'
            << "Intensity Diffuse: (" << l.id.R() << ", " << l.id.G() << ", " << l.id.B() << ")\n"
            << "Intensity Specular: (" << l.is.R() << ", " << l.is.G() << ", " << l.is.B() << ")\n";

            return os;
    }
};

#endif

