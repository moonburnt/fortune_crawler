#pragma once

#include <unordered_map>

enum class SettingsCategory
{
    standard,
    current,
    selected
};

class SettingsManager {
private:
    // There must be a way to organize things better
    std::unordered_map<SettingsCategory, bool> show_fps;
    std::unordered_map<SettingsCategory, bool> show_grid;

public:
    SettingsManager();

    void load_settings();
    // Reset current settings and selected settings to default ones
    void reset_to_defaults();

    bool get_show_fps();
    bool get_show_grid();

    static SettingsManager manager;
};
