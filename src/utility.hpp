#pragma once

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
