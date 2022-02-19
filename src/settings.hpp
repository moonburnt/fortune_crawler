#pragma once

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

struct SavefileFields {
    std::unordered_map<std::string, int> player_stats;
    std::unordered_map<std::string, int> dungeon_stats;
    std::unordered_map<std::string, int> map_settings;
    std::vector<std::vector<int>> map_layout;
};

class SettingsManager {
private:
    enum class SettingsCategory {
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

    std::optional<SavefileFields> savefile;

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

    // Save level's data on disk. Returns success state.
    bool save_level(SavefileFields level_data);
    // Load level's data from disk. Returns success state.
    // Idk where else to put it, will leave there for now.
    bool load_savefile();
    // Forget savefile in memory and reset save data on disk.
    void reset_save();

    static SettingsManager manager;
};
