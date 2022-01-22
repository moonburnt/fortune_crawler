#pragma once

#include "mapgen.hpp"
#include "raylib.h"
#include "scenes.hpp"
#include "ui.hpp"
#include "utility.hpp"
#include <optional>
#include <string>
#include <vector>

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

    DynamicLabel player_stats_label;

    void update_player_stats_hud();

    void set_player_tile(Point tile);

    int last_selected_tile;
    bool show_tile_description;
    // This toggles requirement to update description of current tile.
    bool force_description_update;

    void update_tile_description();

    Timer* turn_switch_timer;

    Point player_tile;
    Vector2 player_pos;
    Player* player_obj;
    Camera2D camera;

    Rectangle left_bg;
    Rectangle right_bg;
    Vector2 playground_vec_start;
    Vector2 playground_vec_end;
    Button back_to_menu_button;

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

    // Exit from level to main menu.
    void back_to_menu();

    // Initial camera configuration. Must be only used during init
    void set_camera();

    // Center camera around player
    void center_camera();

    void change_turn();

    // Set new map and apply related changes.
    void configure_new_map();
    // Configure interface parts. This should be called on screen resize
    void configure_hud();

    // Returns true if vec is not on side screens, false otherwise
    bool is_vec_on_playground(Vector2 vec);

public:
    Level(SceneManager* p);
    ~Level();

    void update(float dt) override;
    void draw() override;

    // Reset current_event and its cause to std::nullopt, then remove the last
    // event (assuming this has been the completed one) from scheduled_events.
    void complete_event();

    // Change existing map to the new one. This should be called to switch the map
    void change_map();
};
