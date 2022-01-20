#include "level.hpp"
#include "entity.hpp"
#include "loader.hpp"
#include "utility.hpp"

#include <algorithm>
#include <optional>
// For rand() in RPS minigame
#include <cstdlib>
#include <raylib.h>
// For basic formatting
#include <fmt/core.h>
// For fmt::join
#include <fmt/format.h>

// TODO: make this configurable from settings
// Sane values would be 1.0 -> 3.0, everything bigger would make things render
// too close. Everything less makes no sense.
static constexpr float CAMERA_ZOOM = 2.0f;

static constexpr Color SIDE_BG_COLOR{203, 219, 252, 255};
static constexpr Color CORNER_COLOR{34, 32, 52, 255};
static constexpr Color BG_COLOR{63, 63, 116, 255};

InputController::InputController() {
    buttons_held = {KEY_NULL};
    add_relationship(KEY_NULL, MovementDirection::none);
}

void InputController::add_relationship(int key, MovementDirection direction) {
    key_binds[key] = direction;
}

void InputController::update() {
    for (auto& kv : key_binds) {
        if (IsKeyDown(kv.first)) {
            if (!std::count(buttons_held.begin(), buttons_held.end(), kv.first)) {
                buttons_held.push_back(kv.first);
            }
        }
        else {
            auto it = std::find(buttons_held.begin(), buttons_held.end(), kv.first);
            if (it != buttons_held.end()) buttons_held.erase(it);
        }
    }
}

MovementDirection InputController::get_movement_direction() {
    return key_binds[buttons_held.back()];
}

EventScreen::EventScreen(Rectangle _bg, Color _bg_color)
    : bg(_bg)
    , bg_color(_bg_color) {
}

class CompletionScreen : public EventScreen {
private:
    Level* lvl;
    Label completion_label;
    TextButton next_level_button;
    Button close_screen_button;

public:
    CompletionScreen(Level* level)
        : EventScreen(
              Rectangle{
                  ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
                  30,
                  (GetScreenWidth() + 30) / 2.0f,
                  (GetScreenHeight() - 60.0f)},
              bg_color)
        , lvl(level)
        , completion_label(
              Label("Level Completed!", GetScreenWidth() / 2, GetScreenHeight() / 2))
        , next_level_button(make_text_button("Go Deeper!"))
        , close_screen_button(make_close_button()) {
        completion_label.center();

        next_level_button.set_pos(Vector2{
            GetScreenWidth() / 2.0f - next_level_button.get_rect().width / 2,
            GetScreenHeight() / 2.0f + 200});

        close_screen_button.set_pos(
            Vector2{bg.x + bg.width - close_screen_button.get_rect().width, bg.y});
    }

    void update() override {
        // TODO: add details to completion screen (amount of turns made,
        // enemies killed, etc)
        next_level_button.update();
        close_screen_button.update();

        if (next_level_button.is_clicked()) {
            lvl->complete_event();
            lvl->change_map();
            next_level_button.reset_state();
            return;
        }

        if (close_screen_button.is_clicked()) {
            lvl->complete_event();
            close_screen_button.reset_state();
            return;
        }
    }

    void draw() override {
        DrawRectangleRec(bg, SIDE_BG_COLOR);
        DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);
        completion_label.draw();
        next_level_button.draw();
        close_screen_button.draw();
    }
};

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

