#include "ui.hpp"
#include "utility.hpp"

#include <raylib.h>

#include <string>
#include <unordered_map>

// It's not necessary to use "this" in these, but it may be good for readability
void Button::reset_state() {
    state = ButtonStates::idle;
    last_state = ButtonStates::idle;
    clicked = false;
}

// Constructors dont need to specify return state
Button::Button(
    Texture2D* texture_default,
    Texture2D* texture_hover,
    Texture2D* texture_pressed,
    Sound* sfx_hover,
    Sound* sfx_click,
    Rectangle rectangle
) {
    textures[ButtonStates::idle] = texture_default;
    textures[ButtonStates::hover] = texture_hover;
    textures[ButtonStates::pressed] = texture_pressed;
    sounds[0] = sfx_hover;
    sounds[1] = sfx_click;
    rect = rectangle;
    pos = Vector2{0, 0};

    reset_state();
}

enum ButtonStates Button::update() {
    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        if (last_state == ButtonStates::pressed) {
            if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                clicked = true;
                PlaySound(*sounds[1]);
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            state = ButtonStates::pressed;

        else {
            if (last_state != ButtonStates::hover) PlaySound(*sounds[0]);
            state = ButtonStates::hover;
        }
    }
    else state = ButtonStates::idle;

    last_state = state;

    return state;
}

void Button::draw() {
    DrawTexture(*textures[state], pos.x, pos.y, WHITE);
}

void Button::set_pos(Vector2 position)  {
    // Hitbox should always retain initial position diff

    int x_diff = rect.x - pos.x;
    int y_diff = rect.y - pos.y;

    pos.x = position.x;
    pos.y = position.y;

    rect.x = position.x+x_diff;
    rect.y = position.y+y_diff;
}

Vector2 Button::get_pos() {
    return pos;
}

Rectangle Button::get_rect() {
    return rect;
}

// Getter for private clicked var. This allows to make it readable, but prevent
// overwriting from outside
bool Button::is_clicked() {
    return clicked;
}

// This is how we call parent's constructor from child constructor, with passing
// required arguments to it. Parent's constructor will be solved after child.
TextButton::TextButton(
    Texture2D* texture_default,
    Texture2D* texture_hover,
    Texture2D* texture_pressed,
    Sound* sfx_hover,
    Sound* sfx_click,
    Rectangle rectangle,
    std::string msg,
    Vector2 msg_pos
) : Button(
        texture_default,
        texture_hover,
        texture_pressed,
        sfx_hover,
        sfx_click,
        rectangle
    ) {
    text = msg;
    text_pos = msg_pos;
}

TextButton::TextButton(
    Texture2D* texture_default,
    Texture2D* texture_hover,
    Texture2D* texture_pressed,
    Sound* sfx_hover,
    Sound* sfx_click,
    Rectangle rectangle,
    std::string msg
) : Button(
        texture_default,
        texture_hover,
        texture_pressed,
        sfx_hover,
        sfx_click,
        rectangle
    ) {
    text = msg;
    // I'm not sure if this should be based on center of rect or on center of
    // texture. For now it's done like that, may change in future. TODO
    text_pos = center_text(
        text,
        Vector2{texture_default->width/2.0f, texture_default->height/2.0f}
    );
}

void TextButton::draw() {
    // And this is how we call parent's functions from child functions.
    // Keep in mind that, to shadow parent's method, we must explicitely specify
    // it in children's description.
    Button::draw();
    DrawText(
        text.c_str(),
        text_pos.x, text_pos.y,
        DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR
    );
}

void TextButton::set_pos(Vector2 position) {
    int text_x_diff = text_pos.x - pos.x;
    int text_y_diff = text_pos.y - pos.y;

    text_pos.x = position.x+text_x_diff;
    text_pos.y = position.y+text_y_diff;
    Button::set_pos(position);
}
