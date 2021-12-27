#include <unordered_map>
#include <string>
#include "raylib.h"
#include "scenes.hpp"
#include "utility.hpp"
#include "loader.hpp"
extern AssetsLoader loader;
#include "ui.hpp"

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
        current_scene->update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        current_scene->draw();

        if (show_fps) DrawText(
            TextFormat("FPS: %02i", GetFPS()),
            1200, 4, 20, BLACK
        );
        EndDrawing();
    }
}

// Default instance of scene manager. Use extern to share it
SceneManager sc_mgr;

// Actual implementations of Scene
class MainMenu: public Scene {
    private:
        SceneManager* parent;
        TextButton* exit_button;

        void call_exit() {
            exit_button->reset_state();
            sc_mgr.active = false;
        }

    public:
        MainMenu(SceneManager* p) {
            parent = p;
            exit_button = new TextButton(
                &loader.sprites["button_default"],
                &loader.sprites["button_hover"],
                &loader.sprites["button_pressed"],
                &loader.sounds["button_hover"],
                &loader.sounds["button_clicked"],
                Rectangle{0, 0, 256, 64},
                "Exit",
                Vector2{116, 20}
            );
            exit_button->set_pos(Vector2{600, 300});
        }

        void update() {
            exit_button->update();

            if (exit_button->is_clicked()) call_exit();
        }

        void draw() {
            // DrawText("Main Menu", 590, 340, 20, BLACK);
            exit_button->draw();
        }

        // MainMenu's destructor, will get called on delete of MainMenu instance
        ~MainMenu() {
            // It's important to delete local vars initialized with new there,
            // to deal with memory leaks
            delete exit_button;
        }
};


// This one gets described before SceneManager, coz its used in its constructor
class TitleScreen: public Scene {
    private:
        SceneManager* parent;
        Timer* timer;

    public:
        TitleScreen(SceneManager* p) {
            parent = p;
            timer = new Timer(2.0f);
            timer->start();
        }

        void update() {
            // TODO: add timer that will automatically switch to main menu after
            // some time has been passed since title's update
            if (timer->tick()) {
                parent->set_current_scene(new MainMenu(parent));
            }
        }

        void draw() {
            DrawText("Hello, World", 590, 340, 20, BLACK);
        }
};

// Default SceneManager's constructor is all way down, coz TitleScreen is in its
// body. But don't worry - even if instantiation is declared above, nothing bad
// will happen - this one will get triggered correctly
SceneManager::SceneManager() {
    // Setting current_scene to null, to avoid segfault below.
    current_scene = nullptr;
    // "this" is cpp's version of "self"
    set_current_scene(new TitleScreen(this));
    show_fps = true;

    active = true;
}
