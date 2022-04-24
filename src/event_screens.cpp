#include "event_screens.hpp"
#include "engine/ui.hpp"
#include "level.hpp"
#include "raylib.h"
#include "common.hpp"

// For rand() in RPS minigame
#include <cstdlib>
// For basic formatting
#include <fmt/core.h>
// For fmt::dynamic_format_arg_store
#include <fmt/args.h>
#include <tuple>

enum RPS_BUTTONS {
    ROCK = 0,
    PAPER,
    SCISSORS
};

// Play rock-paper-scissors against RNGesus. Returns MinigameStatus, based on
// who won.
enum class RPS {
    rock,
    paper,
    scissors
};

enum class MinigameStatus {
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
              ((get_window_width() - get_window_height()) / 2.0f + 30),
              30,
              (get_window_width() + 30) / 2.0f,
              (get_window_height() - 60.0f)},
          {0, 0, 0, 0})
    , title_label(Label(title, {get_window_width() / 2.0f, 50.0f}))
    , body_label(Label(body, {get_window_width() / 2.0f, get_window_height() / 2.0f}))
    , close_button(make_text_button(button_txt))
    , complete(false) {
    title_label.center();
    body_label.center();
    close_button->set_pos(Vector2{
        get_window_width() / 2.0f - close_button->get_rect().width / 2,
        get_window_height() / 2.0f + 200});
}

NotificationScreen::~NotificationScreen() {
    delete close_button;
}

void NotificationScreen::set_title(std::string txt, bool center) {
    title_label.set_text(txt);
    if (center) title_label.center();
}

void NotificationScreen::set_title(std::string txt) {
    set_title(txt, false);
}

void NotificationScreen::set_description(std::string txt, bool center) {
    body_label.set_text(txt);
    if (center) body_label.center();
}

void NotificationScreen::set_description(std::string txt) {
    set_description(txt, false);
}

void NotificationScreen::set_button_text(std::string txt) {
    close_button->set_text(txt);
}

void NotificationScreen::update() {
    if (complete) return;
    close_button->update();

    if (close_button->is_clicked()) {
        complete = true;
    }
}

void NotificationScreen::draw() {
    if (complete) {
        return;
    }

    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);
    title_label.draw();
    body_label.draw();
    close_button->draw();
}

// Completion Screen
CompletionScreen::CompletionScreen(
    std::function<void()> next_lvl_callback, std::function<void()> close_callback)
    : EventScreen({
            ((get_window_width() - get_window_height()) / 2.0f + 30),
            30,
            (get_window_width() + 30) / 2.0f,
            (get_window_height() - 60.0f)},
        {0, 0, 0, 0})
    , title_label("Level Cleared!", {get_window_width() / 2.0f, 50.0f})
    , body_label("", {get_window_width() / 2.0f, get_window_height() / 2.0f}) {
    title_label.center();

    Button* next_lvl_button = make_text_button("Go Deeper!");
    next_lvl_button->set_callback(next_lvl_callback);
    next_lvl_button->set_pos({
        get_window_width() / 2.0f - next_lvl_button->get_rect().width / 2,
        get_window_height() / 2.0f + 200});
    buttons.add_button(next_lvl_button);

    Button* close_button = make_close_button();
    close_button->set_callback(close_callback);
    close_button->set_pos(
        {bg.x + bg.width - close_button->get_rect().width, bg.y});
    buttons.add_button(close_button);
}

void CompletionScreen::set_description(std::string txt) {
    body_label.set_text(txt);
    body_label.center();
}

void CompletionScreen::update() {
    buttons.update();
}

void CompletionScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);
    title_label.draw();
    body_label.draw();
    buttons.draw();
}

// Gameover Screen
// TODO: maybe merge it with some other, or something
// I did not reuse CompletionScreen on purpose, since this one may also include
// additional elements later, such as leaderboard
GameoverScreen::GameoverScreen(
    std::function<void()> restart_callback, std::function<void()> close_callback)
    : EventScreen({
            ((get_window_width() - get_window_height()) / 2.0f + 30),
            30,
            (get_window_width() + 30) / 2.0f,
            (get_window_height() - 60.0f)},
        {0, 0, 0, 0})
    , title_label("Game Over", {get_window_width() / 2.0f, 50.0f})
    , body_label("", {get_window_width() / 2.0f, get_window_height() / 2.0f})
    , buttons(32.0f) {
    title_label.center();

    buttons.set_pos({get_window_width() / 2.0f, get_window_height() / 2.0f + 200});

    Button* restart_button = make_text_button("Restart");
    restart_button->set_callback(restart_callback);
    buttons.add_button(restart_button);

    Button* close_button = make_text_button("Back to Menu");
    close_button->set_callback(close_callback);
    buttons.add_button(close_button);

    buttons.center();
}

