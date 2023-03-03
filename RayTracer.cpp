#include "RayTracer.h"

#include <fstream>
#include <memory>
#include "Random.h"
#include "YuMath.h" 
#include <cmath>

struct Hit 
{
    std::shared_ptr<Vector3d> point;
    Geometry* obj;

    Hit& operator=(const Hit& hit)
    {
        point = hit.point;
        obj = hit.obj;

        return *this;
    }
};

bool RayTracer::Raycast(Ray& ray, double max_distance = DBL_MAX)
{
    auto hits = RaycastAll(ray, max_distance);

    if (hits->empty()) return false;

    // Find the nearest obj
    for (auto& hit : *hits)
    {
        if (ray.IsCloser(hit.point))
        {
            ray.SetClosestHit(hit.point, *hit.obj);
        }
    }

    return true;
}

// Shoot a ray in the scene to find all objects that intersects it.
std::shared_ptr<std::vector<Hit>> RayTracer::RaycastAll(const Ray& ray, double max_distance = DBL_MAX)
{
    auto geometries = scene->GetGeometries();

    size_t size = geometries->size();

    auto hits = std::make_shared<std::vector<Hit>>();

    for (Geometry* geo : *geometries) 
    {
        bool hit = false;

        auto intersect = std::make_shared<Vector3d>();

        if (geo->GetType() == RECTANGLE)
        {
            hit = IntersectCoor(ray, *((Rectangle*)geo), *intersect);
        }
        else if (geo->GetType() == SPHERE)
        {
            hit = IntersectCoor(ray, *((Sphere*)geo), *intersect);
        }

        if (hit && ray.GetDistance(intersect) < max_distance)
        {
            hits->push_back(Hit{ intersect, geo });
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

    if (Discriminant(a, b, c) < 0) return false; // Imaginary numbers
    
    auto t = Quadratic(a, b, c);

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

    long double area1 = HeronTriangle(hit_point, rect.GetP1(), rect.GetP2());
    long double area2 = HeronTriangle(hit_point, rect.GetP2(), rect.GetP3());
    long double area3 = HeronTriangle(hit_point, rect.GetP3(), rect.GetP4());
    long double area4 = HeronTriangle(hit_point, rect.GetP4(), rect.GetP1());

    auto sum = (area1 + area2 + area3 + area4);
    auto area = rect.GetArea();
    auto area_delta = rect.GetArea() - sum;

    auto is_in = std::abs(area_delta) < 0.05f; // 0.05f is for truncation errors.

    if (!is_in) return false;

    // To get this formula, first find formula for intersection between a plane and a line(ray)
    // then to get your d = -(rect.GetP1().dot(rect.GetNormal())); // derived from "Scalar Form of the Equation of a Plane"
    // finally, simplify it due to the cluster of negative sign and dot product. 

    intersect = hit_point;

    return true;
}

// SPECULAR

void RayTracer::GetSpecularColor(Ray& ray)
{
    //Sphere diffuse
    if (ray.hit_obj->GetType() == SPHERE)
    {
        Sphere& sphere = *(Sphere*)(ray.hit_obj);

        Vector3d normal = (*ray.hit_coor - sphere.GetCenter()).normalized();

        CalculateSpecular(normal, ray);
    }

    // Rectangle Diffuse
    else if (ray.hit_obj->GetType() == RECTANGLE)
    {
        Rectangle& rect = *(Rectangle*)(ray.hit_obj);

        Vector3d normal = rect.GetNormal();

        CalculateSpecular(normal, ray);
    }
}

void RayTracer::CalculateSpecular(const Vector3d& hit_normal, Ray& ray)
{
    //Keep for ref
    //auto adjacent = normal * incoming.dot(normal);
    //auto opposite = incoming - adjacent;
    //Vector3d reflect =  adjacent - opposite ;

    Vector3d towards_camera = Vector3d(0,0,0) - *ray.hit_coor;
    Color specular;


    auto& lights = scene->GetLights();
    
    for (auto& light : *lights)
    {
        if (light->GetType() == POINT_LIGHT)
        {
            PointLight& point = *(PointLight*)light;

            Vector3d towards_light = point.GetCenter() - *ray.hit_coor;

            Vector3d reflect = 2.0f * (hit_normal * towards_light.dot(hit_normal)) - towards_light;

            double cos_angle = towards_camera.dot(reflect) / (towards_camera.norm() * reflect.norm());
            
            if (cos_angle < 0.0f) continue;

            specular += (light->GetSpecularIntensity() * ray.hit_obj->GetSpecularCoeff() * ray.hit_obj->GetSpecularColor() * std::pow(cos_angle, ray.hit_obj->GetPhongCoeff() / 4.0f)); // TODO: The Phong coeff behaves weird compared to reference images.
        }
        else if (light->GetType() == AREA_LIGHT)
        {
            AreaLight& area = *(AreaLight*)light;

            auto& hit_points = area.GetHitPoints();

            for (Vector3d& point : hit_points)
            {
                Vector3d towards_light = point - *ray.hit_coor;

                Vector3d reflect = 2.0f * (hit_normal * towards_light.dot(hit_normal)) - towards_light;

                double cos_angle = towards_camera.dot(reflect) / (towards_camera.norm() * reflect.norm());

                if (cos_angle < 0.0f) continue;

                specular += (light->GetSpecularIntensity() * ray.hit_obj->GetSpecularCoeff() * ray.hit_obj->GetSpecularColor() * std::pow(cos_angle, ray.hit_obj->GetPhongCoeff())); // TODO: The Phong coeff behaves weird compared to reference images.
            }

            specular /= hit_points.size();
        }
    }

    ray.specular = specular;
}

// DIFFUSE

void RayTracer::GetDiffuseColor(Ray& ray)
{
    //Sphere diffuse
    if (ray.hit_obj->GetType() == SPHERE)
    {
        Sphere& sphere = *(Sphere*)(ray.hit_obj);

        Vector3d normal = (*ray.hit_coor - sphere.GetCenter()).normalized();

        CalculateDiffuse(normal, ray);
    }

    // Rectangle Diffuse
    else if (ray.hit_obj->GetType() == RECTANGLE)
    {
        Rectangle& rect = *(Rectangle*)(ray.hit_obj);

        Vector3d normal = rect.GetNormal();

        CalculateDiffuse(normal, ray);
    }
}

void RayTracer::CalculateDiffuse(const Vector3d& hit_normal, Ray& ray)
{
    auto& lights = scene->GetLights();

    Color diffuse;

    for (auto& light : *lights)
    {
        if (light->GetType() == POINT_LIGHT)
        {
            PointLight& point = *(PointLight*)light;

            diffuse += CalculatePointLightDiffuse(hit_normal, point.GetCenter(), light->GetDiffuseIntensity(), ray);
        }
        else if (light->GetType() == AREA_LIGHT)
        {
            AreaLight& area = *(AreaLight*)light;

            if (area.GetUseCenter())
            {
                diffuse += CalculatePointLightDiffuse(hit_normal, area.GetCenter(), light->GetDiffuseIntensity(), ray);
            }
            else
            {
                auto& hit_points = area.GetHitPoints();

                Color color;

                for (Vector3d& point : hit_points)
                {
                    color += CalculatePointLightDiffuse(hit_normal, point, light->GetDiffuseIntensity(), ray);
                }

                diffuse += (color / (double)hit_points.size());
            }
        }
    }

    ray.diffuse = diffuse;
}

Color RayTracer::CalculatePointLightDiffuse(const Vector3d& hit_normal, const Vector3d& light_center, const Color& light_diffuse_intensity, Ray& ray)
{
    Vector3d towards_light = light_center - *ray.hit_coor;
    Ray new_ray(*ray.hit_coor, towards_light);

    Geometry& geo = *ray.hit_obj;

    //Vector3d light_direction = -towards_light;
    //double dot = light_direction.dot(hit_normal);
    //if (dot < 0.0f)  return geo.GetAmbientColor(); // Checks which side is faced towards the light

    double towards_light_distance = towards_light.norm();
    auto hits = RaycastAll(new_ray, towards_light_distance);
    std::vector<Hit> filtered_hits;

    //DEBUG_LOG("New Batch");

    // Filter out objects that are not in between light & hit
    for (Hit& hit: *hits) 
    {
        //DEBUG_LOG(hit.obj->GetName());

        double to_light_dist = (*hit.point - light_center).norm();
        double to_hit_coor_dist = (*hit.point - *ray.hit_coor).norm();

        //DEBUG_LOG("To Light Distance: " << to_light_dist);
        //DEBUG_LOG("To Hit Coor Distance: " << to_hit_coor_dist);

        if (to_light_dist > 0.001f  // Object is embedded in light
            && to_hit_coor_dist > 0.001f // Object is hit coordinate
            && to_light_dist <= towards_light_distance) // object is behind hit coor
        {
            //DEBUG_LOG("Taken: " << hit.obj->GetName());
            filtered_hits.push_back(hit);
        }
    }

    if (filtered_hits.size() > 0) return Color::Black();

    double cos_angle = towards_light.dot(hit_normal) / (towards_light_distance * hit_normal.norm());

    if (cos_angle < 0.0f) cos_angle = 0.0f;

    return geo.GetDiffuseColor() * geo.GetDiffuseCoeff() * light_diffuse_intensity * cos_angle;
}



/// OTHERS

void RayTracer::UseMSAA(Camera& camera, Vector3d& px, Vector3d& py, Color& out_final_ambient, Color& out_final_diffuse, Color& out_final_specular, bool use_specular)
{
    const double grid_size = camera.GridSize();
    const double sample_size = camera.SampleSize();
    const Color ai = camera.AmbientIntensity();

    const double subpixel_center = camera.PixelCenter() / grid_size;
    const double total_samples = grid_size * grid_size * sample_size;

    //Scanline for each row -> column
    for (uint16_t grid_y = 0; grid_y < grid_size; grid_y++)
    {
        for (uint16_t grid_x = 0; grid_x < grid_size; grid_x++) // Samples area color around the current pixel
        {
            Vector3d sub_px = px + (camera.PixelCenter() - (2.0f * grid_x + 1.0f) * subpixel_center) * camera.Right(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number
            Vector3d sub_py = py + (camera.PixelCenter() - (2.0f * grid_y + 1.0f) * subpixel_center) * camera.Up(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number
            Vector3d subpixel_shoot_at = camera.OriginLookAt() + sub_px + sub_py;

            for (uint16_t sample = 0; sample < sample_size; sample++) // Samples area color around the current pixel
            {
                Ray ray = camera.MakeRay(subpixel_shoot_at);

                if (Raycast(ray))
                {
                    GetAmbientColor(ray);
                    out_final_ambient += ray.ambient * ai;


                    GetSpecularColor(ray);
                    out_final_specular += ray.specular;


                    GetDiffuseColor(ray);
                    out_final_diffuse += ray.diffuse;
                }
                else
                {
                    out_final_ambient += scene->GetOuput()->GetBgColor();
                }
            }
        }
    }

    //Final Colors
    out_final_ambient /= total_samples;
    out_final_diffuse /= total_samples;
    out_final_specular /= total_samples;
}

void RayTracer::GetAmbientColor(Ray& ray)
{
    Geometry& geo = *ray.hit_obj;
    ray.ambient = geo.GetAmbientColor() * geo.GetAmbientCoeff();
}

void RayTracer::Trace()
{
    PRINT("Tracing...");

    Random rng;

    // Caching lots to precomputed data before casting rays.
    auto& output = scene->GetOuput();

    auto& output_buffer = camera->GetOutputBuffer();

    size_t buffer_size = output_buffer.size();

    Vector3d px, py;

    uint32_t counter = 0;

    bool use_AA = true;// !(output->HasGlobalIllumination() || scene->HasAreaLight()); // If scene has GL or AreaL then no AA 
    bool use_specular =  !output->HasGlobalIllumination(); // If scene has GL then no specular light

    // For each height, trace its row
    for (uint32_t y = 0; y < camera->Height(); y++)
    {
        //std::cout << std::endl;
        for (uint32_t x = 0; x < camera->Width() ; x++)
        {
            Color final_ambient;
            Color final_diffuse;
            Color final_specular;

            px = (camera->ScaledPixel() - (2.0f * x + 1.0f) * camera->PixelCenter()) * camera->Right(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number
            py = (camera->HalfImage() - (2.0f * y + 1.0f) * camera->PixelCenter()) * camera->Up(); //(2.0f * y + 1.0f) == 2k + 1 aka odd number

            if (use_AA)
            {
                UseMSAA(*camera, px, py, final_ambient, final_diffuse, final_specular, use_specular);

            }
            else 
            {
                Vector3d pixel_shoot_at = camera->OriginLookAt() + px + py;
                
                Ray ray = camera->MakeRay(pixel_shoot_at);

                if (Raycast(ray))
                {
                    GetAmbientColor(ray);
                    final_ambient = ray.ambient * camera->AmbientIntensity();

                    GetDiffuseColor(ray);
                    final_diffuse = ray.diffuse;

                    if (use_specular) 
                    {
                        GetSpecularColor(ray);
                        final_specular = ray.specular;
                    }
                }
                else
                {
                    final_ambient = scene->GetOuput()->GetBgColor();
                }
            }

            output_buffer[counter] = (final_ambient + final_diffuse + final_specular).Clamp();

            counter++;
        }
    }
}

void RayTracer::SaveToPPM()
{
    /*
    # Old Comment
    This code was adapted from here:
    https://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C.2B.2B

    # New Comment
    This code has been adapted once again by me.
    */

    PRINT("Saving output as " + scene->GetOuput()->GetFileName() + ".");

    std::ofstream ofs(".\\outputs\\" + scene->GetOuput()->GetFileName(), std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << std::endl << camera->Width() << ' ' << camera->Height() << std::endl << "255" << std::endl;

    auto& buffer = camera->GetOutputBuffer();
    size_t size = buffer.size();

    for (uint32_t i = 0; i < size; i++) {

        ofs << (char)(255.0f * buffer[i].r) << (char)(255.0f * buffer[i].g) << (char)(255.0f * buffer[i].b);
    }

    ofs.close();

    PRINT("Done saving!");
}
