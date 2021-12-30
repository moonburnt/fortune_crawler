#pragma once

enum class TileType {
    floor,
    // wall,
    player,
    entrance,
    exit,
    treasure,
    trap,
    enemy,
    boss
};

class Tile {
    private:
        Vector2 pos;
        Texture2D* texture;
        TileType type;

    public:
        Tile(Texture2D* sprite, TileType tile_type);

        void set_pos(Vector2 position);

        Tile(Texture2D* sprite, TileType tile_type, Vector2 position);

        //In current implementation, tiles have no update() function, since they
        //are static images without any logic attached. This may change later
        void draw();
};

class GameMap {
    private:
        std::vector<Tile*>floor_tiles;
        std::vector<Tile*>object_tiles;
        std::vector<Tile*>creature_tiles;
        Tile* player;

    public:
        GameMap(
            std::vector<Tile*>floor,
            std::vector<Tile*>objects,
            std::vector<Tile*>creatures,
            Vector2 player_spawnpoint
        );

        void update();

        void draw();
};

class MapGenerator {
    private:
        std::unordered_map<int, TileType> tile_relationships;
        std::unordered_map<TileType, std::vector<Vector2>> map_elements;
        Vector2 map_size;

    public:
        MapGenerator();

        void add_relationship(Color color, TileType tile_type);

        void process_template(Image map_file);

        GameMap* generate(Vector2 tile_size);
};
