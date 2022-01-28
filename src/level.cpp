#include "level.hpp"
#include "entity.hpp"
#include "event_screens.hpp"
#include "loader.hpp"
#include "utility.hpp"

#include <algorithm>
#include <optional>
#include <raylib.h>
// For basic formatting
#include <fmt/core.h>
// For fmt::join
#include <fmt/format.h>

// TODO: make this configurable from settings
// Sane values would be 1.0 -> 3.0, everything bigger would make things render
// too close. Everything less makes no sense.
static constexpr float CAMERA_ZOOM = 2.0f;

EventScreen::EventScreen(Rectangle _bg, Color _bg_color)
    : bg(_bg)
    , bg_color(_bg_color) {
}

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
        fmt::format("Current Tile: {:02} x {:02}", player_tile.y, player_tile.x));
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

    dungeon_lvl_label = Label("", right_bg_txt_x, bg_txt_starting_y);
    turn_num_label = Label("", right_bg_txt_x, bg_txt_starting_y + bg_text_vert_gap);
    // text of this one will be overwritten in change_turn()
    turn_label = Label("", right_bg_txt_x, bg_txt_starting_y + bg_text_vert_gap * 2);

    playground_vec_start.x = left_bg.width;
    playground_vec_start.y = left_bg.y;
    playground_vec_end.x = playground_vec_start.x + left_bg.height;
    playground_vec_end.y = playground_vec_start.y + left_bg.height;

    selected_tile_label = Label("", right_bg_txt_x, GetScreenHeight() - 200);

    tile_content_label =
        Label("", right_bg_txt_x, GetScreenHeight() - 200 + bg_text_vert_gap);

    player_info_label = Label("Player Info:", left_bg_txt_x, bg_txt_starting_y);
    player_currency_label =
        Label("", left_bg_txt_x, bg_txt_starting_y + bg_text_vert_gap);
    player_stats_label =
        Label("", left_bg_txt_x, bg_txt_starting_y + bg_text_vert_gap * 3);
    player_tile_label = Label("", left_bg_txt_x, GetScreenHeight() - 50.0);
}

void Level::purge_current_event_screen() {
    if (current_event_screen != nullptr) {
        delete current_event_screen;
        current_event_screen = nullptr;
    }
}

void Level::complete_event() {
    force_description_update = true;
    // show_tile_description = true;
    scheduled_events.pop_back();
    purge_current_event_screen();
}

void Level::give_player_money(int amount) {
    player_obj->money_amount += amount;
    money_collected += amount;
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
        current_event_screen =
            // + 1 coz turns counter is increased after event's completion.
            new CompletionScreen(this, current_turn + 1, money_collected, enemies_killed);
        break;
    }

    case Event::fight: {
        current_event_screen = new BattleScreen(
            this,
            player_obj,
            static_cast<Enemy*>(map->get_object(current_event_cause)),
            current_event_tile_id.value(),
            current_event_cause);
        break;
    }

    case Event::loot: {
        give_player_money(
            static_cast<Treasure*>(map->get_object(current_event_cause))->get_reward());

        complete_event();
        break;
    }

    case Event::lockpick: {
        current_event_screen = new LockpickScreen(
            this,
            static_cast<Treasure*>(map->get_object(current_event_cause)));
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
            "HP: {} / {}\n\nDamage:\nPhysical: {}\nRanged: {}\nMagical: {}\n\n"
            "Defense:\nPhysical: {}\nRanged: {}\nMagical: {}",
            player_obj->current_hp,
            player_obj->max_hp,
            player_obj->offensive_stats[OffensiveStats::pdmg],
            player_obj->offensive_stats[OffensiveStats::rdmg],
            player_obj->offensive_stats[OffensiveStats::mdmg],
            player_obj->defensive_stats[DefensiveStats::pdef],
            player_obj->defensive_stats[DefensiveStats::rdef],
            player_obj->defensive_stats[DefensiveStats::mdef]));
}

void Level::configure_new_map() {
    dungeon_lvl_label.set_text(fmt::format("Dungeon Level: {}", dungeon_lvl));

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
    money_collected = 0;
    enemies_killed = 0;
    last_selected_tile = -1;
    change_turn();
}

void Level::show_gameover() {
    // TODO: add more stats, like global kills amount or money collected
    static_cast<NotificationScreen*>(game_over_screen)
        ->set_description(fmt::format("Died on level {}", dungeon_lvl));
    game_over = true;
}

