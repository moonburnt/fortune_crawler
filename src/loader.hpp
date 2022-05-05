#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "engine/storage.hpp"

// TODO: maybe rework it to storage-based thingy?
class MapLoader {
private:
    std::vector<std::string> map_paths;
    size_t last_map;

public:
    MapLoader();

    void fetch_maps(std::string path, std::string extension);

    // Load map with specific index in map_paths. Doesn't have safety checks for
    // out-of-bounds index value!
    Image load_map(size_t index);
    // Load random map out of map_paths list
    Image load_random_map();
};

struct SavefileFields {
    std::unordered_map<std::string, int> player_stats;
    std::unordered_map<std::string, int> dungeon_stats;
    std::unordered_map<std::string, int> map_settings;
    std::vector<std::vector<int>> map_layout;
};

class SaveManager {
private:
    std::string path;

public:
    SaveManager(std::string path);

    std::optional<SavefileFields> savefile;

    // Save level's data on disk. Returns success state.
    bool save_level(SavefileFields level_data);
    // Load level's data from disk. Returns success state.
    // Idk where else to put it, will leave there for now.
    bool load();
    // Forget savefile in memory and reset save data on disk.
    void reset();
};
