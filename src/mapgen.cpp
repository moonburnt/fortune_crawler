#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "raylib.h"
#include "mapgen.hpp"
#include "loader.hpp"

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO

bool SHOW_GRID = true; // TODO: move this to settings, make configurable

#define GRID_COLOR {63, 63, 116, 255}

MapObject::MapObject(ObjectCategory cat) {
    category = cat;
    has_texture = false;
}

MapObject::MapObject(ObjectCategory cat, Texture2D* sprite) {
    category = cat;
    texture = sprite;
    has_texture = true;
}

ObjectCategory MapObject::get_category() {
    return category;
}

void MapObject::draw(Vector2 pos) {
    if (has_texture) DrawTextureV(*texture, pos, WHITE);
}

Floor::Floor() : MapObject(ObjectCategory::floor) {
    type = FloorType::abyss;
};

Floor::Floor(FloorType tile_type, Texture2D* sprite) :
    MapObject(ObjectCategory::floor, sprite) {
        type = tile_type;
}

Creature::Creature(CreatureType tile_type, Texture2D* sprite) :
    MapObject(ObjectCategory::creature, sprite) {
        type = tile_type;
}

Item::Item(ItemType tile_type, Texture2D* sprite) :
    MapObject(ObjectCategory::item, sprite) {
        type = tile_type;
}

GameMap::GameMap(
    Point m_size,
    Point t_size,
    std::unordered_map<int, MapObject*>map_elems,
    std::vector<std::vector<int>>grid_layout
) {
    map_size = m_size;
    tile_size = t_size;
    map_real_size = Vector2{
        static_cast<float>(map_size.x*tile_size.x),
        static_cast<float>(map_size.y*tile_size.y)
    };
    map_objects = map_elems;
    grid = grid_layout;
    grid_size = static_cast<int>(grid.size());

    for (int current_tile = 0; current_tile < grid_size; current_tile++) {
        for (auto item: grid[current_tile]) {
            if (map_objects[item]->get_category() == ObjectCategory::creature) {
                // Boss should be stationary
                if (static_cast<Creature*>(map_objects[item])->type == CreatureType::enemy)
                    enemy_indexes.push_back(current_tile);
            }
        }
    }
}

int GameMap::get_player_id() {
    // This may act weirdly if player is not there, but that should happen
    int player_id;

    for (auto &kv: map_objects) {

        if (kv.second->get_category() == ObjectCategory::creature &&
            static_cast<Creature*>(kv.second)->type == CreatureType::player) {
            player_id = kv.first;
            break;
        }
    }

    return player_id;
}

std::vector<int>* GameMap::get_tile_content(int grid_index) {
    // Protection against out-of-bounds index values
    grid_index = std::clamp(grid_index, 0, grid_size);

    return &grid[grid_index];
}

Point GameMap::index_to_tile(int index) {
    index = std::clamp(index, 0, grid_size);

    int x = index / map_size.x;
    int y = index - x * map_size.x;
    return Point{x, y};
}

int GameMap::tile_to_index(Point pos) {
    return std::clamp((pos.x*map_size.x + pos.y), 0, grid_size);
}

Point GameMap::vec_to_tile(Vector2 vec) {
    return Point{
        static_cast<int>(vec.x)/tile_size.x,
        static_cast<int>(vec.y)/tile_size.y
    };
}

Vector2 GameMap::tile_to_vec(Point tile) {
    return Vector2{
        static_cast<float>(tile.x*tile_size.x),
        static_cast<float>(tile.y*tile_size.y)
    };
}

Vector2 GameMap::index_to_vec(int index) {
    index = std::clamp(index, 0, grid_size);

    int x = index / map_size.x;
    int y = index - x * map_size.x;
    return Vector2{
        static_cast<float>(x * tile_size.x),
        static_cast<float>(y * tile_size.y)
    };
}

int GameMap::vec_to_index(Vector2 vec) {
    int x = vec.x/tile_size.x;
    int y = vec.y/tile_size.y;
    return std::clamp((x*map_size.x + y), 0, grid_size);
}

bool GameMap::is_vec_on_map(Vector2 vec) {
    return (
        (0 < vec.x) && (vec.x < map_real_size.x) &&
        (0 < vec.y) && (vec.y < map_real_size.y)
    );
}

