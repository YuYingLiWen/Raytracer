#include "external/json.hpp" //#include "../external/json.hpp"

#include "AreaLight.h"
#include "PointLight.h"
#include "Sphere.h"
#include "Rectangle.h"
#include "Output.h"
#include "Scene.h"

using namespace Eigen;

nlohmann::basic_json<>::value_type JSONGetValue(nlohmann::json& j, std::string value)
{
    try {
        return j.at(value);
    }
    catch (nlohmann::json::out_of_range& err) {
        std::cout << "WARNING: " << err.what() << std::endl;
        return nullptr;
    }
}

void JSONReadGeometries(std::vector<Geometry*>& scene_geo, nlohmann::json& geometries)
{
    for (auto& item : geometries.items())
    {
        auto& value = item.value();
        std::string type = (std::string)value.at("type");
        std::string name = (JSONGetValue(value, "comment") != nullptr) ? JSONGetValue(value, "comment") : "";

        auto& val_ac = value.at("ac");
        auto& val_dc = value.at("dc");
        auto& val_sc = value.at("sc");

        Color ac(val_ac.at(0), val_ac.at(1), val_ac.at(2));
        Color dc(val_dc.at(0), val_dc.at(1), val_dc.at(2));
        Color sc(val_sc.at(0), val_sc.at(1), val_sc.at(2));

        float ka = (float)value.at("ka");
        float kd = (float)value.at("kd");
        float ks = (float)value.at("ks");
        float pc = (float)value.at("pc");

        if (type.compare("rectangle") == 0)
        {
            Vector3d points[4];

            for (int i = 0; i < 4; i++) // read the 4 points
            {
                auto& val_p = value.at("p" + std::to_string(i + 1));

                auto x = (double)val_p.at(0);
                auto y = (double)val_p.at(1);
                auto z = (double)val_p.at(2);

                points[i] = Vector3d(x, y, z);
            }
            Rectangle* rect = new Rectangle(type, name, ka, kd, ks, pc, ac, dc, sc, points[0], points[1], points[2], points[3]);

            scene_geo.push_back((Geometry*)rect);
        }
        else if (type.compare("sphere") == 0)
        {
            auto& val_p = value.at("centre");
            auto radius = (double)value.at("radius");


            Vector3d center((double)val_p.at(0), (double)val_p.at(1), (double)val_p.at(2));

            Sphere* sphere = new Sphere(type, name, ka, kd, ks, pc, ac, dc, sc, center, radius);
            scene_geo.push_back((Geometry*)sphere);
        }
        else
        {
            std::cout << "WARNING: Unkown geometry \'" << type << '\'' << std::endl;
        }
    }
}

void JSONReadLights(std::vector<Light*>& scene_lights, nlohmann::json& lights)
{
    for (auto& item : lights.items())
    {
        auto& value = item.value();
        std::string type = (std::string)value.at("type");
        auto& val_id = value.at("id");
        auto& val_is = value.at("is");

        Color id(val_id.at(0), val_id.at(1), val_id.at(2));
        Color is(val_is.at(0), val_is.at(1), val_is.at(2));

        bool use = (JSONGetValue(value, "use") != nullptr) ? (bool)JSONGetValue(value, "use") : true;

        if (!use) continue;

        if (type.compare("area") == 0)
        {
            Vector3d points[4];

            for (int i = 0; i < 4; i++) // read the 4 points
            {
                auto& val_p = value.at("p" + std::to_string(i + 1));
                points[i] = Vector3d((double)val_p.at(0), (double)val_p.at(1), (double)val_p.at(2));
            }

            bool use_center = (JSONGetValue(value, "usecenter") != nullptr) ? (bool)JSONGetValue(value, "usecenter") : true;

            AreaLight* area = new AreaLight(type, id, is, points[0], points[1], points[2], points[3], use_center);

            scene_lights.push_back((Light*)area);
        }
        else if (type.compare("point") == 0)
        {
            auto& val_p = value.at("centre");

            Vector3d center((double)val_p.at(0), (double)val_p.at(1), (double)val_p.at(2));

            PointLight* point = new PointLight(type, id, is, center);
            scene_lights.push_back((Light*)point);
        }
        else
        {
            std::cout << "WARNING: Unkown light type \'" << type << '\'' << std::endl;
        }
    }
}

void JSONReadOutput(Output& scene_output, nlohmann::json& output)
{
    nlohmann::json value = output.at(0);

    OutputData data;

    // Mandatory
    auto& val_look = value.at("lookat");
    auto& val_up = value.at("up");
    auto& val_bkc = value.at("bkc");
    auto& val_ai = value.at("ai");
    auto& val_size = value.at("size");
    auto& val_center = value.at("centre");

    data.file_name = (std::string)value.at("filename");
    data.fov = (float)value.at("fov");
    data.ai = Color(val_ai.at(0), val_ai.at(1), val_ai.at(2));
    data.bkc = Color(val_bkc.at(0), val_bkc.at(1), val_bkc.at(2));
    data.size = Vector2i(val_size.at(0), val_size.at(1));
    data.up = Vector3d(val_up.at(0), val_up.at(1), val_up.at(2));
    data.look_at = Vector3d(val_look.at(0), val_look.at(1), val_look.at(2));
    data.center = Vector3d(val_center.at(0), val_center.at(1), val_center.at(2));

    if (JSONGetValue(value, "globalillum") != nullptr) data.global_illum = new bool(JSONGetValue(value, "globalillum"));
    if (JSONGetValue(value, "probterminate") != nullptr) data.probe_terminate = new double(JSONGetValue(value, "probterminate"));
    if (JSONGetValue(value, "maxbounces") != nullptr) data.max_bounce = new uint8_t(JSONGetValue(value, "maxbounces"));
    if (JSONGetValue(value, "raysperpixel") != nullptr)
    {
        auto val_ray_per_pixel = JSONGetValue(value, "raysperpixel");
        data.rays_per_pixel = new Vector2i(val_ray_per_pixel.at(0), val_ray_per_pixel.at(1));
    }

    scene_output.Set(data);
}
