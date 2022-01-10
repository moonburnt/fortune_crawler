#pragma once

#include "mapgen.hpp"
#include "raylib.h"
#include "scenes.hpp"
#include "utility.hpp"
#include "ui.hpp"
#include <string>

class Level : public Scene {
private:
    GameMap* map;

    int current_turn;
    bool is_player_turn;

    int dungeon_lvl;

    std::string turn_title;
    Vector2 turn_title_pos;

    std::string turn_num_title;
    Vector2 turn_num_title_pos;

    std::string selected_tile_text;
    Vector2 selected_tile_pos;

    std::string player_info_title;
    Vector2 player_info_pos;

    std::string player_tile_text;
    Vector2 player_tile_text_pos;

    std::string tile_content_title;
    Vector2 tile_content_pos;
    int tile_content_vert_gap;

    int last_selected_tile;
    std::string last_selected_descriptions;

    std::string completion_msg;
    Vector2 completion_msg_pos;

    std::string dungeon_lvl_title;
    Vector2 dungeon_lvl_title_pos;

    Timer* turn_switch_timer;

    Point player_tile;
    Vector2 player_pos;
    Camera2D camera;

    Rectangle left_bg;
    Rectangle right_bg;
    Vector2 playground_vec_start;
    Vector2 playground_vec_end;

    Rectangle event_screen_bg;
    TextButton* next_level_button;
    Button* close_event_screen_button;

    Event current_event;

    // Initial camera configuration. Must be only used during init
    void set_camera();

    // Center camera around player
    void center_camera();

    void change_turn();

    // Set new map and apply related changes.
    void configure_new_map();
    // Configure interface parts. This should be called on screen resize
    void configure_hud();

    // Change existing map to the new one. This should be called to switch the map
    void change_map();

    // Returns true if vec is not on side screens, false otherwise
    bool is_vec_on_playground(Vector2 vec);

public:
    Level();
    ~Level();

    void update(float dt) override;
    void draw() override;
};
