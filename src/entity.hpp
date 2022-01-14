#pragma once

#include <raylib.h>

#include <string>

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

enum class ItemType
{
    entrance,
    exit,
    treasure,
    trap
};

enum class Event
{
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
        Texture2D* sprite);
    // The same as previous, but same collision event will get assigned to both
    MapObject(
        ObjectCategory cat, std::string desc, Event collision_event, Texture2D* sprite);

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
protected:
    bool _is_player;

public:
    Creature(
        bool is_player,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite);

    void update();

    // Returns true if its player, false otherwise
    bool is_player();
};

class Player : public Creature {
public:
    Player(Texture2D* sprite);
};

class Enemy : public Creature {
private:
    bool _is_boss;

public:
    Enemy(bool is_boss, Texture2D* sprite);

    bool is_boss();
};

class Item : public MapObject {
public:
    ItemType type;
    Item(
        ItemType tile_type,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite);
};
