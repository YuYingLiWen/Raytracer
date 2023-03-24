#include "RayTracer.h"

#include <fstream>

#include <cmath>
#include <cfloat>

struct Hit 
{
    Vector3d point;
    Geometry* obj;

    Hit& operator=(const Hit& hit)
    {
        point = hit.point;
        obj = hit.obj;

        return *this;
    }
};

#pragma region Main Structure

RayTracer::RayTracer(nlohmann::json json_file)
    :json_file(json_file)
{
    PRINT("JSON file acquired!");
}

RayTracer::~RayTracer() {}

void RayTracer::run()
{
    BuildScene();
    SetupCamera();
    Trace();
    SaveToPPM();
}

/// Builds scene from json file
void RayTracer::BuildScene()
{
    PRINT("Building scene...");

    nlohmann::json geo = json_file.at("geometry");
    nlohmann::json light = json_file.at("light");
    nlohmann::json output = json_file.at("output");

    JSONReadGeometries(scene.GetGeometries(), geo);
    JSONReadLights(scene.GetLights(), light);
    JSONReadOutput(scene.GetOuput(), output);

    //#if _DEBUG
    //        scene->PrintGeometries();
    //        scene->PrintLights();
    //        scene->PrintOutput();
    //#endif
}

void RayTracer::SetupCamera()
{
    PRINT("Setting the camera...");

    Camera::GetInstance().SetData(scene.GetOuput(), RESOLUTION);
}

