#pragma once

#include <raylib.h>

#include <vector>
#include <unordered_map>

struct Point {
    int x;
    int y;
};

enum class ObjectCategory {
    floor,
    obstacle,
    creature,
    item
};

enum class FloorType {
    abyss,
    floor
};

enum class ObstacleType {
    wall
};

enum class CreatureType {
    player,
    enemy,
    boss
};

enum class ItemType {
    entrance,
    exit,
    treasure,
    trap
};

class MapObject {
    private:
        bool has_texture;
        Texture2D* texture;

    protected:
        ObjectCategory category;

    public:
        ObjectCategory get_category();
        MapObject(ObjectCategory cat);
        MapObject(ObjectCategory cat, Texture2D* sprite);

        void draw(Point pos);
};

class Floor: public MapObject {
    public:
        FloorType type;
        Floor();
        Floor(FloorType tile_type, Texture2D* sprite);
};

class Obstacle: public MapObject {
    public:
        ObstacleType type;
        Obstacle(ObstacleType tile_type, Texture2D* sprite);
};

class Creature: public MapObject {
    public:
        CreatureType type;
        Creature(CreatureType tile_type, Texture2D* sprite);

        void update();
};

class Item: public MapObject {
    public:
        ItemType type;
        Item(ItemType tile_type, Texture2D* sprite);
};

class GameMap {
    private:
        Vector2 map_real_size;
        Point map_size;
        Point tile_size;
        int grid_size;
        std::unordered_map<int, MapObject*>map_objects;
        std::vector<std::vector<int>>grid;

    public:
        std::vector<int>enemy_indexes;

        GameMap(
            Point m_size,
            Point t_size,
            std::unordered_map<int, MapObject*>map_elems,
            std::vector<std::vector<int>>grid_layout
        );

        int get_player_id();

        std::vector<int>* get_tile_content(int grid_index);

        Point index_to_pos(int index);
        int pos_to_index(Point pos);
        // This one doesnt have safety checks
        Point vec_to_tile(Vector2 vec);

        // Returns true if vec is part of map, false if its out of bounds
        bool is_vec_on_map(Vector2 vec);

        Point get_player_tile();
        Point get_tile_size();
        Point get_map_size();
        bool is_tile_blocked(Point tile);

        void move_object(int grid_index, int tile_index, int new_grid_index);

        void draw();
};

GameMap* generate_map(Image map_file, Point tile_size);
