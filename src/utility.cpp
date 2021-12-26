#include "raylib.h"
#include "utility.hpp"

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
