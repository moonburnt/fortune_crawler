#include "settings.hpp"
#include "raylib.h"
#include <json-c/json.h>

static constexpr const char* SETTINGS_PATH = "./settings.json";

SettingsManager SettingsManager::manager;

void SettingsManager::reset_to_defaults() {
    show_fps[SettingsCategory::current] = show_fps[SettingsCategory::standard];
    show_grid[SettingsCategory::current] = show_grid[SettingsCategory::standard];
}

SettingsManager::SettingsManager() {
    // This will initialize settings manager's storages with default values
    show_fps[SettingsCategory::standard] = false;
    show_grid[SettingsCategory::standard] = false;

    reset_to_defaults();
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

void SettingsManager::set_show_fps(bool value) {
    show_fps[SettingsCategory::current] = value;
}

void SettingsManager::set_show_grid(bool value) {
    show_grid[SettingsCategory::current] = value;
}
