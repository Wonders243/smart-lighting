#pragma once

#include <Arduino.h>

enum class ActionType {
    EXECUTE_SCENE,
    SET_LAMP_POWER,
    SET_LAMP_BRIGHTNESS,
    SET_LAMP_AUTOMATIC,
    SET_GROUP_POWER,
    SET_GROUP_BRIGHTNESS
};

struct Action {
    ActionType type;

    uint32_t targetId;

    int32_t value;
};