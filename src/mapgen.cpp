#include <unordered_map>
#include <vector>
#include <string>
#include "raylib.h"
#include "mapgen.hpp"
#include "loader.hpp"

#include <stdio.h>

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO

MapObject::MapObject(ObjectCategory cat) {
    category = cat;
    has_texture = false;
}

MapObject::MapObject(ObjectCategory cat, Texture2D* sprite) {
    category = cat;
    texture = sprite;
    has_texture = true;
}

void MapObject::draw(Point pos) {
    if (has_texture) DrawTexture(*texture, pos.x, pos.y, WHITE);
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
    map_objects = map_elems;
    grid = grid_layout;
    grid_size = static_cast<int>(grid.size());
}

void GameMap::update() {
    //TODO: stub
}

void GameMap::draw() {
    for (int current_tile = 0; current_tile < grid_size; current_tile++) {
        for (auto item: grid[current_tile]) {
            int x = current_tile / map_size.x;
            int y = current_tile - x*map_size.x;
            map_objects[item]->draw(Point{x*tile_size.x, y*tile_size.y});
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
