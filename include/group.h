#pragma once

#include <Arduino.h>


// ============================================================
// CONFIGURATION
// ============================================================

constexpr uint8_t MAX_GROUP_LAMPS = 10;


// ============================================================
// GROUPE DE LAMPES
// ============================================================

struct LampGroup {

    uint32_t id;

    const char* name;

    uint32_t lampIds[MAX_GROUP_LAMPS];

    uint8_t lampCount;
};