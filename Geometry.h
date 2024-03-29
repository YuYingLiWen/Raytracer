#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <string>
#include "Color.h"

// Geometry and all its children are data containers
class Geometry
{
public:
    Geometry() {}
    Geometry(std::string& type)
        :type(type)
    {
    }
    
    Geometry(std::string& type, std::string& name,float& ka, float& kd, float& ks, float& pc, Color& ac, Color& dc, Color& sc)
        : type(type), name(name), ka(ka), kd(kd), ks(ks), pc(pc), ac(ac), dc(dc), sc(sc)
    {
    }

    virtual ~Geometry() {}

    inline const auto& GetType() const { return type; }
    inline const auto& GetName() const { return name; }
    inline const auto& GetPhongCoeff() const { return pc; }
    inline const auto& GetAmbientColor() const { return ac; }
    inline const auto& GetDiffuseColor() const { return dc; }
    inline const auto& GetSpecularColor() const { return sc; }
    inline const auto& GetSpecularCoeff() const { return ks; }
    inline const auto& GetDiffuseCoeff() const { return kd; }
    inline const auto& GetAmbientCoeff() const { return ka; }

    virtual std::string ToString() const
    {
        return "\nType: " + GetType() +
               "\nAmbient Coeff: " + std::to_string(ka) +
               "\nDiffuse Coeff: " + std::to_string(kd) +
               "\nSpecular Coeff: " + std::to_string(ks) +
               "\nPhong Coeff: " + std::to_string(pc) +
               "\nAmbient Color " + ac.ToString() +
               "\nDiffuse Color " + dc.ToString() +
               "\nSpecular Color " + sc.ToString() +'\n';
    }

    friend std::ostream& operator <<(std::ostream& os, const Geometry& geo)
    {
        os << geo.ToString();
        return os;
    }

protected:
    std::string type;
    std::string name;
    Color ac; // ambient color,
    Color dc; // diffuse color, 
    Color sc; // specular color
    float ka = 0.0f; // ambient coeff
    float kd = 0.0f; // Diffuse coeff
    float ks = 0.0f; // Specular coeff

    float pc = 0.0f; // phong coefficient
};

#endif
