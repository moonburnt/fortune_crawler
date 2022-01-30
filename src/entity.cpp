#include "entity.hpp"

#include <fmt/core.h>
#include <optional>
#include <raylib.h>
#include <string>
#include <tuple>
#include <unordered_map>
// For std::rand()
#include <cstdlib>

// Base map object
void MapObject::set_description(std::string desc) {
    description = desc;
    full_description = desc;
    texture = nullptr;
    // Not initializing affix there, coz full_description wont be re-generated
    // without calling set_affix() anyway. So, I think, its fine to leave it empty?
}

MapObject::MapObject(bool is_obstacle, ObjectCategory cat, std::string desc) {
    _is_obstacle = is_obstacle;
    category = cat;
    set_description(desc);
    is_inspected = false;
}

MapObject::MapObject(
    bool is_obstacle, ObjectCategory cat, std::string desc, Texture2D* sprite)
    : MapObject(is_obstacle, cat, desc) {
    texture = sprite;
}

void MapObject::set_affix(std::string _affix) {
    affix = _affix;
    full_description = fmt::format("{} ({})", description, affix);
}

void MapObject::set_texture(Texture2D* _texture) {
    texture = _texture;
}

void MapObject::set_player_collision_event(Event event) {
    player_collision_event = event;
}

void MapObject::set_enemy_collision_event(Event event) {
    enemy_collision_event = event;
}

std::optional<Event> MapObject::get_player_collision_event() {
    is_inspected = true;
    return player_collision_event;
}

std::optional<Event> MapObject::get_enemy_collision_event() {
    return enemy_collision_event;
}

ObjectCategory MapObject::get_category() {
    return category;
}

std::string MapObject::get_description() {
    if (is_inspected) return full_description;
    return description;
}

bool MapObject::is_obstacle() {
    return _is_obstacle;
}

void MapObject::draw(Vector2 pos) {
    if (texture != nullptr) DrawTextureV(*texture, pos, WHITE);
}

// Base Structure/Building
Structure::Structure(bool is_obstacle, std::string desc, Texture2D* sprite)
    : MapObject(is_obstacle, ObjectCategory::structure, desc, sprite) {
}

Structure::Structure(bool is_obstacle, std::string desc)
    : MapObject(is_obstacle, ObjectCategory::structure, desc) {
}

// Treasure
// Constructor for normal chest
Treasure::Treasure(
    bool lock_state, int _money_amount, Texture2D* normal_sprite, Texture2D* empty_sprite)
    : Structure(true, "Treasure", normal_sprite) {
    money_amount = _money_amount;
    if (lock_state) lock();
    else unlock();
    empty_texture = empty_sprite;
}

// Constructor for empty chest. This will allow to save map state, without having
// to explicitely save all properties of each object.
Treasure::Treasure(Texture2D* sprite)
    : Structure(true, "Treasure", sprite)
    , empty_texture(sprite)
    , _is_locked(false)
    , money_amount(0) {
    set_affix("empty");
    is_inspected = true;
}

Treasure* Treasure::make_chest(
    bool lock_state,
    int money_amount,
    Texture2D* normal_sprite,
    Texture2D* empty_sprite) {
    return new Treasure(lock_state, money_amount, normal_sprite, empty_sprite);
}

Treasure* Treasure::make_empty_chest(Texture2D* sprite) {
    return new Treasure(sprite);
}

void Treasure::lock() {
    // Ensuring its impossible to change a lock state of empty chest
    if (money_amount) {
        set_affix("locked");
        _is_locked = true;
        player_collision_event = Event::lockpick;
    }
}

void Treasure::unlock() {
    // Be careful: ensure that chest never spawns empty
    if (money_amount) {
        set_affix("unlocked");
        _is_locked = false;
        player_collision_event = Event::loot;
    }
}

bool Treasure::is_locked() {
    return _is_locked;
}

int Treasure::get_reward() {
    int reward;
    if (money_amount) {
        reward = money_amount;
        money_amount = 0;
        set_affix("empty");
        set_texture(empty_texture);
        player_collision_event = std::nullopt;
    }
    else {
        reward = 0;
    }

    return reward;
}

// Base Creature
Creature::Creature(
    bool is_player,
    bool is_obstacle,
    int hp,
    std::unordered_map<OffensiveStats, int> _offensive_stats,
    std::unordered_map<DefensiveStats, int> _defensive_stats,
    std::string desc,
    Texture2D* sprite)
    : MapObject(is_obstacle, ObjectCategory::creature, desc, sprite)
    , _is_player(is_player)
    , current_hp(hp)
    , max_hp(hp)
    , offensive_stats(_offensive_stats)
    , defensive_stats(_defensive_stats) {
}

