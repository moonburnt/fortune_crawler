#include "engine/utility.hpp"
#include "shared.hpp"
#include "raylib.h"
#include "menus.hpp"

#define ASSET_PATH "./Assets/"
static constexpr const char* SPRITE_PATH = ASSET_PATH "Sprites/";
static constexpr const char* SFX_PATH = ASSET_PATH "SFX/";
static constexpr const char* MAP_PATH = "./maps/";

static constexpr const char* SPRITE_FORMAT = ".png";
static constexpr const char* SFX_FORMAT = ".ogg";
static constexpr const char* MAP_FORMAT = SPRITE_FORMAT;

int main() {
    shared::config.load();

    shared::window.init(
        std::max(shared::config.settings["resolution"][0].value_or(1280), 1280),
        std::max(shared::config.settings["resolution"][1].value_or(720), 720),
        "Fortune Crawler");

    if (shared::config.settings["fullscreen"].value_or(false) && !IsWindowFullscreen()) {
        const int current_screen = GetCurrentMonitor();
        ToggleFullscreen();
        SetWindowSize(GetMonitorWidth(current_screen), GetMonitorHeight(current_screen));
    };

    shared::assets.sprites.load(SPRITE_PATH, SPRITE_FORMAT);
    shared::assets.sounds.load(SFX_PATH, SFX_FORMAT);
    shared::assets.maps.fetch_maps(MAP_PATH, MAP_FORMAT);
    shared::save.load();

    if (shared::config.settings["show_fps"].value_or(false)) {
        shared::window.sc_mgr.nodes["fps_counter"] = new FrameCounter();
    };

    shared::window.sc_mgr.set_current_scene(new TitleScreen(&shared::window.sc_mgr));
    shared::window.run();

    return 0;
}