void GameoverScreen::set_description(std::string txt) {
    body_label.set_text(txt);
    body_label.center();
}

void GameoverScreen::update() {
    buttons.update();
}

void GameoverScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);
    title_label.draw();
    body_label.draw();
    buttons.draw();
}

// Lockpick Screen
LockpickScreen::LockpickScreen(
    Treasure* _treasure_obj,
    std::function<void()> complete_cb,
    std::function<void(int)> reward_cb)
    : EventScreen({
            ((get_window_width() - get_window_height()) / 2.0f + 30),
            30,
            (get_window_width() + 30) / 2.0f,
            (get_window_height() - 60.0f)},
        {0, 0, 0, 0})
    , treasure_obj(_treasure_obj)
    , title_label(Label(
        "This chest is locked.\nMaybe it has something pricey inside?",
        {get_window_width() / 2.0f, 100.0f}))
    , buttons(32.0f)
    , result_screen(NotificationScreen("Lockpick Result", "", "OK"))
    , complete(false)
    , complete_callback(complete_cb)
    , reward_callback(reward_cb) {
    title_label.center();

    buttons.set_pos({get_window_width()/2.0f, get_window_height() / 2.0f});

    buttons.add_button(make_text_button("Use brute force"));
    buttons.add_button(make_text_button("Try to lockpick"));
    buttons.add_button(make_text_button("Cast unlocking magic"));

    buttons.center();
}

void LockpickScreen::update() {
    if (complete) {
        result_screen.update();
        if (result_screen.complete) {
            complete_callback();
            return;
        }
    }

    else {
        buttons.update();

        RPS rps_value;
        if (buttons[RPS_BUTTONS::ROCK]->is_clicked()) {
            rps_value = RPS::rock;
        }
        else if (buttons[RPS_BUTTONS::PAPER]->is_clicked()) {
            rps_value = RPS::paper;
        }
        else if (buttons[RPS_BUTTONS::SCISSORS]->is_clicked()) {
            rps_value = RPS::scissors;
        }
        else {
            return;
        }

        auto [status, _] = play_rps(rps_value);
        switch (status) {
        case MinigameStatus::win: {
            int value = treasure_obj->get_reward();
            result_screen.set_description(
                fmt::format(
                    "With no issues, you've flawlessly unlocked the chest.\n"
                    "{} coins lying inside were totally worth it!",
                    value),
                true);
            reward_callback(value);
            break;
        }

        case MinigameStatus::tie: {
            int value = treasure_obj->get_reward() / 2;
            result_screen.set_description(
                fmt::format(
                    "You've spent quite a while to unlock the chest, but\n"
                    "it felt like something has jammed inside. Out of anger,\n"
                    "you kick it in the back. As chest falls, you hear \n"
                    "something clicking inside - the very next second, it\n"
                    "finally opens up. Sadly, you can also hear the sound\n"
                    "of something metal rolling into darkness...\n"
                    "But, well - you've still got {} gold from it.",
                    value),
                true);
            reward_callback(value);
            break;
        }

        case MinigameStatus::lose: {
            treasure_obj->get_reward();
            result_screen.set_description(
                "You've been busy unlocking the chest for what felt as hours.\n"
                "Sadly, when you've finally got inside, you was dissapointed:\n"
                "there was nothing but pair of someone's smelly socks.",
                true);
            break;
        }
        }
        complete = true;

    }
}

void LockpickScreen::draw() {
    if (complete) {
        result_screen.draw();
    }
    else {
        DrawRectangleRec(bg, SIDE_BG_COLOR);
        DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

        title_label.draw();
        buttons.draw();
    }
}

