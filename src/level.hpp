#pragma once

#include "scenes.hpp"

class GameMap;

class Level : public Scene {
    private:
        SceneManager* parent;
        GameMap* map;

    public:
        Level(SceneManager* p);

        void update();
        void draw();
};
