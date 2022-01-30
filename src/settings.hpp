#pragma once

#include <string>
#include <unordered_map>

class SettingsManager {
private:
    enum class SettingsCategory
    {
        standard,
        current
    };

    // There must be a way to organize things better
    std::unordered_map<SettingsCategory, bool> show_fps;
    std::unordered_map<SettingsCategory, bool> show_grid;
    std::unordered_map<SettingsCategory, float> camera_zoom;
    // And this could (and should) definely be done better
    std::unordered_map<SettingsCategory, std::unordered_map<std::string, int>> controls;

public:
    SettingsManager();

    // Load settings from disk. Returns true on success, false on failure
    bool load_settings();
    // Save settings to disk. Same return principle as in loader.
    bool save_settings();
    // Reset current settings and selected settings to default ones
    void reset_to_defaults();

    bool get_show_fps();
    bool get_show_grid();
    float get_camera_zoom();
    std::unordered_map<std::string, int> get_controls();

    void set_show_fps(bool value);
    void set_show_grid(bool value);
    void set_camera_zoom(float value);

    static SettingsManager manager;
};
