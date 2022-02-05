#include "scenes.hpp"
#include "level.hpp"
#include "loader.hpp"
#include "mapgen.hpp"
#include "settings.hpp"
#include "ui.hpp"
#include "utility.hpp"

#include "raylib.h"

#include <string>
#include <tuple>
#include <vector>

Scene::Scene(Color _bg_color)
    : bg_color(_bg_color) {
}

Scene::Scene()
    : bg_color(Color{245, 245, 245, 255}) {
}

SceneManager SceneManager::sc_mgr;

// There are two ways to work with scenes: to add scene manually each time,
// initializing it from zero and clearing up other scenes from memory.
// Or to keep all scenes initialized in some storage. For now, we are going for
// the first one, but this behavior may change in future.
void SceneManager::set_current_scene(Scene* scene) {
    // This will segfault if current_scene is not set, but it shouldn't happen.
    delete current_scene;
    current_scene = scene;
}

bool SceneManager::is_active() {
    return !WindowShouldClose() && active;
}

void SceneManager::run_update_loop() {
    while (is_active()) {
        float dt = static_cast<float>(GetFrameTime());
        current_scene->update(dt);

        BeginDrawing();
        ClearBackground(current_scene->bg_color);
        current_scene->draw();

        // Maybe I should store it somewhere?
        if (SettingsManager::manager.get_show_fps()) {
            DrawText(TextFormat("FPS: %02i", GetFPS()), 1200, 4, 20, BLACK);
        }
        EndDrawing();
    }
}

// Actual implementations of Scene
// Settings menu. I may move it to settings.cpp later
class SettingsScreen : public Scene {
private:
    SceneManager* parent;

    std::string title_msg;
    Vector2 title_pos;

    std::string unsaved_changes_msg;
    Vector2 unsaved_changes_pos;
    bool settings_changed;

    TextButton save_button;
    Button exit_button;

    std::string show_grid_title;
    Vector2 show_grid_pos;
    Checkbox grid_cb;

    std::string show_fps_title;
    Vector2 show_fps_pos;
    Checkbox fps_cb;

    void exit_to_menu() {
        exit_button.reset_state();
        parent->set_current_scene(new MainMenu(parent));
    }

    void save_settings() {
        save_button.reset_state();
        if (!settings_changed) return;

        SettingsManager::manager.set_show_fps(fps_cb.get_toggle());
        SettingsManager::manager.set_show_grid(grid_cb.get_toggle());
        SettingsManager::manager.save_settings();

        grid_cb.reset_state();
        fps_cb.reset_state();

        settings_changed = false;
    }

public:
    SettingsScreen(SceneManager* p)
        : save_button(make_text_button("Save"))
        , exit_button(make_close_button())
        , grid_cb(make_checkbox(SettingsManager::manager.get_show_grid()))
        , fps_cb(make_checkbox(SettingsManager::manager.get_show_fps())) {
        parent = p;
        title_msg = "Settings";
        int center_x = GetScreenWidth() / 2;
        title_pos.x = center_text_h(title_msg, center_x);
        title_pos.y = 30;

        unsaved_changes_msg = "Settings changed. Press save to apply!";
        unsaved_changes_pos.x = center_text_h(unsaved_changes_msg, center_x);
        unsaved_changes_pos.y = 60;

        settings_changed = false;

        save_button.set_pos(Vector2{
            center_x - save_button.get_rect().width / 2,
            GetScreenHeight() - 100.0f});

        exit_button.set_pos(
            Vector2{static_cast<float>(GetScreenWidth() - (30 + 64)), 30.0f});

        show_grid_title = "Show Grid:";
        show_fps_title = "Show FPS:";

        show_grid_pos = Vector2{30.0f, 100.0f};
        show_fps_pos = Vector2{30.0f, 150.0f};

        grid_cb.set_pos(Vector2{200.0f, 100.0f});
        fps_cb.set_pos(Vector2{200.0f, 150.0f});
    }

    ~SettingsScreen() {
    }

    void update(float) override {
        save_button.update();
        exit_button.update();
        grid_cb.update();
        fps_cb.update();

        if (exit_button.is_clicked()) {
            exit_to_menu();
            return;
        }

        if (save_button.is_clicked()) {
            save_settings();
            return;
        }

        if (grid_cb.is_clicked() || fps_cb.is_clicked()) {
            settings_changed = true;
        }
        else settings_changed = false;
    }

