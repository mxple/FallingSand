#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstring>
#include <iostream>
#include <chrono>
#include "simulation.h"
#include "rng.h"

Simulation::Simulation(int window_width, int window_height) :
    window(sf::VideoMode(window_width, window_height), "FallingSand", sf::Style::Resize),
    world()
{
    scale = 1;
    window.setSize(sf::Vector2u(window_width, window_height));
    window.setPosition( {0, 0} );
    // window.setFramerateLimit(1chunk_size0);
    window.setMouseCursorVisible(false);

    view.reset(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    window.setView(view);

    frame_count = 0;
    world_texture.create(WIDTH, HEIGHT);
    brush_radius = 4;
    mouse_position = sf::Vector2i(0, 0);
    drawing_element = IMMOVEABLE_SOLID;
    
    // set all chunks to active
    std::memset(chunks, 1, sizeof(chunks));

    // set pixels to 0 to remove artifacting
    std::memset(pixels, 0, sizeof(pixels));

};

inline void Simulation::updateChunk(int xx, int yy) {
    if (!chunks[xx][yy]) return;
    chunks[xx][yy] = false;
    bool is_active = false;
    for (int y = (yy + 1) * chunk_size - 1; y >= yy * chunk_size; y--) {
        if (fiftyFifty()) {
            for (int x = xx * chunk_size; x < (xx + 1) * chunk_size; x++) {
                if (world.matrix[x + y * world.width] == EMPTY_CELL) continue;
                if (world.matrix[x + y * world.width] & (1 << 31)) continue; // if stepped
                world.matrix[x + y * world.width] = static_cast<ElementType>(world.matrix[x + y * world.width] | (1 << 31)); // set stepped
                is_active = is_active | world.update(x, y);
            }
        } else {
            for (int x = (xx + 1) * chunk_size - 1; x >= xx * chunk_size; x--) {
                if (world.matrix[x + y * world.width] == EMPTY_CELL) continue;
                if (world.matrix[x + y * world.width] & (1 << 31)) continue; // if stepped
                world.matrix[x + y * world.width] = static_cast<ElementType>(world.matrix[x + y * world.width] | (1 << 31)); // set stepped
                is_active = is_active | world.update(x, y);
            }
        }
    }

    if (!is_active) return; 
    for (int y = yy - 1; y <= yy + 1; y++) {
        for (int x = xx - 1; x <= xx + 1; x++) {
            if (y < chunks_height && y >= 0 && x < chunks_width && x >= 0) chunks[x][y] = true;                         
        }
    }
}

void Simulation::updateWorld() {
    // multithreading in 2 stages
    bool is_active;
    for (int yy = chunks_height - 1; yy >= 0; yy--) {
        for (int xx = 0; xx < chunks_width; xx++) {
            updateChunk(xx, yy);
        }
    }

    // CANNOT use chunks, not active does not guarantee no step
    // unstep all elements
    for (int i = 0; i < world.size; i++) world.matrix[i] = static_cast<ElementType>(world.matrix[i] & ~(1 << 31));
}

void Simulation::draw() {
    // step 0: find old and new mouse positions
    sf::Vector2i old_pos = static_cast<sf::Vector2i>(window.mapPixelToCoords(mouse_position));
    sf::Vector2i new_pos = static_cast<sf::Vector2i>(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    // step 1: find line between old pos and new pos
    std::vector<sf::Vector2i> line;
    line.push_back(old_pos);

    if (old_pos != new_pos) {
        // swap old and new pos so that old pos is left of new pos
        if (new_pos.x < old_pos.x) std::swap(new_pos, old_pos);

        float slope = 1. * (old_pos.y - new_pos.y) / (old_pos.x - new_pos.x);
        float y = old_pos.y;
        for (int x_offset = 1; x_offset <= new_pos.x - old_pos.x; x_offset++) {
            y += slope;
            line.push_back(sf::Vector2i(old_pos.x + x_offset, std::round(y)));
        }
    }

    // step 2: iterate across the line
    for (sf::Vector2i& pos : line) {
        // in case brush_radius is 0, just draw at self 
        world.spawnElementAtPosition(pos.x, pos.y, drawing_element);
        if (world.inBounds(pos.x, pos.y)) chunks[pos.x / chunk_size][pos.y / chunk_size] = true;
        // step 3: iterate across square of size 2r centered around line px 
        for (int x = pos.x - brush_radius; x < pos.x + brush_radius; x++) {
            for (int y = pos.y - brush_radius; y < pos.y + brush_radius; y++) {
                // step 4: spawn element at cells within circle (use dist formula)
                if (pow(x - pos.x, 2) + pow(y - pos.y, 2) > pow(brush_radius, 2)) continue;
                world.spawnElementAtPosition(x, y, drawing_element);
                if (world.inBounds(x, y)) chunks[x / chunk_size][y / chunk_size] = true;
            }
        } 
    }
}

void Simulation::renderWorld() {
    // auto t1 = std::chrono::high_resolution_clock::now();

    #if 0
    auto updatePixels = [this] (int start, int end) {
        for (int i = start; i < end; i++) {
            const sf::Color curr_col = PROPERTIES[world.matrix[i]].default_color;
            pixels[4 * i + 0] = curr_col.r;
            pixels[4 * i + 1] = curr_col.g;
            pixels[4 * i + 2] = curr_col.b;
            pixels[4 * i + 3] = curr_col.a;
        }
    };
    // create threads and assign them sections depending on thread count
    for (int t = 0; t < thread_count; t++) {
        int start = t * world.size / thread_count;
        int end = (t + 1) * world.size / thread_count;
        thread_pool[t] = std::thread(updatePixels, start, end);
    }

    // join threads
    for (std::thread& thread : thread_pool) {
        if (thread.joinable()) thread.join();
    }
    #else
    for (int yy = 0; yy < chunks_height; yy++) {
    for (int xx = 0; xx < chunks_width; xx++) {
        if (!chunks[xx][yy]) continue;
        for (int y = yy * chunk_size; y < (yy + 1) * chunk_size; y++) {
        for (int x = xx * chunk_size; x < (xx + 1) * chunk_size; x++) {
            std::memcpy(
                        pixels + (4 * (x + y * world.width)),
                        &PROPERTIES[world.matrix[(x + y * world.width)]].default_color, 
                        4
                    );
            // int i = x + y * world.width;
            // const sf::Color& curr_col = PROPERTIES[world.matrix[i]].default_color;
            // pixels[4 * i + 0] = curr_col.r;
            // pixels[4 * i + 1] = curr_col.g;
            // pixels[4 * i + 2] = curr_col.b;
            // pixels[4 * i + 3] = curr_col.a;
        }
        }
    }
    }
    // for (int i = 0; i < world.size; i++) {
    //     std::memcpy(pixels + (4 * i), &PROPERTIES[world.matrix[i]].default_color, 4);
    //     const sf::Color& curr_col = PROPERTIES[world.matrix[i]].default_color;
    //     pixels[4 * i + 0] = curr_col.r;
    //     pixels[4 * i + 1] = curr_col.g;
    //     pixels[4 * i + 2] = curr_col.b;
    //     pixels[4 * i + 3] = curr_col.a;
    // }
    #endif
    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto us_int = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    // std::cout << us_int.count() << "ns\n";

    world_texture.update(pixels);
    world_sprite.setTexture(world_texture);
    window.draw(world_sprite);
}

void Simulation::renderBrush() {
    brush_circle.setRadius(brush_radius + .2); // magic .2 for 0 radius
    brush_circle.setOutlineThickness(.5 / scale);
    brush_circle.setOutlineColor(sf::Color::White);
    brush_circle.setFillColor(sf::Color::Transparent);
    brush_circle.setOrigin(brush_radius, brush_radius);
    brush_circle.setPosition(window.mapPixelToCoords(mouse_position));

    window.draw(brush_circle);
}

void Simulation::renderChunks() {
    sf::RectangleShape square(sf::Vector2f(chunk_size, chunk_size));
    square.setFillColor(sf::Color::Transparent);
    square.setOutlineColor(sf::Color::Red);
    square.setOutlineThickness(1.);
    for (int x = 0; x < chunks_width; x++) {
        for (int y = 0; y < chunks_height; y++) {
            if (chunks[x][y]) {
                square.setPosition(chunk_size * x, chunk_size * y);
                window.draw(square);
            }
        }
    }
}

void Simulation::run() {
    sf::Clock clock;
    float lastTime = false;
    bool mouse_down = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                        world.reset(); 
                        std::memset(pixels, 0, sizeof(pixels));
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                        drawing_element = IMMOVEABLE_SOLID;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
                        drawing_element = SAND;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
                        drawing_element = WATER;
                    }
                    break;

                case sf::Event::Resized:
                    scale = std::min(window.getSize().x / world.width, window.getSize().y / world.height);
                    scale = std::max(scale, 1);
                    view.setSize(sf::Vector2f(event.size.width/scale, event.size.height/scale));
                    break;
                
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseButtonPressed:
                    mouse_down = true;
                    // std::cout << "Pressed! \n";
                    break;

                case sf::Event::MouseButtonReleased:
                    mouse_down = false;
                    // std::cout << "Released! \n";
                    break;
                
                case sf::Event::MouseWheelMoved:
                    if (event.mouseWheel.delta > 0) {
                        brush_radius = std::min(brush_radius + 1, 75);
                    } else {
                        brush_radius = std::max(brush_radius - 1, 0);
                    }
                    break;
            }
        }
        window.clear();


        updateWorld();
        frame_count++;

        // draw must be after world update to account for the drawing of cells
        // which may be marked inactive by the world update and thus, not be rendered
        if (mouse_down) draw();
        mouse_position = sf::Mouse::getPosition(window);

        renderWorld();
        renderBrush();
        renderChunks();
        
        window.setView(view);
        window.display();

        #if 1
        if (frame_count % 100 == 0) {
            float currentTime = clock.getElapsedTime().asSeconds();
            double fps = 100.f /(currentTime-lastTime);
            lastTime = currentTime;
            std::cerr << "fps: " << fps << std::endl;
        }
        #endif
        xorshf96();
    }
}
