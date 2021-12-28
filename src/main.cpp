#include <unordered_map>
#include <string>
#include "raylib.h"
#include "loader.hpp"
extern AssetsLoader loader;

#include "scenes.hpp"
extern SceneManager sc_mgr;

int main() {
    InitWindow(1280, 720, "Luck Crawler");
    SetTargetFPS(60);

    // Initialize audio device. This needs to be done before loading sounds.
    InitAudioDevice();

    loader.load_sprites();
    loader.load_sounds();

    sc_mgr.set_default_scene();
    sc_mgr.run_update_loop();

    CloseWindow();

    return 0;
}
