#pragma once

#include "level.hpp"
#include "engine/ui.hpp"
#include "raylib.h"

#include <string>

class App;

// This contains various EventScreen implementations, to de-bloat level.cpp
// Abstract EventScreen is still part of level.hpp, due to dependency resolution.

class NotificationScreen : public EventScreen {
private:
    Label title_label;
    Label body_label;
    Button* close_button;

public:
    bool complete;
    NotificationScreen(App* app, std::string title, std::string body, std::string button_txt);
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
    Label title_label;
    Label body_label;
    ButtonStorage buttons;

public:
    CompletionScreen(
        App* app,
        std::function<void()> next_lvl_callback,
        std::function<void()> close_callback);

    void set_description(std::string txt);

    void update() override;
    void draw() override;
};

class GameoverScreen : public EventScreen {
private:
    Label title_label;
    Label body_label;
    VerticalContainer buttons;

public:
    GameoverScreen(
        App* app,
        std::function<void()> restart_callback,
        std::function<void()> close_callback);

    void set_description(std::string txt);

    void update() override;
    void draw() override;
};


class LockpickScreen : public EventScreen {
private:
    Treasure* treasure_obj;
    Label title_label;
    VerticalContainer buttons;
    NotificationScreen result_screen;
    bool complete;
    std::function<void()> complete_callback;
    std::function<void(int)> reward_callback;

public:
    LockpickScreen(
        App* app,
        Treasure* _treasure_obj,
        std::function<void()> complete_cb,
        std::function<void(int)> reward_cb);

    void update() override;
    void draw() override;
};

class PauseScreen : public EventScreen {
private:
    Label title_label;
    VerticalContainer buttons;

public:
    PauseScreen(
        App* app,
        std::function<void()> resume_cb,
        std::function<void()> exit_cb);

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
    // Button* pdmg_button;
    // Button* rdmg_button;
    // Button* mdmg_button;

    // Button* pdef_button;
    // Button* rdef_button;
    // Button* mdef_button;
    VerticalContainer dmg_buttons;
    VerticalContainer def_buttons;

    CompletionResult completion_result;
    NotificationScreen result_screen;

    void update_stats_hud();
    void get_reward();
    void show_gameover();
    void next_phase();

public:
    // This still use old pointer access method, coz I didn't find profits from
    // switching to callbacks to be sufficient enough.
    BattleScreen(
        App* app,
        Level* level,
        Player* player,
        Enemy* enemy,
        int enemy_tile_id,
        int enemy_id);
    // ~BattleScreen();

    void update() override;
    void draw() override;
};
