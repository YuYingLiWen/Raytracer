
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
    std::string files[] = {"cornell_box"};
    //std::string files[] = { "test_scene1" };
    //std::string files[] = { "test_area_light1" };
    //std::string files[] = { "test_scene1B" };
    //std::string files[] = { "test_scene3B" };

    //std::string files[] = { "test_scene1B","test_scene1"  };

    //std::string files[] = { "cornell_box_empty_pl", "cornell_box" };
    //std::string files[] = {"cornell_box_empty_pl", "cornell_box", "test_area_light1", "test_scene1", "test_scene1B", "test_scene2", "test_scene3", "test_scene3B" };
    //std::string files[] = { "test_area_light1", "test_scene1", "test_scene1B", "test_scene2", "test_scene3", "test_scene3B" };

    //std::string files[] = {"test_scene2"};

    //std::string files[] = {"test_scene3"};

    
    for (std::string& scene_name : files) 
    {
        std::cout << "==== " << scene_name << " ====" << std::endl;

        std::ifstream t("scenes\\" + scene_name + ".json");
        if (!t) {
            std::cout << "File " << scene_name << " does not exist!" << std::endl;
            return -1;
        }

        std::stringstream buffer;
        buffer << t.rdbuf();

        nlohmann::json j = nlohmann::json::parse(buffer.str());
        RayTracer tracer(j);

        std::clock_t time = clock();
        tracer.run();
        PRINT("Elapsed: " << ((float)(clock() - time) / CLOCKS_PER_SEC) << " seconds.");

    }

    std::cin.get();
}
