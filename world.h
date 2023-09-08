#pragma once
#include <vector>
#include "elementUtils.h"
#include "constants.h"
#include "rng.h"

// The World class provides an interface and wrapper for the cellular matrix.
// It includes functions to edit cells, translate between indicies and coordinates,
// check bounds, get differing amounts of neighbors, and more.
// However, what it does NOT do is manage the simulation. It serves ONLY as an 
// interface between the matrix world and simulation.
class World {
public:
    const static int width = WIDTH;
    const static int height = HEIGHT;
    const static int size = WIDTH * HEIGHT;

    ElementType matrix[size];

    World();
    ~World();

    // modify elements
    ElementType getElementAtPosition(int x, int y);
    void swapElementsAtPositions(int x1, int x2, int y1, int y2);
    void setElementAtPosition(int x, int y, ElementType e);
    void spawnElementAtPosition(int x, int y, ElementType e);

    bool update(int x, int y);
    void reset();

    // helper functions
    bool inBounds(int x, int y);

    // std::vector<Element*> get8Neighbors(int x, int y);
};

