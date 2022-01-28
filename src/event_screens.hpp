#pragma once

#include "level.hpp"
#include "raylib.h"

#include <string>

// This contains various EventScreen implementations, to de-bloat level.cpp
// Abstract EventScreen is still part of level.hpp, due to dependency resolution.

class NotificationScreen : public EventScreen {
private:
    Label title_label;
    Label body_label;
    TextButton close_button;

public:
    bool complete;
    NotificationScreen(std::string title, std::string body, std::string button_txt);

    void set_title(std::string txt, bool center);
    void set_title(std::string txt);
    void set_description(std::string txt, bool center);
    void set_description(std::string txt);
    void set_button_text(std::string txt);

    void update() override;
    void draw() override;
};

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
    TextButton rock_button;
    TextButton paper_button;
    TextButton scissors_button;
    NotificationScreen result_screen;
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
    enum class CompletionResult
    {
        none,
        win,
        lose
    };

    Level* lvl;
    Player* player;
    Enemy* enemy;
    int enemy_tile_id;
    int enemy_id;

    bool is_bossfight;
    int turn_num;

    Label title_label;
    Label turn_num_label;
    Label turn_phase_label;
    Label turn_phase_description;
    Label turn_result;
    Label player_stats;
    Label enemy_stats;

    bool is_player_turn;

    // Rock is physical, paper is ranged, scissors are magic
    TextButton pdmg_button;
    TextButton rdmg_button;
    TextButton mdmg_button;

    TextButton pdef_button;
    TextButton rdef_button;
    TextButton mdef_button;

    CompletionResult completion_result;
    NotificationScreen result_screen;

    void update_stats_hud();
    void get_reward();
    void show_gameover();
    void next_phase();

public:
    BattleScreen(
        Level* level, Player* player, Enemy* enemy, int enemy_tile_id, int enemy_id);
    void update() override;
    void draw() override;
};
