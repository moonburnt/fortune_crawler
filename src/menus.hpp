#pragma once

#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"
#include "raylib.h"

class App;

class TitleScreen : public Scene {
private:
    App* app;
    SceneManager* parent;
    Timer timer;
    Label greeter;

public:
    TitleScreen(App* app, SceneManager* p);

    void update(float dt) override;
    void draw() override;
};

class MainMenu : public Scene {
private:
    App* app;
    SceneManager* parent;
    VerticalContainer buttons;
    const Texture2D* logo;
    Vector2 logo_pos;

    void call_exit();
    void load_game();
    void new_game();
    void open_settings();

public:
    MainMenu(App* app, SceneManager* p);

    void update(float) override;
    void draw() override;
};
