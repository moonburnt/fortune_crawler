#pragma once

#include <raylib.h>
#include <vector>

class Sprite {
private:
    Texture2D* spritesheet;
    Rectangle rect;

public:
    Sprite(Texture2D* spritesheet, Rectangle rect);
    // TODO: consider using DrawTexturePro under the hood, to allow for angle
    // and colored masks.
    void draw(Vector2 pos);
};

// Turn rectangle on specified coordinates into Sprite. If rectangle would be out
// of bounds - will throw an exception.
Sprite make_sprite(Texture2D* spritesheet, Rectangle sprite_rect);

// Turn specified texture into set of same-sized sprites. If spritesheet doesn't
// cut into specified sized sprites perfectly - will throw an exception.
std::vector<Sprite> make_sprites(Texture2D* spritesheet, Vector2 sprite_size);

// TODO: spritesheet animation, maybe make some AnimationBase, to allow for both
// sprite-driven and texture-driven anims
