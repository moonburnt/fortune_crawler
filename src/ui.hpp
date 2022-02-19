#pragma once

#include <raylib.h>

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// UI primitives.
enum class ButtonStates {
    idle,
    hover,
    pressed,
    clicked
};

class ButtonBase {
public:
    virtual ButtonStates update() = 0;
    virtual void draw() = 0;
    virtual void set_manual_update_mode(bool mode) = 0;
    virtual void reset_state() = 0;
    virtual void set_state(ButtonStates state) = 0;
    virtual void set_pos(Vector2 position) = 0;
    virtual ButtonStates get_state() = 0;
    virtual Rectangle get_rect() = 0;
    virtual bool is_clicked() = 0;

    virtual ~ButtonBase() = default;
};

// Basic label.
class Label {
protected:
    std::string text;

    // Pos set by set_pos and on init
    Vector2 pos;
    // Pos to draw on, that may be different if center() has been used.
    Vector2 real_pos;

public:
    Label(std::string txt, Vector2 position);
    Label(std::string txt, int x, int y);
    Label();

    // Center message around its position
    void center();

    void set_pos(Vector2 pos, bool center);
    void set_pos(Vector2 pos);
    Vector2 get_pos();

    // Set label's text. May need to re-center message after that.
    void set_text(std::string txt);

    void draw();
};

class Button : public ButtonBase {
protected:
    // Button position. Hitbox will always appear at center of it
    Vector2 pos;
    // State is current button state
    ButtonStates state;
    // State update mode. See set_manual_update_mode();
    bool manual_update_mode;
    std::unordered_map<ButtonStates, Texture2D*> textures;

private:
    std::unordered_map<int, Sound*> sounds;
    // Button's hitbox. X and Y are offsets from texture's top left
    Rectangle rect;
    // Button state from previous screen
    ButtonStates last_state;

public:
    Button(
        Texture2D* texture_default,
        Texture2D* texture_hover,
        Texture2D* texture_pressed,
        Sound* sfx_hover,
        Sound* sfx_click,
        Rectangle rectangle);
    ButtonStates update() override;
    void draw() override;

    // Set manual update mode. If true - won't update state with update(),
    // instead will only respond to manual set_state() calls.
    void set_manual_update_mode(bool mode) override;
    // Override button state. Will also toggle on manual update mode.
    void set_state(ButtonStates state) override;
    ButtonStates get_state() override;
    // Reset button state and last_state to ButtonStates::idle.
    void reset_state() override;
    void set_pos(Vector2 position) override;
    bool is_clicked() override;
    // This getter is necessary to use pos from outside without being able
    // to overwrite its value directly (coz its meant to be done together
    // with changing position of other button's elements)
    Vector2 get_pos();
    // Same there
    Rectangle get_rect() override;
};

class TextButton : public Button {
private:
    Label text;

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

    void set_text(std::string txt);

    void draw() override;
    void set_pos(Vector2 position) override;
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

    void draw() override;
    ButtonStates update() override;

    // Returns state_switched.
    bool is_clicked() override;
    // Resets state_switched.
    void reset_state() override;
};

class ButtonStorage {
private:
    // Storage that holds buttons. Since there is no way to remove these, its
    // a vector. May rework into map if there will be need to remove elements.
    std::vector<ButtonBase*> storage;
    // Number of selected button in storage
    size_t selected_button;
    bool manual_update_mode;

public:
    ButtonStorage();
    ~ButtonStorage();
    // Add button or its derivatives to storage.
    void add_button(ButtonBase* button);
    void set_manual_update_mode(bool mode);
    // Select button with specific id in storage. Any of these selectors will
    // automatically toggle on set_manual_update_mode().
    void select_button(size_t button);
    void select_next(bool cycle);
    void select_previous(bool cycle);
    // Update all buttons assigned to storage. If manual_update_mode has been set
    // to true - only updates selected button.
    void update();
    // Set state of selected button. Returns true if there is such button, else false
    bool set_selected_button_state(ButtonStates state);
    // Draw all buttons in storage
    void draw();

    std::optional<std::tuple<int, ButtonStates>> get_selected_button_state();

    // Overload to [], that provides access to storage elements.
    // For now, doesn't have safety checks - requiesting out of bounds value will
    // probably lead to segfault.
    ButtonBase* operator[](int i);
};

// Fabriques for some common button types
Button make_close_button();
TextButton make_text_button(std::string txt);
Checkbox make_checkbox(bool default_state);
