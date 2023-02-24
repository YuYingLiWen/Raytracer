#include "external/json.hpp"

#include "AreaLight.h"
#include "PointLight.h"
#include "Sphere.h"
#include "Rectangle.h"
#include "Output.h"
#include "Scene.h"

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

void JSONReadGeometries(std::vector<Geometry*>* scene_geo, nlohmann::json& geometries)
{
    for (auto& item : geometries.items())
    {
        auto& value = item.value();
        std::string type = (std::string)value.at("type");

        std::string comment = (JSONGetValue(value, "comment") == nullptr) ? "N/A" : (std::string)JSONGetValue(value, "comment");

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

        if (type == "rectangle")
        {
            Eigen::Vector3d points[4];

            for (int i = 0; i < 4; i++) // read the 4 points
            {
                auto& val_p = value.at("p" + std::to_string(i + 1));

                auto x = (double)val_p.at(0);
                auto y = (double)val_p.at(1);
                auto z = (double)val_p.at(2);

                points[i] = Eigen::Vector3d(x, y, z);
            }
            Rectangle* rect = new Rectangle(type, comment, ka, kd, ks, pc, ac, dc, sc, points[0], points[1], points[2], points[3]);

            scene_geo->push_back((Geometry*)rect);
        }
        else if (type == "sphere")
        {
            auto& val_p = value.at("centre");
            auto radius = (double)value.at("radius");


            Eigen::Vector3d center((double)val_p.at(0), (double)val_p.at(1), (double)val_p.at(2));

            Sphere* sphere = new Sphere(type, comment, ka, kd, ks, pc, ac, dc, sc, center, radius);
            scene_geo->push_back((Geometry*)sphere);
        }
        else
        {
            std::cout << "WARNING: Unkown geometry \'" << type << '\'' << std::endl;
        }
    }
}

void JSONReadLights(std::vector<Light*>* scene_lights, nlohmann::json& lights)
{
    for (auto& item : lights.items())
    {
        auto& value = item.value();
        std::string type = (std::string)value.at("type");
        auto& val_id = value.at("id");
        auto& val_is = value.at("is");

        Color id(val_id.at(0), val_id.at(1), val_id.at(2));
        Color is(val_is.at(0), val_is.at(1), val_is.at(2));


        if (type == "area")
        {
            Eigen::Vector3d points[4];

            for (int i = 0; i < 4; i++) // read the 4 points
            {
                auto& val_p = value.at("p" + std::to_string(i + 1));
                points[i] = Eigen::Vector3d((double)val_p.at(0), (double)val_p.at(1), (double)val_p.at(2));
            }

            AreaLight* area = new AreaLight(type, id, is, points[0], points[1], points[2], points[3]);

            scene_lights->push_back((Light*)area);
        }
        else if (type == "point")
        {
            auto& val_p = value.at("centre");

            Eigen::Vector3d center((double)val_p.at(0), (double)val_p.at(1), (double)val_p.at(2));

            PointLight* point = new PointLight(type, id, is, center);
            scene_lights->push_back((Light*)point);
        }
        else
        {
            std::cout << "WARNING: Unkown light type \'" << type << '\'' << std::endl;
        }
    }
}

void JSONReadOutput(Output* scene_output, nlohmann::json& output)
{
    nlohmann::json value = output.at(0);

    // Mandatory
    auto& val_look = value.at("lookat");
    auto& val_up = value.at("up");
    std::string file_name = (std::string)value.at("filename");
    auto& val_bkc = value.at("bkc");
    auto& val_ai = value.at("ai");
    auto& val_size = value.at("size");
    auto& val_center = value.at("centre");
    float fov = (float)value.at("fov");

    Color ai(val_ai.at(0), val_ai.at(1), val_ai.at(2));
    Color bkc(val_bkc.at(0), val_bkc.at(1), val_bkc.at(2));
    Eigen::Vector2i size(val_size.at(0), val_size.at(1));
    Eigen::Vector3d up(val_up.at(0), val_up.at(1), val_up.at(2));
    Eigen::Vector3d lookat(val_look.at(0), val_look.at(1), val_look.at(2));
    Eigen::Vector3d center(val_center.at(0), val_center.at(1), val_center.at(2));

    // Check if have global illum (for the extra data)
    if (JSONGetValue(value, "globalillum") != nullptr)
    {
        //Extra
        OutputExtra* extra = new OutputExtra();

        if (JSONGetValue(value, "globalillum") != nullptr) extra->global_illum = new bool(JSONGetValue(value, "globalillum"));
        if (JSONGetValue(value, "probterminate") != nullptr) extra->probe_terminate = new double(JSONGetValue(value, "probterminate"));
        if (JSONGetValue(value, "maxbounces") != nullptr) extra->max_bounce = new unsigned int(JSONGetValue(value, "maxbounces"));
        if (JSONGetValue(value, "raysperpixel") != nullptr)
        {
            auto val_ray_per_pixel = JSONGetValue(value, "raysperpixel");
            extra->rays_per_pixel = new Eigen::Vector2i(val_ray_per_pixel.at(0), val_ray_per_pixel.at(1));
        }
        scene_output->Set(file_name, size, up, lookat, center, fov, ai, bkc, extra);
    }
    else
    {
        scene_output->Set(file_name, size, up, lookat, center, fov, ai, bkc);
    }
}



//bool TEST_ReadAllFiles()
//{
//    Scene* scene = new Scene();
//
//    //std::string files[] = {"cornell_box_empty_pl"};
//    //std::string files[] = {"cornell_box"};
//    //std::string files[] = { "test_area_light1" };
//
//    std::string files[] = { "cornell_box_empty_pl", "cornell_box", "test_area_light1", "test_scene1", "test_scene1B", "test_scene2", "test_scene3", "test_scene3B" };
//
//    for (std::string& file : files)
//    {
//        std::ifstream t(file + ".json");
//        if (!t) {
//            std::cout << "File " << file << " does not exist!" << std::endl;
//            return false;
//        }
//
//        std::stringstream buffer;
//        buffer << t.rdbuf();
//
//        nlohmann::json j = nlohmann::json::parse(buffer.str());
//
//        std::cout << "\n######### NEW SCENE ###########" << std::endl;
//        std::cout << file << ".json parsed successfuly" << std::endl;
//
//
//        std::cout << "\n\n-----GEOMETRIES-----" << "\n";
//        nlohmann::json geo = j.at("geometry");
//        JSONReadGeometries(scene->GetGeometries(), geo);
//        scene->PrintGeometries();
//
//        std::cout << "\n\n-----LIGHTS-----" << "\n";
//        nlohmann::json light = j.at("light");
//        JSONReadLights(scene->GetLights(), light);
//        scene->PrintLights();
//
//        std::cout << "\n\n-----OUTPUT-----" << "\n";
//        nlohmann::json output = j.at("output");
//        JSONReadOutput(scene->GetOuput(), output);
//
//        scene->PrintOutput();
//
//        delete scene;
//        scene = new Scene();
//    }
//    delete scene;
//
//    return true;
//}