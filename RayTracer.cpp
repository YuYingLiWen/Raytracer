#include "RayTracer.h"

#include <fstream>
#include <memory>
#include "Random.h"
#include "YuMath.h" 
#include <cmath>
//struct Hit {
//    std::shared_ptr<Vector3d> intersect;
//    Geometry* hit_obj;
//
//    Hit& operator=(const Hit& hit)
//    {
//        intersect = hit.intersect;
//        hit_obj = hit.hit_obj;
//
//        return *this;
//    }
//};

using namespace Eigen;

bool RayTracer::Raycast(Ray& ray)
{
    auto objs = RaycastAll(ray);

    if (objs.empty()) return false;


    // Find the nearest obj
    for (auto& obj : objs)
    {
        auto intersect = new Vector3d();

        if (obj->GetType() == RECTANGLE)
        {
            IntersectCoor(ray, *((Rectangle*)obj), *intersect);
        }
        else if (obj->GetType() == SPHERE)
        {
            IntersectCoor(ray, *((Sphere*)obj), *intersect);
        }

        if (ray.IsCloser(*intersect)) {
            ray.SetClosestHit(*intersect, *obj);
        }
        else {
            delete intersect;
        }

    }

    return true;
}

// Shoot a ray in the scene to find all objects that intersects it.
std::vector<Geometry*> RayTracer::RaycastAll(const Ray& ray)
{
    auto geometries = scene->GetGeometries();

    size_t size = geometries->size();

    std::vector<Geometry*> hits;

    for (Geometry* geo : *geometries) 
    {
        bool hit = false;

        if (geo->GetType() == RECTANGLE)
        {
            hit = IsHit(ray, *((Rectangle*)geo));
        }
        else if (geo->GetType() == SPHERE)
        {
            hit = IsHit(ray, *((Sphere*)geo));
        }

        if (hit) hits.push_back(geo);
    }

    return hits;
}

///          SPHERE            ///

bool RayTracer::IsHit(const Ray& ray, Sphere& sphere)
{
    double a, b, c;

    Vector3d distance = ray.GetOrigin() - sphere.GetCenter();
    a = ray.GetDirection().dot(ray.GetDirection());
    b = 2.0f * ray.GetDirection().dot(distance);
    c = distance.dot(distance) - sphere.GetRadius() * sphere.GetRadius();


    //std::cout<< a << ' ' << b << ' ' << c << std::endl;
    return !(Discriminant(a, b, c) < 0);
}

bool RayTracer::IntersectCoor(const Ray& ray, Sphere& sphere, Vector3d& intersect)
{
    double a, b, c;

    Vector3d distance = ray.GetOrigin() - sphere.GetCenter();
    a = ray.GetDirection().dot(ray.GetDirection());
    b = 2.0f * ray.GetDirection().dot(distance);
    c = distance.dot(distance) - sphere.GetRadius() * sphere.GetRadius();

    auto t = Quadratic(a, b, c);

    if (!t) return false; // Ouputing imaginary numbers

    double b_pos_length = (ray.GetPoint(t->b_pos) - ray.GetOrigin()).norm();
    double b_neg_length = (ray.GetPoint(t->b_neg) - ray.GetOrigin()).norm();

    // Saves the closest sphere hit point.
    if (b_pos_length < b_neg_length) intersect = ray.GetPoint(t->b_pos); // point at b_pos is closer
    else intersect = ray.GetPoint(t->b_neg); // point at b_neg is closer

    return true;
}


/// RECTANGLE

