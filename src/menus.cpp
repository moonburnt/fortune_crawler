#include "menus.hpp"
#include "common.hpp"
#include "engine/ui.hpp"
#include "level.hpp"
#include "shared.hpp"
#include "spdlog/spdlog.h"
#include <array>
#include <raylib.h>
#include <functional>

// Title Screen
TitleScreen::TitleScreen(SceneManager* p)
    : parent(p)
    , timer(new Timer(2.0f))
    , greeter(
        "This game has been made with raylib",
        {get_window_width() / 2.0f, get_window_height() / 2.0f}) {

    greeter.center();
    timer->start();
}

void TitleScreen::update(float dt) {
    if (timer->tick(dt)) {
        parent->set_current_scene(new MainMenu(parent));
    }
}

void TitleScreen::draw() {
    greeter.draw();
}

// Settings Screen
class SettingsScreen : public Scene {
private:
    SceneManager* parent;

    // It may be done without this thing, but will do for now
    toml::table current_settings;

    Label title;
    Label unsaved_changes_msg;
    bool settings_changed;

    Button* save_button;
    Button* exit_button;

    Label show_grid_title;
    Checkbox* grid_cb;

    Label show_fps_title;
    Checkbox* fps_cb;

    Label fullscreen_title;
    Checkbox* fullscreen_cb;

    void exit_to_menu() {
        spdlog::info("Switching to main menu");
        exit_button->reset_state();
        parent->set_current_scene(new MainMenu(parent));
    }

    void save_settings() {
        save_button->reset_state();
        if (!settings_changed) return;

        current_settings.insert_or_assign("show_grid", grid_cb->get_toggle());
        current_settings.insert_or_assign("show_fps", fps_cb->get_toggle());
        current_settings.insert_or_assign("fullscreen", fullscreen_cb->get_toggle());

        // fps_cb->reset_state();
        // grid_cb->reset_state();
        // fullscreen_cb->reset_state();

        spdlog::info("Attempting to apply new settings");
        settings_changed = false;

        if (current_settings["show_fps"].value_exact<bool>().value()) {
            if (shared::window.sc_mgr.nodes.count("fps_counter") == 0) {
                shared::window.sc_mgr.nodes["fps_counter"] = new FrameCounter();
            }
        }
        else {
            shared::window.sc_mgr.nodes.erase("fps_counter");
        }

        if (current_settings["fullscreen"].value_exact<bool>().value()) {
            if (!IsWindowFullscreen()) {
                const int current_screen = GetCurrentMonitor();
                const int screen_width = GetMonitorWidth(current_screen);
                const int screen_height = GetMonitorHeight(current_screen);

                ToggleFullscreen();
                SetWindowSize(screen_width, screen_height);

                // current_settings.insert_or_assign(
                //     "fullscreen_resolution", toml::array{screen_width, screen_height});
            };
        }
        else {
            if (IsWindowFullscreen()) {
                SetWindowSize(
                    current_settings["resolution"][0].value_or(1280),
                    current_settings["resolution"][1].value_or(720));
                ToggleFullscreen();
            };
        }

        shared::config.settings = current_settings;
        shared::config.save();

        spdlog::info("Resetting settings screen");
        parent->set_current_scene(new SettingsScreen(parent));
    }

public:
    SettingsScreen(SceneManager* p)
        : parent(p)
        , current_settings(shared::config.settings) // this should get copied
        , title("Settings", {get_window_width() / 2.0f, 30.0f})
        , unsaved_changes_msg(
              "Settings changed. Press save to apply!",
              {get_window_width() / 2.0f, 60.0f})
        , settings_changed(false)
        , save_button(make_text_button("Save"))
        , exit_button(make_close_button())
        , show_grid_title("Show Grid:", {30.0f, 100.0f})
        , grid_cb(make_checkbox(
            shared::config.settings["show_grid"].value_exact<bool>().value()))
        , show_fps_title("Show FPS:", {30.0f, 150.0f})
        , fps_cb(make_checkbox(
            shared::config.settings["show_fps"].value_exact<bool>().value()))
        , fullscreen_title("Fullscreen:", {30.0f, 200.0f})
        , fullscreen_cb(make_checkbox(
            shared::config.settings["fullscreen"].value_exact<bool>().value())) {

        title.center();
        unsaved_changes_msg.center();

        save_button->set_pos(
            {get_window_width() / 2.0f - save_button->get_rect().width / 2.0f,
             get_window_height() - 100.0f});
        exit_button->set_pos({static_cast<float>(get_window_width() - (30 + 64)), 30.0f});

        const float cb_x = 200.0f;
        grid_cb->set_pos({cb_x, 100.0f});
        fps_cb->set_pos({cb_x, 150.0f});
        fullscreen_cb->set_pos({cb_x, 200.0f});
    }

    ~SettingsScreen() {
        delete grid_cb;
        delete fps_cb;
        delete fullscreen_cb;
        delete exit_button;
    }

    void update(float) override {
        save_button->update();
        exit_button->update();
        grid_cb->update();
        fps_cb->update();
        fullscreen_cb->update();

        if (exit_button->is_clicked()) {
            exit_to_menu();
            return;
        }

        if (save_button->is_clicked()) {
            save_settings();
            return;
        }

        if (grid_cb->is_clicked() || fps_cb->is_clicked() || fullscreen_cb->is_clicked()) {
            settings_changed = true;
        }
        else {
            settings_changed = false;
        }
    }

    void draw() override {
        title.draw();

        show_grid_title.draw();
        show_fps_title.draw();
        fullscreen_title.draw();

        save_button->draw();
        exit_button->draw();
        grid_cb->draw();
        fps_cb->draw();
        fullscreen_cb->draw();

        if (settings_changed) {
            unsaved_changes_msg.draw();
        }
    }
};

// Main menu
void MainMenu::call_exit() {
    parent->active = false;
}

void MainMenu::load_game() {
    spdlog::info("Loading savefile");
    parent->set_current_scene(Level::load_save(parent, shared::save.savefile.value()));
}

void MainMenu::new_game() {
    spdlog::info("Switching to level");
    parent->set_current_scene(Level::new_game(parent));
}

void MainMenu::open_settings() {
    spdlog::info("Switching to settings");
    parent->set_current_scene(new SettingsScreen(parent));
}

MainMenu::MainMenu(SceneManager* p)
    : parent(p)
    , buttons(32.0f) {

    buttons.set_pos({get_window_width() / 2.0f, get_window_height() / 2.0f});

    if (shared::save.savefile) {
        Button* cont_button = make_text_button("Continue");
        cont_button->set_callback(std::bind(&MainMenu::load_game, this));
        buttons.add_button(cont_button);
    }

    Button* ng_button = make_text_button("New Game");
    ng_button->set_callback(std::bind(&MainMenu::new_game, this));

    Button* settings_button = make_text_button("Settings");
    settings_button->set_callback(std::bind(&MainMenu::open_settings, this));

    Button* exit_button = make_text_button("Exit");
    exit_button->set_callback(std::bind(&MainMenu::call_exit, this));

    buttons.add_button(ng_button);
    buttons.add_button(settings_button);
    buttons.add_button(exit_button);

    buttons.center();
}

void MainMenu::update(float) {
    // TODO: add keyboard controller, toggle manual update mode on and off,
    // depending on what happend the last - some valid key press or mouse movement
    buttons.update();
}

void MainMenu::draw() {
    buttons.draw();
}

// MainMenu::~MainMenu() {
//     // DO NOT DELETE POINTER TO PARENT, SINCE ITS CARRIED AROUND ALL SCENES
//     // delete parent;
// }
