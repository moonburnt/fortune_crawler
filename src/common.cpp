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

#include "common.hpp"

#include "app.hpp"

#include <engine/ui.hpp>

GuiBuilder::GuiBuilder(App* app)
    : app(app) {}

Button* GuiBuilder::make_close_button() {
    return new Button(
        {
            {ButtonStates::idle, app->assets.sprites["cross_default"]},
            {ButtonStates::hover, app->assets.sprites["cross_hover"]},
            {ButtonStates::pressed, app->assets.sprites["cross_pressed"]},
            {ButtonStates::clicked, app->assets.sprites["cross_default"]},
            {ButtonStates::disabled, app->assets.sprites["cross_default"]}
        },
        {
            {ButtonStates::hover, app->assets.sounds["button_hover"]},
            {ButtonStates::clicked, app->assets.sounds["button_clicked"]}
        },
        Rectangle{0, 0, 64, 64});
}

Button* GuiBuilder::make_text_button(std::string txt) {
    return new Button(
        txt,
        {
            {ButtonStates::idle, app->assets.sprites["button_default"]},
            {ButtonStates::hover, app->assets.sprites["button_hover"]},
            {ButtonStates::pressed, app->assets.sprites["button_pressed"]},
            {ButtonStates::clicked, app->assets.sprites["button_default"]},
            {ButtonStates::disabled, app->assets.sprites["button_default"]}
        },
        {
            {ButtonStates::hover, app->assets.sounds["button_hover"]},
            {ButtonStates::clicked, app->assets.sounds["button_clicked"]}
        },
        Rectangle{0, 0, 256, 64});
}

Checkbox* GuiBuilder::make_checkbox(bool default_state) {
    return new Checkbox(
        {
            {ButtonStates::idle, app->assets.sprites["toggle_on_default"]},
            {ButtonStates::hover, app->assets.sprites["toggle_on_hover"]},
            {ButtonStates::pressed, app->assets.sprites["toggle_on_pressed"]},
            {ButtonStates::clicked, app->assets.sprites["toggle_on_default"]},
            {ButtonStates::disabled, app->assets.sprites["toggle_on_default"]}
        },
        {
            {ButtonStates::idle, app->assets.sprites["toggle_off_default"]},
            {ButtonStates::hover, app->assets.sprites["toggle_off_hover"]},
            {ButtonStates::pressed, app->assets.sprites["toggle_off_pressed"]},
            {ButtonStates::clicked, app->assets.sprites["toggle_off_default"]},
            {ButtonStates::disabled, app->assets.sprites["toggle_off_default"]}
        },
        {
            {ButtonStates::hover, app->assets.sounds["button_hover"]},
            {ButtonStates::clicked, app->assets.sounds["button_clicked"]}
        },
        Rectangle{0, 0, 32, 32},
        default_state);
}

int get_window_width() {
    if (IsWindowFullscreen()) {
        return GetMonitorWidth(GetCurrentMonitor());
    }
    else {
        return GetScreenWidth();
    }
}

int get_window_height() {
    if (IsWindowFullscreen()) {
        return GetMonitorHeight(GetCurrentMonitor());
    }
    else {
        return GetScreenHeight();
    }
}
