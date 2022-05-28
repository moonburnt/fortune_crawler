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

#include "mapgen.hpp"
#include "engine/utility.hpp"
#include "app.hpp"
#include "entity.hpp"
#include "loader.hpp"
#include "raylib.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO

// Things that can't be spawned via color (e.g abyss and player) are not there
static const int FLOOR_COLOR = ColorToInt(Color{203, 219, 252, 255});
static const std::map<int, int> VALID_COLORS = {
    {ColorToInt(Color{0, 255, 9, 255}), EID_ENTRANCE},
    {ColorToInt(Color{0, 242, 255, 255}), EID_EXIT},
    {ColorToInt(Color{255, 0, 0, 255}), EID_ENEMY},
    {ColorToInt(Color{255, 233, 0, 255}), EID_CHEST},
    {ColorToInt(Color{211, 83, 50, 255}), EID_CHEST_EMPTY},
    {ColorToInt(Color{255, 185, 112, 255}), EID_COIN_PILE},
    {ColorToInt(Color{199, 0, 255, 255}), EID_BOSS}};

GameMap::GameMap(Point _map_size, Point _tile_size, bool show_grid)
    : TileMapDeep(_map_size, _tile_size)
    , show_grid(show_grid) {}

void GameMap::set_player_id(int object_id) {
    player_id = object_id;
}

int GameMap::get_player_id() {
    return player_id;
}

int GameMap::add_and_place(MapObject* object, size_t grid_index) {
    int object_id = add_object(object);
    place_object(grid_index, object_id);
    return object_id;
}

std::vector<std::string> GameMap::get_tile_descriptions(size_t grid_index) {
    grid_index = std::clamp(grid_index, static_cast<size_t>(0), grid_size - 1);

    std::vector<std::string> descriptions;

    for (auto item : grid[grid_index]) {
        descriptions.push_back(map_objects[item]->get_description());
    }
    return descriptions;
}

bool GameMap::is_tile_blocked(Point tile) {
    // Safety check to ensure that requested tile is not out of bounds
    // May move it to some other place later
    if ((tile.x < 0) || (tile.y < 0) || (tile.x >= map_size.x) || (tile.y >= map_size.y))
        return true;

    // This may be moved to for
    int index = tile_to_index(tile);

    for (auto item : grid[index]) {
        if (map_objects[item]->is_obstacle()) return true;
    }
    return false;
}

std::vector<std::tuple<int, Event>> GameMap::get_player_events(int grid_index) {
    std::vector<std::tuple<int, Event>> tile_events;

    for (auto i : grid[grid_index]) {
        auto event = map_objects[i]->get_player_collision_event();
        if (event) tile_events.push_back(std::make_tuple(i, event.value()));
    }

    return tile_events;
}

std::vector<std::tuple<int, Event>> GameMap::get_enemy_events(int grid_index) {
    std::vector<std::tuple<int, Event>> tile_events;

    for (auto i : grid[grid_index]) {
        auto event = map_objects[i]->get_enemy_collision_event();
        if (event) tile_events.push_back(std::make_tuple(i, event.value()));
    }

    return tile_events;
}

void GameMap::select_tile(Point tile) {
    selected_pos.x = tile.x * tile_size.x;
    selected_pos.y = tile.y * tile_size.y;
    has_selected_pos = true;
}

void GameMap::deselect_tile() {
    has_selected_pos = false;
}

void GameMap::draw() {
    DrawRectangle(
        0, 0, map_real_size.x, map_real_size.y, {0, 0, 0, 255});

    for (auto current_tile = 0u; current_tile < grid_size; current_tile++) {
        for (auto item : grid[current_tile]) {
            map_objects[item]->draw(index_to_vec(current_tile));
        }
        if (show_grid) {
            Vector2 vec = index_to_vec(current_tile);
            DrawRectangleLines(
                vec.x, vec.y, tile_size.x, tile_size.y, {63, 63, 116, 255});
        }
    }

    if (has_selected_pos)
        DrawRectangleLines(
            selected_pos.x,
            selected_pos.y,
            tile_size.x,
            tile_size.y,
            {255, 255, 255, 255});
}

