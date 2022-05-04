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
