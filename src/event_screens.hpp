#pragma once

#include "level.hpp"
#include "raylib.h"

enum RPS
{
    RPS_ROCK,
    RPS_PAPER,
    RPS_SCISSORS
};

enum class MinigameStatus
{
    win,
    tie,
    lose
};

MinigameStatus play_rps(int your_throw);

// This contains various EventScreen implementations, to de-bloat level.cpp
// Abstract EventScreen is still part of level.hpp, due to dependency resolution.
class CompletionScreen : public EventScreen {
private:
    Level* lvl;
    Label completion_label;
    TextButton next_level_button;
    Button close_screen_button;

public:
    CompletionScreen(Level* level);
    void update() override;
    void draw() override;
};

class LockpickScreen : public EventScreen {
private:
    Level* lvl;
    Player* player_obj;
    Treasure* treasure_obj;
    Label title_label;
    DynamicLabel result_label;
    TextButton rock_button;
    TextButton paper_button;
    TextButton scissors_button;
    TextButton exit_button;
    bool complete;

public:
    LockpickScreen(Level* level, Treasure* _treasure_obj, Player* _player_obj);
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
