#include "utility.hpp"

#include <raylib.h>

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

bool Timer::tick() {
    if (!completed) {
        // It would be more efficient to convert GetFrameTime's double to float,
        // than to perform all operations on doubles. Such precision would be
        // excessive for the most cases anyway.
        time_left -= static_cast<float>(GetFrameTime());
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

Vector2 center_text(const std::string& text, Vector2 center) {
    Vector2 msg_size = MeasureTextEx(
        GetFontDefault(),
        text.c_str(),
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_SIZE / 10.0f
    );

    return Vector2{
        (center.x-msg_size.x/2),
        (center.y-msg_size.y/2)
    };
}

int center_text_h(const std::string& text, int center) {
    int msg_width = MeasureText(
        text.c_str(),
        DEFAULT_TEXT_SIZE
    );

    return center-msg_width/2;
}

void center_rect_ip(Rectangle* rect, Vector2 center) {
    rect->x = center.x - rect->x/2;
    rect->y = center.y - rect->y/2;
}
