#pragma once

#include <Arduino.h>


enum class CommandType {

    SET_POWER,

    SET_BRIGHTNESS,

    SET_AUTOMATIC

};


enum class CommandStatus {

    PENDING,

    EXECUTED,

    FAILED

};


struct Command {

    uint32_t id;

    uint32_t targetId;

    CommandType type;

    int32_t value;

    CommandStatus status;

};