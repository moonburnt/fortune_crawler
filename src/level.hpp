#pragma once

#include "raylib.h"
#include "scenes.hpp"
#include "mapgen.hpp"
#include "utility.hpp"
#include <string>

class Level : public Scene {
    private:
        SceneManager* parent;
        GameMap* map;

        int current_turn;
        bool is_player_turn;

        std::string turn_title;
        Vector2 turn_title_pos;

        std::string turn_num_title;
        Vector2 turn_num_title_pos;

        Timer* turn_switch_timer;

        int player_id;
        Point player_tile;
        Camera2D camera;

        // Initial camera configuration. Must be only used during init
        void set_camera();
        // Center camera around player
        void center_camera();

        void change_turn();

    public:
        Level(SceneManager* p);

        void update();
        void draw();
};