    void draw() override {
        DrawText(
            title_msg.c_str(),
            title_pos.x,
            title_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);

        DrawText(
            show_grid_title.c_str(),
            show_grid_pos.x,
            show_grid_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);

        DrawText(
            show_fps_title.c_str(),
            show_fps_pos.x,
            show_fps_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);

        save_button.draw();
        exit_button.draw();
        grid_cb.draw();
        fps_cb.draw();

        if (settings_changed) {
            DrawText(
                unsaved_changes_msg.c_str(),
                unsaved_changes_pos.x,
                unsaved_changes_pos.y,
                DEFAULT_TEXT_SIZE,
                DEFAULT_TEXT_COLOR);
        }
    }
};

// Main menu logic
void MainMenu::call_exit() {
    parent->active = false;
}

void MainMenu::new_game() {
    parent->set_current_scene(Level::new_game(parent));
}

void MainMenu::load_game() {
    parent->set_current_scene(
        Level::load_save(parent, SettingsManager::manager.savefile.value()));
}

void MainMenu::open_settings() {
    parent->set_current_scene(new SettingsScreen(parent));
}

MainMenu::MainMenu(SceneManager* p) {
    parent = p;

    // TODO: rework this trash
    buttons.add_button(new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "New Game"));
    buttons.add_button(new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Settings"));
    buttons.add_button(new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Exit"));

    float center_x = GetScreenWidth() / 2.0f;
    float center_y = GetScreenHeight() / 2.0f;

    if (SettingsManager::manager.savefile) {
        buttons.add_button(new TextButton(
            &AssetLoader::loader.sprites["button_default"],
            &AssetLoader::loader.sprites["button_hover"],
            &AssetLoader::loader.sprites["button_pressed"],
            &AssetLoader::loader.sounds["button_hover"],
            &AssetLoader::loader.sounds["button_clicked"],
            Rectangle{0, 0, 256, 64},
            "Continue"));

        buttons[MM_CONTINUE]->set_pos(Vector2{
            center_x - buttons[MM_CONTINUE]->get_rect().width / 2,
            center_y - 200});
    }

    buttons[MM_NEWGAME]->set_pos(
        Vector2{center_x - buttons[MM_NEWGAME]->get_rect().width / 2, center_y - 100});

    buttons[MM_SETTINGS]->set_pos(
        Vector2{center_x - buttons[MM_SETTINGS]->get_rect().width / 2, center_y});

    buttons[MM_EXIT]->set_pos(
        Vector2{center_x - buttons[MM_EXIT]->get_rect().width / 2, center_y + 100});
}

void MainMenu::update(float) {
    // TODO: add keyboard controller, toggle manual update mode on and off,
    // depending on what happend the last - some valid key press or mouse movement
    buttons.update();

    if (SettingsManager::manager.savefile && buttons[MM_CONTINUE]->is_clicked()) {
        load_game();
        return;
    }
    if (buttons[MM_NEWGAME]->is_clicked()) {
        new_game();
        return;
    }

    if (buttons[MM_SETTINGS]->is_clicked()) {
        open_settings();
        return;
    }

    if (buttons[MM_EXIT]->is_clicked()) {
        call_exit();
        return;
    }
}

void MainMenu::draw() {
    buttons.draw();
}

MainMenu::~MainMenu() {
    // DONT DELETE POINTER TO PARENT, SINCE ITS CARRIED AROUND ALL SCENES
    // delete parent;
}

// This one gets described before SceneManager, coz its used in its constructor
class TitleScreen : public Scene {
private:
    SceneManager* parent;
    Timer* timer;
    std::string greeter_msg;
    Vector2 greeter_pos;

public:
    TitleScreen(SceneManager* p) {
        parent = p;

        greeter_msg = "This game has been made with raylib\0";
        greeter_pos = center_text(
            greeter_msg,
            Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});

        timer = new Timer(2.0f);
        timer->start();
    }

    void update(float dt) override {
        // TODO: add timer that will automatically switch to main menu after
        // some time has been passed since title's update
        if (timer->tick(dt)) {
            parent->set_current_scene(new MainMenu(parent));
        }
    }

    void draw() override {
        DrawText(
            greeter_msg.c_str(),
            greeter_pos.x,
            greeter_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);
    }
};

// Default SceneManager's constructor is all way down, coz TitleScreen is in its
// body. But don't worry - even if instantiation is declared above, nothing bad
// will happen - this one will get triggered correctly
SceneManager::SceneManager() {
    // Setting current_scene to null, to avoid segfault below.
    current_scene = nullptr;

    active = true;
}

// See explanation of why this is a dedicated function in header
void SceneManager::set_default_scene() {
    set_current_scene(new TitleScreen(this));
}
