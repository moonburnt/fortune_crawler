#include "mapgen.hpp"
#include "loader.hpp"
#include "raylib.h"
#include "settings.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO

static constexpr Color GRID_COLOR{63, 63, 116, 255};

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
    CreatureType tile_type,
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
    type = tile_type;
}

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

GameMap::GameMap(Point _map_size, Point _tile_size)
    : map_size(_map_size)
    , tile_size(_tile_size) {
    grid_size = map_size.x * map_size.y;
    map_real_size = Vector2{
        static_cast<float>(map_size.x * tile_size.x),
        static_cast<float>(map_size.y * tile_size.y)};
    map_objects_amount = 0;

    // There may be a better way to do that
    grid = {};
    for (size_t i = 0; i < grid_size; i++) {
        grid.push_back({});
    };

    has_selected_pos = false;
}

void GameMap::place_object(int grid_index, int object_id) {
    grid[grid_index].push_back(object_id);
}

int GameMap::add_object(MapObject* object) {
    map_objects[map_objects_amount] = object;

    // some additional checks, that may be removed from here later
    if (object->get_category() == ObjectCategory::creature &&
        static_cast<Creature*>(object)->type == CreatureType::player) {
        player_id = map_objects_amount;
    }
    else if (
        object->get_category() == ObjectCategory::item &&
        static_cast<Item*>(object)->type == ItemType::exit) {
        exit_id = map_objects_amount;
    }

    map_objects_amount++;
    return map_objects_amount - 1;
}

int GameMap::add_object(MapObject* object, int grid_index) {
    int object_id = add_object(object);
    place_object(grid_index, object_id);
    return object_id;
}

void GameMap::delete_object(int grid_index, int tile_index, bool delete_from_storage) {
    if (delete_from_storage) {
        map_objects.erase(grid[grid_index][tile_index]);
    }
    grid[grid_index].erase(grid[grid_index].begin() + tile_index);
}

void GameMap::move_object(int grid_index, int tile_index, int new_grid_index) {
    int object_id = grid[grid_index][tile_index];
    delete_object(grid_index, tile_index, false);
    place_object(new_grid_index, object_id);
}

int GameMap::get_player_id() {
    return player_id;
}

int GameMap::get_exit_id() {
    return exit_id;
}

std::vector<int>* GameMap::get_tile_content(size_t grid_index) {
    // Protection against out-of-bounds index values
    grid_index = std::clamp(grid_index, 0lu, grid_size);

    return &grid[grid_index];
}

std::vector<std::string> GameMap::get_tile_descriptions(size_t grid_index) {
    grid_index = std::clamp(grid_index, 0lu, grid_size);

    std::vector<std::string> descriptions;

    for (auto item : grid[grid_index]) {
        descriptions.push_back(map_objects[item]->description);
    }
    return descriptions;
}

Point GameMap::index_to_tile(size_t index) {
    index = std::clamp(index, 0ul, grid_size);

    int y = index / map_size.x;
    int x = index % map_size.x;
    return Point{x, y};
}

int GameMap::tile_to_index(Point pos) {
    return std::clamp(static_cast<size_t>(pos.x * map_size.x + pos.y), 0lu, grid_size);
}

Point GameMap::vec_to_tile(Vector2 vec) {
    return Point{
        static_cast<int>(vec.x) / tile_size.x,
        static_cast<int>(vec.y) / tile_size.y};
}

Vector2 GameMap::tile_to_vec(Point tile) {
    return Vector2{
        static_cast<float>(tile.x * tile_size.x),
        static_cast<float>(tile.y * tile_size.y)};
}

Vector2 GameMap::index_to_vec(size_t index) {
    auto tile = index_to_tile(index);
    return Vector2{
        static_cast<float>(tile.x * tile_size.x),
        static_cast<float>(tile.y * tile_size.y)};
}

int GameMap::vec_to_index(Vector2 vec) {
    int x = vec.x / tile_size.x;
    int y = vec.y / tile_size.y;
    return std::clamp(static_cast<size_t>(x * map_size.x + y), 0lu, grid_size);
}

bool GameMap::is_vec_on_map(Vector2 vec) {
    return (
        (0 < vec.x) && (vec.x < map_real_size.x) && (0 < vec.y) &&
        (vec.y < map_real_size.y));
}

Point GameMap::get_player_tile() {
    // Same possible caveats as in get_player_id()
    int player_tile_index;

    int player_id = get_player_id();

    bool player_found = false;

    for (auto index = 0u; index < grid_size; index++) {
        if (player_found) break;
        for (auto tile_i = 0u; tile_i < grid[index].size(); tile_i++) {
            if (player_found) break;
            if (grid[index][tile_i] == player_id) {
                player_tile_index = index;
                player_found = true;
                break;
            }
        }
    }

    return index_to_tile(player_tile_index);
}

Point GameMap::get_tile_size() {
    return tile_size;
}

Point GameMap::get_map_size() {
    return map_size;
}

int GameMap::get_tile_elements_amount(int grid_index) {
    return grid[grid_index].size();
}

MapObject* GameMap::get_object(int object_id) {
    return map_objects[object_id];
}

MapObject* GameMap::get_object(int grid_index, int tile_index) {
    return get_object(grid[grid_index][tile_index]);
}

