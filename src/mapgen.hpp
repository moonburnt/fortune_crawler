#pragma once

#include <cstddef>
#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

struct Point {
    int x;
    int y;
};

enum class ObjectCategory
{
    floor,
    obstacle,
    creature,
    item
};

enum class FloorType
{
    abyss,
    floor
};

enum class ObstacleType
{
    wall
};

enum class CreatureType
{
    player,
    enemy,
    boss
};

enum class ItemType
{
    entrance,
    exit,
    treasure,
    trap
};

enum class Event {
    nothing,
    exit_map,
    fight
};

class MapObject {
private:
    bool has_texture;
    Texture2D* texture;

protected:
    ObjectCategory category;
    Event player_collision_event;
    // This theoretically also affects boss, but it doesn't move so whatever
    Event enemy_collision_event;

public:
    std::string description;
    ObjectCategory get_category();
    Event get_player_collision_event();
    Event get_enemy_collision_event();
    MapObject(ObjectCategory cat, std::string desc);
    MapObject(ObjectCategory cat, std::string desc, Texture2D* sprite);
    MapObject(
        ObjectCategory cat,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite
    );
    // The same as previous, but same collision event will get assigned to both
    MapObject(
        ObjectCategory cat,
        std::string desc,
        Event collision_event,
        Texture2D* sprite
    );

    void draw(Vector2 pos);
};

class Floor : public MapObject {
public:
    FloorType type;
    Floor();
    Floor(FloorType tile_type, std::string desc, Texture2D* sprite);
};

class Obstacle : public MapObject {
public:
    ObstacleType type;
    Obstacle(ObstacleType tile_type, std::string desc, Texture2D* sprite);
};

class Creature : public MapObject {
public:
    CreatureType type;
    Creature(
        CreatureType tile_type,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite
    );

    void update();
};

class Item : public MapObject {
public:
    ItemType type;
    Item(
        ItemType tile_type,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite
    );
};

class GameMap {
private:
    Vector2 map_real_size;
    Point map_size;
    Point tile_size;
    size_t grid_size;
    std::unordered_map<int, MapObject*> map_objects;
    std::vector<std::vector<int>> grid;
    bool has_selected_pos;
    Vector2 selected_pos;

    int player_id;
    int exit_id;

public:
    std::vector<int> enemy_indices;

    GameMap(
        Point map_size,
        Point tile_size,
        std::unordered_map<int, MapObject*> map_objects,
        std::vector<std::vector<int>> grid);

    int get_player_id();
    int get_exit_id();

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

    Point get_player_tile();
    Point get_tile_size();
    Point get_map_size();

    // Returns true if tile has obstacle in it or is abyss, false otherwise.
    bool is_tile_blocked(Point tile);

    // Returns false if tile has only floor (e.g only one element in vector),
    // true otherwise
    bool is_tile_occupied(int grid_index);
    bool is_tile_occupied(Point tile);

    // Search in tile for object with specific id. Returns success status,
    // overwrites provided tile_index with index of tile's element that contains
    // provided item (or with map_objects.end() - thats why bool exists there)
    bool object_in_tile(int grid_index, int object_id, int* tile_index);

    // Returns last event in tile or Event::nothing. If is_player_event is set
    // to true - searches for player events, else - for enemy events.
    // TODO: remake it into function that returns vector of items, to support
    // multiple events per tile (since these may and will occur)
    Event get_tile_event(int grid_index, bool is_player_event);

    void move_object(int grid_index, int tile_index, int new_grid_index);

    // Set specified tile to be highlighted
    void select_tile(Point tile);
    void deselect_tile();

    void draw();
};

GameMap* generate_map(Image map_file, Point tile_size);
