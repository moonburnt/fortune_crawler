#include "loader.hpp"
#include "raylib.h"
#include <algorithm>
#include <cstdlib>
#include <string>

#define ASSET_PATH "./Assets/"
static constexpr const char* SPRITE_PATH = ASSET_PATH "Sprites/";
static constexpr const char* SFX_PATH = ASSET_PATH "SFX/";
static constexpr const char* MAP_PATH = "./maps/";

static constexpr const char* SPRITE_FORMAT = ".png";
static constexpr const char* SFX_FORMAT = ".ogg";
static constexpr const char* MAP_FORMAT = SPRITE_FORMAT;

AssetLoader AssetLoader::loader;

AssetLoader::AssetLoader() {
    last_map = -1;
}

void AssetLoader::load_sprites() {
    int amount;

    char** dir_files = GetDirectoryFiles(SPRITE_PATH, &amount);

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], SPRITE_FORMAT)) {
            std::string name_key(GetFileNameWithoutExt(dir_files[current]));
            this->sprites[name_key] =
                LoadTexture((std::string(SPRITE_PATH) + dir_files[current]).c_str());
        }
    }

    ClearDirectoryFiles();
}

void AssetLoader::load_sounds() {
    int amount;

    char** dir_files = GetDirectoryFiles(SFX_PATH, &amount);

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], SFX_FORMAT)) {
            std::string name_key(GetFileNameWithoutExt(dir_files[current]));
            this->sounds[name_key] =
                LoadSound((std::string(SFX_PATH) + dir_files[current]).c_str());
        }
    }

    ClearDirectoryFiles();
}

void AssetLoader::load_maps_list() {
    // TODO: automatic verification of map's colors, to throw away invalid ones
    int amount;

    char** dir_files = GetDirectoryFiles(MAP_PATH, &amount);

    map_paths = {};

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], MAP_FORMAT)) {
            map_paths.push_back(std::string(MAP_PATH) + dir_files[current]);
        }
    }

    ClearDirectoryFiles();
}

Image AssetLoader::load_map(size_t index) {
    last_map = index;
    return LoadImage(map_paths[index].c_str());
}

Image AssetLoader::load_random_map() {
    // This version of mapgen will ensure that the same map can only appear again
    // if the whole roaster of maps has been completed
    last_map++;
    if (last_map == map_paths.size()) {
        last_map = 0;
        std::random_shuffle(map_paths.begin(), map_paths.end());
    }

    return load_map(last_map);
}