bool RayTracer::IsHit(const Ray& ray, Rectangle& rect)
{
    auto vn = ray.GetDirection().dot(rect.GetNormal()); 
    
    if (vn == 0) return false; // Checks if ray is parallele to plane, if parallele return false


    // To get this formula, first find formula for intersection between a plane and a line(ray)
    // then to get your d = -(rect.GetP1().dot(rect.GetNormal())); // derived from "Scalar Form of the Equation of a Plane"
    // finally, simplify it due to the cluster of negative sign and dot product. 
    auto t = (rect.GetP1() - ray.GetOrigin()).dot(rect.GetNormal()) / vn; 
    

    if(t == 0) return false;

    auto hit_point = ray.GetPoint(t);

    long double area1 = HeronTriangle(hit_point, rect.GetP1(), rect.GetP2());
    long double area2 = HeronTriangle(hit_point, rect.GetP2(), rect.GetP3());
    long double area3 = HeronTriangle(hit_point, rect.GetP3(), rect.GetP4());
    long double area4 = HeronTriangle(hit_point, rect.GetP4(), rect.GetP1());

    auto sum = (area1 + area2 + area3 + area4);
    auto area = rect.GetArea();
    auto area_delta = rect.GetArea() - sum;

    auto is_in = std::abs(area_delta) < 0.05f;

    return (is_in); 
}

bool RayTracer::IntersectCoor(const Ray& ray, Rectangle& rect, Vector3d& intersect)
{

    auto vn = ray.GetDirection().dot(rect.GetNormal());

    if (vn == 0) return false; // Checks if ray is parallele to plane, if parallele return false

    //if (vn > 0.0f)
    //{
    //    //rect.RecalculateNormal();
    //    vn = ray.GetDirection().dot(-rect.GetNormal());
    //}

    // To get this formula, first find formula for intersection between a plane and a line(ray)
    // then to get your d = -(rect.GetP1().dot(rect.GetNormal())); // derived from "Scalar Form of the Equation of a Plane"
    // finally, simplify it due to the cluster of negative sign and dot product. 
    auto t = (rect.GetP1() - ray.GetOrigin()).dot(rect.GetNormal()) / vn;

    intersect = ray.GetPoint(t);

    return true;
}

Color RayTracer::CalculateDiffuse(const Vector3d& normal, const Vector3d& hit_coor)
{
    auto& lights = scene->GetLights();

    Color intensity;

    for (auto& light : *lights)
    {
        if (light->GetType() == POINT_LIGHT)
        {
            PointLight& point = *(PointLight*)light;
            Vector3d hit_dir = point.GetCenter() - hit_coor;

            Ray ray(hit_coor, hit_dir);

            double cos_angle = (hit_dir).dot(normal);

            if (cos_angle < 0.0f) cos_angle = 0.0f;

            intensity += (light->GetDiffuseIntensity() / hit_dir.norm()) * cos_angle;
        }
        else if (light->GetType() == AREA_LIGHT)
        {
            AreaLight& area = *(AreaLight*)light;

            if (area.GetUseCenter()) 
            {
                Vector3d hit_dir = area.GetCenter() - hit_coor;

                double cos_angle = (hit_dir).dot(normal);

                if (cos_angle < 0.0f) cos_angle = 0.0f;

                intensity += (light->GetDiffuseIntensity() / hit_dir.norm()) * cos_angle;
            }
            else 
            {
                Vector3d hit_dir1 = area.GetP1() - hit_coor;
                Vector3d hit_dir2 = area.GetP2() - hit_coor;
                Vector3d hit_dir3 = area.GetP3() - hit_coor;
                Vector3d hit_dir4 = area.GetP4() - hit_coor;


                Ray ray1(hit_coor, area.GetP1() - hit_coor);
                Ray ray2(hit_coor, area.GetP2() - hit_coor);
                Ray ray3(hit_coor, area.GetP3() - hit_coor);
                Ray ray4(hit_coor, area.GetP4() - hit_coor);

                if (IsHit(ray1, area.GetRectangle())) {

                }

                /*if (!Raycast(ray1) || !Raycast(ray2) || !Raycast(ray3) || !Raycast(ray4))
                {
                    area
                }*/


                double cos_angle1 = (hit_dir1).dot(normal);
                double cos_angle2 = (hit_dir2).dot(normal);
                double cos_angle3 = (hit_dir3).dot(normal);
                double cos_angle4 = (hit_dir4).dot(normal);


                if (cos_angle1 < 0.0f) cos_angle1 = 0.0f;
                if (cos_angle2 < 0.0f) cos_angle2 = 0.0f;
                if (cos_angle3 < 0.0f) cos_angle3 = 0.0f;
                if (cos_angle4 < 0.0f) cos_angle4 = 0.0f;


                intensity += (light->GetDiffuseIntensity() / hit_dir1.norm()) * cos_angle1;
                //intensity += (light->GetDiffuseIntensity() / hit_dir2.norm()) * cos_angle2;
                //intensity += (light->GetDiffuseIntensity() / hit_dir3.norm()) * cos_angle3;
                //intensity += (light->GetDiffuseIntensity() / hit_dir4.norm()) * cos_angle4;

            }
        }

    }

    return intensity;
}

