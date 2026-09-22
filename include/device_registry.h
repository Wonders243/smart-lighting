#pragma once

#include "lamp.h"


constexpr uint8_t MAX_LAMPS = 10;


struct LampRegistry {

    Lamp lamps[MAX_LAMPS];

    uint8_t count;

};


void initLampRegistry(
    LampRegistry& registry
);


Lamp* findLamp(
    LampRegistry& registry,
    uint32_t id
);


bool addLamp(
    LampRegistry& registry,
    const Lamp& lamp
);


void printLampRegistry(
    const LampRegistry& registry
);