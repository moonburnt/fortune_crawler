#include "entity.hpp"

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

MapObject::MapObject(
    bool is_obstacle,
    ObjectCategory cat,
    std::string desc,
    Event _player_collision_event,
    Event _enemy_collision_event) {
    _is_obstacle = is_obstacle;
    category = cat;
    set_description(desc);
    player_collision_event = _player_collision_event;
    enemy_collision_event = _enemy_collision_event;
    is_inspected = false;
}

MapObject::MapObject(bool is_obstacle, ObjectCategory cat, std::string desc)
    : MapObject(is_obstacle, cat, desc, Event::nothing, Event::nothing) {
}

MapObject::MapObject(
    bool is_obstacle,
    ObjectCategory cat,
    std::string desc,
    Event _player_collision_event,
    Event _enemy_collision_event,
    Texture2D* sprite)
    : MapObject(is_obstacle, cat, desc, _player_collision_event, _enemy_collision_event) {
    texture = sprite;
}

MapObject::MapObject(
    bool is_obstacle, ObjectCategory cat, std::string desc, Texture2D* sprite)
    : MapObject(is_obstacle, cat, desc, Event::nothing, Event::nothing, sprite) {
}

void MapObject::set_affix(std::string _affix) {
    // This is a really messy way of doing it, but gcc doesn't have <format> yet.
    // TODO: bump project's cpp to c++20, rework this to std::format.
    affix = _affix;
    full_description = TextFormat("%s (%s)", description.c_str(), affix.c_str());
}

void MapObject::set_texture(Texture2D* _texture) {
    texture = _texture;
}

Event MapObject::get_player_collision_event() {
    is_inspected = true;
    return player_collision_event;
}

Event MapObject::get_enemy_collision_event() {
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
Structure::Structure(
    bool is_obstacle,
    std::string desc,
    Event player_collision_event,
    Event enemy_collision_event,
    Texture2D* sprite)
    : MapObject(
          is_obstacle,
          ObjectCategory::structure,
          desc,
          player_collision_event,
          enemy_collision_event,
          sprite) {
}

Structure::Structure(
    bool is_obstacle,
    std::string desc,
    Event player_collision_event,
    Event enemy_collision_event)
    : MapObject(
          is_obstacle,
          ObjectCategory::structure,
          desc,
          player_collision_event,
          enemy_collision_event) {
}

Structure::Structure(bool is_obstacle, std::string desc, Texture2D* sprite)
    : Structure(is_obstacle, desc, Event::nothing, Event::nothing, sprite) {
}

Structure::Structure(bool is_obstacle, std::string desc)
    : Structure(is_obstacle, desc, Event::nothing, Event::nothing) {
}

// Treasure / Chest.
Treasure::Treasure(
    bool lock_state, int _money_amount, Texture2D* normal_sprite, Texture2D* empty_sprite)
    : Structure(false, "Treasure", Event::loot, Event::nothing, normal_sprite) {
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
Creature::Creature(
    bool is_player,
    std::string desc,
    Event player_collision_event,
    Event enemy_collision_event,
    Texture2D* sprite)
    : MapObject(
          // For now, there wont be implenetrable creatures, due to how collision
          // handling works. May rework this in future. TODO
          false,
          ObjectCategory::creature,
          desc,
          player_collision_event,
          enemy_collision_event,
          sprite) {
    _is_player = is_player;
}

bool Creature::is_player() {
    return _is_player;
}

// Player
Player::Player(Texture2D* sprite)
    : Creature(true, "Player", Event::nothing, Event::fight, sprite) {
    money_amount = 0;
};

// Enemy
// There may be a better way to initialize it
Enemy::Enemy(bool is_boss, Texture2D* sprite)
    : Creature(false, "", Event::fight, Event::nothing, sprite) {
    if (is_boss) set_description("Boss");
    else set_description("Enemy");

    _is_boss = is_boss;
};

bool Enemy::is_boss() {
    return _is_boss;
};
