#include "level.hpp"
#include "utility.hpp"

#include <algorithm>
#include <iterator>
#include <raylib.h>

#include <stdio.h>

// TODO: make this configurable from settings
// Sane values would be 1.0 -> 3.0, everything bigger would make things render
// too close. Everything less makes no sense.
static constexpr float CAMERA_ZOOM = 2.0f;

static constexpr Color SIDE_BG_COLOR{203, 219, 252, 255};
static constexpr Color CORNER_COLOR{34, 32, 52, 255};

void Level::center_camera() {
    camera.target = player_pos;
}

void Level::set_camera() {
    center_camera();
    Point tile_size = map->get_tile_size();
    camera.zoom = CAMERA_ZOOM;
    camera.offset = Vector2{
        GetScreenWidth() / 2.0f - tile_size.x / 2.0f * camera.zoom,
        GetScreenHeight() / 2.0f - tile_size.y / 2.0f * camera.zoom};
    camera.rotation = 0.0f;
}

Level::Level() {
    // TODO. Current version is but hardcoded placeholder
    map = generate_map(LoadImage("maps/map_0.png"), Point{32, 32});
    player_tile = map->get_player_tile();
    player_pos = map->tile_to_vec(player_tile);
    set_camera();

    // Maybe I shouldnt do it like that. But for now, borders on sides should
    // make visible part of screen form a perfect rectangle.
    // This would obviously cause issues on rectangular/vertical screens.
    // Idk how to solve it for now. TODO
    left_bg.x = 0.0f;
    left_bg.y = 0.0f;
    left_bg.width = (GetScreenWidth() - GetScreenHeight()) / 2.0f;
    left_bg.height = GetScreenHeight();

    right_bg.x = GetScreenWidth() - left_bg.width;
    right_bg.y = 0.0f;
    right_bg.width = left_bg.width;
    right_bg.height = left_bg.height;

    playground_vec_start.x = left_bg.width;
    playground_vec_start.y = left_bg.y;
    playground_vec_end.x = playground_vec_start.x + left_bg.height;
    playground_vec_end.y = playground_vec_start.y + left_bg.height;

    event_screen_bg.x = left_bg.width+30;
    event_screen_bg.y = left_bg.y+30;
    event_screen_bg.width = (GetScreenWidth() - left_bg.width*2)-60;
    event_screen_bg.height = left_bg.height-60;

    selected_tile_text = "Selected Tile: ";
    selected_tile_pos.x =
        center_text_h(selected_tile_text + "00 x 00", right_bg.x + right_bg.width / 2);
    selected_tile_pos.y = GetScreenHeight() - 200;

    tile_content_vert_gap = 30;

    tile_content_title = "Contains: ";
    tile_content_pos.x = selected_tile_pos.x;
    tile_content_pos.y = selected_tile_pos.y + tile_content_vert_gap;

    player_info_title = "Player Info:";
    player_info_pos.x = center_text_h(selected_tile_text, left_bg.x + left_bg.width / 2);
    player_info_pos.y = 30;

    player_tile_text = "Current Tile: ";
    player_tile_text_pos.x =
        center_text_h(player_tile_text + "00 x 00", left_bg.x + left_bg.width / 2);
    player_tile_text_pos.y = GetScreenHeight() - 50;

    completion_msg = "You've won!";

    is_player_turn = false;
    turn_switch_timer = new Timer(0.1f);
    current_turn = 0;
    current_event = Event::nothing;
    draw_completion_screen = false;
    last_selected_tile = -1;
    change_turn();
}

bool Level::is_vec_on_playground(Vector2 vec) {
    return playground_vec_start.x < vec.x && vec.x < playground_vec_end.x && playground_vec_start.y < vec.y && vec.y < playground_vec_end.y;
}

