#include "mapgen.hpp"
#include "engine/utility.hpp"
#include "app.hpp"
#include "entity.hpp"
#include "loader.hpp"
#include "raylib.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO
static constexpr Color GRID_COLOR{63, 63, 116, 255};
static constexpr Color HIGHLIGHT_COLOR{255, 255, 255, 255};
static constexpr Color ABYSS_COLOR{0, 0, 0, 255};

// Things that can't be spawned via color (e.g abyss and player) are not there
static const int FLOOR_COLOR = ColorToInt(Color{203, 219, 252, 255});
static const std::map<int, int> VALID_COLORS = {
    {ColorToInt(Color{0, 255, 9, 255}), EID_ENTRANCE},
    {ColorToInt(Color{0, 242, 255, 255}), EID_EXIT},
    {ColorToInt(Color{255, 0, 0, 255}), EID_ENEMY},
    {ColorToInt(Color{255, 233, 0, 255}), EID_CHEST},
    {ColorToInt(Color{211, 83, 50, 255}), EID_CHEST_EMPTY},
    {ColorToInt(Color{255, 185, 112, 255}), EID_COIN_PILE},
    {ColorToInt(Color{199, 0, 255, 255}), EID_BOSS}};

GameMap::GameMap(Point _map_size, Point _tile_size, bool show_grid)
    : map_size(_map_size)
    , tile_size(_tile_size)
    , show_grid(show_grid) {
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

std::vector<int>* GameMap::get_tile_content(size_t grid_index) {
    // Protection against out-of-bounds index values
    // I HATE PLATFORM-SPECIFIC SIZES, I HATE PLATFORM-SPECIFIC SIZES
    grid_index = std::clamp(grid_index, static_cast<size_t>(0), grid_size - 1);

    return &grid[grid_index];
}

std::vector<std::string> GameMap::get_tile_descriptions(size_t grid_index) {
    grid_index = std::clamp(grid_index, static_cast<size_t>(0), grid_size - 1);

    std::vector<std::string> descriptions;

    for (auto item : grid[grid_index]) {
        descriptions.push_back(map_objects[item]->get_description());
    }
    return descriptions;
}

Point GameMap::index_to_tile(size_t index) {
    int y = index / map_size.x;
    int x = index % map_size.x;
    return Point{x, y};
}

int GameMap::tile_to_index(Point pos) {
    return static_cast<size_t>(pos.y * map_size.x + pos.x);
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
    return static_cast<size_t>(y * map_size.x + x);
}

bool GameMap::is_vec_on_map(Vector2 vec) {
    return (
        (0 <= vec.x) && (vec.x < map_real_size.x) && (0 <= vec.y) &&
        (vec.y < map_real_size.y));
}

bool GameMap::is_index_on_map(size_t grid_index) {
    return grid_index < grid_size;
}

bool GameMap::is_tile_on_map(Point tile) {
    return (
        (0 <= tile.x) && (tile.x < map_size.x) && (0 <= tile.y) && (tile.y < map_size.y));
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

size_t GameMap::get_grid_size() {
    return grid_size;
}

std::vector<std::vector<int>> GameMap::get_map_layout() {
    std::vector<std::vector<int>> layout = {};

    for (size_t grid_id = 0; grid_id < grid_size; grid_id++) {
        std::vector<int> tile_layout = {};
        for (size_t tile_id = 0; tile_id < grid[grid_id].size(); tile_id++) {
            tile_layout.push_back(map_objects[grid[grid_id][tile_id]]->get_entity_id());
        }
        layout.push_back(tile_layout);
    }

    return layout;
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

std::vector<std::tuple<int, Event>> GameMap::get_player_events(int grid_index) {
    std::vector<std::tuple<int, Event>> tile_events;

    for (auto i : grid[grid_index]) {
        auto event = map_objects[i]->get_player_collision_event();
        if (event) tile_events.push_back(std::make_tuple(i, event.value()));
    }

    return tile_events;
}

std::vector<std::tuple<int, Event>> GameMap::get_enemy_events(int grid_index) {
    std::vector<std::tuple<int, Event>> tile_events;

    for (auto i : grid[grid_index]) {
        auto event = map_objects[i]->get_enemy_collision_event();
        if (event) tile_events.push_back(std::make_tuple(i, event.value()));
    }

    return tile_events;
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
    DrawRectangle(0, 0, map_real_size.x, map_real_size.y, ABYSS_COLOR);

    for (auto current_tile = 0u; current_tile < grid_size; current_tile++) {
        for (auto item : grid[current_tile]) {
            map_objects[item]->draw(index_to_vec(current_tile));
        }
        if (show_grid) {
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
            HIGHLIGHT_COLOR);
}

GameMap* generate_map(
    App* app,
    std::vector<std::vector<int>> map_content,
    Point map_size,
    Point tile_size,
    int dungeon_level,
    MapObject* player_object) {
    GameMap* gm = new GameMap(
        map_size,
        tile_size,
        app->config->settings["show_grid"].value_exact<bool>().value());

    static const std::string coin_sprite_names[3] = {
        "coin_pile_tile_0",
        "coin_pile_tile_1",
        "coin_pile_tile_2"};

    int abyss_id = gm->add_object(new Structure(EID_ABYSS, true, "Abyss"));
    int floor_id = gm->add_object(new Structure(
        EID_FLOOR,
        false,
        "Floor",
        app->assets.sprites["floor_tile"]));

    bool player_on_grid = false;
    int entrance_grid_id = -1;

    int grid_index = 0;
    for (auto tile : map_content) {
        for (auto item : tile) {
            switch (item) {
            case EID_FLOOR: {
                gm->place_object(grid_index, floor_id);
                break;
            }
            case EID_PLAYER: {
                gm->add_object(player_object, grid_index);
                player_on_grid = true;
                break;
            }
            case EID_ENTRANCE: {
                gm->add_object(
                    new Structure(
                        EID_ENTRANCE,
                        false,
                        "Entrance",
                        app->assets.sprites["entrance_tile"]),
                    grid_index);
                entrance_grid_id = grid_index;
                break;
            }
            case EID_EXIT: {
                gm->add_object(
                    Structure::make_exit(
                        EID_EXIT,
                        app->assets.sprites["exit_tile"]),
                    grid_index);
                break;
            }
            case EID_ENEMY: {
                gm->add_object(
                    Enemy::make_enemy(
                        EID_ENEMY,
                        dungeon_level,
                        app->assets.sprites["enemy_tile"]),
                    grid_index);
                break;
            }
            case EID_CHEST: {
                gm->add_object(
                    Treasure::make_chest(
                        EID_CHEST,
                        randbool(),
                        std::max(std::rand() % 100 * dungeon_level, 10 * dungeon_level),
                        app->assets.sprites["treasure_tile_full"],
                        app->assets.sprites["treasure_tile_empty"]),
                    grid_index);
                break;
            }
            case EID_CHEST_EMPTY: {
                gm->add_object(
                    Treasure::make_empty_chest(
                        EID_CHEST_EMPTY,
                        app->assets.sprites["treasure_tile_empty"]),
                    grid_index);
                break;
            }
            case EID_COIN_PILE: {
                gm->add_object(
                    Treasure::make_coin_pile(
                        EID_COIN_PILE,
                        std::max(std::rand() % 20 * dungeon_level, 5 * dungeon_level),
                        app->assets.sprites[coin_sprite_names[std::rand() % 3]]),
                    grid_index);
                break;
            }
            case EID_BOSS: {
                gm->add_object(
                    Enemy::make_boss(
                        EID_BOSS,
                        dungeon_level,
                        app->assets.sprites["boss_tile"]),
                    grid_index);
                break;
            }
            default: {
                gm->place_object(grid_index, abyss_id);
                break;
            }
            }
        }
        grid_index++;
    }

    if (!player_on_grid) {
        gm->add_object(player_object, entrance_grid_id);
    }

    return gm;
}

GameMap* generate_map(
    App* app,
    std::vector<std::vector<int>> map_content,
    Point map_size,
    Point tile_size,
    int dungeon_level) {
    MapObject* player_object =
        new Player(EID_PLAYER, app->assets.sprites["player_tile"]);

    return generate_map(
        app, map_content, map_size, tile_size, dungeon_level, player_object);
}

GameMap* generate_map(
    App* app,
    Image map_file,
    Point tile_size,
    int dungeon_level,
    MapObject* player_object) {
    Point map_size = {map_file.width, map_file.height};

    std::vector<std::vector<int>> map_content = {};

    for (auto current_y = 0; current_y < map_size.y; current_y++) {
        for (auto current_x = 0; current_x < map_size.x; current_x++) {
            int pix_color = ColorToInt(GetImageColor(map_file, current_x, current_y));

            std::vector<int> tile_content;

            if (pix_color == FLOOR_COLOR) {
                tile_content = {EID_FLOOR};
            }
            else {
                bool color_found = false;
                for (auto& kv : VALID_COLORS) {
                    if (pix_color == kv.first) {
                        color_found = true;
                        tile_content = {EID_FLOOR, kv.second};
                        break;
                    }
                }
                if (!color_found) {
                    tile_content = {EID_ABYSS};
                }
            }

            map_content.push_back(tile_content);
        }
    }

    return generate_map(
        app, map_content, map_size, tile_size, dungeon_level, player_object);
}

GameMap* generate_map(App* app, Image map_file, Point tile_size) {
    MapObject* player_object =
        new Player(EID_PLAYER, app->assets.sprites["player_tile"]);

    return generate_map(app, map_file, tile_size, 1, player_object);
}
