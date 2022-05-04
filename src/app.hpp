#pragma once

#include "platform.hpp"

#include <engine/core.hpp>
#include <engine/settings.hpp>
#include <engine/storage.hpp>
#include "loader.hpp"

#include <memory>

struct AssetLoader {
    SpriteStorage sprites;
    SoundStorage sounds;
    MusicStorage music;
    MapLoader maps;
};

class App {
public:
    App();

    void run();

    GameWindow window;
    AssetLoader assets;
    std::unique_ptr<SaveManager> save;
    std::unique_ptr<SettingsManager> config;
    std::unique_ptr<Platform> platform;
};
