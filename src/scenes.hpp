#pragma once

// Scene is an abstract class that can't be instantiated directly, but can be
// subclassed. This is how we do ABC interfaces in c++
// In this case its located in header, coz SceneManager needs it
class Scene {
    public:
        // Thats how we define abstract functions
        virtual void update() = 0;
        virtual void draw() = 0;
        // This is a scene's destructor. Which can be overriden, but not necessary.
        virtual ~Scene() = default;
};

class SceneManager {
    // We are using pointer to scene, to allow for inheritance
    private:
        Scene* current_scene;

    public:
        SceneManager();
        void set_current_scene(Scene* scene);
        void run_update_loop();
};