void RayTracer::Trace()
{
    PRINT("Tracing...");

    // Caching lots to precomputed data before casting rays.
    auto& output = scene.GetOuput();

    Camera& camera = Camera::GetInstance();

    auto& output_buffer = camera.GetOutputBuffer();

    size_t buffer_size = output_buffer.size();

    Vector3d px, py;

    uint32_t counter = 0;

    bool use_AA = false; //!(output->HasGlobalIllumination() || scene.HasAreaLight()); // If scene has GL or AreaL then no AA 
    bool use_specular =  !output.HasGlobalIllumination(); // If scene has GL then no specular light

    // For each height, trace its row
    for (uint32_t y = 0; y < camera.Height(); y++)
    {
        //std::cout << std::endl;
        for (uint32_t x = 0; x < camera.Width() ; x++)
        {
            Color final_ambient;
            Color final_diffuse;
            Color final_specular;

            px = (camera.ScaledPixel() - (2.0f * x + 1.0f) * camera.PixelCenter()) * camera.Right(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number
            py = (camera.HalfImage() - (2.0f * y + 1.0f) * camera.PixelCenter()) * camera.Up(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number

            Vector3d pixel_shoot_at = camera.OriginLookAt() + px + py;

            Ray ray = camera.MakeRay(pixel_shoot_at);
            bool hit = Raycast(ray);

            if (use_AA)
            {
                UseMSAA(px, py, final_ambient, final_diffuse);
            }
            else // No AA
            {
                if (hit)
                {
                    final_diffuse = GetDiffuseColor(ray, false);
                    final_ambient = GetAmbientColor(ray);
                }
                else
                {
                    final_ambient = scene.GetOuput().GetBgColor();
                }
            }

            if (hit && use_specular) final_specular = GetSpecularColor(ray);

            output_buffer[counter] = (final_ambient * camera.AmbientIntensity() + final_diffuse + final_specular).Clamp();

            counter++;
        }
    }
}

void RayTracer::SaveToPPM()
{
    Camera& camera = Camera::GetInstance();

    PRINT("Saving output as " + scene.GetOuput().GetFileName() + ".");

    std::ofstream ofs(".\\outputs\\" + scene.GetOuput().GetFileName(), std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << std::endl << camera.Width() << ' ' << camera.Height() << std::endl << "255" << std::endl;

    auto& buffer = camera.GetOutputBuffer();
    size_t size = buffer.size();

    for (uint32_t i = 0; i < size; i++) {

        ofs << (char)(255.0f * buffer[i].r) << (char)(255.0f * buffer[i].g) << (char)(255.0f * buffer[i].b);
    }

    ofs.close();

    PRINT("Done saving!");
}

#pragma endregion

#pragma region Raytracer Core

bool RayTracer::Raycast(Ray& ray, double max_distance)
{
    auto hits = RaycastAll(ray, max_distance);

    if (hits.empty()) return false;

    // Find the nearest obj
    for (auto& hit : hits)
    {
        if (ray.IsCloser(hit.point))
        {
            ray.SetClosestHit(hit.point, *hit.obj);
        }
    }

    return true;
}

// Shoot a ray in the scene to find all objects that intersects it.
std::vector<Hit> RayTracer::RaycastAll(const Ray& ray, double max_distance = DBL_MAX)
{
    auto& geometries = scene.GetGeometries();

    std::vector<Hit> hits;

    for (Geometry* geo : geometries)
    {
        bool hit = false;

        Vector3d intersect;

        if (geo->GetType().compare(RECTANGLE) == 0)
        {
            hit = IntersectCoor(ray, *((Rectangle*)geo), intersect);
        }
        else if (geo->GetType().compare(SPHERE) == 0)
        {
            hit = IntersectCoor(ray, *((Sphere*)geo), intersect);
        }

        if (hit && ray.GetDistance(intersect) < max_distance)
        {
            hits.push_back(Hit{ intersect, geo });
        }
    }

    return hits;
}

///          SPHERE            ///
bool RayTracer::IntersectCoor(const Ray& ray, Sphere& sphere, Vector3d& intersect)
{

    double a, b, c;

    Vector3d distance = ray.GetOrigin() - sphere.GetCenter();
    a = ray.GetDirection().dot(ray.GetDirection());
    b = 2.0f * ray.GetDirection().dot(distance);
    c = distance.dot(distance) - sphere.GetRadius() * sphere.GetRadius();

    double disc = YuMath::Discriminant(a, b, c);
    if (disc < 0) return false; // Imaginary numbers

    auto t = YuMath::Quadratic(a, b, c, disc);

    double b_pos_length = (ray.GetPoint(t->b_pos) - ray.GetOrigin()).norm();
    double b_neg_length = (ray.GetPoint(t->b_neg) - ray.GetOrigin()).norm();

    // Saves the closest sphere hit point.
    if (b_pos_length < b_neg_length) intersect = (ray.GetPoint(t->b_pos)); // point at b_pos is closer
    else intersect = (ray.GetPoint(t->b_neg)); // point at b_neg is closer

    return true;
}


/// RECTANGLE
bool RayTracer::IntersectCoor(const Ray& ray, Rectangle& rect, Vector3d& intersect)
{
    auto vn = ray.GetDirection().dot(rect.GetNormal());

    if (vn == 0) return false; // Checks if ray is parallele to plane, if parallele return false

    auto t = (rect.GetP1() - ray.GetOrigin()).dot(rect.GetNormal()) / vn;

    if (t == 0) return false;

    auto hit_point = ray.GetPoint(t);

    // Area Triangle Implementation
    long double area1 = YuMath::TriangleArea(hit_point, rect.GetP1(), rect.GetP2());
    long double area2 = YuMath::TriangleArea(hit_point, rect.GetP2(), rect.GetP3());
    long double area3 = YuMath::TriangleArea(hit_point, rect.GetP3(), rect.GetP4());
    long double area4 = YuMath::TriangleArea(hit_point, rect.GetP4(), rect.GetP1());

    auto area_delta = rect.GetArea() - area1 - area2 - area3 - area4;

    if (!(std::abs(area_delta) < 0.05f)) return false;// 0.05f is for truncation errors.

    // To get this formula, first find formula for intersection between a plane and a line(ray)
    // then to get your d = -(rect.GetP1().dot(rect.GetNormal())); // derived from "Scalar Form of the Equation of a Plane"
    // finally, simplify it due to the cluster of negative sign and dot product. 

    intersect = hit_point;

    return true;
}

// SPECULAR

Color RayTracer::GetSpecularColor(const Ray& ray)
{
    //Keep for ref
    //auto adjacent = normal * incoming.dot(normal);
    //auto opposite = incoming - adjacent;
    //Vector3d reflect =  adjacent - opposite ;

    Vector3d towards_camera = (Camera::GetInstance().Position() - ray.GetHitCoor()).normalized();
    Color specular;

    Vector3d hit_normal = GetNormal(ray);

    auto& lights = scene.GetLights();

    for (auto& light : lights)
    {
        if (light->GetType().compare(POINT_LIGHT) == 0)
        {
            PointLight& point = *(PointLight*)light;

            Vector3d towards_light = (point.GetCenter() - ray.GetHitCoor()).normalized();

            if (IsLightHidden(point.GetCenter(), ray))
            {
                specular += Color::Black();
                continue;
            }

            //Phong
            //Vector3d reflect = Reflect(hit_normal, towards_light);
            //double cos_angle = towards_camera.dot(reflect) / (towards_camera.norm() * reflect.norm());

            //Blinn-Phong
            double cos_angle = BlinnPhong(hit_normal, towards_light, towards_camera);

            if (cos_angle < 0.0f) continue;

            specular += (light->GetSpecularIntensity() * ray.hit_obj->GetSpecularCoeff() * ray.hit_obj->GetSpecularColor() * std::pow(cos_angle, ray.hit_obj->GetPhongCoeff()));
        }
        else if (light->GetType().compare(AREA_LIGHT) == 0)
        {
            AreaLight& area = *(AreaLight*)light;

            auto& hit_points = area.GetHitPoints();

            Color spec;

            for (Vector3d& point : hit_points)
            {
                Vector3d towards_light = (point - ray.GetHitCoor()).normalized();

                if (IsLightHidden(point, ray))
                {
                    specular += Color::Black();
                    continue;
                }

                //Phong
                //Vector3d reflect = YuMath::Reflect(hit_normal, towards_light);
                //double cos_angle = towards_camera.dot(reflect) / (towards_camera.norm() * reflect.norm());

                double cos_angle = BlinnPhong(hit_normal, towards_light, towards_camera);

                if (cos_angle < 0.0f) continue;

                spec += (light->GetSpecularIntensity() * ray.hit_obj->GetSpecularCoeff() * ray.hit_obj->GetSpecularColor() * std::pow(cos_angle, ray.hit_obj->GetPhongCoeff()));
            }

            specular = spec / (double)hit_points.size();
        }
    }

    return specular;
}

inline double RayTracer::BlinnPhong(const Vector3d& normal, const Vector3d& towards_light, const Vector3d& towards_camera)
{
    return normal.dot((towards_light + towards_camera).normalized());
}

// DIFFUSE

Color RayTracer::GetDiffuseColor(const Ray& ray, bool gl = true)
{
    auto& lights = scene.GetLights();

    Color diffuse;

    for (auto& light : lights)
    {
        if (light->GetType().compare(POINT_LIGHT) == 0)
        {
            PointLight& point = *(PointLight*)light;

            diffuse += CalculatePointLightDiffuse(point.GetCenter(), light->GetDiffuseIntensity(), ray, gl);
        }
        else if (light->GetType().compare(AREA_LIGHT) == 0)
        {
            AreaLight& area = *(AreaLight*)light;

            if (area.GetUseCenter())
            {
                diffuse += CalculatePointLightDiffuse(area.GetCenter(), light->GetDiffuseIntensity(), ray, gl);
            }
            else
            {
                auto& hit_points = area.GetHitPoints();

                Color color;

                for (Vector3d& point : hit_points)
                {
                    color += CalculatePointLightDiffuse(point, light->GetDiffuseIntensity(), ray, gl);
                }

                diffuse += (color / (double)hit_points.size());
            }
        }
    }

    return diffuse;
}

Color RayTracer::CalculatePointLightDiffuse(const Vector3d& light_center, const Color& light_diffuse_intensity, const Ray& ray, bool& gl)
{
    unsigned int hit_count = 0;
    Color diffuse;

    Helper_CalculatePointLightDiffuse(light_center, light_diffuse_intensity, ray, diffuse, hit_count, gl);

    return diffuse / hit_count;
}

void RayTracer::Helper_CalculatePointLightDiffuse(const Vector3d& light_center, const Color& light_diffuse_intensity, const Ray& ray, Color& diffuse, unsigned int& hit_count, bool& gl)
{
    Vector3d hit_normal = GetNormal(ray);

    Vector3d towards_light = (light_center - ray.GetHitCoor()).normalized();

    hit_count++;

    if (!gl // Not using global illum
        || hit_count >= Camera::GetInstance().MaxBounce()
        || CustomRandom::GetInstance().Generate() <= Camera::GetInstance().ProbeTerminate())
    {
        if (IsLightHidden(light_center, ray))
        {
            diffuse += Color::Black();
        }
        else
        {
            double cos_angle = towards_light.dot(hit_normal);// / (towards_light_distance * hit_normal.norm());

            if (cos_angle < 0.0f) cos_angle = 0.0f;

            Geometry* geo = ray.hit_obj;
            diffuse += (geo->GetDiffuseColor() * geo->GetDiffuseCoeff() * light_diffuse_intensity * cos_angle);
        }

        return;
    }

    //// Find next bounce
    Ray next_ray(ray.GetHitCoor(), YuMath::RandomDir(hit_normal)); // Reflect(GetNormal(ray), -ray.direction);

    if (Raycast(next_ray))
        Helper_CalculatePointLightDiffuse(light_center, light_diffuse_intensity, next_ray, diffuse, hit_count, gl);
    else
        return;
}


/// OTHERS

/// Finds the number to intersecting item between a light and a point
bool RayTracer::IsLightHidden(const Vector3d& light_center, const Ray& ray)
{
    Vector3d towards_light = (light_center - ray.GetHitCoor());
    double towards_light_distance = towards_light.norm();
    towards_light.normalize();

    Ray ray_towards_light(ray.GetHitCoor(), towards_light);

    auto hits = RaycastAll(ray_towards_light, towards_light_distance);
    std::vector<Hit> filtered_hits;

    // Filter out objects that are not in between light & hit
    for (Hit& hit : hits)
    {
        double to_light_dist = (hit.point - light_center).norm();
        double to_hit_coor_dist = (hit.point - ray.GetHitCoor()).norm();

        if (to_light_dist > 0.001f  // Object is embedded in light
            && to_hit_coor_dist > 0.001f // Object is hit coordinate
            && to_light_dist <= towards_light_distance) // object is behind hit coor
        {
            filtered_hits.push_back(hit);
        }
    }

    if (filtered_hits.size() > 0) return true;

    return false;
}


void RayTracer::UseMSAA(const Vector3d& px, const Vector3d& py, Color& out_final_ambient, Color& out_final_diffuse)
{
    const double grid_size = Camera::GetInstance().GridSize();
    const double sample_size = Camera::GetInstance().SampleSize();

    const double subpixel_center = Camera::GetInstance().PixelCenter() / grid_size;
    const double total_samples = grid_size * grid_size; // * sample_size;

    //Scanline for each row -> column
    for (uint32_t grid_y = 0; grid_y < grid_size; grid_y++)
    {
        for (uint32_t grid_x = 0; grid_x < grid_size; grid_x++) // Samples area color around the current pixel
        {
            Vector3d sub_px = px + (Camera::GetInstance().PixelCenter() - (2.0f * grid_x + 1.0f) * subpixel_center) * Camera::GetInstance().Right(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number
            Vector3d sub_py = py + (Camera::GetInstance().PixelCenter() - (2.0f * grid_y + 1.0f) * subpixel_center) * Camera::GetInstance().Up(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number
            Vector3d subpixel_shoot_at = Camera::GetInstance().OriginLookAt() + sub_px + sub_py;

            Ray ray = Camera::GetInstance().MakeRay(subpixel_shoot_at);

            if (Raycast(ray))
            {
                for (uint16_t sample = 0; sample < sample_size; sample++)
                {
                    out_final_ambient += GetAmbientColor(ray) * Camera::GetInstance().AmbientIntensity();

                    out_final_diffuse += GetDiffuseColor(ray);
                }
            }
            else
            {
                out_final_ambient += scene.GetOuput().GetBgColor();
            }
        }
    }

    //Final Colors
    out_final_ambient /= total_samples;
    out_final_diffuse /= total_samples;
}

Color RayTracer::GetAmbientColor(const Ray& ray)
{
    Geometry& geo = *ray.hit_obj;
    return geo.GetAmbientColor() * geo.GetAmbientCoeff();
}

Vector3d RayTracer::GetNormal(const Ray& ray)
{
    if (ray.hit_obj->GetType().compare(SPHERE) == 0)  return (ray.GetHitCoor() - (*(Sphere*)(ray.hit_obj)).GetCenter()).normalized();
    else if (ray.hit_obj->GetType().compare(RECTANGLE) == 0) return (*(Rectangle*)(ray.hit_obj)).GetNormal();
    else {
        PRINT("Something went wrong...");
        return Vector3d();
    }
}

#pragma endregion
