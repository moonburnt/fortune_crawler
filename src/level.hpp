// Fortune Crawler - a conceptual dungeon crawler with rock-paper-scissors events.
// Copyright (c) 2022 moonburnt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see https://www.gnu.org/licenses/gpl-3.0.txt

#pragma once

#include "engine/core.hpp"
#include "mapgen.hpp"
#include "raylib.h"
#include "menus.hpp"
#include "loader.hpp"
#include "common.hpp"
#include <optional>
#include <string>
#include <vector>

static constexpr Color SIDE_BG_COLOR{203, 219, 252, 255};
static constexpr Color CORNER_COLOR{34, 32, 52, 255};
static constexpr Color BG_COLOR{63, 63, 116, 255};

class App;

// This may be an overkill, but its starting to get hard to get through level's
// items, so I've needed to move these primitives somewhere
class EventScreen {
protected:
    Rectangle bg;
    Color bg_color;

public:
    EventScreen(Rectangle bg, Color bg_color);
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual ~EventScreen() = default;
};

class Level : public Scene {
private:
    enum class MovementDirection {
        none,
        upleft,
        up,
        upright,
        left,
        stand,
        right,
        downleft,
        down,
        downright,
    };

    Level(App* app, SceneManager* p, bool set_new_map);
    Level(App* app, SceneManager* p, SavefileFields save_data);

    App* app;
    SceneManager* parent;
    GameMap* map;
    InputController<MovementDirection> input_controller;

    int current_turn;
    int money_collected;
    int enemies_killed;
    bool is_player_turn;

    int dungeon_lvl;

    Label turn_label;
    Label turn_num_label;
    Label selected_tile_label;
    Label player_info_label;
    Label player_currency_label;
    Label player_tile_label;
    Label tile_content_label;
    Label dungeon_lvl_label;

    Label player_stats_label;

    void set_player_tile(Point tile);

    int last_selected_tile;
    bool show_tile_description;
    // This toggles requirement to update description of current tile.
    bool force_description_update;

    void update_tile_description();

    Timer turn_switch_timer;

    Point player_tile;
    Vector2 player_pos;
    Player* player_obj;
    Camera2D camera;

    Rectangle left_bg;
    Rectangle right_bg;
    Vector2 playground_vec_start;
    Vector2 playground_vec_end;

    EventScreen* pause_menu;
    bool game_over;
    EventScreen* game_over_screen;

    EventScreen* current_event_screen = nullptr;
    void purge_current_event_screen();

    std::vector<std::tuple<int, Event>> scheduled_events;
    // This will cause problems if at some point we will need to operate on events
    // That came from multiple tiles.
    std::optional<int> current_event_tile_id;

    // Unpack current_event and current_event_cause from tail of scheduled_events.
    // Returns true if new event is scheduled, false if all events has been done.
    bool set_new_event();

    // Handle input and move player accordingly
    void handle_player_movement();

    // Initial camera configuration. Must be only used during init
    void set_camera();

    // Center camera around player
    void center_camera();

    void change_turn();

    // Set new map and apply related changes.
    void configure_new_map();

    void reset_level_stats();

    // Configure interface parts. This should be called on screen resize
    void configure_hud();

    // Returns true if vec is not on side screens, false otherwise
    bool is_vec_on_playground(Vector2 vec);

public:
    bool show_hud;
    bool is_paused;

    static Level* new_game(App* app, SceneManager* p);
    static Level* load_save(App* app, SceneManager* p, SavefileFields save_data);
    ~Level();

    void update(float dt) override;
    void draw() override;

    // Reset current_event and its cause to std::nullopt, then remove the last
    // event (assuming this has been the completed one) from scheduled_events.
    void complete_event();

    void change_level();

    void resume();
    void save_end_exit();
    void restart();

    // Change existing map to the new one. This should be called to switch the map
    void change_map();

    // Exit from level to main menu.
    void exit_to_menu();

    // Give player specified amount of money.
    // Will also increase level's "money_collected" counter.
    void give_player_money(int amount);

    void update_player_stats_hud();

    void kill_enemy(int tile_id, int entity_id);
    void show_gameover();

    // Return data required to save level on disk
    SavefileFields get_save_data();

    // Save level on disk.
    bool save();
};
