#pragma once

#include <raylib.h>

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>

enum class ObjectCategory
{
    structure,
    creature
};

enum class Event
{
    exit_map,
    loot,
    lockpick,
    fight
};

enum class OffensiveStats
{
    // Physical Damage
    pdmg,
    // Ranged Damage
    rdmg,
    // Magical Damage
    mdmg
};

enum class DefensiveStats
{
    // Physical Defence
    pdef,
    // Ranged Defence
    rdef,
    // Magical Defence
    mdef
};

class MapObject {
private:
    Texture2D* texture;

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
    std::optional<Event> player_collision_event;
    std::optional<Event> enemy_collision_event;

    // If set to true - prevents from passing this tile.
    bool _is_obstacle;

    // Inspection state of object. If player has interacted with it at least
    // once - will be set to true.
    bool is_inspected;

    // Set description. Overrides both description and full_description.
    void set_description(std::string desc);

    // Change/set affix'es text and update full_description.
    void set_affix(std::string affix);

    // Change/set object's texture.
    void set_texture(Texture2D* texture);

public:
    MapObject(bool is_obstacle, ObjectCategory cat, std::string desc);
    MapObject(bool is_obstacle, ObjectCategory cat, std::string desc, Texture2D* sprite);

    // Get object's category
    ObjectCategory get_category();

    // Get object's description string
    std::string get_description();

    // Setters for collision events
    void set_player_collision_event(Event event);
    void set_enemy_collision_event(Event event);

    // Getters for collision events
    std::optional<Event> get_player_collision_event();
    std::optional<Event> get_enemy_collision_event();

    // Returns _is_obstacle
    bool is_obstacle();

    // Draw
    void draw(Vector2 pos);
};

class Structure : public MapObject {
public:
    Structure(bool is_obstacle, std::string desc, Texture2D* sprite);
    Structure(bool is_obstacle, std::string desc);
};

class Treasure : public Structure {
private:
    // Texture for looted treasure
    Texture2D* empty_texture;
    // Lock state of treasure.
    bool _is_locked;
    // Amount of money inside chest. Added on init.
    int money_amount;
    // Lock treasure and change player collision event to Event::lockpick
    void lock();

    Treasure(
        bool lock_state,
        int money_amount,
        Texture2D* normal_sprite,
        Texture2D* empty_sprite);

    Treasure(Texture2D* sprite);

public:
    static Treasure* make_chest(
        bool lock_state,
        int money_amount,
        Texture2D* normal_sprite,
        Texture2D* empty_sprite);

    static Treasure* make_empty_chest(Texture2D* sprite);

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
    // These are public solely coz player's ui require it
    int current_hp;
    int max_hp;
    std::unordered_map<OffensiveStats, int> offensive_stats;
    std::unordered_map<DefensiveStats, int> defensive_stats;

    Creature(
        bool is_player,
        bool is_obstacle,
        int hp,
        std::unordered_map<OffensiveStats, int> offensive_stats,
        std::unordered_map<DefensiveStats, int> defensive_stats,
        std::string desc,
        Texture2D* sprite);

    // Returns true if its player, false otherwise
    bool is_player();

    // Returns true if hp <= 0, false otherwise
    bool is_dead();

    // Damage creature. Will deal no damage if creature is already dead.
    // Returns actual amount of damage applied to creature, after its defences.
    int damage(int dmg_amount, OffensiveStats dmg_type);
    // Heal creature for specified amount of hp, just not over the max_hp
    void heal(int amount);
    // Increase max hp by provided value. If heal is set to true - will also
    // increase current_hp to new max_hp value.
    void increase_max_hp(int amount, bool heal);
    // Increase max stat amount by provided value.
    void increase_stat(int amount, OffensiveStats stat);
    void increase_stat(int amount, DefensiveStats stat);
};

class Player : public Creature {
public:
    int money_amount;
    Player(Texture2D* sprite);
};

class Enemy : public Creature {
private:
    bool _is_boss;
    Enemy(
        bool is_boss,
        int hp,
        std::unordered_map<OffensiveStats, int> offensive_stats,
        std::unordered_map<DefensiveStats, int> defensive_stats,
        std::string desc,
        Texture2D* sprite);

public:
    static Enemy* make_enemy(int stats_multiplier, Texture2D* sprite);
    static Enemy* make_boss(int stats_multiplier, Texture2D* sprite);
    bool is_boss();
};

// Give random stats of specified level/multiplier.
std::tuple<
    int,
    std::unordered_map<OffensiveStats, int>,
    std::unordered_map<DefensiveStats, int>>
give_random_stats(int multiplier);
