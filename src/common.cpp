#include "common.hpp"
#include "raylib.h"
#include "shared.hpp"

Button* make_close_button() {
    return new Button(
        shared::assets.sprites["cross_default"],
        shared::assets.sprites["cross_hover"],
        shared::assets.sprites["cross_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64});
}

Button* make_text_button(std::string txt) {
    return new Button(
        txt,
        shared::assets.sprites["button_default"],
        shared::assets.sprites["button_hover"],
        shared::assets.sprites["button_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64});
}

Checkbox* make_checkbox(bool default_state) {
    return new Checkbox(
        shared::assets.sprites["toggle_on_default"],
        shared::assets.sprites["toggle_on_hover"],
        shared::assets.sprites["toggle_on_pressed"],
        shared::assets.sprites["toggle_off_default"],
        shared::assets.sprites["toggle_off_hover"],
        shared::assets.sprites["toggle_off_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
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
