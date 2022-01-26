#pragma once

#include "level.hpp"
#include "raylib.h"

// This contains various EventScreen implementations, to de-bloat level.cpp
// Abstract EventScreen is still part of level.hpp, due to dependency resolution.
class CompletionScreen : public EventScreen {
private:
    Level* lvl;
    Label completion_label;
    TextButton next_level_button;
    Button close_screen_button;

public:
    CompletionScreen(
        Level* level, int turns_made, int money_collected, int enemies_killed);
    void update() override;
    void draw() override;
};

class LockpickScreen : public EventScreen {
private:
    Level* lvl;
    Treasure* treasure_obj;
    Label title_label;
    DynamicLabel result_label;
    TextButton rock_button;
    TextButton paper_button;
    TextButton scissors_button;
    TextButton exit_button;
    bool complete;

public:
    LockpickScreen(Level* level, Treasure* _treasure_obj);
    void update() override;
    void draw() override;
};

class PauseScreen : public EventScreen {
private:
    Level* lvl;
    Label title_label;
    TextButton continue_button;
    TextButton exit_button;

public:
    PauseScreen(Level* level);
    void update() override;
    void draw() override;
};

class BattleScreen : public EventScreen {
private:
    Level* lvl;
    Player* player;
    Enemy* enemy;
    int enemy_tile_id;
    int enemy_id;

    bool is_bossfight;
    int turn_num;

    Label title_label;
    DynamicLabel turn_num_label;
    DynamicLabel turn_phase_label;
    DynamicLabel turn_phase_description;
    // DynamicLabel turn_result;

    bool is_player_turn;

    // Rock is physical, paper is ranged, scissors are magic
    TextButton pdmg_button;
    TextButton rdmg_button;
    TextButton mdmg_button;

    TextButton pdef_button;
    TextButton rdef_button;
    TextButton mdef_button;

    bool is_completed;

    void get_reward();
    void show_gameover();
    void next_phase();

public:
    BattleScreen(
        Level* level, Player* player, Enemy* enemy, int enemy_tile_id, int enemy_id);
    void update() override;
    void draw() override;
};
