#include "raylib.h"
#include "scenes.hpp"

// Actual implementation of Scene
class TitleScreen: public Scene {
    public:
        void update() {
            // TODO: add timer that will automatically switch to main menu after
            // some time has been passed since title's update
        }

        void draw() {
            DrawText("Hello, World", 590, 340, 20, BLACK);
        }
};

// Thats how we define functions and variables if class is described in header
// In this particular case we override SceneManager's default constructor behavior
// (which is kinda similar to __init__() in python), to specify default values
// for all instances.
SceneManager::SceneManager() {
    // Setting current_scene to null, to avoid segfault below.
    current_scene = nullptr;
    set_current_scene(new TitleScreen());
    show_fps = true;
}

// There are two ways to work with scenes: to add scene manually each time,
// initializing it from zero and clearing up other scenes from memory.
// Or to keep all scenes initialized in some storage. For now, we are going for
// the first one, but this behavior may change in future. TODO
void SceneManager::set_current_scene(Scene* scene) {
    // This will segfault if current_scene is not set, but it shouldn't happen.
    delete current_scene;
    current_scene = scene;
}

void SceneManager::run_update_loop() {
    while (!WindowShouldClose()) {
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
