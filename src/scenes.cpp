#include "scenes.hpp"
#include "level.hpp"
#include "loader.hpp"
#include "mapgen.hpp"
#include "settings.hpp"
#include "ui.hpp"
#include "utility.hpp"

#include "raylib.h"

#include <string>
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

    TextButton* save_button;
    Button* exit_button;

    std::string show_grid_title;
    Vector2 show_grid_pos;
    Checkbox* grid_cb;

    std::string show_fps_title;
    Vector2 show_fps_pos;
    Checkbox* fps_cb;

    void exit_to_menu() {
        exit_button->reset_state();
        parent->set_current_scene(new MainMenu(parent));
    }

    void save_settings() {
        save_button->reset_state();
        if (!settings_changed) return;

        SettingsManager::manager.set_show_fps(fps_cb->get_toggle());
        SettingsManager::manager.set_show_grid(grid_cb->get_toggle());
        SettingsManager::manager.save_settings();

        grid_cb->reset_state();
        fps_cb->reset_state();

        settings_changed = false;
    }

public:
    SettingsScreen(SceneManager* p) {
        parent = p;
        title_msg = "Settings";
        int center_x = GetScreenWidth() / 2;
        title_pos.x = center_text_h(title_msg, center_x);
        title_pos.y = 30;

        unsaved_changes_msg = "Settings changed. Press save to apply!";
        unsaved_changes_pos.x = center_text_h(unsaved_changes_msg, center_x);
        unsaved_changes_pos.y = 60;

        settings_changed = false;

        save_button = new TextButton(
            &AssetLoader::loader.sprites["button_default"],
            &AssetLoader::loader.sprites["button_hover"],
            &AssetLoader::loader.sprites["button_pressed"],
            &AssetLoader::loader.sounds["button_hover"],
            &AssetLoader::loader.sounds["button_clicked"],
            Rectangle{0, 0, 256, 64},
            "Save");

        save_button->set_pos(Vector2{
            center_x - save_button->get_rect().width / 2,
            GetScreenHeight() - 100.0f});

        exit_button = new Button(
            &AssetLoader::loader.sprites["cross_default"],
            &AssetLoader::loader.sprites["cross_hover"],
            &AssetLoader::loader.sprites["cross_pressed"],
            &AssetLoader::loader.sounds["button_hover"],
            &AssetLoader::loader.sounds["button_clicked"],
            Rectangle{0, 0, 64, 64});

        exit_button->set_pos(
            Vector2{static_cast<float>(GetScreenWidth() - (30 + 64)), 30.0f});

        show_grid_title = "Show Grid:";
        show_fps_title = "Show FPS:";

        show_grid_pos = Vector2{30.0f, 100.0f};
        show_fps_pos = Vector2{30.0f, 150.0f};

        grid_cb = new Checkbox(
            &AssetLoader::loader.sprites["toggle_on_default"],
            &AssetLoader::loader.sprites["toggle_on_hover"],
            &AssetLoader::loader.sprites["toggle_on_pressed"],
            &AssetLoader::loader.sprites["toggle_off_default"],
            &AssetLoader::loader.sprites["toggle_off_hover"],
            &AssetLoader::loader.sprites["toggle_off_pressed"],
            &AssetLoader::loader.sounds["button_hover"],
            &AssetLoader::loader.sounds["button_clicked"],
            Rectangle{0, 0, 32, 32},
            SettingsManager::manager.get_show_grid());

        fps_cb = new Checkbox(
            &AssetLoader::loader.sprites["toggle_on_default"],
            &AssetLoader::loader.sprites["toggle_on_hover"],
            &AssetLoader::loader.sprites["toggle_on_pressed"],
            &AssetLoader::loader.sprites["toggle_off_default"],
            &AssetLoader::loader.sprites["toggle_off_hover"],
            &AssetLoader::loader.sprites["toggle_off_pressed"],
            &AssetLoader::loader.sounds["button_hover"],
            &AssetLoader::loader.sounds["button_clicked"],
            Rectangle{0, 0, 32, 32},
            SettingsManager::manager.get_show_fps());

        grid_cb->set_pos(Vector2{200.0f, 100.0f});
        fps_cb->set_pos(Vector2{200.0f, 150.0f});
    }

    ~SettingsScreen() {
        delete save_button;
        delete exit_button;
        delete grid_cb;
        delete fps_cb;
    }

    void update(float) override {
        save_button->update();
        exit_button->update();
        grid_cb->update();
        fps_cb->update();

        if (exit_button->is_clicked()) {
            exit_to_menu();
            return;
        }

        if (save_button->is_clicked()) {
            save_settings();
            return;
        }

        if (grid_cb->is_clicked() || fps_cb->is_clicked()) {
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

        save_button->draw();
        exit_button->draw();
        grid_cb->draw();
        fps_cb->draw();

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
    exit_button->reset_state();
    parent->active = false;
}

void MainMenu::start_game() {
    start_button->reset_state();
    parent->set_current_scene(new Level(parent));
}

void MainMenu::open_settings() {
    settings_button->reset_state();
    parent->set_current_scene(new SettingsScreen(parent));
}

MainMenu::MainMenu(SceneManager* p) {
    parent = p;

    start_button = new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Start");

    settings_button = new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Settings");

    exit_button = new TextButton(
        &AssetLoader::loader.sprites["button_default"],
        &AssetLoader::loader.sprites["button_hover"],
        &AssetLoader::loader.sprites["button_pressed"],
        &AssetLoader::loader.sounds["button_hover"],
        &AssetLoader::loader.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Exit");

    float center_x = GetScreenWidth() / 2.0f;
    float center_y = GetScreenHeight() / 2.0f;

    start_button->set_pos(
        Vector2{center_x - start_button->get_rect().width / 2, center_y - 100});
    settings_button->set_pos(
        Vector2{center_x - exit_button->get_rect().width / 2, center_y});
    exit_button->set_pos(
        Vector2{center_x - exit_button->get_rect().width / 2, center_y + 100});
}

void MainMenu::update(float) {
    start_button->update();
    settings_button->update();
    exit_button->update();

    if (start_button->is_clicked()) {
        start_game();
        return;
    }

    if (settings_button->is_clicked()) {
        open_settings();
        return;
    }

    if (exit_button->is_clicked()) {
        call_exit();
        return;
    }
}

void MainMenu::draw() {
    start_button->draw();
    settings_button->draw();
    exit_button->draw();
}

MainMenu::~MainMenu() {
    // It's important to delete local vars initialized with new there,
    // to deal with memory leaks
    delete exit_button;
    delete start_button;
    delete settings_button;
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
