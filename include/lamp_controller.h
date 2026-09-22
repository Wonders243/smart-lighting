#pragma once

#include "lamp.h"


void setLampPower(
    Lamp& lamp,
    bool state
);


void setLampBrightness(
    Lamp& lamp,
    uint8_t brightness
);


void setLampAutomatic(
    Lamp& lamp,
    bool enabled
);


void printLampState(
    const Lamp& lamp
);