GameMap* generate_map(
    App* app,
    std::vector<std::vector<int>> map_content,
    Point map_size,
    Point tile_size,
    int dungeon_level,
    MapObject* player_object) {
    GameMap* gm = new GameMap(
        map_size,
        tile_size,
        app->config->settings["show_grid"].value_exact<bool>().value());

    static const std::string coin_sprite_names[3] = {
        "coin_pile_tile_0",
        "coin_pile_tile_1",
        "coin_pile_tile_2"};

    int abyss_id = gm->add_object(new Structure(EID_ABYSS, true, "Abyss"));
    int floor_id = gm->add_object(new Structure(
        EID_FLOOR,
        false,
        "Floor",
        app->assets.sprites["floor_tile"]));

    bool player_on_grid = false;
    int entrance_grid_id = -1;

    int grid_index = 0;
    for (auto tile : map_content) {
        for (auto item : tile) {
            switch (item) {
            case EID_FLOOR: {
                gm->place_object(grid_index, floor_id);
                break;
            }
            case EID_PLAYER: {
                // I'm not sure if this works correctly *derp*
                int player_id = gm->add_and_place(player_object, grid_index);
                gm->set_player_id(player_id);
                player_on_grid = true;
                break;
            }
            case EID_ENTRANCE: {
                gm->add_and_place(
                    new Structure(
                        EID_ENTRANCE,
                        false,
                        "Entrance",
                        app->assets.sprites["entrance_tile"]),
                    grid_index);
                entrance_grid_id = grid_index;
                break;
            }
            case EID_EXIT: {
                gm->add_and_place(
                    Structure::make_exit(
                        EID_EXIT,
                        app->assets.sprites["exit_tile"]),
                    grid_index);
                break;
            }
            case EID_ENEMY: {
                gm->add_and_place(
                    Enemy::make_enemy(
                        EID_ENEMY,
                        dungeon_level,
                        app->assets.sprites["enemy_tile"]),
                    grid_index);
                break;
            }
            case EID_CHEST: {
                gm->add_and_place(
                    Treasure::make_chest(
                        EID_CHEST,
                        randbool(),
                        std::max(std::rand() % 100 * dungeon_level, 10 * dungeon_level),
                        app->assets.sprites["treasure_tile_full"],
                        app->assets.sprites["treasure_tile_empty"]),
                    grid_index);
                break;
            }
            case EID_CHEST_EMPTY: {
                gm->add_and_place(
                    Treasure::make_empty_chest(
                        EID_CHEST_EMPTY,
                        app->assets.sprites["treasure_tile_empty"]),
                    grid_index);
                break;
            }
            case EID_COIN_PILE: {
                gm->add_and_place(
                    Treasure::make_coin_pile(
                        EID_COIN_PILE,
                        std::max(std::rand() % 20 * dungeon_level, 5 * dungeon_level),
                        app->assets.sprites[coin_sprite_names[std::rand() % 3]]),
                    grid_index);
                break;
            }
            case EID_BOSS: {
                gm->add_and_place(
                    Enemy::make_boss(
                        EID_BOSS,
                        dungeon_level,
                        app->assets.sprites["boss_tile"]),
                    grid_index);
                break;
            }
            default: {
                gm->place_object(grid_index, abyss_id);
                break;
            }
            }
        }
        grid_index++;
    }

    if (!player_on_grid) {
        if (entrance_grid_id == -1) {
            throw std::range_error("entrance_grid_id must be set to something");
        }
        int player_id = gm->add_and_place(
            player_object,
            entrance_grid_id);
        gm->set_player_id(player_id);
    }

    return gm;
}

GameMap* generate_map(
    App* app,
    std::vector<std::vector<int>> map_content,
    Point map_size,
    Point tile_size,
    int dungeon_level) {
    MapObject* player_object =
        new Player(EID_PLAYER, app->assets.sprites["player_tile"]);

    return generate_map(
        app, map_content, map_size, tile_size, dungeon_level, player_object);
}

GameMap* generate_map(
    App* app,
    Image map_file,
    Point tile_size,
    int dungeon_level,
    MapObject* player_object) {
    Point map_size = {map_file.width, map_file.height};

    std::vector<std::vector<int>> map_content = {};

    for (auto current_y = 0; current_y < map_size.y; current_y++) {
        for (auto current_x = 0; current_x < map_size.x; current_x++) {
            int pix_color = ColorToInt(GetImageColor(map_file, current_x, current_y));

            std::vector<int> tile_content;

            if (pix_color == FLOOR_COLOR) {
                tile_content = {EID_FLOOR};
            }
            else {
                bool color_found = false;
                for (auto& kv : VALID_COLORS) {
                    if (pix_color == kv.first) {
                        color_found = true;
                        tile_content = {EID_FLOOR, kv.second};
                        break;
                    }
                }
                if (!color_found) {
                    tile_content = {EID_ABYSS};
                }
            }

            map_content.push_back(tile_content);
        }
    }

    return generate_map(
        app, map_content, map_size, tile_size, dungeon_level, player_object);
}

GameMap* generate_map(App* app, Image map_file, Point tile_size) {
    MapObject* player_object =
        new Player(EID_PLAYER, app->assets.sprites["player_tile"]);

    return generate_map(app, map_file, tile_size, 1, player_object);
}
