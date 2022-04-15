#pragma once

#include "level.hpp"
#include "engine/ui.hpp"
#include "raylib.h"

#include <string>

// This contains various EventScreen implementations, to de-bloat level.cpp
// Abstract EventScreen is still part of level.hpp, due to dependency resolution.

class NotificationScreen : public EventScreen {
private:
    Label title_label;
    Label body_label;
    Button* close_button;

public:
    bool complete;
    NotificationScreen(std::string title, std::string body, std::string button_txt);
    ~NotificationScreen();

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
    Button* next_level_button;
    Button* close_screen_button;

public:
    CompletionScreen(
        Level* level, int turns_made, int money_collected, int enemies_killed);
    ~CompletionScreen();

    void update() override;
    void draw() override;
};

class LockpickScreen : public EventScreen {
private:
    Level* lvl;
    Treasure* treasure_obj;
    Label title_label;
    Button* rock_button;
    Button* paper_button;
    Button* scissors_button;
    NotificationScreen result_screen;
    bool complete;

public:
    LockpickScreen(Level* level, Treasure* _treasure_obj);
    ~LockpickScreen();

    void update() override;
    void draw() override;
};

class PauseScreen : public EventScreen {
private:
    Level* lvl;
    Label title_label;
    Button* continue_button;
    Button* exit_button;

public:
    PauseScreen(Level* level);
    ~PauseScreen();

    void update() override;
    void draw() override;
};

class BattleScreen : public EventScreen {
private:
    enum class CompletionResult {
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

    // This is garbage, but for now this is how we will toggle showcase of certain
    // enemy stats. The last one is toggled when everything else is toggled
    bool know_pdmg;
    bool know_rdmg;
    bool know_mdmg;
    bool know_pdef;
    bool know_rdef;
    bool know_mdef;
    bool know_everything;

    // Rock is physical, paper is ranged, scissors are magic
    Button* pdmg_button;
    Button* rdmg_button;
    Button* mdmg_button;

    Button* pdef_button;
    Button* rdef_button;
    Button* mdef_button;

    CompletionResult completion_result;
    NotificationScreen result_screen;

    void update_stats_hud();
    void get_reward();
    void show_gameover();
    void next_phase();

public:
    BattleScreen(
        Level* level, Player* player, Enemy* enemy, int enemy_tile_id, int enemy_id);
    ~BattleScreen();

    void update() override;
    void draw() override;
};
