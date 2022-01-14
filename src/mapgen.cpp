#include "mapgen.hpp"
#include "entity.hpp"
#include "loader.hpp"
#include "raylib.h"
#include "settings.hpp"
#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO

static constexpr Color GRID_COLOR{63, 63, 116, 255};

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
        static_cast<Creature*>(object)->is_player()) {
        player_id = map_objects_amount;
    }
    else if (
        object->get_category() == ObjectCategory::structure &&
        object->get_player_collision_event() == Event::exit_map) {
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
    return std::clamp(static_cast<size_t>(pos.y * map_size.x + pos.x), 0lu, grid_size);
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
    return std::clamp(static_cast<size_t>(y * map_size.x + x), 0lu, grid_size);
}

bool GameMap::is_vec_on_map(Vector2 vec) {
    return (
        (0 < vec.x) && (vec.x < map_real_size.x) && (0 < vec.y) &&
        (vec.y < map_real_size.y));
}

std::optional<Point> GameMap::find_object_tile(int object_id) {
    for (auto index = 0u; index < grid_size; index++) {
        for (auto tile_i = 0u; tile_i < grid[index].size(); tile_i++) {
            if (grid[index][tile_i] == object_id) {
                return index_to_tile(index);
            }
        }
    }

    return std::nullopt;
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

    // This may be moved to for
    int index = tile_to_index(tile);

    for (auto item : grid[index]) {
        if (map_objects[item]->is_obstacle()) return true;
    }
    return false;
}

bool GameMap::is_tile_occupied(int grid_index) {
    return (grid[grid_index].size() > 1);
}

bool GameMap::is_tile_occupied(Point tile) {
    return is_tile_occupied(tile_to_index(tile));
}

std::optional<int> GameMap::find_object_in_tile(int grid_index, int object_id) {
    std::vector<int>::iterator object_iterator;
    object_iterator =
        std::find(grid[grid_index].begin(), grid[grid_index].end(), object_id);

    if (object_iterator != grid[grid_index].end()) {
        return std::distance(grid[grid_index].begin(), object_iterator);
    }
    return std::nullopt;
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

    int abyss_id = gm->add_object(new Structure(true, "Abyss"));
    int floor_id = gm->add_object(
        new Structure(false, "Floor", &AssetLoader::loader.sprites["floor_tile"]));

    int grid_index = 0;
    for (auto current_y = 0; current_y < map_size.y; current_y++) {
        for (auto current_x = 0; current_x < map_size.x; current_x++) {
            int pix_color = ColorToInt(GetImageColor(map_file, current_x, current_y));

            if (pix_color == ColorToInt(Color{203, 219, 252, 255})) {
                gm->place_object(grid_index, floor_id);
            }
            else if (pix_color == ColorToInt(Color{0, 255, 9, 255})) {
                gm->place_object(grid_index, floor_id);
                gm->add_object(
                    new Structure(
                        false,
                        "Entrance",
                        Event::nothing,
                        Event::nothing,
                        &AssetLoader::loader.sprites["entrance_tile"]),
                    grid_index);
                gm->add_object(player_object, grid_index);
            }
            else if (pix_color == ColorToInt(Color{0, 242, 255, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Structure(
                        false,
                        "Exit",
                        Event::exit_map,
                        Event::nothing,
                        &AssetLoader::loader.sprites["exit_tile"]),
                    grid_index);
            }
            else if (pix_color == ColorToInt(Color{255, 0, 0, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Enemy(false, &AssetLoader::loader.sprites["enemy_tile"]),
                    grid_index);
            }
            else if (pix_color == ColorToInt(Color{255, 233, 0, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Structure(
                        false,
                        "Treasure",
                        Event::nothing,
                        Event::nothing,
                        &AssetLoader::loader.sprites["treasure_tile"]),
                    grid_index);
            }
            else if (pix_color == ColorToInt(Color{199, 0, 255, 255})) {
                gm->place_object(grid_index, floor_id);

                gm->add_object(
                    new Enemy(true, &AssetLoader::loader.sprites["boss_tile"]),
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
    MapObject* player_object = new Player(&AssetLoader::loader.sprites["player_tile"]);

    return generate_map(map_file, tile_size, player_object);
}
