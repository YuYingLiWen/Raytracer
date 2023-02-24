#include "RayTracer.h"

#include "YuMath.h"

#include <fstream>
#include <memory>
#include "Random.h"
//struct Hit {
//    std::shared_ptr<Eigen::Vector3d> intersect;
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

bool RayTracer::Raycast(Ray& ray)
{
    auto objs = RaycastAll(ray);

    if (objs.empty()) return false;


    // Find the nearest obj
    for (auto& obj : objs)
    {
        auto intersect = new Eigen::Vector3d();

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

bool RayTracer::IsHit(const Ray& ray, const Sphere& sphere)
{
    double a, b, c;

    Eigen::Vector3d distance = ray.GetOrigin() - sphere.GetCenter();
    a = ray.GetDirection().dot(ray.GetDirection());
    b = 2.0f * ray.GetDirection().dot(distance);
    c = distance.dot(distance) - sphere.GetRadius() * sphere.GetRadius();


    //std::cout<< a << ' ' << b << ' ' << c << std::endl;
    return !(YuMath::Discriminant(a, b, c) < 0);
}

bool RayTracer::IntersectCoor(const Ray& ray, const Sphere& sphere, Eigen::Vector3d& intersect)
{
    double a, b, c;

    Eigen::Vector3d distance = ray.GetOrigin() - sphere.GetCenter();
    a = ray.GetDirection().dot(ray.GetDirection());
    b = 2.0f * ray.GetDirection().dot(distance);
    c = distance.dot(distance) - sphere.GetRadius() * sphere.GetRadius();

    auto t = YuMath::Quadratic(a, b, c);

    if (!t) return false; // Ouputing imaginary numbers

    double b_pos_length = (ray.GetPoint(t->b_pos) - ray.GetOrigin()).norm();
    double b_neg_length = (ray.GetPoint(t->b_neg) - ray.GetOrigin()).norm();

    // Saves the closest sphere hit point.
    if (b_pos_length < b_neg_length) intersect = ray.GetPoint(t->b_pos); // point at b_pos is closer
    else intersect = ray.GetPoint(t->b_neg); // point at b_neg is closer

    return true;
}


/// RECTANGLE

bool RayTracer::IsHit(const Ray& ray, const Rectangle& rect)
{
    if (ray.GetDirection().dot(rect.GetNormal()) == 0) return false;

    auto vn = ray.GetDirection().dot(rect.GetNormal());
    auto an = rect.GetP1().dot(rect.GetNormal());
    auto og = ray.GetOrigin().dot(rect.GetNormal());

    auto t = (an - og) / (vn);
    

    if(t == 0) return false;

    auto point = ray.GetPoint(t);

    double area1 = YuMath::HeronTriangle(point, rect.GetP1(), rect.GetP2());
    double area2 = YuMath::HeronTriangle(point, rect.GetP2(), rect.GetP3());
    double area3 = YuMath::HeronTriangle(point, rect.GetP3(), rect.GetP4());
    double area4 = YuMath::HeronTriangle(point, rect.GetP4(), rect.GetP1());

    auto sum = (area1 + area2 + area3 + area4);
    auto area = rect.GetArea();
    auto area_delta = rect.GetArea() - sum;
    auto is_in = std::abs(area_delta) < 0.001f;

    return (is_in); 
}



bool RayTracer::IntersectCoor(const Ray& ray, const Rectangle& rect, Eigen::Vector3d& intersect)
{
    if (ray.GetDirection().dot(rect.GetNormal()) == 0) return false;

    auto vn = ray.GetDirection().dot(rect.GetNormal());
    auto an = rect.GetP1().dot(rect.GetNormal());
    auto og = ray.GetOrigin().dot(rect.GetNormal());

    auto dir = ray.GetDirection();
    auto t = (an - og) / (vn);

    intersect = ray.GetPoint(t);

    return true;
}

Color RayTracer::GetShading(const Vector3d& normal, const Vector3d& hit_coor)
{
    auto& lights = scene->GetLights();

    Color intensity;

    for (auto light : *lights)
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

        intensity = GetShading(normal, *ray.hit_coor);
    }

    // Rectangle Diffuse
    else if (ray.hit_obj->GetType() == RECTANGLE)
    {
        Rectangle& rect = *(Rectangle*)(ray.hit_obj);

        normal = rect.GetNormal();

        intensity = GetShading(normal, *ray.hit_coor);
    }

    ray.hit_obj->intensity_diffuse = intensity;
}







void RayTracer::Trace()
{
    DEBUG_LOG("Tracing...");

    Random rng;

    // Caching lots to precomputed data before casting rays.
    auto& output = scene->GetOuput();
    auto& buffer = output->GetBuffer();

    size_t buffer_size = buffer.size();

    uint16_t height = output->GetHeight();
    uint16_t width = output->GetWidth();

    const double aspect_ratio = (double)width / (double)height;
    const double pixel = std::tan(Deg2Rad * output->GetFOV() * 0.5f);
    const double x_scaled_pixel = pixel * aspect_ratio;
    const double pixel_center = pixel / height;
    
    const auto origin_look_at = output->GetCenter() + output->GetLookAt();
    const auto& up = output->GetUp();

    const auto right = up.cross(output->GetLookAt());

    Eigen::Vector3d px, py, pixel_shoot_at;

    uint32_t counter = 0;

    uint16_t sample_size =  output->GetRaysPerPixel() ? output->GetRaysPerPixel()->y() : 1;
    uint16_t grid_size = output->GetRaysPerPixel() ? output->GetRaysPerPixel()->x() : 1;

    bool use_AA = !(output->HasGlobalIllumination() || scene->HasAreaLight()); // If scene has GL or AreaL then no AA 
    bool use_specular = !output->HasGlobalIllumination(); // If scene has GL then no specular light

    // For each height, trace its row
    for (uint32_t y = 0; y < height; y++)
    {
        //std::cout << std::endl;
        for (uint32_t x = 0; x < width ; x++)
        {
            Color sample_color;
            Color diffuse_color;

            for (uint16_t sample = 0; sample < sample_size; sample++) // Samples area color around the current pixel
            {
                double rand = use_AA ? rng.Generate(pixel_center) : 0.0f; // For AA

                px = (x_scaled_pixel - (2.0f * x + 1.0f + rand) * pixel_center) * right; //(2.0f * y + 1.0f) == 2k + 1 aka odd number
                py = (pixel - (2.0f * y + 1.0f + rand) * pixel_center) * up; //(2.0f * y + 1.0f) == 2k + 1 aka odd number
                pixel_shoot_at = origin_look_at + px + py;

                Ray ray(output->GetCenter(), pixel_shoot_at - output->GetCenter());

                if (Raycast(ray))
                {
                    if(sample == 0) sample_color = ray.hit_obj->GetAmbientColor(output->GetAmbientIntensity());
                    else sample_color = (sample_color + ray.hit_obj->GetAmbientColor(output->GetAmbientIntensity())) * 0.5f;
                    
                    GetDiffuseColor(ray);

                    if (sample == 0) diffuse_color = ray.hit_obj->GetDiffuseColor();
                    //else diffuse_color = (diffuse_color + ray.hit_obj->GetDiffuseColor());
                }
                else 
                {
                    sample_color = (sample_color + scene->GetOuput()->GetBgColor());
                    break;
                }
            }

            buffer[counter] = sample_color + diffuse_color;

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