Point GameMap::get_player_tile() {
    // Same possible caveats as in get_player_id()
    int player_tile_index;

    int player_id = get_player_id();

    bool player_found = false;

    for (int index = 0; index < grid_size; index++) {
        for (auto tile_i: grid[index]) {
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

bool GameMap::is_tile_blocked(Point tile) {
    int index = tile_to_index(tile);

    for (auto item: grid[index]) {
        if (map_objects[item]->get_category() == ObjectCategory::obstacle) return true;
        else if (
            map_objects[item]->get_category() == ObjectCategory::floor &&
            static_cast<Floor*>(map_objects[item])->type == FloorType::abyss
            ) return true;
    }
    return false;
}

void GameMap::move_object(int grid_index, int tile_index, int new_grid_index) {
    int object_id = grid[grid_index][tile_index];
    // I think this will work?
    grid[grid_index].erase(grid[grid_index].begin()+tile_index);
    grid[new_grid_index].push_back(object_id);
}

void GameMap::draw() {
    for (int current_tile = 0; current_tile < grid_size; current_tile++) {
        for (auto item: grid[current_tile]) {
            map_objects[item]->draw(index_to_vec(current_tile));
        }

        if (SHOW_GRID) {
            Vector2 vec = index_to_vec(current_tile);
            DrawRectangleLines(vec.x, vec.y, tile_size.x, tile_size.y, GRID_COLOR);
        }
    }
}

GameMap* generate_map(Image map_file, Point tile_size) {
    Point map_size = {map_file.width, map_file.height};

    std::vector<std::vector<int>>grid;
    std::unordered_map<int, MapObject*>map_objects;

    map_objects[0] = new Floor();
    map_objects[1] = new Floor(
        FloorType::floor,
        &AssetLoader::loader.sprites["floor_tile"]
    );
    int current_map_object = map_objects.size();

    int i = 0;

    for (auto current_x = 0; current_x < map_size.x; current_x++) {
        for (auto current_y = 0; current_y < map_size.y; current_y++) {
            grid.push_back({});
            int pix_color = ColorToInt(
                GetImageColor(map_file, current_x, current_y)
            );

            if (pix_color == ColorToInt(Color{203, 219, 252, 255})) {
                grid[i].push_back({1});
            }
            else if (pix_color == ColorToInt(Color{0, 255, 9, 255})) {
                grid[i].push_back({1});

                map_objects[current_map_object] = new Item(
                    ItemType::entrance,
                    &AssetLoader::loader.sprites["entrance_tile"]
                );
                grid[i].push_back(current_map_object);
                current_map_object++;

                map_objects[current_map_object] = new Creature(
                    CreatureType::player,
                    &AssetLoader::loader.sprites["player_tile"]
                );
                grid[i].push_back(current_map_object);
                current_map_object++;
            }
            else if (pix_color == ColorToInt(Color{0, 242, 255, 255})) {
                grid[i].push_back({1});

                map_objects[current_map_object] = new Item(
                    ItemType::exit,
                    &AssetLoader::loader.sprites["exit_tile"]
                );
                grid[i].push_back(current_map_object);
                current_map_object++;
            }
            else if (pix_color == ColorToInt(Color{255, 0, 0, 255})) {
                grid[i].push_back({1});

                map_objects[current_map_object] = new Creature(
                    CreatureType::enemy,
                    &AssetLoader::loader.sprites["enemy_tile"]
                );
                grid[i].push_back(current_map_object);
                current_map_object++;
            }
            else if (pix_color == ColorToInt(Color{255, 233, 0, 255})) {
                grid[i].push_back({1});

                map_objects[current_map_object] = new Item(
                    ItemType::treasure,
                    &AssetLoader::loader.sprites["treasure_tile"]
                );
                grid[i].push_back(current_map_object);
                current_map_object++;
            }
            else if (pix_color == ColorToInt(Color{199, 0, 255, 255})) {
                grid[i].push_back({1});

                map_objects[current_map_object] = new Creature(
                    CreatureType::boss,
                    &AssetLoader::loader.sprites["boss_tile"]
                );
                grid[i].push_back(current_map_object);
                current_map_object++;
            }
            else grid[i].push_back({0});

            i++;
        }
    }

    return new GameMap(map_size, tile_size, map_objects, grid);
};
