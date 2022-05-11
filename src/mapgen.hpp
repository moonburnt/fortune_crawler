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

class App;

struct Point {
    int x;
    int y;
};

class GameMap {
private:
    Vector2 map_real_size;
    Point map_size;
    Point tile_size;
    size_t grid_size;
    std::unordered_map<int, MapObject*> map_objects;
    // This shouldn't be changed on removal of older objects, I think
    int map_objects_amount;
    std::vector<std::vector<int>> grid;
    bool has_selected_pos;
    Vector2 selected_pos;

    int player_id;
    bool show_grid;

public:
    GameMap(Point map_size, Point tile_size, bool show_grid);

    // Place specific object from storage on specified space, without removing it
    // from any other place. May be used to spawn items and to place items that
    // may exist on multiple tiles at once, such as floor.
    void place_object(int grid_index, int object_id);
    // Add new object to map_objects storage. Returns object id
    int add_object(MapObject* object);
    // Overload that automatically places object to specified tile
    int add_object(MapObject* object, int grid_index);

    // Delete object from grid[grid_index][tile_index].
    // If delete_from_storage is set to true - also delete from map_objects.
    // This is a thing, coz same object may appear on multiple tiles - floors, etc.
    void delete_object(int grid_index, int tile_index, bool delete_from_storage);

    // Move object from grid[grid_index][tile_index] to grid[new_grid_index]
    void move_object(int grid_index, int tile_index, int new_grid_index);

    int get_player_id();

    std::vector<int>* get_tile_content(size_t grid_index);

    // Get descriptions of items in specified tile
    std::vector<std::string> get_tile_descriptions(size_t grid_index);

    // Returns true if requrested things are within map's borders, false otherwise
    bool is_vec_on_map(Vector2 vec);
    bool is_index_on_map(size_t grid_index);
    bool is_tile_on_map(Point tile);

    // Attempting to use out-of-bounds values on these will cause issues.
    // Thus don't forget to call is_<type>_on_map before using them!
    Point index_to_tile(size_t index);
    int tile_to_index(Point pos);
    Vector2 index_to_vec(size_t index);
    Point vec_to_tile(Vector2 vec);
    Vector2 tile_to_vec(Point tile);
    int vec_to_index(Vector2 vec);

    Point get_tile_size();
    Point get_map_size();
    size_t get_grid_size();

    // Returns entity id of each object on map.
    std::vector<std::vector<int>> get_map_layout();

    // Get first tile that contains specified object, or std::nullopt
    std::optional<Point> find_object_tile(int object_id);

    // Search in tile for object with specific id. Returns that object's
    // tile_index or std::nullopt
    std::optional<int> find_object_in_tile(int grid_index, int object_id);

    // Get amount of elements in tile with provided index.
    int get_tile_elements_amount(int grid_index);

    // Get pointer to object with specified id
    MapObject* get_object(int object_id);
    // Overload to get object from specific grid/tile
    MapObject* get_object(int grid_index, int tile_index);

    // Returns true if tile has obstacle in it or is abyss, false otherwise.
    bool is_tile_blocked(Point tile);

    // Returns false if tile has only floor (e.g only one element in vector),
    // true otherwise
    bool is_tile_occupied(int grid_index);
    bool is_tile_occupied(Point tile);

    // Get events and their causes from tile. Will be empty if no events available
    std::vector<std::tuple<int, Event>> get_player_events(int grid_index);
    std::vector<std::tuple<int, Event>> get_enemy_events(int grid_index);

    // Set specified tile to be highlighted
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
