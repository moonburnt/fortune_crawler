#pragma once

#include <raylib.h>

#include <string>

#define DEFAULT_TEXT_SIZE 20
#define DEFAULT_TEXT_COLOR BLACK

class CounterBase {
    public:
        virtual void start() = 0;
        virtual bool tick() = 0;
        virtual ~CounterBase() = default;
};

class Timer : public CounterBase {
    private:
        bool started;
        bool completed;
        float duration;
        float time_left;

    public:
        Timer(float length);
        void start();
        bool tick();
};

// Get position that will be perfect text's center.
Vector2 center_text(std::string* text, Vector2 center);

// Get position that will be perfect text's horizontal center.
int center_text_h(std::string* text, int center);

// Center rectangle in place, overriding its existing position.
void center_rect_ip(Rectangle* rect, Vector2 center);
