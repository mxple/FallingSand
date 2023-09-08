#pragma once
#include <SFML/Graphics.hpp>
#include <limits.h>

enum ElementType : int32_t {
    EMPTY_CELL,
    IMMOVEABLE_SOLID,
    SAND,
    WATER,
    GAS,
    ACID,
    WOOD,
    FIRE,
    LAVA,
    NULL_ELEMENT = -1
};

// in most cases, a value of -1 means Not Applicable
struct ElementProperties {
    int density; // solids: INT_MAX, powders & liquids: +int, gases -int, empty: INT_MIN
    int blast_resistance;

    float corrodability; // chance from 0 to 1 to die if acid is above
    float flammability; // chance from 0 to 1 to catch fire
    
    float chance_to_die; // chance to die on a frame. 0 for no chance.
    

    int friction;
    int dispersion_rate;

    sf::Color default_color;
};

inline const ElementProperties PROPERTIES[] = {
    // EMPTY_CELL
    ElementProperties {
        .density            = INT_MIN,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 0,
        .default_color      = sf::Color::Transparent,
    },
    // IMMOVEABLE_SOLID
    ElementProperties {
        .density            = INT_MAX,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 0,
        .default_color      = sf::Color::White,
    },
    // SAND
    ElementProperties {
        .density            = 10,
        .blast_resistance   = 40,
        .corrodability      = 0.1,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 5,
        .dispersion_rate    = 0,
        .default_color      = sf::Color::Yellow,
    },
    // WATER
    ElementProperties {
        .density            = 8,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 5,
        .default_color      = sf::Color::Cyan,
    },
    // GAS 
    ElementProperties {
        .density            = -3,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 1,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 8,
        .default_color      = sf::Color::Green,
    },
    // ACID
    ElementProperties {
        .density            = 8,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 5,
        .default_color      = sf::Color::Green,
    },
    // WOOD
    ElementProperties {
        .density            = 8,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 0,
        .default_color      = sf::Color(139, 69, 19), // brown
    },
    // FIRE
    ElementProperties {
        .density            = 8,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0.004,
        .friction           = 0,
        .dispersion_rate    = 0,
        .default_color      = sf::Color::Red,
    },
    // LAVA
    ElementProperties {
        .density            = 12,
        .blast_resistance   = INT_MAX,
        .corrodability      = 0,
        .flammability       = 0,
        .chance_to_die      = 0,
        .friction           = 0,
        .dispersion_rate    = 2,
        .default_color      = sf::Color::Red,
    },

};


