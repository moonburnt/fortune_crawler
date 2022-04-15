#include "loader.hpp"
#include "raylib.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <string>
#include <json-c/json.h>

// Map loader
MapLoader::MapLoader() : last_map(-1) {}

void MapLoader::fetch_maps(std::string path, std::string extension) {
    // TODO: automatic verification of map's colors, to throw away invalid ones
    int amount;

    char** dir_files = GetDirectoryFiles(path.c_str(), &amount);

    map_paths = {};

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], extension.c_str())) {
            map_paths.push_back(path + dir_files[current]);
        }
    }

    ClearDirectoryFiles();
    // Overriding last_map counter, to make load_random_map() work correctly.
    last_map = map_paths.size() - 1;
}

Image MapLoader::load_map(size_t index) {
    last_map = index;
    return LoadImage(map_paths[index].c_str());
}

Image MapLoader::load_random_map() {
    // This version of mapgen will ensure that the same map can only appear again
    // if the whole roaster of maps has been completed
    last_map++;
    if (last_map == map_paths.size()) {
        last_map = 0;

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(map_paths.begin(), map_paths.end(), g);
    }

    return load_map(last_map);
}

// Save manager
SaveManager::SaveManager(std::string p) : path(p) {
    // reset(); // DO NOT DO THAT
}

std::optional<int> get_key(const std::string& key_name, json_object* storage) {
    std::optional<int> key_index;

    json_object* key_store;
    json_object_object_get_ex(storage, key_name.c_str(), &key_store);

    if (json_object_get_type(key_store) == json_type_int) {
        key_index = json_object_get_int(key_store);
    }
    return key_index;
}

bool SaveManager::save_level(SavefileFields level_data) {
    bool success_state = true;

    json_object* data = json_object_new_object();
    if (!data) return false;

    json_object* player_stats = json_object_new_object();
    for (auto& kv : level_data.player_stats) {
        json_object_object_add(
            player_stats,
            kv.first.c_str(),
            json_object_new_int(kv.second));
    }

    json_object* dungeon_stats = json_object_new_object();
    for (auto& kv : level_data.dungeon_stats) {
        json_object_object_add(
            dungeon_stats,
            kv.first.c_str(),
            json_object_new_int(kv.second));
    }

    json_object* map_settings = json_object_new_object();
    for (auto& kv : level_data.map_settings) {
        json_object_object_add(
            map_settings,
            kv.first.c_str(),
            json_object_new_int(kv.second));
    }

    json_object* map_layout = json_object_new_array();
    for (auto item : level_data.map_layout) {
        json_object* tile_array = json_object_new_array();
        for (auto elem : item) {
            json_object_array_add(tile_array, json_object_new_int(elem));
        }
        json_object_array_add(map_layout, tile_array);
    }

    json_object_object_add(data, "player_stats", player_stats);
    json_object_object_add(data, "dungeon_stats", dungeon_stats);
    json_object_object_add(data, "map_settings", map_settings);
    json_object_object_add(data, "map_layout", map_layout);

    if (json_object_to_file(path.c_str(), data)) {
        success_state = false;
    }

    json_object_put(data);
    return success_state;
}

bool SaveManager::load() {
    bool success_state = true;

    static const std::vector<std::string> player_stat_fields =
        {"current_hp", "max_hp", "pdmg", "rdmg", "mdmg", "pdef", "rdef", "mdef", "money"};
    static const std::vector<std::string> dungeon_stat_fields =
        {"lvl", "current_turn", "money_collected", "enemies_killed"};

    static const std::vector<std::string> map_settings_fields =
        {"map_x", "map_y", "tile_x", "tile_y"};

    json_object* data = json_object_from_file(path.c_str());
    if (data) {
        SavefileFields save_data;

        json_object* player_stats;
        json_object_object_get_ex(data, "player_stats", &player_stats);

        if (json_object_get_type(player_stats) == json_type_object) {
            for (auto key : player_stat_fields) {
                std::optional<int> value = get_key(key, player_stats);
                if (value) {
                    save_data.player_stats[key] = value.value();
                }
                else {
                    success_state = false;
                    break;
                }
            }
        }
        else success_state = false;

        if (success_state) {
            json_object* dungeon_stats;
            json_object_object_get_ex(data, "dungeon_stats", &dungeon_stats);

            if (json_object_get_type(dungeon_stats) == json_type_object) {
                for (auto key : dungeon_stat_fields) {
                    std::optional<int> value = get_key(key, dungeon_stats);
                    if (value) {
                        save_data.dungeon_stats[key] = value.value();
                    }
                    else {
                        success_state = false;
                        break;
                    }
                }
            }
            else success_state = false;
        }

        if (success_state) {
            json_object* map_settings;
            json_object_object_get_ex(data, "map_settings", &map_settings);

            if (json_object_get_type(map_settings) == json_type_object) {
                for (auto key : map_settings_fields) {
                    std::optional<int> value = get_key(key, map_settings);
                    if (value) {
                        save_data.map_settings[key] = value.value();
                    }
                    else {
                        success_state = false;
                        break;
                    }
                }
            }
            else success_state = false;
        }

        if (success_state) {
            json_object* map_layout;
            json_object_object_get_ex(data, "map_layout", &map_layout);

            save_data.map_layout = {};

            if (json_object_get_type(map_layout) == json_type_array) {
                // TODO: add safety checks to this part
                for (size_t i = 0lu; i < json_object_array_length(map_layout); i++) {
                    std::vector<int> tile_items = {};
                    json_object* tile_content = json_object_array_get_idx(map_layout, i);
                    for (size_t tile_index = 0lu;
                         tile_index < json_object_array_length(tile_content);
                         tile_index++) {
                        tile_items.push_back(json_object_get_int(
                            json_object_array_get_idx(tile_content, tile_index)));
                    }

                    save_data.map_layout.push_back(tile_items);
                }
            }
            else success_state = false;
        }

        if (success_state) {
            savefile = save_data;
        }
    }

    else {
        success_state = false;
    }

    json_object_put(data);

    return success_state;
}

void SaveManager::reset() {
    savefile = std::nullopt;

    json_object* data = json_object_new_object();
    json_object_to_file(path.c_str(), data);
    json_object_put(data);
}
