#pragma once

#include <Arduino.h>
#include "device.h"


struct LampState {

    bool power;

    uint8_t brightness;

    bool automatic;

};


struct Lamp {

    Device device;

    LampState state;

};