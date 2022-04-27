#pragma once

#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"
#include "raylib.h"

class TitleScreen : public Scene {
private:
    SceneManager* parent;
    Timer* timer;
    Label greeter;

public:
    TitleScreen(SceneManager* p);

    void update(float dt) override;
    void draw() override;
};

class MainMenu : public Scene {
private:
    SceneManager* parent;
    VerticalContainer buttons;
    const Texture2D* logo;
    Vector2 logo_pos;

    void call_exit();
    void load_game();
    void new_game();
    void open_settings();

public:
    MainMenu(SceneManager* p);

    void update(float) override;
    void draw() override;
};
