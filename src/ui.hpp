#pragma once

// UI primitives.
enum class ButtonStates {
    idle,
    hover,
    pressed
};

class Button {
    protected:
        // Button position. Hitbox will always appear at center of it
        Vector2 pos;

    private:
        // Texture2D* textures[3];
        // Sound* sounds[2]; // Hover and click
        // Same as above, but as maps, which are cpp's version of dict
        // These ones in particular don't retain order, like dicts in >3.4
        std::unordered_map<ButtonStates, Texture2D*> textures;
        std::unordered_map<int, Sound*> sounds;
        // Button's hitbox. X and Y are offsets from texture's top left
        Rectangle rect;
        // State is current button state
        ButtonStates state;
        // Button state from previous screen
        ButtonStates last_state;
        // This shows if button has been clicked
        bool clicked;

    public:
        Button(
            Texture2D* texture_default,
            Texture2D* texture_hover,
            Texture2D* texture_pressed,
            Sound* sfx_hover,
            Sound* sfx_click,
            Rectangle rectangle
        );
        ButtonStates update();
        void draw();
        void reset_state();
        void set_pos(Vector2 position);
        bool is_clicked();
};

class TextButton : public Button {
    private:
        std::string text;
        Vector2 text_pos;

    public:
        TextButton(
            Texture2D* texture_default,
            Texture2D* texture_hover,
            Texture2D* texture_pressed,
            Sound* sfx_hover,
            Sound* sfx_click,
            Rectangle rectangle,
            std::string msg,
            Vector2 msg_pos
        );

        void draw();
        void set_pos(Vector2 position);
};
