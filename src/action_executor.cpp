#include <Arduino.h>

#include "action_executor.h"
#include "lamp_controller.h"
#include "scene_executor.h"
#include "group_command_handler.h"

bool executeAction(
    const Action& action,
    SceneRegistry& scenes,
    GroupRegistry& groups,
    LampRegistry& lamps
) {
    switch (action.type) {

        // =====================================
        // SCENE
        // =====================================

        case ActionType::EXECUTE_SCENE: {

            Serial.print(
                "Action : EXECUTE_SCENE -> "
            );

            Serial.println(
                action.targetId
            );

            SceneExecutionResult result;

            SceneExecutionStatus status =
                executeScene(
                    scenes,
                    groups,
                    lamps,
                    action.targetId,
                    result
                );

            return (
                status !=
                SceneExecutionStatus::FAILED
            );
        }

        // =====================================
        // LAMPE POWER
        // =====================================

        case ActionType::SET_LAMP_POWER: {

            Lamp* lamp =
                findLamp(
                    lamps,
                    action.targetId
                );

            if (lamp == nullptr) {
                Serial.println(
                    "Action impossible : lampe introuvable"
                );

                return false;
            }

            if (
                lamp->device.status ==
                DeviceStatus::OFFLINE
            ) {
                Serial.println(
                    "Action impossible : lampe OFFLINE"
                );

                return false;
            }

            setLampPower(
                *lamp,
                action.value != 0
            );

            return true;
        }

        // =====================================
        // LAMPE BRIGHTNESS
        // =====================================

        case ActionType::SET_LAMP_BRIGHTNESS: {

            Lamp* lamp =
                findLamp(
                    lamps,
                    action.targetId
                );

            if (lamp == nullptr) {
                Serial.println(
                    "Action impossible : lampe introuvable"
                );

                return false;
            }

            if (
                lamp->device.status ==
                DeviceStatus::OFFLINE
            ) {
                Serial.println(
                    "Action impossible : lampe OFFLINE"
                );

                return false;
            }

            setLampBrightness(
                *lamp,
                action.value
            );

            return true;
        }

        // =====================================
        // LAMPE AUTOMATIC
        // =====================================

        case ActionType::SET_LAMP_AUTOMATIC: {

            Lamp* lamp =
                findLamp(
                    lamps,
                    action.targetId
                );

            if (lamp == nullptr) {
                Serial.println(
                    "Action impossible : lampe introuvable"
                );

                return false;
            }

            if (
                lamp->device.status ==
                DeviceStatus::OFFLINE
            ) {
                Serial.println(
                    "Action impossible : lampe OFFLINE"
                );

                return false;
            }

            setLampAutomatic(
                *lamp,
                action.value != 0
            );

            return true;
        }

        // =====================================
        // GROUPE POWER
        // =====================================

        case ActionType::SET_GROUP_POWER: {

            Command command = {
                0,
                action.targetId,
                CommandType::SET_GROUP_POWER,
                action.value,
                CommandStatus::PENDING
            };

            return executeGroupCommand(
                groups,
                lamps,
                command
            );
        }

        // =====================================
        // GROUPE BRIGHTNESS
        // =====================================

        case ActionType::SET_GROUP_BRIGHTNESS: {

            Command command = {
                0,
                action.targetId,
                CommandType::SET_GROUP_BRIGHTNESS,
                action.value,
                CommandStatus::PENDING
            };

            return executeGroupCommand(
                groups,
                lamps,
                command
            );
        }
    }

    return false;
}