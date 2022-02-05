#include "loader.hpp"
#include "raylib.h"
#include "settings.hpp"

#include "scenes.hpp"

int main() {
    InitWindow(1280, 720, "Fortune Crawler");
    SetTargetFPS(60);
    // Disable ability to close the window by pressing esc
    SetExitKey(KEY_NULL);

    // Initialize audio device. This needs to be done before loading sounds.
    InitAudioDevice();

    if (!SettingsManager::manager.load_settings()) {
        SettingsManager::manager.save_settings();
    }

    AssetLoader::loader.load_sprites();
    AssetLoader::loader.load_sounds();
    AssetLoader::loader.load_maps_list();
    SettingsManager::manager.load_savefile();

    SceneManager::sc_mgr.set_default_scene();
    SceneManager::sc_mgr.run_update_loop();

    CloseWindow();

    return 0;
}
