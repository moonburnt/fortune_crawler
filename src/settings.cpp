#include "settings.hpp"
#include "raylib.h"

SettingsManager SettingsManager::manager;

void SettingsManager::reset_to_defaults() {
    show_fps[SettingsCategory::current] = show_fps[SettingsCategory::standard];
    show_fps[SettingsCategory::selected] = show_fps[SettingsCategory::standard];

    show_grid[SettingsCategory::current] = show_grid[SettingsCategory::standard];
    show_grid[SettingsCategory::selected] = show_grid[SettingsCategory::standard];
}

SettingsManager::SettingsManager() {
    // This will initialize settings manager's storages with default values
    show_fps[SettingsCategory::standard] = false;
    show_grid[SettingsCategory::standard] = false;

    reset_to_defaults();
}

bool SettingsManager::get_show_fps() {
    return show_fps[SettingsCategory::current];
}

bool SettingsManager::get_show_grid() {
    return show_grid[SettingsCategory::current];
}
