#pragma once
#include <SFML/Graphics.hpp>
#include <thread>
#include "elementUtils.h"
#include "world.h"
#include "constants.h"

// The simulation class is responsible for simulating the world, providing an
// interface for user interaction, and updating and providing the texture of
// the cellular matrix.
class Simulation {
    World world;
    int frame_count;
    
    // graphics stuff
    sf::Uint8 pixels[4 * WIDTH * HEIGHT];
    sf::Texture world_texture;
    sf::Sprite world_sprite;
    sf::CircleShape brush_circle;

    // video and window stuff
    int scale; // scale world sprite UP 
    sf::RenderWindow window;
    sf::View view;

    // GUI and player interaction
    int brush_radius;
    sf::Vector2i mouse_position; // mouse pos on last frame 
    ElementType drawing_element;

    // threading + optimization
    const static int chunk_size = 16;
    bool chunks[WIDTH / chunk_size][HEIGHT / chunk_size];
    int chunks_width = WIDTH / chunk_size;
    int chunks_height = HEIGHT / chunk_size;

    int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> thread_pool = std::vector<std::thread>(thread_count);
    
public:
    Simulation(int simulation_width, int simulation_height);
    ~Simulation() = default;

    inline void updateChunk(int xx, int yy);
    void updateWorld();
    void draw();

    void renderWorld();
    void renderBrush(); // renders circle around the mouse for brush size
    void renderChunks(); // for debug purposes

    void run();
    // helper functions
    sf::Vector2i windowPositionToWorldPosition(sf::Vector2i windowPos);
};
