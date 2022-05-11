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

#include <raylib.h>
#include <string>

class App;
class Button;
class Checkbox;

// Constructors of commonly used buttons.
// Two things to keep in mind:
// - These require shared::assets to be initialized and have default textures
// and sounds loaded in.
// - These use "new" under the hood, thus require manual deletion.
class GuiBuilder {
public:
    GuiBuilder(App* app);

    Button* make_close_button();
    Button* make_text_button(std::string txt);
    Checkbox* make_checkbox(bool default_state);

private:
    App* app;
};

// Get current window's size, regardless if its fullscreen or not
int get_window_width();
int get_window_height();
