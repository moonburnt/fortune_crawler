#pragma once

class Level : public Scene {
    private:
        SceneManager* parent;

        // void exit_to_menu();

    public:
        Level(SceneManager* p);

        void update();
        void draw();

        // ~Level();
};
