#pragma once

#include <raylib.h>
#include <string>

static constexpr uint32_t DEFAULT_TEXT_SIZE = 20u;
static constexpr Color DEFAULT_TEXT_COLOR = BLACK;

void handle_assert(const char* file, int line, const char* fun, const char* expr);

#define ASSERT(expr)                                                                     \
    while (!(expr)) {                                                                    \
        handle_assert(__FILE__, __LINE__, __FUNCTION__, #expr);                          \
        break;                                                                           \
    }

class Timer {
private:
    bool started;
    bool completed;
    float duration;
    float time_left;

public:
    Timer(float length);
    // Start/restart timer.
    void start();
    // Update timer for specific value. Returns completion status.
    bool tick(float dt);
    // Stop timer.
    void stop();
    // Get timer's startup status.
    bool is_started();
};

// Get position that will be perfect text's center.
Vector2 center_text(const std::string& text, Vector2 center);

// Get position that will be perfect text's horizontal center.
int center_text_h(const std::string& text, int center);

// Center rectangle in place, overriding its existing position.
void center_rect_ip(Rectangle* rect, Vector2 center);

// Get random bool value
bool randbool();
