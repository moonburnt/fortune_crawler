#include "event_screens.hpp"
#include "level.hpp"
#include "raylib.h"
#include "ui.hpp"

// For rand() in RPS minigame
#include <cstdlib>
// For basic formatting
#include <fmt/core.h>
#include <tuple>

// Play rock-paper-scissors against RNGesus. Returns MinigameStatus, based on
// who won.
enum class RPS
{
    rock,
    paper,
    scissors
};

enum class MinigameStatus
{
    win,
    tie,
    lose
};

int rps_to_int(RPS value) {
    int result_value;
    switch (value) {
    case RPS::rock: {
        result_value = 0;
        break;
    }
    case RPS::paper: {
        result_value = 1;
        break;
    }
    case RPS::scissors: {
        result_value = 2;
        break;
    }
    }
    return result_value;
}

OffensiveStats rps_to_offensive(RPS value) {
    OffensiveStats stat;
    switch (value) {
    case RPS::rock: {
        stat = OffensiveStats::pdmg;
        break;
    }
    case RPS::paper: {
        stat = OffensiveStats::rdmg;
        break;
    }
    case RPS::scissors: {
        stat = OffensiveStats::mdmg;
        break;
    }
    }
    return stat;
}

DefensiveStats rps_to_defensive(RPS value) {
    DefensiveStats stat;
    switch (value) {
    case RPS::rock: {
        stat = DefensiveStats::pdef;
        break;
    }
    case RPS::paper: {
        stat = DefensiveStats::rdef;
        break;
    }
    case RPS::scissors: {
        stat = DefensiveStats::mdef;
        break;
    }
    }
    return stat;
}

std::tuple<MinigameStatus, RPS> play_rps(RPS your_throw) {
    int throw_value = rps_to_int(your_throw);

    // 0 is rock, 1 is paper, 2 is scissors
    const static RPS int_to_rps[3]{RPS::rock, RPS::paper, RPS::scissors};

    const static MinigameStatus result[3][3]{
        {MinigameStatus::tie, MinigameStatus::win, MinigameStatus::lose},
        {MinigameStatus::lose, MinigameStatus::tie, MinigameStatus::win},
        {MinigameStatus::win, MinigameStatus::lose, MinigameStatus::tie}};

    int their_throw = rand() % 3;
    return std::make_tuple(result[their_throw][throw_value], int_to_rps[their_throw]);
}

// Notification

NotificationScreen::NotificationScreen(
    std::string title, std::string body, std::string button_txt)
    : EventScreen(
          Rectangle{
              ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
              30,
              (GetScreenWidth() + 30) / 2.0f,
              (GetScreenHeight() - 60.0f)},
          {0, 0, 0, 0})
    , title_label(Label(title, GetScreenWidth() / 2, 50))
    , body_label(Label(body, GetScreenWidth() / 2, GetScreenHeight() / 2))
    , close_button(make_text_button(button_txt))
    , complete(false) {
    title_label.center();
    body_label.center();
    close_button.set_pos(Vector2{
        GetScreenWidth() / 2.0f - close_button.get_rect().width / 2,
        GetScreenHeight() / 2.0f + 200});
}

void NotificationScreen::update() {
    if (complete) return;
    close_button.update();

    if (close_button.is_clicked()) complete = true;
}

void NotificationScreen::draw() {
    if (complete) return;

    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);
    title_label.draw();
    body_label.draw();
    close_button.draw();
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
    , rock_button(make_text_button("Rock"))
    , paper_button(make_text_button("Paper"))
    , scissors_button(make_text_button("Scissors"))
    , exit_button(make_text_button("OK")) {
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
    if (result_screen) {
        result_screen.value().update();
        if (result_screen.value().complete) {
            lvl->complete_event();
            return;
        }
    }

    else {
        rock_button.update();
        paper_button.update();
        scissors_button.update();

        bool button_clicked = false;
        RPS rps_value;

        if (rock_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::rock;
        }
        else if (paper_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::paper;
        }
        else if (scissors_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::scissors;
        }

        if (button_clicked) {
            std::string result_label = "Lockpick Result";
            std::string close_button_txt = "OK";
            auto [status, _] = play_rps(rps_value);
            switch (status) {
            case MinigameStatus::win: {
                int value = treasure_obj->get_reward();
                result_screen = NotificationScreen(
                    result_label,
                    fmt::format(
                        "With no issues, you've flawlessly unlocked the chest.\n"
                        "{} coins lying inside were totally worth it!",
                        value),
                    close_button_txt);
                lvl->give_player_money(value);
                break;
            }

            case MinigameStatus::tie: {
                int value = treasure_obj->get_reward() / 2;
                result_screen = NotificationScreen(
                    result_label,
                    fmt::format(
                        "While attempting to unlock the chest, your pick has broke.\n"
                        "With no other options left, you've had to use brute force.\n"
                        "Sadly, while doing so, some coins felt into darkness...\n"
                        "But you've still got {} gold from it.",
                        value),
                    close_button_txt);
                lvl->give_player_money(value);
                break;
            }

            case MinigameStatus::lose: {
                treasure_obj->get_reward();
                result_screen = NotificationScreen(
                    result_label,
                    "You've spent quite a while to unlock the chest.\n"
                    "Sadly, when you've finally got inside, you was dissapointed:\n"
                    "there was nothing but pair of someone's smelly socks.",
                    close_button_txt);
                break;
            }
            }
        }
    }
}

