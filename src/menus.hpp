// Fortune Crawler - a conceptual dungeon crawler with rock-paper-scissors events.
// Copyright (c) 2022 moonburnt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see https://www.gnu.org/licenses/gpl-3.0.txt

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
    Label copyright;

    void call_exit();
    void load_game();
    void new_game();
    void open_settings();

public:
    MainMenu(App* app, SceneManager* p);

    void update(float) override;
    void draw() override;
};