// Pause Screen
PauseScreen::PauseScreen(std::function<void()> resume_cb, std::function<void()> exit_cb)
    : EventScreen(
        {
            ((get_window_width() - get_window_height()) / 2.0f + 30),
            30,
            (get_window_width() + 30) / 2.0f,
            (get_window_height() - 60.0f)},
        {0, 0, 0, 0})
    , title_label(Label("Game Paused", {get_window_width() / 2.0f, 160.0f}))
    , buttons(32.0f) {
    title_label.center();

    Button* resume_button = make_text_button("Continue");
    resume_button->set_callback(resume_cb);
    buttons.add_button(resume_button);

    Button* exit_button = make_text_button("Back to menu");
    exit_button->set_callback(exit_cb);
    buttons.add_button(exit_button);

    buttons.set_pos({get_window_width()/2.0f, get_window_height() / 2.0f});
    buttons.center();
}

void PauseScreen::update() {
    buttons.update();
}

void PauseScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    buttons.draw();
}

// Battle Screen

void BattleScreen::update_stats_hud() {
    player_stats.set_text(fmt::format(
        "Your Stats:\n\n"
        "HP: {} / {}\n\nDamage:\nPhysical: {}\nRanged: {}\nMagical: {}\n\n"
        "Defense:\nPhysical: {}\nRanged: {}\nMagical: {}",
        player->current_hp,
        player->max_hp,
        player->offensive_stats[OffensiveStats::pdmg],
        player->offensive_stats[OffensiveStats::rdmg],
        player->offensive_stats[OffensiveStats::mdmg],
        player->defensive_stats[DefensiveStats::pdef],
        player->defensive_stats[DefensiveStats::rdef],
        player->defensive_stats[DefensiveStats::mdef]));

    // This could and should be done way better. But for now it is what it is. TODO
    if (!know_everything && know_pdmg && know_rdmg && know_mdmg && know_pdef &&
        know_rdef && know_mdef) {
        know_everything = true;
    }

    if (know_everything) {
        enemy_stats.set_text(fmt::format(
            "Enemy Stats:\n\n"
            "HP: {} / {}\n\nDamage:\nPhysical: {}\nRanged: {}\nMagical: {}\n\n"
            "Defense:\nPhysical: {}\nRanged: {}\nMagical: {}",
            enemy->current_hp,
            enemy->max_hp,
            enemy->offensive_stats[OffensiveStats::pdmg],
            enemy->offensive_stats[OffensiveStats::rdmg],
            enemy->offensive_stats[OffensiveStats::mdmg],
            enemy->defensive_stats[DefensiveStats::pdef],
            enemy->defensive_stats[DefensiveStats::rdef],
            enemy->defensive_stats[DefensiveStats::mdef]));
    }
    else {
        fmt::dynamic_format_arg_store<fmt::format_context> args;
        args.push_back(enemy->current_hp);
        args.push_back(enemy->max_hp);

        if (know_pdmg) args.push_back(enemy->offensive_stats[OffensiveStats::pdmg]);
        else args.push_back("???");

        if (know_rdmg) args.push_back(enemy->offensive_stats[OffensiveStats::rdmg]);
        else args.push_back("???");

        if (know_mdmg) args.push_back(enemy->offensive_stats[OffensiveStats::mdmg]);
        else args.push_back("???");

        if (know_pdef) args.push_back(enemy->defensive_stats[DefensiveStats::pdef]);
        else args.push_back("???");

        if (know_rdef) args.push_back(enemy->defensive_stats[DefensiveStats::rdef]);
        else args.push_back("???");

        if (know_mdef) args.push_back(enemy->defensive_stats[DefensiveStats::mdef]);
        else args.push_back("???");

        enemy_stats.set_text(fmt::vformat(
            "Enemy Stats:\n\n"
            "HP: {} / {}\n\nDamage:\nPhysical: {}\nRanged: {}\nMagical: {}\n\n"
            "Defense:\nPhysical: {}\nRanged: {}\nMagical: {}",
            args));
    }
}

