#include <cstring>
#include <iostream>
#include "elementUtils.h"
#include "world.h"

World::World() {
    reset();
}

World::~World() {}

ElementType World::getElementAtPosition(int x, int y) {
    if (!inBounds(x, y)) return NULL_ELEMENT;
    return static_cast<ElementType>(matrix[x + y * width] & 0xFF);
}

void World::swapElementsAtPositions(int x1, int y1, int x2, int y2) {
    std::swap(matrix[x1 + y1 * width], matrix[x2 + y2 * width]);
}

void World::setElementAtPosition(int x, int y, ElementType e) {
    matrix[x + y * width] = e;
}

// ONLY to be used AFTER all cells are initialized 
void World::spawnElementAtPosition(int x, int y, ElementType e) {
    if (inBounds(x, y) && getElementAtPosition(x, y) == EMPTY_CELL) {
        setElementAtPosition(x, y, e);
    }
}

void World::reset() {
    memset(matrix, 0, sizeof(matrix));
}

bool World::update(int x, int y) {
    ElementType e = static_cast<ElementType>(matrix[x + y * width] & 0xFF);
    ElementType o;
    switch (e) {
        case EMPTY_CELL:
            return false;
            break;

        case IMMOVEABLE_SOLID:
            return false;
            break;

        case SAND:
            o = getElementAtPosition(x, y + 1);
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x, y + 1);
                return true;
            }
            o = getElementAtPosition(x + (fiftyFifty() ? 1 : -1), y + 1);       
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x + (fiftyFifty() ? 1 : -1), y + 1);
                return true;
            }
            o = getElementAtPosition(x + (!fiftyFifty() ? 1 : -1), y + 1);       
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x + (!fiftyFifty() ? 1 : -1), y + 1);
                return true;
            }
            break;
        
        case WATER:
            o = getElementAtPosition(x, y + 1);
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x, y + 1);
                return true;
            }
            o = getElementAtPosition(x + (fiftyFifty() ? 1 : -1), y + 1);       
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x + (fiftyFifty() ? 1 : -1), y + 1);
                return true;
            }
            o = getElementAtPosition(x + (!fiftyFifty() ? 1 : -1), y + 1);       
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x + (!fiftyFifty() ? 1 : -1), y + 1);
                return true;
            }
            o = getElementAtPosition(x + (fiftyFifty() ? 1 : -1), y);       
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x + (fiftyFifty() ? 1 : -1), y);
                return true;
            }
            o = getElementAtPosition(x + (!fiftyFifty() ? 1 : -1), y);       
            if (o != NULL_ELEMENT && PROPERTIES[o].density < PROPERTIES[e].density) {
                swapElementsAtPositions(x, y, x + (!fiftyFifty() ? 1 : -1), y);
                return true;
            }
            break;

        case GAS:

        case ACID:

        case WOOD:

        case FIRE:

        case LAVA:

        return false;
    }
    return false;
}

bool World::inBounds(int x, int y) {
    return y < height && y >= 0 && x < width && x >= 0;
}