Color RayTracer::CalculateSpecular(const Vector3d& incoming, const Vector3d& normal, const Ray& ray)
{
    //Keep for ref
    //auto adjacent = normal * incoming.dot(normal);
    //auto opposite = incoming - adjacent;
    //Vector3d reflect =  adjacent - opposite ;
    
    Vector3d reflect = 2.0f * (normal * incoming.dot(normal)) - incoming;

    Color intensity;

    auto& lights = scene->GetLights();
    
    for (auto& light : *lights)
    {
        if (light->GetType() == POINT_LIGHT)
        {
            PointLight& point = *(PointLight*)light;
            Vector3d towards_light = point.GetCenter() - *ray.hit_coor;

            double cos_angle = towards_light.dot(reflect) / (towards_light.norm() * reflect.norm());
            
            if (cos_angle < 0.0f) continue;

            //intensity = std::pow((reflect.dot(incoming)), ray.hit_obj->GetPhongCoeff()) * ray.hit_obj->GetSpecularCoeff();

            double attenuation = 1.0f / std::pow(towards_light.norm(), 2.0f); // Works with test_scene1 but not others, must use a division of some sort

            intensity += (light->GetSpecularIntensity() * std::pow(cos_angle, ray.hit_obj->GetPhongCoeff())); // TODO: The Phong coeff behaves weird compared to reference images.
        }
    }

    return intensity;
}

void RayTracer::GetDiffuseColor(Ray& ray)
{
    Color intensity;
    Vector3d normal;

    //Sphere diffuse
    if (ray.hit_obj->GetType() == SPHERE)
    {
        Sphere& sphere = *(Sphere*)(ray.hit_obj);

        normal = (*ray.hit_coor - sphere.GetCenter()).normalized();

        intensity = CalculateDiffuse(normal, *ray.hit_coor);
    }

    // Rectangle Diffuse
    else if (ray.hit_obj->GetType() == RECTANGLE)
    {
        Rectangle& rect = *(Rectangle*)(ray.hit_obj);

        normal = rect.GetNormal();

        intensity = CalculateDiffuse(normal, *ray.hit_coor);
    }

    ray.hit_obj->intensity_diffuse = intensity;
}

void RayTracer::GetSpecularColor(Ray& ray)
{
    Color intensity;
    Vector3d normal;

    //Sphere diffuse
    if (ray.hit_obj->GetType() == SPHERE)
    {
        Sphere& sphere = *(Sphere*)(ray.hit_obj);

        normal = (*ray.hit_coor - sphere.GetCenter()).normalized();

        intensity = CalculateSpecular(Vector3d(0,0,0) - (*ray.hit_coor), normal, ray);
    }

    // Rectangle Diffuse
    else if (ray.hit_obj->GetType() == RECTANGLE)
    {
        Rectangle& rect = *(Rectangle*)(ray.hit_obj);

        normal = rect.GetNormal();

        intensity = CalculateSpecular(Vector3d(0, 0, 0) - (*ray.hit_coor),normal, ray);
    }

    ray.hit_obj->intensity_specular = intensity;
}

