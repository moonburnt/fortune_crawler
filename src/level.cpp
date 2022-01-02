#include "level.hpp"
#include "mapgen.hpp"

#include <raylib.h>

Level::Level(SceneManager* p) {
    parent = p;

    //TODO. Current version is but hardcoded placeholder
    map = generate_map(LoadImage("maps/map_0.png"), Point{32, 32});
};

void Level::update() {
    //TODO: stub
};

void Level::draw() {
    //TODO: stub
    map->draw();
};
