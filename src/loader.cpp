#include <string>
#include "raylib.h"
#include "loader.hpp"

#define SETTINGS_PATH "./settings.json"

#define ASSETS_PATH "./Assets/"
#define SPRITE_PATH ASSETS_PATH "Sprites/"
#define SFX_PATH ASSETS_PATH "SFX/"

#define SPRITE_FORMAT ".png"
#define SFX_FORMAT ".ogg"

AssetLoader AssetLoader::loader;

void AssetLoader::load_sprites() {
    int sprites_amount;

    char** sprite_files = GetDirectoryFiles(SPRITE_PATH, &sprites_amount);

    int current_sprite = 0;
    while (current_sprite < sprites_amount) {
        if (IsFileExtension(sprite_files[current_sprite], SPRITE_FORMAT)) {
            std::string name_key(GetFileNameWithoutExt(sprite_files[current_sprite]));
            this->sprites[name_key] = LoadTexture(
                (std::string(SPRITE_PATH)+sprite_files[current_sprite]).c_str()
            );
        }
        current_sprite++;
    }

    ClearDirectoryFiles();
}

void AssetLoader::load_sounds() {
    int amount;

    char** dir_files = GetDirectoryFiles(SFX_PATH, &amount);

    int current = 0;

    while (current < amount) {
        if (IsFileExtension(dir_files[current], SFX_FORMAT)) {
            std::string name_key(GetFileNameWithoutExt(dir_files[current]));
            this->sounds[name_key] = LoadSound(
                (std::string(SFX_PATH)+dir_files[current]).c_str()
            );
        }
        current++;
    }

    ClearDirectoryFiles();
}
