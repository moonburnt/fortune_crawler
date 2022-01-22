#include "event_screens.hpp"
#include "level.hpp"
#include "raylib.h"
#include "ui.hpp"

// For rand() in RPS minigame
#include <cstdlib>
// For basic formatting
#include <fmt/core.h>

// Play rock-paper-scissors against RNGesus. Returns MinigameStatus, based on
// who won.
MinigameStatus play_rps(int your_throw) {
    // 0 is rock, 1 is paper, 2 is scissors
    const static MinigameStatus result[3][3]{
        {MinigameStatus::tie, MinigameStatus::win, MinigameStatus::lose},
        {MinigameStatus::lose, MinigameStatus::tie, MinigameStatus::win},
        {MinigameStatus::win, MinigameStatus::lose, MinigameStatus::tie}};

    return result[rand() % 3][your_throw];
}

// Completion Screen
CompletionScreen::CompletionScreen(
    Level* level, int turns_made, int money_collected, int enemies_killed)
    : EventScreen(
          Rectangle{
              ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
              30,
              (GetScreenWidth() + 30) / 2.0f,
              (GetScreenHeight() - 60.0f)},
          {0, 0, 0, 0})
    , lvl(level)
    , completion_label(Label(
          fmt::format(
              "Level Completed!\n\n"
              "Turns made: {}\nMoney collected: {}\nEnemies killed: {}",
              turns_made,
              money_collected,
              enemies_killed),
          GetScreenWidth() / 2,
          GetScreenHeight() / 2))
    , next_level_button(make_text_button("Go Deeper!"))
    , close_screen_button(make_close_button()) {
    completion_label.center();

    next_level_button.set_pos(Vector2{
        GetScreenWidth() / 2.0f - next_level_button.get_rect().width / 2,
        GetScreenHeight() / 2.0f + 200});

    close_screen_button.set_pos(
        Vector2{bg.x + bg.width - close_screen_button.get_rect().width, bg.y});
}

void CompletionScreen::update() {
    // TODO: add details to completion screen (amount of turns made,
    // enemies killed, etc)
    next_level_button.update();
    close_screen_button.update();

    if (next_level_button.is_clicked()) {
        lvl->change_map();
        lvl->complete_event();
        return;
    }

    if (close_screen_button.is_clicked()) {
        lvl->complete_event();
        return;
    }
}

void CompletionScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);
    completion_label.draw();
    next_level_button.draw();
    close_screen_button.draw();
}

// Lockpick Screen
LockpickScreen::LockpickScreen(Level* level, Treasure* _treasure_obj)
    : EventScreen(
          Rectangle{
              ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
              30,
              (GetScreenWidth() + 30) / 2.0f,
              (GetScreenHeight() - 60.0f)},
          {0, 0, 0, 0})
    , lvl(level)
    , treasure_obj(_treasure_obj)
    , title_label(
          Label("This chest is locked.\nTry to lockpick it!", GetScreenWidth() / 2, 100))
    , result_label(DynamicLabel("", GetScreenWidth() / 2, GetScreenHeight() / 2))
    , rock_button(make_text_button("Rock"))
    , paper_button(make_text_button("Paper"))
    , scissors_button(make_text_button("Scissors"))
    , exit_button(make_text_button("OK"))
    , complete(false) {
    title_label.center();
    float button_x = (GetScreenWidth() - GetScreenHeight()) / 2.0f + 30 * 2.0f;
    rock_button.set_pos(Vector2{button_x, 200.0f});
    paper_button.set_pos(Vector2{button_x, 300.0f});
    scissors_button.set_pos(Vector2{button_x, 400.0f});

    exit_button.set_pos(Vector2{
        GetScreenWidth() / 2.0f - exit_button.get_rect().width / 2,
        GetScreenHeight() / 2.0f + 200});
}

void LockpickScreen::update() {
    if (complete) {
        exit_button.update();

        if (exit_button.is_clicked()) {
            lvl->complete_event();
            return;
        }
    }

    else {
        rock_button.update();
        paper_button.update();
        scissors_button.update();

        bool button_clicked = false;
        int rps_value;

        if (rock_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS_ROCK;
        }
        else if (paper_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS_PAPER;
        }
        else if (scissors_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS_SCISSORS;
        }

        if (button_clicked) {
            switch (play_rps(rps_value)) {
            case MinigameStatus::win: {
                int value = treasure_obj->get_reward();
                result_label.set_text(fmt::format(
                    "With no issues, you've flawlessly unlocked the chest.\n"
                    "{} coins lying inside were totally worth it!",
                    value));
                lvl->give_player_money(value);
                break;
            }

            case MinigameStatus::tie: {
                int value = treasure_obj->get_reward() / 2;
                result_label.set_text(fmt::format(
                    "While attempting to unlock the chest, your pick has broke.\n"
                    "With no other options left, you've had to use brute force.\n"
                    "Sadly, while doing so, some coins felt into darkness...\n"
                    "But you've still got {} gold from it.",
                    value));
                lvl->give_player_money(value);
                break;
            }

            case MinigameStatus::lose: {
                treasure_obj->get_reward();
                result_label.set_text(
                    "You've spent quite a while to unlock the chest.\n"
                    "Sadly, when you've finally got inside, you was dissapointed:\n"
                    "there was nothing but pair of someone's smelly socks.");
                break;
            }
            }
            result_label.center();
            complete = true;
        }
    }
}

void LockpickScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    if (complete) {
        result_label.draw();
        exit_button.draw();
    }
    else {
        title_label.draw();

        rock_button.draw();
        paper_button.draw();
        scissors_button.draw();
    }
}

// Pause Screen
PauseScreen::PauseScreen(Level* level)
    : EventScreen(
          Rectangle{
              ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
              30,
              (GetScreenWidth() + 30) / 2.0f,
              (GetScreenHeight() - 60.0f)},
          {0, 0, 0, 0})
    , lvl(level)
    , title_label(Label("Game Paused", GetScreenWidth() / 2, 160.0f))
    , continue_button(make_text_button("Continue"))
    , exit_button(make_text_button("Back to menu")) {
    title_label.center();
    continue_button.set_pos(Vector2{
        GetScreenWidth() / 2.0f - continue_button.get_rect().width / 2,
        GetScreenHeight() / 2.0f});
    exit_button.set_pos(Vector2{
        GetScreenWidth() / 2.0f - exit_button.get_rect().width / 2,
        GetScreenHeight() / 2.0f + 100});
}

void PauseScreen::update() {
    continue_button.update();
    exit_button.update();

    if (continue_button.is_clicked()) {
        lvl->is_paused = false;
        continue_button.reset_state();
        return;
    }
    if (exit_button.is_clicked()) {
        lvl->exit_to_menu();
        return;
    }
}

void PauseScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    continue_button.draw();
    exit_button.draw();
}