bool Creature::is_player() {
    return _is_player;
}

bool Creature::is_dead() {
    return (current_hp <= 0);
}

int Creature::damage(int dmg_amount, OffensiveStats dmg_type) {
    if (is_dead()) return 0;

    // Actual amount of damage, dealt to creature after all defences.
    int clear_dmg;

    switch (dmg_type) {
    case OffensiveStats::pdmg: {
        // damage amount cant be nullified below 1
        clear_dmg = std::max(dmg_amount - defensive_stats[DefensiveStats::pdef], 1);
        break;
    }
    case OffensiveStats::rdmg: {
        clear_dmg = std::max(dmg_amount - defensive_stats[DefensiveStats::rdef], 1);
        break;
    }
    case OffensiveStats::mdmg: {
        clear_dmg = std::max(dmg_amount - defensive_stats[DefensiveStats::mdef], 1);
        break;
    }
    }

    current_hp -= clear_dmg;

    return clear_dmg;
}

void Creature::heal(int amount) {
    // This can be weird with negative healing numbers, but why would you do that?
    current_hp = std::min(current_hp + amount, max_hp);
}

void Creature::increase_max_hp(int amount, bool heal) {
    max_hp += amount;
    if (heal) {
        current_hp = max_hp;
    }
}

void Creature::increase_stat(int amount, OffensiveStats stat) {
    offensive_stats[stat] += amount;
}

void Creature::increase_stat(int amount, DefensiveStats stat) {
    defensive_stats[stat] += amount;
}

// Player
// For now, starting stats will be hardcoded
Player::Player(Texture2D* sprite)
    : Creature(
          true,
          false,
          100,
          // I think we can initialize map like that?
          {{OffensiveStats::pdmg, 10},
           {OffensiveStats::rdmg, 10},
           {OffensiveStats::mdmg, 10}},
          {{DefensiveStats::pdef, 10},
           {DefensiveStats::rdef, 10},
           {DefensiveStats::mdef, 10}},
          "Player",
          sprite) {
    enemy_collision_event = Event::fight;
    money_amount = 0;
}

// Enemy
std::tuple<
    int,
    std::unordered_map<OffensiveStats, int>,
    std::unordered_map<DefensiveStats, int>>
give_random_stats(int multiplier) {
    // Values are temporary, will play with them later.
    int stats_amount = 10 + 5 * multiplier;

    // All stats but hp are calculated randomly, from general stats amount
    int stats[6];
    for (int i = 0; i < 6; i++) {
        if (stats_amount > 0) {
            stats[i] = rand() % stats_amount;
            stats_amount -= stats[i];
        }
        else {
            stats[i] = 0;
        }
    }

    return std::make_tuple(
        // Max hp will be about 20, + 10 per dungeon level
        20 + 10 * multiplier,
        std::unordered_map<OffensiveStats, int>{
            {OffensiveStats::pdmg, stats[0]},
            {OffensiveStats::rdmg, stats[1]},
            {OffensiveStats::mdmg, stats[2]}},
        std::unordered_map<DefensiveStats, int>{
            {DefensiveStats::pdef, stats[3]},
            {DefensiveStats::rdef, stats[4]},
            {DefensiveStats::mdef, stats[5]}});
}

Enemy::Enemy(
    bool boss,
    int hp,
    std::unordered_map<OffensiveStats, int> offensive_stats,
    std::unordered_map<DefensiveStats, int> defensive_stats,
    std::string desc,
    Texture2D* sprite)
    : Creature(false, true, hp, offensive_stats, defensive_stats, desc, sprite)
    , _is_boss(boss) {
    player_collision_event = Event::fight;
}

Enemy* Enemy::make_enemy(int stats_multiplier, Texture2D* sprite) {
    auto [hp, offensive, defensive] = give_random_stats(stats_multiplier);
    return new Enemy(false, hp, offensive, defensive, "Enemy", sprite);
}

Enemy* Enemy::make_boss(int stats_multiplier, Texture2D* sprite) {
    auto [hp, offensive, defensive] = give_random_stats(stats_multiplier * 2);
    return new Enemy(true, hp, offensive, defensive, "Boss", sprite);
}

bool Enemy::is_boss() {
    return _is_boss;
}
