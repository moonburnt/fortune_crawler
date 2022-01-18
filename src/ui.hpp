#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>

// UI primitives.
enum class ButtonStates
{
    idle,
    hover,
    pressed
};

class Button {
protected:
    // Button position. Hitbox will always appear at center of it
    Vector2 pos;
    // State is current button state
    ButtonStates state;

private:
    std::unordered_map<ButtonStates, Texture2D*> textures;
    std::unordered_map<int, Sound*> sounds;
    // Button's hitbox. X and Y are offsets from texture's top left
    Rectangle rect;
    // Button state from previous screen
    ButtonStates last_state;
    // This shows if button has been clicked
    bool clicked;

public:
    Button(
        Texture2D* texture_default,
        Texture2D* texture_hover,
        Texture2D* texture_pressed,
        Sound* sfx_hover,
        Sound* sfx_click,
        Rectangle rectangle);
    ButtonStates update();
    void draw();
    void reset_state();
    void set_pos(Vector2 position);
    bool is_clicked();
    // This getter is necessary to use pos from outside without being able
    // to overwrite its value directly (coz its meant to be done together
    // with changing position of other button's elements)
    Vector2 get_pos();
    // Same there
    Rectangle get_rect();
};

class TextButton : public Button {
private:
    std::string text;
    Vector2 text_pos;

public:
    TextButton(
        Texture2D* texture_default,
        Texture2D* texture_hover,
        Texture2D* texture_pressed,
        Sound* sfx_hover,
        Sound* sfx_click,
        Rectangle rectangle,
        std::string msg,
        Vector2 msg_pos);
    // This is how we define alternative constructor. It will be used
    // automatically, if amount/order of items received by constructor match.
    // This may be useful, since cpp doesn't have kwargs.
    TextButton(
        Texture2D* texture_default,
        Texture2D* texture_hover,
        Texture2D* texture_pressed,
        Sound* sfx_hover,
        Sound* sfx_click,
        Rectangle rectangle,
        std::string msg);

    void draw();
    void set_pos(Vector2 position);
};

class Checkbox : public Button {
private:
    // Storage for textures of disabled checkbox
    std::unordered_map<ButtonStates, Texture2D*> textures_off;
    // State of checkbox (on or off)
    bool toggle_state;
    // Has checkbox'es state been changed with toggle() or not.
    bool state_switched;

public:
    Checkbox(
        Texture2D* texture_on_default,
        Texture2D* texture_on_hover,
        Texture2D* texture_on_pressed,
        Texture2D* texture_off_default,
        Texture2D* texture_off_hover,
        Texture2D* texture_off_pressed,
        Sound* sfx_hover,
        Sound* sfx_click,
        Rectangle rectangle);

    Checkbox(
        Texture2D* texture_on_default,
        Texture2D* texture_on_hover,
        Texture2D* texture_on_pressed,
        Texture2D* texture_off_default,
        Texture2D* texture_off_hover,
        Texture2D* texture_off_pressed,
        Sound* sfx_hover,
        Sound* sfx_click,
        Rectangle rectangle,
        bool default_state);

    // Returns toggle state of checkbox
    bool get_toggle();
    // Toggle checkbox on or off. Doesn't affect "state_switched".
    void toggle(bool toggle_state);
    // Toggle checkbox state back and forth. Toggles "state_switched", until
    // reset_state() is used.
    void toggle();

    void draw();
    void update();

    // Returns state_switched.
    bool is_clicked();
    // Resets state_switched.
    void reset_state();
};

// Basic label. You can move it around, but cant alter its text after creation.
class Label {
protected:
    std::string text;

public:
    Label(std::string txt, Vector2 position);
    Label(std::string txt, int x, int y);
    Label();

    Vector2 pos;

    // Center message around its position
    void center();

    void draw();
};

// Dynamic label. It has an additional storage for text set during initialization,
// which allows to reuse it
class DynamicLabel : public Label {
private:
    std::string default_text;

public:
    DynamicLabel(std::string txt, Vector2 position);
    DynamicLabel(std::string txt, int x, int y);
    DynamicLabel();

    void set_text(std::string txt, bool override_default);
    void set_text(std::string txt);
    std::string get_default_text();
};

// Fabriques for some common button types
Button make_close_button();
TextButton make_text_button(std::string txt);
Checkbox make_checkbox(bool default_state);