void RayTracer::UseMSAA(Camera& camera, Output& output, Vector3d& px, Vector3d& py, Color& out_final_ambient, Color& out_final_diffuse, Color& out_final_specular)
{
    const double grid_size = output.GetGridSize();
    const double sample_size = output.GetRaySampleSize();

    const double subpixel_center = camera.PixelCenter() / grid_size;
    const double total_samples = grid_size * grid_size * sample_size;

    std::vector<Color> ambient_color_samples;
    std::vector<Color> diffuse_color_samples;
    std::vector<Color> specular_color_samples;


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
                    ambient_color_samples.push_back(ray.hit_obj->GetAmbientColor(output.GetAmbientIntensity()));

                    GetDiffuseColor(ray);

                    diffuse_color_samples.push_back(ray.hit_obj->GetDiffuseColor());
                }
                else
                {
                    ambient_color_samples.push_back(scene->GetOuput()->GetBgColor());
                }
            }
        }
    }

    //Ambient
    double ar = 0.0f, ab = 0.0f, ag = 0.0f;

    for (Color& sample : ambient_color_samples)
    {
        ar += sample.R();
        ab += sample.B();
        ag += sample.G();
    }

    //Diffuse
    double dr = 0.0f, db = 0.0f, dg = 0.0f;

    for (Color& sample : diffuse_color_samples)
    {
        dr += sample.R();
        db += sample.B();
        dg += sample.G();
    }

    //Specular
    double sr = 0.0f, sb = 0.0f, sg = 0.0f;

    for (Color& sample : diffuse_color_samples)
    {
        sr += sample.R();
        sb += sample.B();
        sg += sample.G();
    }

    //Final Colors
    out_final_ambient = Color((float)(ar / total_samples), (float)(ag / total_samples), (float)(ab / total_samples));
    out_final_diffuse = Color((float)(dr / total_samples), (float)(dg / total_samples), (float)(db / total_samples));
    out_final_specular = Color((float)(sr / total_samples), (float)(sg / total_samples), (float)(sb / total_samples));
}

void RayTracer::Trace()
{
    DEBUG_LOG("Tracing...");

    Random rng;

    // Caching lots to precomputed data before casting rays.
    auto& output = scene->GetOuput();

    Camera camera(*output);

    auto& output_buffer = output->GetBuffer();

    size_t buffer_size = output_buffer.size();

    Vector3d px, py;

    uint32_t counter = 0;

    bool use_AA = false;// !(output->HasGlobalIllumination() || scene->HasAreaLight()); // If scene has GL or AreaL then no AA 
    bool use_specular = !output->HasGlobalIllumination(); // If scene has GL then no specular light

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

            if (use_AA)
            {
                UseMSAA(camera, *output, px, py, final_ambient, final_diffuse, final_specular);
            }
            else 
            {

                Vector3d pixel_shoot_at = camera.OriginLookAt() + px + py;
                
                Ray ray = camera.MakeRay(pixel_shoot_at);

                if (Raycast(ray))
                {
                    final_ambient = ray.hit_obj->GetAmbientColor(output->GetAmbientIntensity());

                    GetDiffuseColor(ray);
                    GetSpecularColor(ray);

                    final_diffuse = ray.hit_obj->GetDiffuseColor();
                    final_specular = ray.hit_obj->GetSpecularColor();
                }
                else
                {
                    final_ambient = scene->GetOuput()->GetBgColor();
                }
            }

            output_buffer[counter] = final_ambient + final_diffuse + final_specular;

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

    DEBUG_LOG("Saving output as " + scene->GetOuput()->GetFileName() + ".");

    std::ofstream ofs(".\\outputs\\" + scene->GetOuput()->GetFileName(), std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << std::endl << scene->GetOuput()->GetSize().x() << ' ' << scene->GetOuput()->GetSize().y() << std::endl << "255" << std::endl;

    auto& buffer = scene->GetOuput()->GetBuffer();
    size_t size = buffer.size();

    for (uint32_t i = 0; i < size; i++) {

        ofs << (char)(255.0f * buffer[i].R()) << (char)(255.0f * buffer[i].G()) << (char)(255.0f * buffer[i].B());
    }

    ofs.close();

    DEBUG_LOG("Done saving!");
}
