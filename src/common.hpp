#include "engine/ui.hpp"

// Constructors of commonly used buttons.
// Two things to keep in mind:
// - These require shared::assets to be initialized and have default textures
// and sounds loaded in.
// - These use "new" under the hood, thus require manual deletion.
Button* make_close_button();
Button* make_text_button(std::string txt);
Checkbox* make_checkbox(bool default_state);
