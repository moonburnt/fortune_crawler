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

#include "entity.hpp"

#include <cstddef>
#include <raylib.h>

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "engine/mapgen.hpp"

class App;

class GameMap : public TiledMapDeep<MapObject*> {
private:
    bool show_grid;
    // TODO: merge these two into std::optional<Vector2>
    bool has_selected_pos = false;
    Vector2 selected_pos;

    // TODO: maybe add some default value to it, idk
    int player_id;

public:
    GameMap(Point _map_size, Point _tile_size, bool show_grid);
    ~GameMap() = default;

    // WIP; TODO: remove this garbage
    void set_player_id(int object_id);
    int get_player_id();

    int add_and_place(MapObject* object, size_t grid_index);

    std::vector<std::string> get_tile_descriptions(size_t grid_index);
    bool is_tile_blocked(Point tile);

    std::vector<std::tuple<int, Event>> get_player_events(int grid_index);
    std::vector<std::tuple<int, Event>> get_enemy_events(int grid_index);

    void select_tile(Point tile);
    void deselect_tile();

    void draw();
};

GameMap* generate_map(App* app, Image map_file, Point tile_size);

GameMap* generate_map(
    App* app,
    Image map_file,
    Point tile_size,
    int dungeon_level,
    MapObject* player_object);

GameMap* generate_map(
    App* app,
    std::vector<std::vector<int>> map_content,
    Point map_size,
    Point tile_size,
    int dungeon_level,
    MapObject* player_object);

GameMap* generate_map(
    App* app,
    std::vector<std::vector<int>> map_content,
    Point map_size,
    Point tile_size,
    int dungeon_level);
