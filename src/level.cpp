#include "level.hpp"
#include "utility.hpp"

#include <raylib.h>
#include <algorithm>
#include <iterator>

// TODO: make this configurable from settings
// Sane values would be 1.0 -> 3.0, everything bigger would make things render
// too close. Everything less makes no sense.
#define CAMERA_ZOOM 2.0f

#define SIDE_BG_COLOR {203, 219, 252, 255}
#define CORNER_COLOR {34, 32, 52, 255}

void Level::center_camera() {
    camera.target = player_pos;
};

void Level::set_camera() {
    center_camera();
    Point tile_size = map->get_tile_size();
    camera.zoom = CAMERA_ZOOM;
    camera.offset = Vector2{
        GetScreenWidth()/2.0f - tile_size.x/2.0f*camera.zoom,
        GetScreenHeight()/2.0f - tile_size.y/2.0f*camera.zoom
    };
    camera.rotation = 0.0f;
};

Level::Level(SceneManager* p) {
    parent = p;

    // TODO. Current version is but hardcoded placeholder
    map = generate_map(LoadImage("maps/map_0.png"), Point{32, 32});
    player_id = map->get_player_id();
    player_tile = map->get_player_tile();
    player_pos = map->tile_to_vec(player_tile);
    set_camera();

    // Maybe I shouldnt do it like that. But for now, borders on sides should
    // make visible part of screen form a perfect rectangle.
    // This would obviously cause issues on rectangular/vertical screens.
    // Idk how to solve it for now. TODO
    left_bg.x = 0.0f;
    left_bg.y = 0.0f;
    left_bg.width = (GetScreenWidth()-GetScreenHeight())/2.0f;
    left_bg.height = GetScreenHeight();

    right_bg.x = GetScreenWidth() - left_bg.width;
    right_bg.y = 0.0f;
    right_bg.width = left_bg.width;
    right_bg.height = left_bg.height;

    playground_vec_start.x = left_bg.width;
    playground_vec_start.y = left_bg.y;
    playground_vec_end.x = playground_vec_start.x+left_bg.height;
    playground_vec_end.y = playground_vec_start.y+left_bg.height;

    selected_tile_text = "Selected Tile: ";
    selected_tile_pos.x =
        center_text_h(selected_tile_text+"00 x 00", right_bg.x+right_bg.width/2);
    selected_tile_pos.y = GetScreenHeight()-200;

    tile_content_vert_gap = 30;

    tile_content_title = "Contains: ";
    tile_content_pos.x = selected_tile_pos.x;
    tile_content_pos.y = selected_tile_pos.y + tile_content_vert_gap;

    player_info_title = "Player Info:";
    player_info_pos.x =
        center_text_h(selected_tile_text, left_bg.x+left_bg.width/2);
    player_info_pos.y = 30;

    player_tile_text = "Current Tile: ";
    player_tile_text_pos.x =
        center_text_h(player_tile_text+"00 x 00", left_bg.x+left_bg.width/2);
    player_tile_text_pos.y = GetScreenHeight()-50;

    is_player_turn = false;
    turn_switch_timer = new Timer(0.1f);
    current_turn = 0;
    last_selected_tile = -1;
    change_turn();
};

bool Level::is_vec_on_playground(Vector2 vec) {
    return (
        (playground_vec_start.x < vec.x) && (vec.x < playground_vec_end.x) &&
        (playground_vec_start.y < vec.y) && (vec.y < playground_vec_end.y)
    );
};

void Level::change_turn() {
    if (is_player_turn) {
        is_player_turn = false;
        turn_title = "Enemy's Turn";
        turn_title_pos = {
            static_cast<float>(center_text_h(turn_title, right_bg.x+right_bg.width/2)),
            50
        };
    }
    else {
        is_player_turn = true;
        turn_title = "Player's Turn";
        turn_title_pos = {
            static_cast<float>(center_text_h(turn_title, right_bg.x+right_bg.width/2)),
            50
        };
        // This may backfire on multiple players
        current_turn++;
        turn_num_title = TextFormat("Current Turn: %i", current_turn);
        turn_num_title_pos = {
            static_cast<float>(center_text_h(turn_num_title, right_bg.x+right_bg.width/2)),
            30
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
        Vector2 new_pos;
        if (IsKeyDown(KEY_UP)) {
            new_pos = player_pos;
            new_pos.y -= map->get_tile_size().y;
            if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
            else new_pos = player_pos;
        }
        else if (IsKeyDown(KEY_DOWN)) {
            new_pos = player_pos;
            new_pos.y += map->get_tile_size().y;
            if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
            else new_pos = player_pos;
        }
        else if (IsKeyDown(KEY_LEFT)) {
            new_pos = player_pos;
            new_pos.x -= map->get_tile_size().x;
            if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
            else new_pos = player_pos;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            new_pos = player_pos;
            new_pos.x += map->get_tile_size().x;
            if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
            else new_pos = player_pos;
        }

        if (move_player) {
            int current_tile_id = map->tile_to_index(player_tile);

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
                map->vec_to_index(new_pos)
            );
            player_pos = new_pos;
            player_tile = map->vec_to_tile(player_pos);

            center_camera();

            change_turn();
        }
    }
    // TODO: stub
    else change_turn();
};

void Level::draw() {
    BeginMode2D(camera);
    map->draw();
    EndMode2D();

    DrawRectangleRec(
        left_bg,
        SIDE_BG_COLOR
    );
    DrawLine(
        left_bg.width,
        left_bg.y,
        left_bg.width,
        left_bg.height,
        CORNER_COLOR
    );

    DrawRectangleRec(
        right_bg,
        SIDE_BG_COLOR
    );
    DrawLine(
        right_bg.x,
        right_bg.y,
        right_bg.x,
        right_bg.height,
        CORNER_COLOR
    );

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

    // I may want to move this above everything else in draw cycle
    // This isn't really efficient, may need some improvements. TODO
    Vector2 mouse_pos = GetMousePosition();
    if (is_vec_on_playground(mouse_pos)) {
        Vector2 real_mouse_pos = GetScreenToWorld2D(mouse_pos, camera);
        if (map->is_vec_on_map(real_mouse_pos)) {
            Point mtt = map->vec_to_tile(real_mouse_pos);
            DrawText(
                TextFormat("%s%02i x %02i", selected_tile_text.c_str(), mtt.x, mtt.y),
                selected_tile_pos.x,
                selected_tile_pos.y,
                DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
            );
            map->select_tile(mtt);

            int selected_tile = map->tile_to_index(mtt);
            // This may backfire if selected tile has been changed between checks
            if (selected_tile != last_selected_tile) {
                last_selected_tile = selected_tile;
                std::string descriptions = tile_content_title;
                for (auto desc: map->get_tile_descriptions(last_selected_tile)) {
                    descriptions += "\n - " + desc;
                }
                last_selected_descriptions = descriptions;
            }

            DrawText(
                last_selected_descriptions.c_str(),
                tile_content_pos.x, tile_content_pos.y,
                DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
            );
        }
        else map->deselect_tile();
    }
    else map->deselect_tile();

    DrawText(
        player_info_title.c_str(),
        player_info_pos.x, player_info_pos.y,
        DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
    );

    DrawText(
        TextFormat(
            "%s%02i x %02i", player_tile_text.c_str(),
            player_tile.x, player_tile.y
        ),
        player_tile_text_pos.x,
        player_tile_text_pos.y,
        DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
    );
};
