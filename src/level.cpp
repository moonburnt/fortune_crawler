#include "level.hpp"
#include "mapgen.hpp"

#include <raylib.h>

#include <unordered_map>
#include <string>
#include <vector>

Level::Level(SceneManager* p) {
    parent = p;

    MapGenerator mapgen;
    //TODO. Current version is but hardcoded placeholder
    mapgen.add_relationship(Color{203, 219, 252, 255}, TileType::floor);
    mapgen.add_relationship(Color{0, 255, 9, 255}, TileType::entrance);
    mapgen.add_relationship(Color{0, 242, 255, 255}, TileType::exit);
    mapgen.add_relationship(Color{255, 0, 0, 255}, TileType::enemy);
    mapgen.add_relationship(Color{255, 233, 0, 255}, TileType::treasure);
    mapgen.add_relationship(Color{199, 0, 255, 255}, TileType::boss);
    Image img = LoadImage("maps/map_0.png");
    mapgen.process_template(img);
    map = mapgen.generate(Vector2{32, 32});
};

void Level::update() {
    //TODO: stub
};

void Level::draw() {
    //TODO: stub
    // DrawText("There will be level, once I will implement it", 500, 500, 20, BLACK);
    map->draw();
};

// ~Level::Level() {
//     //TODO: stub
// };
