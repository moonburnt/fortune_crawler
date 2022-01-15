#pragma once

#include <raylib.h>

#include <optional>
#include <string>

enum class ObjectCategory
{
    structure,
    creature
};

enum class Event
{
    nothing,
    exit_map,
    loot,
    fight
};

class MapObject {
private:
    std::optional<Texture2D*> texture;

protected:
    ObjectCategory category;
    Event player_collision_event;
    // This theoretically also affects boss, but it doesn't move so whatever
    Event enemy_collision_event;

    // If set to true - prevents from passing this tile.
    bool _is_obstacle;

public:
    std::string description; // TODO: move to protected, add getter
    ObjectCategory get_category();
    Event get_player_collision_event();
    Event get_enemy_collision_event();
    MapObject(bool is_obstacle, ObjectCategory cat, std::string desc);
    MapObject(bool is_obstacle, ObjectCategory cat, std::string desc, Texture2D* sprite);
    MapObject(
        bool is_obstacle,
        ObjectCategory cat,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite);
    MapObject(
        bool is_obstacle,
        ObjectCategory cat,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event);

    // Returns _is_obstacle
    bool is_obstacle();

    // Draw
    void draw(Vector2 pos);
};

class Structure : public MapObject {
public:
    Structure(
        bool is_obstacle,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event,
        Texture2D* sprite);
    Structure(
        bool is_obstacle,
        std::string desc,
        Event player_collision_event,
        Event enemy_collision_event);
    Structure(bool is_obstacle, std::string desc, Texture2D* sprite);
    Structure(bool is_obstacle, std::string desc);
};

class Treasure : public Structure {
private:
    // Lock state of treasure. If yes - player will be forced to play lockpick
    // minigame, in order to get reward. TODO.
    // bool _is_locked;
    int money_amount;

public:
    Treasure(int money_amount, Texture2D* sprite);
    // Return money_amount and set it to 0.
    int get_reward();
    // bool is_locked();
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
    int money_amount;
    Player(Texture2D* sprite);
};

class Enemy : public Creature {
private:
    bool _is_boss;

public:
    Enemy(bool is_boss, Texture2D* sprite);

    bool is_boss();
};