BattleScreen::BattleScreen(
    Level* level, Player* _player, Enemy* _enemy, int _enemy_tile_id, int _enemy_id)
    : EventScreen(
          Rectangle{30.0f, 30.0f, get_window_width() - 60.0f, get_window_height() - 60.0f},
          {0, 0, 0, 0})
    , lvl(level)
    , player(_player)
    , enemy(_enemy)
    , enemy_tile_id(_enemy_tile_id)
    , enemy_id(_enemy_id)
    , is_bossfight(_enemy->is_boss())
    , turn_num(0)
    , title_label(Label("Battle", {get_window_width() / 2.0f, 60.0f}))
    , turn_num_label(Label("", {get_window_width() / 2.0f, 90.0f}))
    , turn_phase_label(Label("", {get_window_width() / 2.0f, 120.0f}))
    , turn_phase_description(Label("", {get_window_width() / 2.0f, 150.0f}))
    , turn_result(Label(
          "With your trusted weapon, you\nstand before unholy creature",
          Vector2{0.0f, 0.0f}))
    , player_stats(Label("", Vector2{0.0f, 0.0f}))
    , enemy_stats(Label("", Vector2{0.0f, 0.0f}))
    , is_player_turn(false)
    , know_pdmg(false)
    , know_rdmg(false)
    , know_mdmg(false)
    , know_pdef(false)
    , know_rdef(false)
    , know_mdef(false)
    , know_everything(false)
    , dmg_buttons(32.0f)
    , def_buttons(32.0f)
    , completion_result(CompletionResult::none)
    , result_screen(NotificationScreen("", "", "")) {
    // TODO: add ambushes where enemy actually start first
    next_phase();
    title_label.center();
    turn_num_label.center();
    turn_phase_label.center();
    turn_phase_description.center();

    player_stats.set_pos({bg.x + (bg.width / 10.0f), bg.y + bg.height / 2.0f});
    enemy_stats.set_pos({bg.x + (bg.width / 10.0f) * 9.0f, bg.y + bg.height / 2.0f});
    update_stats_hud();
    player_stats.center();
    enemy_stats.center();

    turn_result.set_pos({bg.x + (bg.width / 5.0f) * 3.0f, bg.y + bg.height / 2.0f});
    turn_result.center();

    float button_x = bg.x + (bg.width / 5.0f);
    float button_y = bg.y + (bg.height / 2.0f) / 1.5f;

    dmg_buttons.add_button(make_text_button("Use sword (Physical)"));
    dmg_buttons.add_button(make_text_button("Use bow (Ranged)"));
    dmg_buttons.add_button(make_text_button("Use magic (Magical)"));

    def_buttons.add_button(make_text_button("Raise shield (Physical)"));
    def_buttons.add_button(make_text_button("Try to evade (Ranged)"));
    def_buttons.add_button(make_text_button("Cast protection (Magical)"));

    dmg_buttons.set_pos({button_x, button_y});
    def_buttons.set_pos({button_x, button_y});
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
        turn_num_label.set_text(fmt::format("Turn {}", turn_num));
        turn_phase_description.set_text("Pick attack type to use");
    }
}

void BattleScreen::get_reward() {
    // TODO: make enemies hold some money in their pouches, like chests
    // Until then, its hardcoded
    int reward = 100;
    lvl->give_player_money(reward);

    std::string result_txt = "Surviving the most powerful enemy attacks, you finally\n"
                             "deal the final blow.\n";

    if (is_bossfight) {
        int hp_reward = 10;
        player->increase_max_hp(hp_reward, true);
        result_txt += fmt::format(
            "Beheaded, creature collapses - yet something keeps beating\n"
            "in its chest, you can clearly hear it. With help of your\n"
            "knife, you carve this thing out and give it a bite.\n"
            "Surprisingly, it not only tastes like freshly-baked bun, but\n"
            "also increases your max health by {}!",
            hp_reward);
    }

    result_txt += fmt::format(
        "\nAs creature's remains decay, you notice something shiny\n"
        "lying on the ground. Yay, {} gold to buy some snacks!",
        reward);

    // TODO: add battle statistics (turns made, damage dealt/received)
    completion_result = CompletionResult::win;
    result_screen.set_title("Battle Results", true);
    result_screen.set_description(result_txt, true);
    result_screen.set_button_text("OK");
}

void BattleScreen::show_gameover() {
    completion_result = CompletionResult::lose;
    result_screen.set_title("Death", true);
    result_screen.set_description(
        "As enemy's attacks start getting more fearsome, your\n"
        "breath gets heavier. Blood drips onto your eyes, making it\n"
        "harder to see what's coming next. And thats how you miss it.\n"
        "You can't see what exactly hit you. Only feel sadness mixed\n"
        "with relief. \"Finally, it's over\" are your last thoughts\n"
        "until your body collapses and consciousness fades away.",
        true);
    result_screen.set_button_text("F");
}

