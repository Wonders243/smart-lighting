#pragma once

#include <Arduino.h>

struct AutomationContext {

    uint32_t timestamp;

    int32_t lightLevel;

    bool presenceDetected;

    uint8_t hour;

    uint8_t minute;
};