void LockpickScreen::draw() {
    if (result_screen) {
        result_screen.value().draw();
    }
    else {
        DrawRectangleRec(bg, SIDE_BG_COLOR);
        DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

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

// Battle Screen

BattleScreen::BattleScreen(
    Level* level, Player* _player, Enemy* _enemy, int _enemy_tile_id, int _enemy_id)
    : EventScreen(
          Rectangle{
              ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
              30,
              (GetScreenWidth() + 30) / 2.0f,
              (GetScreenHeight() - 60.0f)},
          {0, 0, 0, 0})
    , lvl(level)
    , player(_player)
    , enemy(_enemy)
    , enemy_tile_id(_enemy_tile_id)
    , enemy_id(_enemy_id)
    , is_bossfight(_enemy->is_boss())
    , turn_num(0)
    , title_label(Label("Battle", GetScreenWidth() / 2, 60))
    , turn_num_label(DynamicLabel("Turn {}", GetScreenWidth() / 2, 90))
    , turn_phase_label(DynamicLabel("", GetScreenWidth() / 2, 120))
    , turn_phase_description(DynamicLabel("", GetScreenWidth() / 2, 150))
    , is_player_turn(false)
    , pdmg_button(make_text_button("Use sword (Physical)"))
    , rdmg_button(make_text_button("Use bow (Ranged)"))
    , mdmg_button(make_text_button("Use magic (Magical)"))
    , pdef_button(make_text_button("Raise shield (Physical)"))
    , rdef_button(make_text_button("Try to evade (Ranged)"))
    , mdef_button(make_text_button("Cast protection (Magical)"))
    , is_completed(false) {
    // TODO: add ambushes where enemy actually start first
    next_phase();
    title_label.center();
    turn_num_label.center();
    turn_phase_label.center();
    turn_phase_description.center();

    float button_x = (GetScreenWidth() - GetScreenHeight()) / 2.0f + 30 * 2.0f;
    pdmg_button.set_pos(Vector2{button_x, 200.0f});
    rdmg_button.set_pos(Vector2{button_x, 300.0f});
    mdmg_button.set_pos(Vector2{button_x, 400.0f});

    pdef_button.set_pos(Vector2{button_x, 200.0f});
    rdef_button.set_pos(Vector2{button_x, 300.0f});
    mdef_button.set_pos(Vector2{button_x, 400.0f});
}

void BattleScreen::next_phase() {
    if (is_player_turn) {
        is_player_turn = false;
        turn_phase_label.set_text("Enemy Turn");
        turn_phase_description.set_text("Pick defence type to use");
    }
    else {
        is_player_turn = true;
        turn_phase_label.set_text("Your Turn");
        turn_num++;
        turn_num_label.set_text(fmt::format(turn_num_label.get_default_text(), turn_num));
        turn_phase_description.set_text("Pick attack type to use");
    }
}

void BattleScreen::get_reward() {
    // TODO: make enemies hold some money in their pouches, like chests
    // Until then, its hardcoded
    lvl->give_player_money(100);

    if (is_bossfight) {
        player->increase_max_hp(10, true);
    }
    // TODO: stub
    lvl->kill_enemy(enemy_tile_id, enemy_id);
    lvl->complete_event();
}

void BattleScreen::show_gameover() {
    // TODO: stub
}

void BattleScreen::update() {
    // TODO: stub
    if (is_completed) return;

    bool button_clicked = false;
    RPS rps_value;

    if (is_player_turn) {
        pdmg_button.update();
        rdmg_button.update();
        mdmg_button.update();

        if (pdmg_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::rock;
        }
        else if (rdmg_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::paper;
        }
        else if (mdmg_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::scissors;
        }
    }
    else {
        pdef_button.update();
        rdef_button.update();
        mdef_button.update();

        if (pdef_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::rock;
        }
        else if (rdef_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::paper;
        }
        else if (mdef_button.is_clicked()) {
            button_clicked = true;
            rps_value = RPS::scissors;
        }
    }

    if (button_clicked) {
        auto [status, their_throw] = play_rps(rps_value);
        switch (status) {
        case MinigameStatus::win: {
            OffensiveStats dmg_stat = rps_to_offensive(rps_value);
            if (is_player_turn) {
                enemy->damage(player->offensive_stats[dmg_stat], dmg_stat);
            }
            else {
                enemy->damage(player->offensive_stats[dmg_stat] / 2, dmg_stat);
            }
            break;
        }

        case MinigameStatus::tie: {
            // TODO: add description text, telling that damage has been shielded
            break;
        }

        case MinigameStatus::lose: {
            OffensiveStats dmg_stat = rps_to_offensive(their_throw);
            if (is_player_turn) {
                player->damage(enemy->offensive_stats[dmg_stat] / 2, dmg_stat);
            }
            else {
                player->damage(enemy->offensive_stats[dmg_stat], dmg_stat);
            }
            break;
        }
        }
        lvl->update_player_stats_hud();

        if (player->is_dead()) {
            is_completed = true;
            show_gameover();
        }
        else if (enemy->is_dead()) {
            is_completed = true;
            get_reward();
        }
        else {
            next_phase();
        }
    }
}
void BattleScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    turn_num_label.draw();
    turn_phase_label.draw();
    turn_phase_description.draw();

    if (is_player_turn) {
        pdmg_button.draw();
        rdmg_button.draw();
        mdmg_button.draw();
    }
    else {
        pdef_button.draw();
        rdef_button.draw();
        mdef_button.draw();
    }
}