bool GameMap::is_tile_blocked(Point tile) {
    // Safety check to ensure that requested tile is not out of bounds
    // May move it to some other place later
    if ((tile.x < 0) || (tile.y < 0) || (tile.x >= map_size.x) || (tile.y >= map_size.y))
        return true;
    int index = tile_to_index(tile);

    for (auto item : grid[index]) {
        if (map_objects[item]->get_category() == ObjectCategory::obstacle) return true;
        else if (
            map_objects[item]->get_category() == ObjectCategory::floor &&
            static_cast<Floor*>(map_objects[item])->type == FloorType::abyss)
            return true;
    }
    return false;
}

bool GameMap::is_tile_occupied(int grid_index) {
    return (grid[grid_index].size() > 1);
}

bool GameMap::is_tile_occupied(Point tile) {
    return is_tile_occupied(tile_to_index(tile));
}

bool GameMap::object_in_tile(int grid_index, int object_id, int* tile_index) {
    std::vector<int>::iterator object_iterator;
    object_iterator =
        std::find(grid[grid_index].begin(), grid[grid_index].end(), object_id);
    // Overwriting value of variable, to which tile_index points
    *tile_index = std::distance(grid[grid_index].begin(), object_iterator);

    if (object_iterator == grid[grid_index].end()) return false;
    return true;
}

Event GameMap::get_tile_event(int grid_index, bool is_player_event) {
    // Event tile_event;
    // if (is_player_event) {
    //     for (auto i: grid[grid_index]) {
    //         tile_event = map_objects[i]->get_player_collision_event();
    //     }
    // }
    // else {
    //     for (auto i: grid[grid_index]) {
    //         tile_event = map_objects[i]->get_enemy_collision_event();
    //     }
    // }
    // return tile_event;
    if (is_player_event)
        return map_objects[grid[grid_index].back()]->get_player_collision_event();

    return map_objects[grid[grid_index].back()]->get_enemy_collision_event();
}

void GameMap::select_tile(Point tile) {
    selected_pos.x = tile.x * tile_size.x;
    selected_pos.y = tile.y * tile_size.y;
    has_selected_pos = true;
}

void GameMap::deselect_tile() {
    has_selected_pos = false;
}

void GameMap::draw() {
    DrawRectangle(0, 0, map_real_size.x, map_real_size.y, RAYWHITE);

    for (auto current_tile = 0u; current_tile < grid_size; current_tile++) {
        for (auto item : grid[current_tile]) {
            map_objects[item]->draw(index_to_vec(current_tile));
        }
        if (SettingsManager::manager.get_show_grid()) {
            Vector2 vec = index_to_vec(current_tile);
            DrawRectangleLines(vec.x, vec.y, tile_size.x, tile_size.y, GRID_COLOR);
        }
    }

    if (has_selected_pos)
        DrawRectangleLines(
            selected_pos.x,
            selected_pos.y,
            tile_size.x,
            tile_size.y,
            BLACK);
}

GameMap* generate_map(Image map_file, Point tile_size, MapObject* player_object) {
    Point map_size = {map_file.width, map_file.height};

    GameMap* gm = new GameMap(map_size, tile_size);

    int abyss_id = gm->add_object(new Floor());
    int floor_id = gm->add_object(
        new Floor(FloorType::floor, "floor", &AssetLoader::loader.sprites["floor_tile"]));

    int grid_index = 0;
    for (auto current_x = 0; current_x < map_size.x; current_x++) {
        for (auto current_y = 0; current_y < map_size.y; current_y++) {
            int pix_color = ColorToInt(GetImageColor(map_file, current_x, current_y));

            if (pix_color == ColorToInt(Color{203, 219, 252, 255})) {
                gm->place_object(grid_index, floor_id);
            }
            else if (pix_color == ColorToInt(Color{0, 255, 9, 255})) {
                gm->place_object(grid_index, floor_id);
                gm->add_object(
                    new Item(
                        ItemType::entrance,
                        "entrance",
                        Event::nothing,
                        Event::nothing,
                        &AssetLoader::loader.sprites["entrance_tile"]),
                    grid_index);
                gm->add_object(player_object, grid_index);
            }
            else if (pix_color == ColorToInt(Color{0, 242, 255, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Item(
                        ItemType::exit,
                        "exit",
                        Event::exit_map,
                        Event::nothing,
                        &AssetLoader::loader.sprites["exit_tile"]),
                    grid_index);
            }
            else if (pix_color == ColorToInt(Color{255, 0, 0, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Creature(
                        CreatureType::enemy,
                        "enemy",
                        Event::fight,
                        Event::nothing,
                        &AssetLoader::loader.sprites["enemy_tile"]),
                    grid_index);
            }
            else if (pix_color == ColorToInt(Color{255, 233, 0, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Item(
                        ItemType::treasure,
                        "treasure",
                        Event::nothing,
                        Event::nothing,
                        &AssetLoader::loader.sprites["treasure_tile"]),
                    grid_index);
            }
            else if (pix_color == ColorToInt(Color{199, 0, 255, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Creature(
                        CreatureType::boss,
                        "boss",
                        Event::fight,
                        Event::nothing,
                        &AssetLoader::loader.sprites["boss_tile"]),
                    grid_index);
            }
            else {
                gm->place_object(grid_index, abyss_id);
            }

            grid_index++;
        }
    }

    return gm;
}

GameMap* generate_map(Image map_file, Point tile_size) {
    MapObject* player_object = new Creature(
        CreatureType::player,
        "player",
        Event::nothing,
        Event::fight,
        &AssetLoader::loader.sprites["player_tile"]);

    return generate_map(map_file, tile_size, player_object);
}
