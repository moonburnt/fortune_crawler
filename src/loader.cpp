#include "loader.hpp"
#include "raylib.h"
#include <string>

static constexpr const char* SETTINGS_PATH = "./settings.json";

#define ASSET_PATH "./Assets/"
static constexpr const char* SPRITE_PATH = ASSET_PATH "Sprites/";
static constexpr const char* SFX_PATH = ASSET_PATH "SFX/";

static constexpr const char* SPRITE_FORMAT = ".png";
static constexpr const char* SFX_FORMAT = ".ogg";

AssetLoader AssetLoader::loader;

void AssetLoader::load_sprites() {
    int sprites_amount;

    char** sprite_files = GetDirectoryFiles(SPRITE_PATH, &sprites_amount);

    int current_sprite = 0;
    while (current_sprite < sprites_amount) {
        if (IsFileExtension(sprite_files[current_sprite], SPRITE_FORMAT)) {
            std::string name_key(GetFileNameWithoutExt(sprite_files[current_sprite]));
            this->sprites[name_key] = LoadTexture(
                (std::string(SPRITE_PATH) + sprite_files[current_sprite]).c_str());
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
            this->sounds[name_key] =
                LoadSound((std::string(SFX_PATH) + dir_files[current]).c_str());
        }
        current++;
    }

    ClearDirectoryFiles();
}
