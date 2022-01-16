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
    lockpick,
    fight
};

class MapObject {
private:
    std::optional<Texture2D*> texture;

    // These things below can probably be done better. TODO
    // Default description of an object.
    std::string description;
    // Affix that will be added to description if is_inspected is true.
    std::string affix;
    // New description that will be created rom description + affix and returned,
    // if is_inspected is true.
    std::string full_description;

protected:
    ObjectCategory category;
    Event player_collision_event;
    // This theoretically also affects boss, but it doesn't move so whatever
    Event enemy_collision_event;

    // If set to true - prevents from passing this tile.
    bool _is_obstacle;

    // Inspection state of object. If player has interacted with it at least
    // once - will be set to true.
    bool is_inspected;

    // Set description. Overrides both description and full_description.
    void set_description(std::string desc);

    // Change/set affix'es text and update full_description.
    void set_affix(std::string affix);

public:
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

    // Get object's category
    ObjectCategory get_category();

    // Get object's description string
    std::string get_description();

    // Get object's player collision event
    Event get_player_collision_event();

    // Get object's enemy collision event
    Event get_enemy_collision_event();

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
    // Lock state of treasure.
    bool _is_locked;
    // Amount of money inside chest. Added on init.
    int money_amount;
    // Lock treasure and change player collision event to Event::lockpick
    void lock();

public:
    Treasure(bool lock_state, int money_amount, Texture2D* sprite);

    // Return money_amount and set it to 0.
    int get_reward();
    // Return lock state
    bool is_locked();
    // Unlock treasure and change player collision event to Event::loot
    void unlock();
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