// Play rock-paper-scissors against RNGesus. Returns MinigameStatus, based on
// who won.
MinigameStatus play_rps(RPS your_throw) {
    // 0 is rock, 1 is paper, 2 is scissors

    MinigameStatus result;

    // This looks cursed, I know...
    switch (rand() % 3) {
    case 0: {
        switch (your_throw) {
        case RPS::rock: {
            result = MinigameStatus::tie;
            break;
        }
        case RPS::paper: {
            result = MinigameStatus::win;
            break;
        }
        case RPS::scissors: {
            result = MinigameStatus::lose;
            break;
        }
        }
        break;
    }
    case 1: {
        switch (your_throw) {
        case RPS::rock: {
            result = MinigameStatus::lose;
            break;
        }
        case RPS::paper: {
            result = MinigameStatus::tie;
            break;
        }
        case RPS::scissors: {
            result = MinigameStatus::win;
            break;
        }
        }
        break;
    }
    case 2: {
        switch (your_throw) {
        case RPS::rock: {
            result = MinigameStatus::win;
            break;
        }
        case RPS::paper: {
            result = MinigameStatus::lose;
            break;
        }
        case RPS::scissors: {
            result = MinigameStatus::tie;
            break;
        }
        }
        break;
    }
    }

    return result;
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
    LockpickScreen(Level* level, Treasure* _treasure_obj, Player* _player_obj)
        : EventScreen(
              Rectangle{
                  ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
                  30,
                  (GetScreenWidth() + 30) / 2.0f,
                  (GetScreenHeight() - 60.0f)},
              bg_color)
        , lvl(level)
        , player_obj(_player_obj)
        , treasure_obj(_treasure_obj)
        , title_label(Label(
              "This chest is locked.\nTry to lockpick it!", GetScreenWidth() / 2, 100))
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

    void update() override {
        if (complete) {
            exit_button.update();

            if (exit_button.is_clicked()) {
                lvl->complete_event();
                // exit_button.reset_state();
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
                switch (play_rps(rps_value)) {
                case MinigameStatus::win: {
                    int value = treasure_obj->get_reward();
                    result_label.set_text(fmt::format(
                        "With no issues, you've flawlessly unlocked the chest.\n"
                        "{} coins lying inside were totally worth it!",
                        value));
                    player_obj->money_amount += value;
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
                    player_obj->money_amount += value;
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

    void draw() override {
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
};

void Level::center_camera() {
    camera.target = player_pos;
}

void Level::set_camera() {
    center_camera();
    Point tile_size = map->get_tile_size();
    camera.zoom = CAMERA_ZOOM;
    camera.offset = Vector2{
        GetScreenWidth() / 2.0f - tile_size.x / 2.0f * camera.zoom,
        GetScreenHeight() / 2.0f - tile_size.y / 2.0f * camera.zoom};
    camera.rotation = 0.0f;
}

void Level::set_player_tile(Point tile) {
    player_tile = tile;
    player_tile_label.set_text(
        fmt::format(player_tile_label.get_default_text(), player_tile.y, player_tile.x));
}

void Level::configure_hud() {
    // Maybe I shouldnt do it like that. But for now, borders on sides should
    // make visible part of screen form a perfect rectangle.
    // This would obviously cause issues on rectangular/vertical screens.
    // Idk how to solve it for now. TODO
    left_bg.x = 0.0f;
    left_bg.y = 0.0f;
    left_bg.width = (GetScreenWidth() - GetScreenHeight()) / 2.0f;
    left_bg.height = GetScreenHeight();

    right_bg.x = GetScreenWidth() - left_bg.width;
    right_bg.y = 0.0f;
    right_bg.width = left_bg.width;
    right_bg.height = left_bg.height;

    // This is nasty but will do for now
    int left_bg_txt_x = 30;
    int right_bg_txt_x = right_bg.x + left_bg_txt_x;
    int bg_txt_starting_y = 30;
    int bg_text_vert_gap = 30;

    dungeon_lvl_label =
        DynamicLabel("Dungeon Level: {}", right_bg_txt_x, bg_txt_starting_y);
    turn_num_label = DynamicLabel(
        "Current Turn: {}",
        right_bg_txt_x,
        bg_txt_starting_y + bg_text_vert_gap);
    // text of this one will be overwritten in change_turn()
    turn_label =
        DynamicLabel("", right_bg_txt_x, bg_txt_starting_y + bg_text_vert_gap * 2);

    playground_vec_start.x = left_bg.width;
    playground_vec_start.y = left_bg.y;
    playground_vec_end.x = playground_vec_start.x + left_bg.height;
    playground_vec_end.y = playground_vec_start.y + left_bg.height;

    selected_tile_label = DynamicLabel(
        "Selected Tile: {:02} x {:02}",
        right_bg_txt_x,
        GetScreenHeight() - 200);

    tile_content_label = DynamicLabel(
        "Contains: \n - {}",
        right_bg_txt_x,
        GetScreenHeight() - 200 + bg_text_vert_gap);

    player_info_label = Label("Player Info:", left_bg_txt_x, bg_txt_starting_y);
    player_currency_label =
        DynamicLabel("Coins: {}", left_bg_txt_x, bg_txt_starting_y + bg_text_vert_gap);
    player_stats_label = DynamicLabel(
        "HP: {}\n\nDamage:\nPhysical: {}\nRanged: {}\nMagical: {}\n\n"
        "Defense:\nPhysical: {}\nRanged: {}\nMagical: {}",
        left_bg_txt_x,
        bg_txt_starting_y + bg_text_vert_gap * 3);
    player_tile_label = DynamicLabel(
        "Current Tile: {:02} x {:02}",
        left_bg_txt_x,
        GetScreenHeight() - 50.0);

    back_to_menu_button.set_pos(
        Vector2{// static_cast<float>(GetScreenWidth() - (30 + 64)), 30.0f});
                // Temporary pos to dont overlap with other elements.
                // TODO: reposition things
                static_cast<float>(GetScreenWidth() - (30 + 64)),
                150.0f});
}

void Level::purge_current_event_screen() {
    if (current_event_screen) {
        delete current_event_screen.value();
        current_event_screen = std::nullopt;
    }
}

void Level::complete_event() {
    force_description_update = true;
    // show_tile_description = true;
    scheduled_events.pop_back();
    purge_current_event_screen();
}

bool Level::set_new_event() {
    if (scheduled_events.empty()) {
        // This may be inefficient, but will do for now
        current_event_tile_id = std::nullopt;
        // purge_current_event_screen();
        return false;
    }

    int current_event_cause;
    Event current_event;

    std::tie(current_event_cause, current_event) = scheduled_events.back();

    // purge_current_event_screen();

    switch (current_event) {
    case Event::exit_map: {
        // show_tile_description = false;
        current_event_screen = new CompletionScreen(this);
        break;
    }

    case Event::fight: {
        // TODO: stub
        // show_tile_description = false;

        // For now its but average roguelike battle system, which kinda falls
        // apart with pure-random stats generation (and without ability to
        // see enemy stats. And with autoattack with physical damage. Yeah).
        // TODO: remake this into a proper minigame
        if (static_cast<Creature*>(map->get_object(current_event_cause))
                ->damage(player_obj->stats.pdmg, DamageType::physical)) {
            map->delete_object(
                current_event_tile_id.value(),
                map->find_object_in_tile(
                       current_event_tile_id.value(),
                       current_event_cause)
                    .value(),
                true);

            // complete_event();
        }
        complete_event();
        break;
    }

    case Event::loot: {
        player_obj->money_amount +=
            static_cast<Treasure*>(map->get_object(current_event_cause))->get_reward();

        complete_event();
        break;
    }

    case Event::lockpick: {
        current_event_screen = new LockpickScreen(
            this,
            static_cast<Treasure*>(map->get_object(current_event_cause)),
            player_obj);
        break;
    }

    default: {
        complete_event();
        break;
    }
    }

    return true;
}

void Level::update_player_stats_hud() {
    player_stats_label.set_text(
        // TODO: write a formatter template for this
        fmt::format(
            player_stats_label.get_default_text(),
            player_obj->stats.hp,
            player_obj->stats.pdmg,
            player_obj->stats.rdmg,
            player_obj->stats.mdmg,
            player_obj->stats.pdef,
            player_obj->stats.rdef,
            player_obj->stats.mdef));
}

void Level::configure_new_map() {
    dungeon_lvl_label.set_text(
        fmt::format(dungeon_lvl_label.get_default_text(), dungeon_lvl));

    // This will fail if no player spawns are available
    set_player_tile(map->find_object_tile(map->get_player_id()).value());
    player_pos = map->tile_to_vec(player_tile);
    player_obj = static_cast<Player*>(map->get_object(map->get_player_id()));
    set_camera();
    is_player_turn = false;
    force_description_update = false;
    show_tile_description = true;
    turn_switch_timer = new Timer(0.1f);
    current_turn = 0;
    last_selected_tile = -1;
    change_turn();
}

Level::Level(SceneManager* p)
    : Scene(BG_COLOR)
    , back_to_menu_button(make_close_button()) {
    parent = p;

    input_controller.add_relationship(KEY_KP_7, MovementDirection::upleft);
    input_controller.add_relationship(KEY_KP_8, MovementDirection::up);
    input_controller.add_relationship(KEY_KP_9, MovementDirection::upright);
    input_controller.add_relationship(KEY_KP_4, MovementDirection::left);
    input_controller.add_relationship(KEY_KP_6, MovementDirection::right);
    input_controller.add_relationship(KEY_KP_1, MovementDirection::downleft);
    input_controller.add_relationship(KEY_KP_2, MovementDirection::down);
    input_controller.add_relationship(KEY_KP_3, MovementDirection::downright);

    configure_hud();
    map = generate_map(AssetLoader::loader.load_random_map(), Point{32, 32});
    dungeon_lvl = 1;
    configure_new_map();
}

Level::~Level() {
    delete map;
    delete turn_switch_timer;
    delete player_obj;
    purge_current_event_screen();
}

void Level::change_map() {
    dungeon_lvl++;
    map = generate_map(
        AssetLoader::loader.load_random_map(),
        Point{32, 32},
        dungeon_lvl,
        static_cast<MapObject*>(player_obj));
    configure_new_map();
    scheduled_events.clear();
}

bool Level::is_vec_on_playground(Vector2 vec) {
    return playground_vec_start.x < vec.x && vec.x < playground_vec_end.x &&
        playground_vec_start.y < vec.y && vec.y < playground_vec_end.y;
}

void Level::back_to_menu() {
    back_to_menu_button.reset_state();
    parent->set_current_scene(new MainMenu(parent));
}

void Level::change_turn() {
    if (is_player_turn) {
        is_player_turn = false;
        turn_label.set_text("Enemy's Turn");
    }
    else {
        is_player_turn = true;
        turn_label.set_text("Player's Turn");
        // This may backfire on multiple players
        current_turn++;
        turn_num_label.set_text(
            fmt::format(turn_num_label.get_default_text(), current_turn));
        // TODO: move this somewhere else
        update_player_stats_hud();
    }
    turn_switch_timer->start();
}

void Level::update_tile_description() {
    tile_content_label.set_text(fmt::format(
        tile_content_label.get_default_text(),
        fmt::join(map->get_tile_descriptions(last_selected_tile), "\n - ")));
}

void Level::handle_player_movement() {
    bool key_pressed = false;

    Vector2 new_pos = player_pos;
    input_controller.update();
    switch (input_controller.get_movement_direction()) {
    case MovementDirection::none:
        break;
    case MovementDirection::upleft: {
        new_pos.x -= map->get_tile_size().x;
        new_pos.y -= map->get_tile_size().y;
        key_pressed = true;
        break;
    }
    case MovementDirection::up: {
        new_pos.y -= map->get_tile_size().y;
        key_pressed = true;
        break;
    }
    case MovementDirection::upright: {
        new_pos.x += map->get_tile_size().x;
        new_pos.y -= map->get_tile_size().y;
        key_pressed = true;
        break;
    }
    case MovementDirection::left: {
        new_pos.x -= map->get_tile_size().x;
        key_pressed = true;
        break;
    }
    // No ability to stay on same tile and pass turn, for now
    case MovementDirection::right: {
        new_pos.x += map->get_tile_size().x;
        key_pressed = true;
        break;
    }
    case MovementDirection::downleft: {
        new_pos.x -= map->get_tile_size().x;
        new_pos.y += map->get_tile_size().y;
        key_pressed = true;
        break;
    }
    case MovementDirection::down: {
        new_pos.y += map->get_tile_size().y;
        key_pressed = true;
        break;
    }
    case MovementDirection::downright: {
        new_pos.x += map->get_tile_size().x;
        new_pos.y += map->get_tile_size().y;
        key_pressed = true;
        break;
    }
    default:
        break;
    }

    if (key_pressed) {
        int new_tile_id = map->vec_to_index(new_pos);
        // This may be an overkill or oversight. May need to remove it
        // if I will ever add floor tiles that cause events
        if (map->is_tile_occupied(new_tile_id)) {
            scheduled_events = map->get_player_events(new_tile_id);
            current_event_tile_id = new_tile_id;
        }

        if (map->is_tile_blocked(map->vec_to_tile(new_pos))) {
            if (set_new_event()) change_turn();
        }
        else {
            int current_tile_id = map->tile_to_index(player_tile);
            // This should always return player index in tile, thus not
            // checking the completion status
            int pt_index =
                map->find_object_in_tile(current_tile_id, map->get_player_id()).value();

            map->move_object(current_tile_id, pt_index, new_tile_id);
            player_pos = new_pos;
            set_player_tile(map->vec_to_tile(player_pos));

            center_camera();
            set_new_event();
            change_turn();
        }
    }
}

void Level::update(float dt) {
    if (turn_switch_timer->is_started()) {
        if (turn_switch_timer->tick(dt)) turn_switch_timer->stop();
        else return;
    }

    show_tile_description = true;
    if (current_event_screen) {
        show_tile_description = false;
        current_event_screen.value()->update();
    }

    else {
        if (is_player_turn) handle_player_movement();
        // TODO: enemy movement handler
        else change_turn();
    }

    if (show_tile_description) {
        // This isn't really efficient, may need some improvements. TODO
        Vector2 mouse_pos = GetMousePosition();
        if (is_vec_on_playground(mouse_pos)) {
            Vector2 real_mouse_pos = GetScreenToWorld2D(mouse_pos, camera);
            if (map->is_vec_on_map(real_mouse_pos)) {
                Point mtt = map->vec_to_tile(real_mouse_pos);
                int selected_tile = map->tile_to_index(mtt);
                // This may backfire if selected tile has been changed between checks
                if (selected_tile != last_selected_tile) {
                    selected_tile_label.set_text(fmt::format(
                        selected_tile_label.get_default_text(),
                        mtt.y,
                        mtt.x));
                    map->select_tile(mtt);

                    last_selected_tile = selected_tile;
                    update_tile_description();
                }
                else if (force_description_update) {
                    update_tile_description();
                    force_description_update = false;
                }
            }
            else {
                map->deselect_tile();
                show_tile_description = false;
            }
        }
        else {
            map->deselect_tile();
            show_tile_description = false;
        }
    }

    back_to_menu_button.update();
    if (back_to_menu_button.is_clicked()) {
        // TODO: perform some serialization there, to be able to continue
        back_to_menu();
        return;
    }

    player_currency_label.set_text(
        fmt::format(player_currency_label.get_default_text(), player_obj->money_amount));
}

void Level::draw() {
    BeginMode2D(camera);
    map->draw();
    EndMode2D();

    DrawRectangleRec(left_bg, SIDE_BG_COLOR);
    DrawLine(left_bg.width, left_bg.y, left_bg.width, left_bg.height, CORNER_COLOR);

    DrawRectangleRec(right_bg, SIDE_BG_COLOR);
    DrawLine(right_bg.x, right_bg.y, right_bg.x, right_bg.height, CORNER_COLOR);

    dungeon_lvl_label.draw();
    turn_num_label.draw();
    turn_label.draw();

    if (current_event_screen) {
        current_event_screen.value()->draw();
    }

    if (show_tile_description) {
        selected_tile_label.draw();
        tile_content_label.draw();
    }

    player_info_label.draw();
    player_tile_label.draw();
    player_currency_label.draw();
    player_stats_label.draw();

    back_to_menu_button.draw();
}
