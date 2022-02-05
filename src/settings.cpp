#include "settings.hpp"
#include "raylib.h"
#include <json-c/json.h>
// For clamp
#include <algorithm>
#include <optional>

static constexpr const char* SETTINGS_PATH = "./settings.json";
static constexpr const char* SAVE_PATH = "./save.json";

static const std::string KEY_NAMES[8] = {
    "UPLEFT", "UP", "UPRIGHT", "LEFT", "RIGHT", "DOWNLEFT", "DOWN", "DOWNRIGHT"};

SettingsManager SettingsManager::manager;

void SettingsManager::reset_to_defaults() {
    show_fps[SettingsCategory::current] = show_fps[SettingsCategory::standard];
    show_grid[SettingsCategory::current] = show_grid[SettingsCategory::standard];
    camera_zoom[SettingsCategory::current] = camera_zoom[SettingsCategory::standard];
    controls[SettingsCategory::current] = controls[SettingsCategory::standard];
}

SettingsManager::SettingsManager() {
    // This will initialize settings manager's storages with default values
    show_fps[SettingsCategory::standard] = false;
    show_grid[SettingsCategory::standard] = false;
    camera_zoom[SettingsCategory::standard] = 2.0f;
    controls[SettingsCategory::standard] = {
        {"UPLEFT", KEY_KP_7},
        {"UP", KEY_KP_8},
        {"UPRIGHT", KEY_KP_9},
        {"LEFT", KEY_KP_4},
        {"RIGHT", KEY_KP_6},
        {"DOWNLEFT", KEY_KP_1},
        {"DOWN", KEY_KP_2},
        {"DOWNRIGHT", KEY_KP_3}};

    reset_to_defaults();
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

bool SettingsManager::load_settings() {
    bool success_state = true;

    json_object* data = json_object_from_file(SETTINGS_PATH);
    if (data) {
        json_object* fps;
        json_object_object_get_ex(data, "show_fps", &fps);

        if (json_object_get_type(fps) == json_type_boolean) {
            show_fps[SettingsCategory::current] = json_object_get_boolean(fps);
        }

        json_object* grid;
        json_object_object_get_ex(data, "show_grid", &grid);

        if (json_object_get_type(grid) == json_type_boolean) {
            show_grid[SettingsCategory::current] = json_object_get_boolean(grid);
        }

        json_object* cam_distance;
        json_object_object_get_ex(data, "camera_zoom", &cam_distance);

        // jsons don't have floats - only ints and doubles
        if (json_object_get_type(cam_distance) == json_type_double) {
            set_camera_zoom(static_cast<float>(json_object_get_double(cam_distance)));
        }

        json_object* ctrl_storage;
        json_object_object_get_ex(data, "controls", &ctrl_storage);

        if (json_object_get_type(ctrl_storage) == json_type_object) {
            for (auto key : KEY_NAMES) {
                std::optional<int> key_value = get_key(key, ctrl_storage);

                if (key_value) {
                    controls[SettingsCategory::current][key] = key_value.value();
                }
            }
        }
    }
    else {
        success_state = false;
    }

    json_object_put(data);

    return success_state;
}

bool SettingsManager::save_settings() {
    bool success_state = true;

    json_object* data = json_object_new_object();
    if (!data) return false;

    json_object_object_add(
        data,
        "show_fps",
        json_object_new_boolean(show_fps[SettingsCategory::current]));

    json_object_object_add(
        data,
        "show_grid",
        json_object_new_boolean(show_grid[SettingsCategory::current]));

    json_object_object_add(
        data,
        "camera_zoom",
        json_object_new_double(camera_zoom[SettingsCategory::current]));

    json_object* ctrl_storage = json_object_new_object();
    for (auto key : KEY_NAMES) {
        json_object_object_add(
            ctrl_storage,
            key.c_str(),
            json_object_new_int(controls[SettingsCategory::current][key]));
    }
    json_object_object_add(data, "controls", ctrl_storage);

    if (json_object_to_file(SETTINGS_PATH, data)) {
        success_state = false;
    }

    json_object_put(data);
    return success_state;
}

bool SettingsManager::get_show_fps() {
    return show_fps[SettingsCategory::current];
}

bool SettingsManager::get_show_grid() {
    return show_grid[SettingsCategory::current];
}

float SettingsManager::get_camera_zoom() {
    return camera_zoom[SettingsCategory::current];
}

std::unordered_map<std::string, int> SettingsManager::get_controls() {
    return controls[SettingsCategory::current];
}

void SettingsManager::set_show_fps(bool value) {
    show_fps[SettingsCategory::current] = value;
}

void SettingsManager::set_show_grid(bool value) {
    show_grid[SettingsCategory::current] = value;
}

void SettingsManager::set_camera_zoom(float value) {
    // Sane values would be 1.0 -> 3.0, everything bigger would make things render
    // too close. Everything less makes no sense.
    camera_zoom[SettingsCategory::current] = std::clamp(value, 1.0f, 3.0f);
}

bool SettingsManager::save_level(SavefileFields level_data) {
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

    if (json_object_to_file(SAVE_PATH, data)) {
        success_state = false;
    }

    json_object_put(data);
    return success_state;
}

bool SettingsManager::load_savefile() {
    bool success_state = true;

    static const std::vector<std::string> player_stat_fields =
        {"current_hp", "max_hp", "pdmg", "rdmg", "mdmg", "pdef", "rdef", "mdef", "money"};
    static const std::vector<std::string> dungeon_stat_fields =
        {"lvl", "current_turn", "money_collected", "enemies_killed"};

    static const std::vector<std::string> map_settings_fields =
        {"map_x", "map_y", "tile_x", "tile_y"};

    json_object* data = json_object_from_file(SAVE_PATH);
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
                // TODO: stub, no safety checks in this part.
                for (size_t i = 0lu; i < json_object_array_length(map_layout); i++) {
                    // I think this will do?
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
            SettingsManager::manager.savefile = save_data;
        }
    }

    else {
        success_state = false;
    }

    json_object_put(data);

    return success_state;
}
