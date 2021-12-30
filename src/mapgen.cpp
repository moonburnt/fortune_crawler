#include <unordered_map>
#include <vector>
#include <string>
#include "raylib.h"
#include "mapgen.hpp"
#include "loader.hpp"
extern AssetsLoader loader;

// Map generator. For now, valid colors and their relations to events are hardcoded,
// but it may be changed it future. TODO

Tile::Tile(Texture2D* sprite, TileType tile_type) {
    texture = sprite;
    type = tile_type;
}

void Tile::set_pos(Vector2 position) {
    pos = position;
}

Tile::Tile(Texture2D* sprite, TileType tile_type, Vector2 position) {
    texture = sprite;
    type = tile_type;
    set_pos(position);
}

void Tile::draw() {
    DrawTexture(*texture, pos.x, pos.y, WHITE);
}


GameMap::GameMap(
    std::vector<Tile*>floor,
    std::vector<Tile*>objects,
    std::vector<Tile*>creatures,
    Vector2 player_spawnpoint
) {
    floor_tiles = floor;
    object_tiles = objects;
    creature_tiles = creatures;
    player = new Tile(
        &loader.sprites["player_tile"],
        TileType::player,
        player_spawnpoint
    );
}

void GameMap::update() {
    //TODO: stub
}

void GameMap::draw() {
    for (auto current_tile: floor_tiles) {
        current_tile->draw();
    }

    for (auto current_tile: object_tiles) {
        current_tile->draw();
    }

    for (auto current_tile: creature_tiles) {
        current_tile->draw();
    }

    player->draw();
}


MapGenerator::MapGenerator() {};

void MapGenerator::add_relationship(Color color, TileType tile_type) {
    tile_relationships[ColorToInt(color)] = tile_type;
}

void MapGenerator::process_template(Image map_file) {
    map_size.x = map_file.width;
    map_size.y = map_file.height;

    for (auto current_x = 0; current_x < map_size.x; current_x++) {
        for (auto current_y = 0; current_y < map_size.y; current_y++) {

            int pix_color = ColorToInt(
                GetImageColor(map_file, current_x, current_y)
            );

            if (tile_relationships.count(pix_color)) {
                map_elements[tile_relationships[pix_color]].push_back(
                    Vector2{
                        static_cast<float>(current_x),
                        static_cast<float>(current_y)
                    }
                );
            }
        }
    }
}

// This function is kinda jank, idk how to make it more flexible. TODO
GameMap* MapGenerator::generate(Vector2 tile_size) {
    // These are the separate vectors due to draw order
    std::vector<Tile*>floor_tiles;
    std::vector<Tile*>structure_tiles;
    std::vector<Tile*>creature_tiles;
    Vector2 player_spawnpoint;

    for (auto &kv: map_elements) {
        bool unique_or_floor = false;

        // Unique structures, only one of which can exist on map
        // TODO: maybe dont fetch the very first one, but random one?
        // Just so in case multiple tiles of that color exist, the one to actually
        // use will be picked by RNGesus. This could add some variety
        if (kv.first == TileType::entrance) {
            player_spawnpoint = kv.second.at(0);
            player_spawnpoint.x *= tile_size.x;
            player_spawnpoint.y *= tile_size.y;
            structure_tiles.push_back(
                new Tile(
                    &loader.sprites["entrance_tile"],
                    TileType::entrance,
                    player_spawnpoint
                )
            );
            unique_or_floor = true;
        }
        else if (kv.first == TileType::exit) {
            structure_tiles.push_back(
                new Tile(
                    &loader.sprites["exit_tile"],
                    TileType::exit,
                    Vector2{
                        tile_size.x*kv.second.at(0).x,
                        tile_size.y*kv.second.at(0).y
                    }
                )
            );
            unique_or_floor = true;
        }
        else if (kv.first == TileType::floor) unique_or_floor = true;

        for (auto current_tile: kv.second) {
            floor_tiles.push_back(
                new Tile(
                    &loader.sprites["floor_tile"],
                    TileType::floor,
                    Vector2{
                        tile_size.x*current_tile.x,
                        tile_size.y*current_tile.y
                    }
                )
            );

            if (unique_or_floor) continue;

            if (kv.first == TileType::treasure) {
                structure_tiles.push_back(
                    new Tile(
                        &loader.sprites["treasure_tile"],
                        TileType::treasure,
                        Vector2{
                            tile_size.x*current_tile.x,
                            tile_size.y*current_tile.y
                        }
                    )
                );
            }
            else if (kv.first == TileType::enemy) {
                creature_tiles.push_back(
                    new Tile(
                        &loader.sprites["enemy_tile"],
                        TileType::enemy,
                        Vector2{
                            tile_size.x*current_tile.x,
                            tile_size.y*current_tile.y
                        }
                    )
                );
            }
            else if (kv.first == TileType::boss) {
                creature_tiles.push_back(
                    new Tile(
                        &loader.sprites["boss_tile"],
                        TileType::boss,
                        Vector2{
                            tile_size.x*current_tile.x,
                            tile_size.y*current_tile.y
                        }
                    )
                );
            }

        }


    }

    return new GameMap(floor_tiles, structure_tiles, creature_tiles, player_spawnpoint);
}
