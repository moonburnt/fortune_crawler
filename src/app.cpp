#include "app.hpp"
#include "platform.hpp"
#include "menus.hpp"

#include <engine/utility.hpp>

#include <raylib.h>

#include <fmt/format.h>

App::App() {
    platform = Platform::make_platform();

    auto resource_dir = platform->get_resource_dir();
    auto settings_dir = platform->get_settings_dir();

    config = std::make_unique<SettingsManager>(
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
        fmt::format("{}settings.toml", settings_dir));

    save = std::make_unique<SaveManager>(fmt::format("{}save.json", settings_dir));

    config->load();

    window.init(
        std::max(config->settings["resolution"][0].value_or(1280), 1280),
        std::max(config->settings["resolution"][1].value_or(720), 720),
        "Fortune Crawler");

    // Set music stream's volume
    float volume = std::clamp(
        static_cast<int>(
            config->settings["music_volume"].value_exact<int64_t>().value()),
        0, 100) / 100.0f;

    window.music_mgr.set_concurrent_sounds_limit(1);
    window.music_mgr.set_volume(volume);

    if (config->settings["fullscreen"].value_or(false) && !IsWindowFullscreen()) {
        // TODO: add ability to specify active monitor
        const int current_screen = GetCurrentMonitor();
        // Its important to first toggle fullscreen and only them apply size.
        // Else this won't work (I spent 3 hours trying to debug this)
        ToggleFullscreen();
        SetWindowSize(GetMonitorWidth(current_screen), GetMonitorHeight(current_screen));
    };

    assets.sprites.load(platform->get_sprites_dir(), ".png");
    assets.sounds.load(platform->get_sounds_dir(), ".ogg");
    assets.music.load(platform->get_music_dir(), ".ogg");
    assets.maps.fetch_maps(platform->get_maps_dir(), ".png");
    save->load();
}

void App::run() {
    if (config->settings["show_fps"].value_or(false)) {
        window.sc_mgr.nodes["fps_counter"] = new FrameCounter({4.0f, 4.0f});
    };

    window.sc_mgr.set_current_scene(new TitleScreen(this, &window.sc_mgr));
    window.run();
}