void BattleScreen::update() {
    // This could be redone better with switch, I think
    if (completion_result != CompletionResult::none) {
        result_screen.update();
        if (result_screen.complete) {
            if (completion_result == CompletionResult::win) {
                lvl->kill_enemy(enemy_tile_id, enemy_id);
            }
            else {
                lvl->show_gameover();
            }
            lvl->update_player_stats_hud();
            lvl->complete_event();
            return;
        }
        return;
    }

    RPS rps_value;
    ButtonStorage* current_buttons;

    if (is_player_turn) {
        current_buttons = &dmg_buttons;
    }
    else {
        current_buttons = &def_buttons;
    }

    current_buttons->update();
    if (current_buttons->at(RPS_BUTTONS::ROCK)->is_clicked()) {
        rps_value = RPS::rock;
    }
    else if (current_buttons->at(RPS_BUTTONS::PAPER)->is_clicked()) {
        rps_value = RPS::paper;
    }
    else if (current_buttons->at(RPS_BUTTONS::SCISSORS)->is_clicked()) {
        rps_value = RPS::scissors;
    }
    else {
        return;
    }

    auto [status, their_throw] = play_rps(rps_value);
    // TODO: result text based on damage type used.
    std::string result;
    switch (status) {
    case MinigameStatus::win: {
        OffensiveStats dmg_stat = rps_to_offensive(rps_value);
        int dmg_value;
        if (is_player_turn) {
            dmg_value = enemy->damage(player->offensive_stats[dmg_stat], dmg_stat);
            result = fmt::format("You smash the monster for {} dmg!", dmg_value);
        }
        else {
            dmg_value =
                enemy->damage(player->offensive_stats[dmg_stat] / 2, dmg_stat);
            result = fmt::format(
                "While creature tries to hit you,\n"
                "you find an opening and counter\n"
                "its attack, dealing {} dmg to it!\n",
                dmg_value);
        }

        if (!know_everything) {
            switch (dmg_stat) {
            case OffensiveStats::pdmg: {
                know_pdef = true;
                break;
            }
            case OffensiveStats::rdmg: {
                know_rdef = true;
                break;
            }
            case OffensiveStats::mdmg: {
                know_mdef = true;
                break;
            }
            }
        }
        break;
    }

    case MinigameStatus::tie: {
        if (is_player_turn) {
            result = "You try to hit the beast, but\n"
                        "it easily shields the strike.";
        }
        else {
            result = "You've guessed enemy's intentions\n"
                        "correctly and avoided getting hurt.";
        }
        break;
    }

    case MinigameStatus::lose: {
        OffensiveStats dmg_stat = rps_to_offensive(their_throw);
        int dmg_value;
        if (is_player_turn) {
            dmg_value =
                player->damage(enemy->offensive_stats[dmg_stat] / 2, dmg_stat);
            result = fmt::format(
                "As you try to land a perfect hit,\n"
                "enemy suddenly strikes back, dealing\n"
                "{} damage to you.",
                dmg_value);
        }
        else {
            dmg_value = player->damage(enemy->offensive_stats[dmg_stat], dmg_stat);
            result = fmt::format(
                "Your attempt to protect youself\n"
                "fails miserably, as beast strikes\n"
                "you for {} damage. Shame.",
                dmg_value);
        }

        if (!know_everything) {
            switch (dmg_stat) {
            case OffensiveStats::pdmg: {
                know_pdmg = true;
                break;
            }
            case OffensiveStats::rdmg: {
                know_rdmg = true;
                break;
            }
            case OffensiveStats::mdmg: {
                know_mdmg = true;
                break;
            }
            }
        }
        break;
    }
    }

    update_stats_hud();

    if (player->is_dead()) {
        show_gameover();
        return;
    }
    else if (enemy->is_dead()) {
        get_reward();
        return;
    }

    result += "\n\nYour enemy stands still.";
    turn_result.set_text(result);
    turn_result.center();
    next_phase();
}

void BattleScreen::draw() {
    if (completion_result != CompletionResult::none) {
        result_screen.draw();
        return;
    }

    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    turn_num_label.draw();
    turn_phase_label.draw();
    turn_phase_description.draw();
    player_stats.draw();
    enemy_stats.draw();
    turn_result.draw();

    if (is_player_turn) {
        dmg_buttons.draw();
    }
    else {
        def_buttons.draw();
    }
}
