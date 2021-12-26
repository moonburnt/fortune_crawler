#include "raylib.h"
#include "scenes.hpp"

int main() {
    InitWindow(1280, 720, "Luck Crawler");
    SetTargetFPS(60);

    SceneManager sc_mgr;
    sc_mgr.run_update_loop();

    CloseWindow();

    return 0;
}
