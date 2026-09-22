#include <Arduino.h>

#include "device_registry.h"


void initLampRegistry(
    LampRegistry& registry
) {

    registry.count = 0;
}


Lamp* findLamp(
    LampRegistry& registry,
    uint32_t id
) {

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        if (
            registry.lamps[i].device.id == id
        ) {

            return &registry.lamps[i];
        }
    }

    return nullptr;
}


bool addLamp(
    LampRegistry& registry,
    const Lamp& lamp
) {

    if (
        registry.count >= MAX_LAMPS
    ) {

        return false;
    }


    if (
        findLamp(
            registry,
            lamp.device.id
        ) != nullptr
    ) {

        return false;
    }


    registry.lamps[
        registry.count
    ] = lamp;

    registry.count++;

    return true;
}


void printLampRegistry(
    const LampRegistry& registry
) {

    Serial.println();
    Serial.println(
        "===== REGISTRE LAMPES ====="
    );

    Serial.print(
        "Nombre de lampes : "
    );

    Serial.println(
        registry.count
    );


    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        const Lamp& lamp =
            registry.lamps[i];


        Serial.print("#");
        Serial.print(i);

        Serial.print(" | ID=");
        Serial.print(
            lamp.device.id
        );

        Serial.print(" | ");
        Serial.print(
            lamp.device.name
        );

        Serial.print(" | ");

        Serial.println(
            lamp.device.status ==
            DeviceStatus::ONLINE
                ? "ONLINE"
                : "OFFLINE"
        );
    }

    Serial.println(
        "==========================="
    );
}