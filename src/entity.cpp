#include "entity.hpp"

#include <fmt/core.h>
#include <optional>
#include <raylib.h>
#include <string>

// Base map object
void MapObject::set_description(std::string desc) {
    description = desc;
    full_description = desc;
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
    if (texture) DrawTextureV(*texture.value(), pos, WHITE);
}

// Base Structure/Building
Structure::Structure(bool is_obstacle, std::string desc, Texture2D* sprite)
    : MapObject(is_obstacle, ObjectCategory::structure, desc, sprite) {
}

Structure::Structure(bool is_obstacle, std::string desc)
    : MapObject(is_obstacle, ObjectCategory::structure, desc) {
}

// Treasure / Chest.
Treasure::Treasure(
    bool lock_state, int _money_amount, Texture2D* normal_sprite, Texture2D* empty_sprite)
    : Structure(false, "Treasure", normal_sprite) {
    if (lock_state) lock();
    else unlock();
    money_amount = _money_amount;
    empty_texture = empty_sprite;
}

void Treasure::lock() {
    set_affix("locked");
    _is_locked = true;
    player_collision_event = Event::lockpick;
}

void Treasure::unlock() {
    set_affix("unlocked");
    _is_locked = false;
    player_collision_event = Event::loot;
}

bool Treasure::is_locked() {
    return _is_locked;
}

int Treasure::get_reward() {
    int reward = money_amount;
    money_amount = 0;
    set_affix("empty");
    set_texture(empty_texture);

    return reward;
}

// Base Creature
// For now, there wont be implenetrable creatures, due to how collision
// handling works. May rework this in future. TODO
Creature::Creature(bool is_player, std::string desc, Texture2D* sprite)
    : MapObject(false, ObjectCategory::creature, desc, sprite) {
    _is_player = is_player;
}

bool Creature::is_player() {
    return _is_player;
}

// Player
Player::Player(Texture2D* sprite)
    : Creature(true, "Player", sprite) {
    enemy_collision_event = Event::fight;
    money_amount = 0;
};

// Enemy
// There may be a better way to initialize it
Enemy::Enemy(bool is_boss, Texture2D* sprite)
    : Creature(false, "", sprite) {
    if (is_boss) set_description("Boss");
    else set_description("Enemy");

    player_collision_event = Event::fight;

    _is_boss = is_boss;
};

bool Enemy::is_boss() {
    return _is_boss;
};