Level::Level(SceneManager* p)
    : Scene(BG_COLOR)
    , pause_menu(new PauseScreen(this))
    , game_over(false)
    , game_over_screen(new NotificationScreen("Game Over", "", "Back to Menu")) {
    parent = p;

    input_controller.add_relationship(KEY_KP_7, MovementDirection::upleft);
    input_controller.add_relationship(KEY_KP_8, MovementDirection::up);
    input_controller.add_relationship(KEY_KP_9, MovementDirection::upright);
    input_controller.add_relationship(KEY_KP_4, MovementDirection::left);
    input_controller.add_relationship(KEY_KP_6, MovementDirection::right);
    input_controller.add_relationship(KEY_KP_1, MovementDirection::downleft);
    input_controller.add_relationship(KEY_KP_2, MovementDirection::down);
    input_controller.add_relationship(KEY_KP_3, MovementDirection::downright);

    // TODO: remove duplicates, make buttons above configurable from settings.
    input_controller.add_relationship(KEY_U, MovementDirection::upleft);
    input_controller.add_relationship(KEY_I, MovementDirection::up);
    input_controller.add_relationship(KEY_O, MovementDirection::upright);
    input_controller.add_relationship(KEY_J, MovementDirection::left);
    input_controller.add_relationship(KEY_L, MovementDirection::right);
    input_controller.add_relationship(KEY_M, MovementDirection::downleft);
    input_controller.add_relationship(KEY_COMMA, MovementDirection::down);
    input_controller.add_relationship(KEY_PERIOD, MovementDirection::downright);

    input_controller.add_relationship(KEY_UP, MovementDirection::up);
    input_controller.add_relationship(KEY_LEFT, MovementDirection::left);
    input_controller.add_relationship(KEY_RIGHT, MovementDirection::right);
    input_controller.add_relationship(KEY_DOWN, MovementDirection::down);

    configure_hud();
    map = generate_map(AssetLoader::loader.load_random_map(), Point{32, 32});
    dungeon_lvl = 1;
    is_paused = false;
    configure_new_map();
}

Level::~Level() {
    delete map;
    delete turn_switch_timer;
    delete player_obj;
    delete pause_menu;
    delete game_over_screen;

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
}

bool Level::is_vec_on_playground(Vector2 vec) {
    return playground_vec_start.x < vec.x && vec.x < playground_vec_end.x &&
        playground_vec_start.y < vec.y && vec.y < playground_vec_end.y;
}

void Level::exit_to_menu() {
    parent->set_current_scene(new MainMenu(parent));
}

void Level::kill_enemy(int tile_id, int entity_id) {
    map->delete_object(
        tile_id,
        map->find_object_in_tile(tile_id, entity_id).value(),
        true);
    enemies_killed++;
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
        turn_num_label.set_text(fmt::format("Current Turn: {}", current_turn));
        // TODO: move this somewhere else
        update_player_stats_hud();
    }
    turn_switch_timer->start();
}

void Level::update_tile_description() {
    // Safety thing to fix possible attempt to get description of failsafe tile.
    if (last_selected_tile == -1) return;

    tile_content_label.set_text(fmt::format(
        "Contains: \n - {}",
        fmt::join(map->get_tile_descriptions(last_selected_tile), "\n - ")));
}

void Level::handle_player_movement() {
    bool key_pressed = false;

    Vector2 new_pos = player_pos;
    input_controller.update();
    switch (input_controller.get_action()) {
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

    if (key_pressed && map->is_vec_on_map(new_pos)) {
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
    if (game_over) {
        game_over_screen->update();
        if (static_cast<NotificationScreen*>(game_over_screen)->complete) {
            exit_to_menu();
            return;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (is_paused) is_paused = false;
        else is_paused = true;
    }
    if (is_paused) {
        pause_menu->update();
        return;
    }

    if (turn_switch_timer->is_started()) {
        if (turn_switch_timer->tick(dt)) turn_switch_timer->stop();
        else return;
    }

    show_tile_description = true;
    if (current_event_screen != nullptr) {
        show_tile_description = false;
        current_event_screen->update();
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
                    selected_tile_label.set_text(
                        fmt::format("Selected Tile: {:02} x {:02}", mtt.y, mtt.x));
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
                // This should fix issue with tile highlighting not working if
                // cursor left map's borders while some tile was selected, but
                // then returned back to that very tile.
                last_selected_tile = -1;
                show_tile_description = false;
            }
        }
        else {
            map->deselect_tile();
            show_tile_description = false;
        }
    }

    player_currency_label.set_text(fmt::format("Coins: {}", player_obj->money_amount));
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
    player_info_label.draw();
    player_tile_label.draw();
    player_currency_label.draw();
    player_stats_label.draw();

    if (game_over) {
        game_over_screen->draw();
        return;
    }

    if (is_paused) {
        pause_menu->draw();
        return;
    }

    if (current_event_screen != nullptr) {
        current_event_screen->draw();
    }

    if (show_tile_description) {
        selected_tile_label.draw();
        tile_content_label.draw();
    }
}
