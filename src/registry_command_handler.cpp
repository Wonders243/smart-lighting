#include <Arduino.h>

#include "registry_command_handler.h"
#include "command_handler.h"


bool executeRegistryCommand(
    LampRegistry& registry,
    Command& command
) {

    Lamp* lamp = findLamp(
        registry,
        command.targetId
    );


    if (lamp == nullptr) {

        Serial.print(
            "Lampe introuvable : "
        );

        Serial.println(
            command.targetId
        );

        command.status =
            CommandStatus::FAILED;

        return false;
    }


    return executeCommand(
        *lamp,
        command
    );
}