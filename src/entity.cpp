#include "entity.hpp"

#include <raylib.h>
#include <string>

MapObject::MapObject(ObjectCategory cat, std::string desc) {
    category = cat;
    description = desc;
    has_texture = false;
    player_collision_event = Event::nothing;
    enemy_collision_event = Event::nothing;
}

MapObject::MapObject(ObjectCategory cat, std::string desc, Texture2D* sprite) {
    category = cat;
    description = desc;
    texture = sprite;
    has_texture = true;
    player_collision_event = Event::nothing;
    enemy_collision_event = Event::nothing;
}

MapObject::MapObject(
    ObjectCategory cat,
    std::string desc,
    Event _player_collision_event,
    Event _enemy_collision_event,
    Texture2D* sprite) {
    category = cat;
    description = desc;
    texture = sprite;
    has_texture = true;
    player_collision_event = _player_collision_event;
    enemy_collision_event = _enemy_collision_event;
}

MapObject::MapObject(
    ObjectCategory cat, std::string desc, Event collision_event, Texture2D* sprite) {
    category = cat;
    description = desc;
    texture = sprite;
    has_texture = true;
    player_collision_event = collision_event;
    enemy_collision_event = collision_event;
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

void MapObject::draw(Vector2 pos) {
    if (has_texture) DrawTextureV(*texture, pos, WHITE);
}

Floor::Floor()
    : MapObject(ObjectCategory::floor, "abyss") {
    type = FloorType::abyss;
}

Floor::Floor(FloorType tile_type, std::string desc, Texture2D* sprite)
    : MapObject(ObjectCategory::floor, desc, sprite) {
    type = tile_type;
}

Creature::Creature(
    bool is_player,
    std::string desc,
    Event player_collision_event,
    Event enemy_collision_event,
    Texture2D* sprite)
    : MapObject(
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

Player::Player(Texture2D* sprite)
    : Creature(true, "Player", Event::nothing, Event::fight, sprite){};

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

Item::Item(
    ItemType tile_type,
    std::string desc,
    Event player_collision_event,
    Event enemy_collision_event,
    Texture2D* sprite)
    : MapObject(
          ObjectCategory::item,
          desc,
          player_collision_event,
          enemy_collision_event,
          sprite) {
    type = tile_type;
}
