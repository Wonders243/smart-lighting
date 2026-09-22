#include <Arduino.h>

#include "device_manager.h"


void updateDeviceSeen(
    Device& device
) {

    device.lastSeen = millis();

    device.status =
        DeviceStatus::ONLINE;
}


void updateDeviceStatus(
    LampRegistry& registry
) {

    uint32_t now = millis();


    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        Device& device =
            registry.lamps[i].device;


        if (
            device.status ==
            DeviceStatus::ONLINE
        ) {

            if (
                now - device.lastSeen >
                DEVICE_TIMEOUT
            ) {

                device.status =
                    DeviceStatus::OFFLINE;


                Serial.print(
                    "Device OFFLINE : "
                );

                Serial.println(
                    device.name
                );
            }
        }
    }
}