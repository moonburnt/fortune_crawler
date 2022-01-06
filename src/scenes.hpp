#pragma once

// Scene is an abstract class that can't be instantiated directly, but can be
// subclassed. This is how we do ABC interfaces in c++
// In this case its located in header, coz SceneManager needs it
class Scene {
public:
    // Thats how we define abstract functions
    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // This is a scene's destructor. Which can be overriden, but not necessary.
    virtual ~Scene() = default;
};

class SceneManager {
    // We are using pointer to Scene, to make it work with Scene's children

private:
    Scene* current_scene;
    bool show_fps;

public:
    SceneManager();
    void set_current_scene(Scene* scene);
    void run_update_loop();
    bool active;
    bool is_active();
    // This one exists solely coz otherwise constructor will segfault the
    // application, coz TitleScreen require graphics to be initialized, and
    // default instance of SceneManager will get initialized prior that.
    void set_default_scene();

    // Default instance of scene manager
    static SceneManager sc_mgr;
};
