#include <Arduino.h>

#include "lamp.h"
#include "lamp_controller.h"
#include "device_registry.h"
#include "command.h"
#include "registry_command_handler.h"
#include "device_manager.h"


LampRegistry registry;


Lamp lamp1 = {

    {
        1,
        "LAMP_01",
        DeviceRole::LAMP,
        DeviceStatus::ONLINE,
        0
    },

    {
        false,
        100,
        false
    }
};


Lamp lamp2 = {

    {
        2,
        "LAMP_02",
        DeviceRole::LAMP,
        DeviceStatus::ONLINE,
        0
    },

    {
        true,
        70,
        false
    }
};


Lamp lamp3 = {

    {
        3,
        "LAMP_03",
        DeviceRole::LAMP,
        DeviceStatus::OFFLINE,
        0
    },

    {
        false,
        100,
        true
    }
};


void setup() {

    Serial.begin(115200);

    delay(500);


    Serial.println();
    Serial.println(
        "=============================="
    );

    Serial.println(
        "      SMART LIGHTING V1"
    );

    Serial.println(
        "=============================="
    );


    // --------------------------------
    // Initialisation du registre
    // --------------------------------

    initLampRegistry(
        registry
    );


    // --------------------------------
    // Ajout des appareils
    // --------------------------------

    addLamp(
        registry,
        lamp1
    );

    addLamp(
        registry,
        lamp2
    );

    addLamp(
        registry,
        lamp3
    );


    // --------------------------------
    // Simulation de présence
    // --------------------------------

    updateDeviceSeen(
        registry.lamps[0].device
    );

    updateDeviceSeen(
        registry.lamps[1].device
    );


    // --------------------------------
    // Affichage initial
    // --------------------------------

    printLampRegistry(
        registry
    );


    // --------------------------------
    // Commande ciblant LAMP_02
    // --------------------------------

    Command command = {

        100,

        2,

        CommandType::SET_POWER,

        1,

        CommandStatus::PENDING
    };


    executeRegistryCommand(
        registry,
        command
    );


    // --------------------------------
    // Affichage après commande
    // --------------------------------

    printLampRegistry(
        registry
    );


    // --------------------------------
    // Etat de LAMP_02
    // --------------------------------

    Lamp* lamp =
        findLamp(
            registry,
            2
        );


    if (lamp != nullptr) {

        printLampState(
            *lamp
        );
    }


    Serial.println();
    Serial.println(
        "SMART LIGHTING V1 READY"
    );
}


void loop() {

    updateDeviceStatus(
        registry
    );

    delay(1000);
}