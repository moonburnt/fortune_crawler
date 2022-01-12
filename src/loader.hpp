#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

class AssetLoader {
private:
    std::vector<std::string> map_paths;
    size_t last_map;

public:
    AssetLoader();

    std::unordered_map<std::string, Texture2D> sprites;
    std::unordered_map<std::string, Sound> sounds;
    void load_sprites();
    void load_sounds();
    void load_maps_list();

    // Load map with specific index in map_paths. Doesn't have safety checks for
    // out-of-bounds index value!
    Image load_map(size_t index);
    // Load random map out of map_paths list
    Image load_random_map();

    // Default instance of loader
    static AssetLoader loader;
};
