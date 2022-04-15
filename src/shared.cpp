#include "shared.hpp"
#include "loader.hpp"

static constexpr const char* SETTINGS_PATH = "./settings.toml";
static constexpr const char* SAVE_PATH = "./save.json";

GameWindow shared::window;
AssetLoader shared::assets;
SettingsManager shared::config = SettingsManager(
    toml::table{
        {"show_fps", false},
        {"show_grid", false},
        {"camera_zoom", 2.0f},
        {"fullscreen", false},
        {"resolution", toml::array{1280, 720}},
        {"sfx_volume", 100},
        {"music_volume", 100},
        {"controls", toml::table{
            // This looks dumb, but without these it did not compile
            {"UPLEFT", static_cast<int>(KEY_KP_7)},
            {"UP", static_cast<int>(KEY_KP_8)},
            {"UPRIGHT", static_cast<int>(KEY_KP_9)},
            {"LEFT", static_cast<int>(KEY_KP_4)},
            {"RIGHT", static_cast<int>(KEY_KP_6)},
            {"DOWNLEFT", static_cast<int>(KEY_KP_1)},
            {"DOWN", static_cast<int>(KEY_KP_2)},
            {"DOWNRIGHT", static_cast<int>(KEY_KP_3)}}}},
    SETTINGS_PATH);

SaveManager shared::save = SaveManager(SAVE_PATH);
