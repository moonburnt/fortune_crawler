#include <unordered_map>
#include <string>
#include "raylib.h"
#include "scenes.hpp"
extern SceneManager sc_mgr;
#include "level.hpp"
#include "loader.hpp"
extern AssetsLoader loader;

Level::Level(SceneManager* p) {
    parent = p;
};

void Level::update() {
    //TODO: stub
};

void Level::draw() {
    //TODO: stub
    DrawText("There will be level, once I will implement it", 500, 500, 20, BLACK);
};

// ~Level::Level() {
//     //TODO: stub
// };
