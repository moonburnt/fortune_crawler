#include "common.hpp"

#include "app.hpp"

#include <engine/ui.hpp>

GuiBuilder::GuiBuilder(App* app)
    : app(app) {}

Button* GuiBuilder::make_close_button() {
    return new Button(
        app->assets.sprites["cross_default"],
        app->assets.sprites["cross_hover"],
        app->assets.sprites["cross_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64});
}

Button* GuiBuilder::make_text_button(std::string txt) {
    return new Button(
        txt,
        app->assets.sprites["button_default"],
        app->assets.sprites["button_hover"],
        app->assets.sprites["button_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64});
}

Checkbox* GuiBuilder::make_checkbox(bool default_state) {
    return new Checkbox(
        app->assets.sprites["toggle_on_default"],
        app->assets.sprites["toggle_on_hover"],
        app->assets.sprites["toggle_on_pressed"],
        app->assets.sprites["toggle_off_default"],
        app->assets.sprites["toggle_off_hover"],
        app->assets.sprites["toggle_off_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
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
