#pragma once

#include "engine/core.hpp"
#include "engine/settings.hpp"
#include "engine/storage.hpp"
#include "loader.hpp"

// There we specify things, access to which should be shared across other files
struct AssetLoader {
    SpriteStorage sprites;
    SoundStorage sounds;
    MapLoader maps;
};

// Shared instances are packed these into namespace, to make them easy to recognize
// These are non-static, but extern and usually defined in "shared.cpp"
// It may be a bad idea to make things work like that, but will do for now. TODO
namespace shared {
extern GameWindow window;
extern AssetLoader assets;
extern SettingsManager config;
extern SaveManager save;
}
