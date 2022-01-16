#pragma once

#include "entity.hpp"

#include <cstddef>
#include <raylib.h>

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

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

public:
    GameMap(Point map_size, Point tile_size);

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

    // These do have safety clamps to avoid out-of-bounds issues
    Point index_to_tile(size_t index);
    int tile_to_index(Point pos);
    Vector2 index_to_vec(size_t index);
    int vec_to_index(Vector2 vec);

    // These ones don't
    Point vec_to_tile(Vector2 vec);
    Vector2 tile_to_vec(Point tile);

    // Returns true if vec is part of map, false if its out of bounds
    bool is_vec_on_map(Vector2 vec);

    Point get_tile_size();
    Point get_map_size();

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

    // Returns last event in tile and object_id of item that caused it.
    // If is_player_event is set to true - will search for player events, else -
    // for enemy events.
    // TODO: remake it into function that returns vector of items, to support
    // multiple events per tile (since these may and will occur)
    std::tuple<int, Event> get_tile_event(int grid_index, bool is_player_event);

    // Set specified tile to be highlighted
    void select_tile(Point tile);
    void deselect_tile();

    void draw();
};

GameMap* generate_map(Image map_file, Point tile_size);
GameMap* generate_map(
    Image map_file, Point tile_size, int dungeon_level, MapObject* player_object);
