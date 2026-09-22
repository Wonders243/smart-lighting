#pragma once

#include "device_registry.h"


constexpr uint32_t DEVICE_TIMEOUT = 10000;


void updateDeviceSeen(
    Device& device
);


void updateDeviceStatus(
    LampRegistry& registry
);