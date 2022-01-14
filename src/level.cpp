#include "level.hpp"
#include "loader.hpp"
#include "utility.hpp"

#include <raylib.h>

// TODO: make this configurable from settings
// Sane values would be 1.0 -> 3.0, everything bigger would make things render
// too close. Everything less makes no sense.
static constexpr float CAMERA_ZOOM = 2.0f;

static constexpr Color SIDE_BG_COLOR{203, 219, 252, 255};
static constexpr Color CORNER_COLOR{34, 32, 52, 255};
static constexpr Color BG_COLOR{63, 63, 116, 255};

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

    playground_vec_start.x = left_bg.width;
    playground_vec_start.y = left_bg.y;
    playground_vec_end.x = playground_vec_start.x + left_bg.height;
    playground_vec_end.y = playground_vec_start.y + left_bg.height;

    event_screen_bg.x = left_bg.width + 30;
    event_screen_bg.y = left_bg.y + 30;
    event_screen_bg.width = (GetScreenWidth() - left_bg.width * 2) - 60;
    event_screen_bg.height = left_bg.height - 60;

    selected_tile_text = "Selected Tile: ";
    selected_tile_pos.x =
        center_text_h(selected_tile_text + "00 x 00", right_bg.x + right_bg.width / 2);
    selected_tile_pos.y = GetScreenHeight() - 200;

    tile_content_vert_gap = 30;

    tile_content_title = "Contains: ";
    tile_content_pos.x = selected_tile_pos.x;
    tile_content_pos.y = selected_tile_pos.y + tile_content_vert_gap;

    player_info_title = "Player Info:";
    player_info_pos.x = center_text_h(selected_tile_text, left_bg.x + left_bg.width / 2);
    player_info_pos.y = 30;

    player_tile_text = "Current Tile: ";
    player_tile_text_pos.x =
        center_text_h(player_tile_text + "00 x 00", left_bg.x + left_bg.width / 2);
    player_tile_text_pos.y = GetScreenHeight() - 50;

    completion_msg = "Level Completed!";
    completion_msg_pos = center_text(
        completion_msg,
        Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});

    next_level_button = new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Go Deeper!");
    next_level_button->set_pos(Vector2{
        GetScreenWidth() / 2.0f - next_level_button->get_rect().width / 2,
        GetScreenHeight() / 2.0f + 200});

    close_event_screen_button = new Button(
        &AssetLoader::loader.sprites["cross_default"],
        &AssetLoader::loader.sprites["cross_hover"],
        &AssetLoader::loader.sprites["cross_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64});

    close_event_screen_button->set_pos(Vector2{
        event_screen_bg.x + event_screen_bg.width -
            close_event_screen_button->get_rect().width,
        event_screen_bg.y});

    back_to_menu_button = new Button(
        &AssetLoader::loader.sprites["cross_default"],
        &AssetLoader::loader.sprites["cross_hover"],
        &AssetLoader::loader.sprites["cross_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64});

    back_to_menu_button->set_pos(
        Vector2{// static_cast<float>(GetScreenWidth() - (30 + 64)), 30.0f});
                // Temporary pos to dont overlap with other elements.
                // TODO: reposition things
                static_cast<float>(GetScreenWidth() - (30 + 64)),
                150.0f});
}

void Level::configure_new_map() {
    dungeon_lvl++;
    dungeon_lvl_title = TextFormat("Dungeon Level: %i", dungeon_lvl);
    dungeon_lvl_title_pos = Vector2{
        static_cast<float>(
            center_text_h(dungeon_lvl_title, right_bg.x + right_bg.width / 2)),
        30};

    player_tile = map->get_player_tile();
    player_pos = map->tile_to_vec(player_tile);
    set_camera();
    is_player_turn = false;
    turn_switch_timer = new Timer(0.1f);
    current_turn = 0;
    current_event = Event::nothing;
    last_selected_tile = -1;
    change_turn();
}

Level::Level(SceneManager* p)
    : Scene(BG_COLOR) {
    parent = p;

    configure_hud();
    map = generate_map(AssetLoader::loader.load_random_map(), Point{32, 32});
    dungeon_lvl = 0;
    configure_new_map();
}

Level::~Level() {
    delete next_level_button;
    delete close_event_screen_button;
    delete map;
    delete turn_switch_timer;
    delete back_to_menu_button;
}

void Level::change_map() {
    // TODO: randomly pick map from list of available in maps/
    int player_id = map->get_player_id();
    MapObject* player_obj = map->get_object(player_id);
    map = generate_map(AssetLoader::loader.load_random_map(), Point{32, 32}, player_obj);
    configure_new_map();
}

bool Level::is_vec_on_playground(Vector2 vec) {
    return playground_vec_start.x < vec.x && vec.x < playground_vec_end.x &&
        playground_vec_start.y < vec.y && vec.y < playground_vec_end.y;
}

void Level::back_to_menu() {
    back_to_menu_button->reset_state();
    parent->set_current_scene(new MainMenu(parent));
}

void Level::change_turn() {
    if (is_player_turn) {
        is_player_turn = false;
        turn_title = "Enemy's Turn";
        turn_title_pos = {
            static_cast<float>(
                center_text_h(turn_title, right_bg.x + right_bg.width / 2)),
            70};
    }
    else {
        is_player_turn = true;
        turn_title = "Player's Turn";
        turn_title_pos = {
            static_cast<float>(
                center_text_h(turn_title, right_bg.x + right_bg.width / 2)),
            70};
        // This may backfire on multiple players
        current_turn++;
        turn_num_title = TextFormat("Current Turn: %i", current_turn);
        turn_num_title_pos = {
            static_cast<float>(
                center_text_h(turn_num_title, right_bg.x + right_bg.width / 2)),
            50};
    }
    turn_switch_timer->start();
}

void Level::update(float dt) {
    if (turn_switch_timer->is_started()) {
        if (turn_switch_timer->tick(dt)) turn_switch_timer->stop();
        else return;
    }

    switch (current_event) {
    case Event::nothing: {
        if (is_player_turn) {
            bool move_player = false;
            Vector2 new_pos = player_pos;
            if (IsKeyDown(KEY_UP)) {
                new_pos.y -= map->get_tile_size().y;
                if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
                else new_pos = player_pos;
            }
            else if (IsKeyDown(KEY_DOWN)) {
                new_pos.y += map->get_tile_size().y;
                if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
                else new_pos = player_pos;
            }
            else if (IsKeyDown(KEY_LEFT)) {
                new_pos.x -= map->get_tile_size().x;
                if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
                else new_pos = player_pos;
            }
            else if (IsKeyDown(KEY_RIGHT)) {
                new_pos.x += map->get_tile_size().x;
                if (!map->is_tile_blocked(map->vec_to_tile(new_pos))) move_player = true;
                else new_pos = player_pos;
            }

            if (move_player) {
                int current_tile_id = map->tile_to_index(player_tile);

                // This should always return player index in tile, thus not
                // checking the completion status
                int pt_index;
                map->object_in_tile(current_tile_id, map->get_player_id(), &pt_index);

                int new_tile_id = map->vec_to_index(new_pos);

                // This may be an overkill or oversight. May need to remove it
                // if I will ever add floor tiles that cause events
                if (map->is_tile_occupied(new_tile_id))
                    current_event = map->get_tile_event(new_tile_id, true);

                map->move_object(current_tile_id, pt_index, new_tile_id);
                player_pos = new_pos;
                player_tile = map->vec_to_tile(player_pos);

                center_camera();
                change_turn();
            }
        }
        // TODO: stub
        else change_turn();
        break;
    }

    case Event::exit_map: {
        // TODO: add details to completion screen (amount of turns made, enemies
        // killed, etc)
        next_level_button->update();
        close_event_screen_button->update();

        if (next_level_button->is_clicked()) {
            change_map();
            next_level_button->reset_state();
            return;
        }

        if (close_event_screen_button->is_clicked()) {
            current_event = Event::nothing;
            close_event_screen_button->reset_state();
            return;
        }
        break;
    }

    case Event::fight: {
        // TODO: stub

        // Assuming that player has moved above enemy and deleting object that
        // should reffer to enemy. Hopefully. This is a really nasty placeholder
        // that may break due to circuimstances. But it will do for now. TODO.
        int current_tile_id = map->tile_to_index(player_tile);
        map->delete_object(
            current_tile_id,
            map->get_tile_elements_amount(current_tile_id) - 2,
            true);

        current_event = Event::nothing;
        break;
    }

    default:
        break;
    }

    back_to_menu_button->update();
    if (back_to_menu_button->is_clicked()) {
        // TODO: perform some serialization there, to be able to continue
        back_to_menu();
        return;
    }
}

void Level::draw() {
    BeginMode2D(camera);
    map->draw();
    EndMode2D();

    DrawRectangleRec(left_bg, SIDE_BG_COLOR);
    DrawLine(left_bg.width, left_bg.y, left_bg.width, left_bg.height, CORNER_COLOR);

    DrawRectangleRec(right_bg, SIDE_BG_COLOR);
    DrawLine(right_bg.x, right_bg.y, right_bg.x, right_bg.height, CORNER_COLOR);

    DrawText(
        dungeon_lvl_title.c_str(),
        dungeon_lvl_title_pos.x,
        dungeon_lvl_title_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);
    DrawText(
        turn_num_title.c_str(),
        turn_num_title_pos.x,
        turn_num_title_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);
    DrawText(
        turn_title.c_str(),
        turn_title_pos.x,
        turn_title_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);

    switch (current_event) {
    case Event::nothing: {
        // I may want to move this above everything else in draw cycle
        // This isn't really efficient, may need some improvements. TODO
        Vector2 mouse_pos = GetMousePosition();
        if (is_vec_on_playground(mouse_pos)) {
            Vector2 real_mouse_pos = GetScreenToWorld2D(mouse_pos, camera);
            if (map->is_vec_on_map(real_mouse_pos)) {
                Point mtt = map->vec_to_tile(real_mouse_pos);
                DrawText(
                    TextFormat("%s%02i x %02i", selected_tile_text.c_str(), mtt.y, mtt.x),
                    selected_tile_pos.x,
                    selected_tile_pos.y,
                    DEFAULT_TEXT_SIZE,
                    DEFAULT_TEXT_COLOR);
                map->select_tile(mtt);

                int selected_tile = map->tile_to_index(mtt);
                // This may backfire if selected tile has been changed between checks
                if (selected_tile != last_selected_tile) {
                    last_selected_tile = selected_tile;
                    std::string descriptions = tile_content_title;
                    for (auto desc : map->get_tile_descriptions(last_selected_tile)) {
                        descriptions += "\n - " + desc;
                    }
                    last_selected_descriptions = descriptions;
                }

                DrawText(
                    last_selected_descriptions.c_str(),
                    tile_content_pos.x,
                    tile_content_pos.y,
                    DEFAULT_TEXT_SIZE,
                    DEFAULT_TEXT_COLOR);
            }
            else map->deselect_tile();
        }
        else map->deselect_tile();
        break;
    }

    case Event::exit_map: {
        DrawRectangleRec(event_screen_bg, SIDE_BG_COLOR);
        DrawRectangleLinesEx(event_screen_bg, 1.0f, CORNER_COLOR);
        DrawText(
            completion_msg.c_str(),
            completion_msg_pos.x,
            completion_msg_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);
        next_level_button->draw();
        close_event_screen_button->draw();
        break;
    }

    default:
        break;
    }

    DrawText(
        player_info_title.c_str(),
        player_info_pos.x,
        player_info_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);

    DrawText(
        TextFormat(
            "%s%02i x %02i",
            player_tile_text.c_str(),
            player_tile.y,
            player_tile.x),
        player_tile_text_pos.x,
        player_tile_text_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);

    back_to_menu_button->draw();
}
