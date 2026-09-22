#pragma once

#include <Arduino.h>
#include "roles.h"


enum class DeviceStatus {

    OFFLINE,

    ONLINE

};


struct Device {

    uint32_t id;

    const char* name;

    DeviceRole role;

    DeviceStatus status;

    uint32_t lastSeen;

};