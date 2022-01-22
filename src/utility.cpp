#include "utility.hpp"

#include <cstdlib>
#include <fmt/format.h>
#include <iostream>
#include <raylib.h>
// To find() things in InputHandler
#include <algorithm>

void handle_assert(const char* file, int line, const char* fun, const char* expr) {
    std::cout
        << fmt::format("Assertion failed at {} in {}:{} '{}'\n", fun, file, line, expr);
    abort();
}

Timer::Timer(float length) {
    started = false;
    completed = false;
    duration = length;
    time_left = length;
}

// Some of this kinda doubles init stage, but started can also be used to restart,
// So... I guess, its fine?
void Timer::start() {
    completed = false;
    time_left = duration;
    started = true;
}

bool Timer::tick(float dt) {
    if (!completed) {
        // It would be more efficient to convert GetFrameTime's double to float,
        // than to perform all operations on doubles. Such precision would be
        // excessive for the most cases anyway.
        time_left -= dt;
        if (time_left <= 0.0f) {
            completed = true;
        }
    }

    return completed;
}

void Timer::stop() {
    started = false;
    completed = false;
}

bool Timer::is_started() {
    return started;
}

InputController::InputController() {
}

void InputController::add_relationship(int key, int action) {
    key_binds[key] = action;
}

void InputController::update() {
    for (auto& kv : key_binds) {
        if (IsKeyDown(kv.first)) {
            if (!std::count(buttons_held.begin(), buttons_held.end(), kv.first)) {
                buttons_held.push_back(kv.first);
            }
        }
        else {
            auto it = std::find(buttons_held.begin(), buttons_held.end(), kv.first);
            if (it != buttons_held.end()) buttons_held.erase(it);
        }
    }
}

void InputController::reset_active() {
    buttons_held.clear();
}

int InputController::get_action() {
    if (buttons_held.empty()) return 0;
    else return key_binds[buttons_held.back()];
}

Vector2 center_text(const std::string& text, Vector2 center) {
    Vector2 msg_size = MeasureTextEx(
        GetFontDefault(),
        text.c_str(),
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_SIZE / 10.0f);

    return Vector2{(center.x - msg_size.x / 2), (center.y - msg_size.y / 2)};
}

int center_text_h(const std::string& text, int center) {
    int msg_width = MeasureText(text.c_str(), DEFAULT_TEXT_SIZE);

    return center - msg_width / 2;
}

void center_rect_ip(Rectangle* rect, Vector2 center) {
    rect->x = center.x - rect->x / 2;
    rect->y = center.y - rect->y / 2;
}

bool randbool() {
    return (std::rand() % 2);
}
