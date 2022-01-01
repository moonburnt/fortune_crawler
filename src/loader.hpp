#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>

class AssetLoader {
    // private:
    //     std::unordered_map<std::string, Texture2D> sprites;
    //     std::unordered_map<std::string, Sound> sounds;
    public:
        std::unordered_map<std::string, Texture2D> sprites;
        std::unordered_map<std::string, Sound> sounds;
        void load_sprites();
        void load_sounds();

    // Default instance of loader, use extern to use it in other modules
    static AssetLoader loader;
};
