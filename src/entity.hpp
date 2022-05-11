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

#include <raylib.h>

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>

enum ENTITY_IDS {
    EID_ABYSS,
    EID_FLOOR,
    EID_PLAYER,
    EID_ENTRANCE,
    EID_EXIT,
    EID_ENEMY,
    EID_CHEST,
    EID_CHEST_EMPTY,
    EID_COIN_PILE,
    EID_BOSS
};

enum class ObjectCategory {
    structure,
    creature
};

enum class Event {
    exit_map,
    loot,
    lockpick,
    fight,
    heal,
    trade,
    trap
};

enum class OffensiveStats {
    // Physical Damage
    pdmg,
    // Ranged Damage
    rdmg,
    // Magical Damage
    mdmg
};

enum class DefensiveStats {
    // Physical Defence
    pdef,
    // Ranged Defence
    rdef,
    // Magical Defence
    mdef
};

class MapObject {
private:
    const Texture2D* texture;

    // These things below can probably be done better. TODO
    // Default description of an object.
    std::string description;
    // Affix that will be added to description if is_inspected is true.
    std::string affix;
    // New description that will be created rom description + affix and returned,
    // if is_inspected is true.
    std::string full_description;

protected:
    // Entity type id. Set on object creation, used during save/load/mapgen cycle.
    int eid;

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
    void set_texture(const Texture2D* texture);

public:
    MapObject(int eid, bool is_obstacle, ObjectCategory cat, std::string desc);
    MapObject(
        int eid,
        bool is_obstacle,
        ObjectCategory cat,
        std::string desc,
        const Texture2D* sprite);

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

    int get_entity_id();
};

class Structure : public MapObject {
public:
    Structure(int eid, bool is_obstacle, std::string desc, const Texture2D* sprite);
    Structure(int eid, bool is_obstacle, std::string desc);

    static Structure* make_exit(int eid, const Texture2D* sprite);
};

class Treasure : public Structure {
private:
    // Texture for looted treasure
    const Texture2D* empty_texture;
    // Lock state of treasure.
    bool _is_locked;
    // Amount of money inside chest. Added on init.
    int money_amount;

    // Lock treasure and change player collision event to Event::lockpick
    void lock();

    Treasure(
        int eid,
        bool lock_state,
        int money_amount,
        const Texture2D* normal_sprite,
        const Texture2D* empty_sprite);

    Treasure(int eid, const Texture2D* sprite);

    Treasure(int eid, int money_amount, const Texture2D* sprite);

public:
    bool destroy_on_empty;

    static Treasure* make_chest(
        int eid,
        bool lock_state,
        int money_amount,
        const Texture2D* normal_sprite,
        const Texture2D* empty_sprite);

    static Treasure* make_empty_chest(int eid, const Texture2D* sprite);

    static Treasure* make_coin_pile(int eid, int money_amount, const Texture2D* sprite);

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
        int eid,
        bool is_player,
        bool is_obstacle,
        int hp,
        std::unordered_map<OffensiveStats, int> offensive_stats,
        std::unordered_map<DefensiveStats, int> defensive_stats,
        std::string desc,
        const Texture2D* sprite);

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
    Player(int eid, const Texture2D* sprite);
};

class Enemy : public Creature {
private:
    bool _is_boss;
    Enemy(
        int eid,
        bool is_boss,
        int hp,
        std::unordered_map<OffensiveStats, int> offensive_stats,
        std::unordered_map<DefensiveStats, int> defensive_stats,
        std::string desc,
        const Texture2D* sprite);

public:
    static Enemy* make_enemy(int eid, int stats_multiplier, const Texture2D* sprite);
    static Enemy* make_boss(int eid, int stats_multiplier, const Texture2D* sprite);
    bool is_boss();
};

// Give random stats of specified level/multiplier.
std::tuple<
    int,
    std::unordered_map<OffensiveStats, int>,
    std::unordered_map<DefensiveStats, int>>
give_random_stats(int multiplier);
