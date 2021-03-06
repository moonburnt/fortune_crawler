// Fortune Crawler - a conceptual dungeon crawler with rock-paper-scissors events.
// Copyright (c) 2022 moonburnt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see https://www.gnu.org/licenses/gpl-3.0.txt

#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "raylib.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <random>
#include <string>

// Map loader
MapLoader::MapLoader() : last_map(-1) {}

void MapLoader::fetch_maps(std::string path, std::string extension) {
    // TODO: automatic verification of map's colors, to throw away invalid ones
    int amount;

    char** dir_files = GetDirectoryFiles(path.c_str(), &amount);

    map_paths = {};

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], extension.c_str())) {
            map_paths.push_back(path + dir_files[current]);
        }
    }

    ClearDirectoryFiles();
    // Overriding last_map counter, to make load_random_map() work correctly.
    last_map = map_paths.size() - 1;
}

Image MapLoader::load_map(size_t index) {
    last_map = index;
    return LoadImage(map_paths[index].c_str());
}

Image MapLoader::load_random_map() {
    // This version of mapgen will ensure that the same map can only appear again
    // if the whole roaster of maps has been completed
    last_map++;
    if (last_map == map_paths.size()) {
        last_map = 0;

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(map_paths.begin(), map_paths.end(), g);
    }

    return load_map(last_map);
}

// Save manager
SaveManager::SaveManager(std::string p) : path(p) {
    // reset(); // DO NOT DO THAT
}

bool SaveManager::save_level(SavefileFields level_data) {
    // For now, will always return true.
    // TODO: add error handling in case file cant be written
    bool success_state = true;

    nlohmann::json data {
        {"player_stats", level_data.player_stats},
        {"dungeon_stats", level_data.dungeon_stats},
        {"map_settings", level_data.map_settings},
        {"map_layout", level_data.map_layout}
    };

    std::ofstream file;
    file.open(path);
    file << data;

    return success_state;
}

bool SaveManager::load() {
    std::ifstream file;
    file.open(path);
    // TODO: more detailed error handling
    if (!file.good()) {
        spdlog::warn("Unable to open {}", path);
        return false;
    }

    nlohmann::json data;
    SavefileFields save_data;

    try {
        file >> data;
    }
    catch (const nlohmann::detail::parse_error& err) {
        spdlog::warn(err.what());

        return false;
    }

    try {
        save_data = {
            data["player_stats"],
            data["dungeon_stats"],
            data["map_settings"],
            data["map_layout"]
        };
    }
    catch (const nlohmann::detail::type_error& err) {
        spdlog::warn(err.what());

        return false;
    }

    savefile = save_data;

    return true;
}

void SaveManager::reset() {
    savefile = std::nullopt;

    // TODO: error handling
    std::ofstream file;
    file.open(path);
    file << nlohmann::json({});
}
