#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <string>
#include "Color.h"

// Geometry and all its children are data containers
class Geometry
{
public:
    Geometry() {}
    Geometry(std::string& type, std::string& comment)
        :type(type), comment(comment) 
    {
    }
    
    Geometry(std::string& type, std::string& comment, float& ka, float& kd, float& ks, float& pc, Color& ac, Color& dc, Color& sc)
        : type(type), ka(ka), kd(kd), ks(ks), pc(pc), ac(ac), dc(dc), sc(sc), comment(comment)
    {
    }

    virtual ~Geometry() {}

    inline const auto& GetType() const { return type; }
    inline float GetPhongCoeff() const { return pc; }
    inline auto GetAmbientColor() const { return ac; }
    inline auto GetDiffuseColor() const { return dc; }
    inline auto GetSpecularColor() const { return sc; }
    inline const auto& GetComment() const { return comment; }
    inline Color GetAmbientColor(const Color& ai) { return ai * ac * ka; }
    inline Color GetDiffuseColor() { return dc * kd * intensity_diffuse; }
    inline Color GetSpecularColor() { return sc * ks; }
    //inline Color GetColorMixture(const Color& ai) 
    //{ 
    //    //Color final_ambient{ ai.R() * ac.R() * ka, ai.G() * ac.G() * ka, ai.B() * ac.B() * ka };
    //    Color final_ambient = (ac * ai) * ka;
    //    Color final_diffuse = dc * intensity_diffuse * kd;
    //    return final_ambient + final_diffuse;
    //}

    virtual std::string ToString() const
    {
        return "\nType: " + GetType() +
               "\nComment: " + GetComment() +
               "\nAmbient Coeff: " + std::to_string(ka) +
               "\nDiffuse Coeff: " + std::to_string(kd) +
               "\nSpecular Coeff: " + std::to_string(ks) +
               "\nPhong Coeff: " + std::to_string(GetPhongCoeff()) +
               "\nAmbient Color " + GetAmbientColor().ToString() +
               "\nDiffuse Color " + GetDiffuseColor().ToString() +
               "\nSpecular Color " + GetSpecularColor().ToString() +'\n';
    }

    friend std::ostream& operator <<(std::ostream& os, const Geometry& geo)
    {
        os << geo.ToString();
        return os;
    }

protected:
    std::string type;
    std::string comment;
    Color ac; // ambient color,
    Color dc; // diffuse color, 
    Color sc; // specular color
    float ka = 0.0f; // ambient coeff
    float kd = 0.0f; // Diffuse coeff
    float ks = 0.0f; // Specular coeff

    float pc = 0.0f; // phong coefficient
public:
    Color intensity_diffuse;
};

#endif
