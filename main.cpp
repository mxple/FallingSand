#include <SFML/Graphics.hpp>
#include <iostream>
#include "simulation.h"
#include "constants.h"

int main() {
    Simulation simulation(1280, 720);
    simulation.run();
    return 0;
}
