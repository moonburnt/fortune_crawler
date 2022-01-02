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

    protected:
        Texture2D* texture;
        ObjectCategory category;

    public:
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
};

class Item: public MapObject {
    public:
        ItemType type;
        Item(ItemType tile_type, Texture2D* sprite);
};

class GameMap {
    private:
        Point map_size;
        Point tile_size;
        int grid_size;
        std::unordered_map<int, MapObject*>map_objects;
        std::vector<std::vector<int>>grid;

    public:
        GameMap(
            Point m_size,
            Point t_size,
            std::unordered_map<int, MapObject*>map_elems,
            std::vector<std::vector<int>>grid_layout
        );

        void update();

        void draw();
};

GameMap* generate_map(Image map_file, Point tile_size);
