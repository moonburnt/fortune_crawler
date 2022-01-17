#pragma once

#include "mapgen.hpp"
#include "raylib.h"
#include "scenes.hpp"
#include "ui.hpp"
#include "utility.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

enum class MovementDirection
{
    none,
    upleft,
    up,
    upright,
    left,
    stand,
    right,
    downleft,
    down,
    downright
};

class InputController {
private:
    std::vector<int> buttons_held;
    std::unordered_map<int, MovementDirection> key_binds;

public:
    InputController();
    // Bind specific key to specific movement direction
    void add_relationship(int key, MovementDirection direction);
    // Update list of held buttons
    void update();
    // Get direction of last button held
    MovementDirection get_movement_direction();
};

class Level : public Scene {
private:
    SceneManager* parent;
    GameMap* map;
    InputController input_controller;

    int current_turn;
    bool is_player_turn;

    int dungeon_lvl;

    DynamicLabel turn_label;
    DynamicLabel turn_num_label;
    DynamicLabel selected_tile_label;
    Label player_info_label;
    DynamicLabel player_currency_label;
    DynamicLabel player_tile_label;
    DynamicLabel tile_content_label;
    DynamicLabel dungeon_lvl_label;
    Label completion_label;

    void set_player_tile(Point tile);

    int last_selected_tile;

    Timer* turn_switch_timer;

    Point player_tile;
    Vector2 player_pos;
    Player* player_obj;
    Camera2D camera;

    Rectangle left_bg;
    Rectangle right_bg;
    Vector2 playground_vec_start;
    Vector2 playground_vec_end;
    Button* back_to_menu_button;

    Rectangle event_screen_bg;
    TextButton* next_level_button;
    Button* close_event_screen_button;

    Event current_event;
    std::optional<int> current_event_cause;

    // Exit from level to main menu.
    void back_to_menu();

    // Initial camera configuration. Must be only used during init
    void set_camera();

    // Center camera around player
    void center_camera();

    void change_turn();

    // Reset current event to Event::nothing and event cause to std::nullopt
    void reset_event();

    // Set new map and apply related changes.
    void configure_new_map();
    // Configure interface parts. This should be called on screen resize
    void configure_hud();

    // Change existing map to the new one. This should be called to switch the map
    void change_map();

    // Returns true if vec is not on side screens, false otherwise
    bool is_vec_on_playground(Vector2 vec);

public:
    Level(SceneManager* p);
    ~Level();

    void update(float dt) override;
    void draw() override;
};
