#include "entity.hpp"

#include <optional>
#include <raylib.h>
#include <string>

// Base map object
MapObject::MapObject(
    bool is_obstacle,
    ObjectCategory cat,
    std::string desc,
    Event _player_collision_event,
    Event _enemy_collision_event) {
    _is_obstacle = is_obstacle;
    category = cat;
    description = desc;
    player_collision_event = _player_collision_event;
    enemy_collision_event = _enemy_collision_event;
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

Event MapObject::get_player_collision_event() {
    return player_collision_event;
}

Event MapObject::get_enemy_collision_event() {
    return enemy_collision_event;
}

ObjectCategory MapObject::get_category() {
    return category;
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
Treasure::Treasure(int _money_amount, Texture2D* sprite) :
    Structure(false, "Treasure", Event::loot, Event::nothing, sprite) {
    money_amount = _money_amount;
}

int Treasure::get_reward() {
    int reward = money_amount;
    money_amount = 0;

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
    if (is_boss) {
        description = "Boss";
    }
    else {
        description = "Enemy";
    }

    _is_boss = is_boss;
};

bool Enemy::is_boss() {
    return _is_boss;
};