void Level::change_turn() {
    if (is_player_turn) {
        is_player_turn = false;
        turn_title = "Enemy's Turn";
        turn_title_pos = {
            static_cast<float>(
                center_text_h(turn_title, right_bg.x + right_bg.width / 2)),
            50};
    }
    else {
        is_player_turn = true;
        turn_title = "Player's Turn";
        turn_title_pos = {
            static_cast<float>(
                center_text_h(turn_title, right_bg.x + right_bg.width / 2)),
            50};
        // This may backfire on multiple players
        current_turn++;
        turn_num_title = TextFormat("Current Turn: %i", current_turn);
        turn_num_title_pos = {
            static_cast<float>(
                center_text_h(turn_num_title, right_bg.x + right_bg.width / 2)),
            30};
    }
    turn_switch_timer->start();
}

void Level::update(float dt) {
    if (turn_switch_timer->is_started()) {
        if (turn_switch_timer->tick(dt)) turn_switch_timer->stop();
        else return;
    }

    switch (current_event) {
    case Event::nothing:
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
                    map->get_player_id());
                int pt_index = std::distance(current_tile_content->begin(), pt_iterator);

                int new_tile_id = map->vec_to_index(new_pos);
                // Some really inefficiend way to check if new tile contains
                // someting that may trigger an event
                std::vector<int>* next_tile_content =
                    map->get_tile_content(new_tile_id);
                std::vector<int>::iterator et_iterator;
                et_iterator = std::find(
                    next_tile_content->begin(),
                    next_tile_content->end(),
                    map->get_exit_id()
                );
                if (et_iterator != next_tile_content->end()) {
                    current_event = Event::exit_map;
                }

                map->move_object(current_tile_id, pt_index, new_tile_id);
                player_pos = new_pos;
                player_tile = map->vec_to_tile(player_pos);

                center_camera();

                change_turn();
            }
        }
        // TODO: stub
        else change_turn();
        break;

    case Event::exit_map:
        draw_completion_screen = true;
        // Doing it there, coz completion msg may later include some dynamic
        // stats that may affect text position
        completion_msg_pos = center_text(
            completion_msg,
            Vector2{GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}
        );
        break;

    default: break;
    }
}

void Level::draw() {
    BeginMode2D(camera);
    map->draw();
    EndMode2D();

    DrawRectangleRec(left_bg, SIDE_BG_COLOR);
    DrawLine(left_bg.width, left_bg.y, left_bg.width, left_bg.height, CORNER_COLOR);

    DrawRectangleRec(right_bg, SIDE_BG_COLOR);
    DrawLine(right_bg.x, right_bg.y, right_bg.x, right_bg.height, CORNER_COLOR);

    DrawText(
        turn_num_title.c_str(),
        turn_num_title_pos.x,
        turn_num_title_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);
    DrawText(
        turn_title.c_str(),
        turn_title_pos.x,
        turn_title_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);

    if (draw_completion_screen) {
        DrawRectangleRec(event_screen_bg, SIDE_BG_COLOR);
        DrawRectangleLinesEx(event_screen_bg, 1.0f, CORNER_COLOR);
        DrawText(
            completion_msg.c_str(),
            completion_msg_pos.x, completion_msg_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR
        );
    }
    else {
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
                    DEFAULT_TEXT_SIZE,
                    DEFAULT_TEXT_COLOR);
                map->select_tile(mtt);

                int selected_tile = map->tile_to_index(mtt);
                // This may backfire if selected tile has been changed between checks
                if (selected_tile != last_selected_tile) {
                    last_selected_tile = selected_tile;
                    std::string descriptions = tile_content_title;
                    for (auto desc : map->get_tile_descriptions(last_selected_tile)) {
                        descriptions += "\n - " + desc;
                    }
                    last_selected_descriptions = descriptions;
                }

                DrawText(
                    last_selected_descriptions.c_str(),
                    tile_content_pos.x,
                    tile_content_pos.y,
                    DEFAULT_TEXT_SIZE,
                    DEFAULT_TEXT_COLOR);
            }
            else map->deselect_tile();
        }
        else map->deselect_tile();
    }

    DrawText(
        player_info_title.c_str(),
        player_info_pos.x,
        player_info_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);

    DrawText(
        TextFormat(
            "%s%02i x %02i",
            player_tile_text.c_str(),
            player_tile.x,
            player_tile.y),
        player_tile_text_pos.x,
        player_tile_text_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);
}
