
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

#include "RayTracer.h"

#include "external/json.hpp"


Scene* LoadScene(std::string&);

int main()
{
    //std::string files[] = {"cornell_box_empty_pl"};
    //std::string files[] = {"cornell_box"};
    //std::string files[] = { "cornell_box_al" };
    //std::string files[] = { "test_scene1" };
    //std::string files[] = { "test_area_light1" };
    //std::string files[] = { "test_scene1B" };
    //std::string files[] = { "test_scene3B" };
    
    std::string files[] = { "cornell_box_ball" };

    //std::string files[] = { "test_scene3B","test_scene3"};

    //std::string files[] = { "cornell_box_empty_pl", "cornell_box" };
    //std::string files[] = { "cornell_box","cornell_box_ball", "cornell_box_empty_pl", "test_area_light1", "test_scene1", "test_scene1B", "test_scene2", "test_scene3", "test_scene3B", "cornell_box_al" };
    //std::string files[] = { "test_area_light1", "test_scene1", "test_scene1B", "test_scene2", "test_scene3", "test_scene3B" };

    //std::string files[] = {"test_scene2"};

    //std::string files[] = {"test_scene3"};

    
    for (std::string& scene_name : files) 
    {
        PRINT("==== " << scene_name << " ====");

        std::ifstream t("scenes\\" + scene_name + ".json");
        if (!t) 
        {
            PRINT("File " << scene_name << " does not exist!");
            return -1;
        }

        std::stringstream buffer;
        buffer << t.rdbuf();

        nlohmann::json j = nlohmann::json::parse(buffer.str());
        RayTracer tracer(j);

        std::clock_t time = clock();
        tracer.run();
        PRINT("Elapsed: " << ((float)(clock() - time) / CLOCKS_PER_SEC) << " seconds OR " << ((float)(clock() - time) / CLOCKS_PER_SEC / 60.0f) << " minutes.");

    }
    PRINT("\n>> END OF TASKS <<");
    std::cin.get();
}
