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
