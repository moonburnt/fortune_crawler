#include "level.hpp"
#include "utility.hpp"

#include <raylib.h>
#include <algorithm>
#include <iterator>

Level::Level(SceneManager* p) {
    parent = p;

    //TODO. Current version is but hardcoded placeholder
    map = generate_map(LoadImage("maps/map_0.png"), Point{32, 32});
    player_id = map->get_player_id();
    player_tile = map->get_player_tile();
    is_player_turn = false;
    turn_switch_timer = new Timer(0.1f);
    current_turn = 0;
    change_turn();
};

void Level::change_turn() {
    if (is_player_turn) {
        is_player_turn = false;
        turn_title = "Enemy's Turn";
        turn_title_pos = {
            static_cast<float>(center_text_h(turn_title, GetScreenWidth()/2)),
            40
        };
    }
    else {
        is_player_turn = true;
        turn_title = "Player's Turn";
        turn_title_pos = {
            static_cast<float>(center_text_h(turn_title, GetScreenWidth()/2)),
            40
        };
        // This may backfire on multiple players
        current_turn++;
        turn_num_title = TextFormat("Current Turn: %i", current_turn);
        turn_num_title_pos = {
            static_cast<float>(center_text_h(turn_num_title, GetScreenWidth()/2)),
            20
        };
    }
    turn_switch_timer->start();
}

void Level::update() {
    if (turn_switch_timer->is_started()) {
        if (turn_switch_timer->tick()) turn_switch_timer->stop();
        else return;
    }

    if (is_player_turn) {
        bool move_player = false;
        Point new_pos;
        if (IsKeyDown(KEY_UP)) {
            new_pos = player_tile;
            new_pos.y -= map->get_tile_size().y;
            if (!map->is_tile_blocked(new_pos)) move_player = true;
            else new_pos = player_tile;
        }
        else if (IsKeyDown(KEY_DOWN)) {
            new_pos = player_tile;
            new_pos.y += map->get_tile_size().y;
            if (!map->is_tile_blocked(new_pos)) move_player = true;
            else new_pos = player_tile;
        }
        else if (IsKeyDown(KEY_LEFT)) {
            new_pos = player_tile;
            new_pos.x -= map->get_tile_size().x;
            if (!map->is_tile_blocked(new_pos)) move_player = true;
            else new_pos = player_tile;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            Point new_pos = player_tile;
            new_pos.x += map->get_tile_size().x;
            if (!map->is_tile_blocked(new_pos)) move_player = true;
            else new_pos = player_tile;
        }

        if (move_player) {
            int current_tile_id = map->pos_to_index(player_tile);

            std::vector<int>* current_tile_content =
                                        map->get_tile_content(current_tile_id);
            // This shouldn't return .end(), I think
            std::vector<int>::iterator pt_iterator;
            pt_iterator = std::find(
                current_tile_content->begin(),
                current_tile_content->end(),
                player_id
            );
            int pt_index = std::distance(current_tile_content->begin(), pt_iterator);

            map->move_object(
                current_tile_id,
                pt_index,
                map->pos_to_index(new_pos)
            );
            player_tile = new_pos;
            change_turn();
        }
    }
    // TODO: stub
    else change_turn();
};

void Level::draw() {
    map->draw();
    DrawText(
        turn_num_title.c_str(),
        turn_num_title_pos.x, turn_num_title_pos.y,
        DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
    );
    DrawText(
        turn_title.c_str(),
        turn_title_pos.x, turn_title_pos.y,
        DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
    );
};
