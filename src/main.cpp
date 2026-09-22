#include <Arduino.h>

#include "roles.h"

#include "device.h"
#include "device_manager.h"

#include "lamp.h"
#include "lamp_controller.h"

#include "device_registry.h"

#include "group_manager.h"

#include "scene_manager.h"
#include "scene_executor.h"

#include "automation_manager.h"
#include "automation_engine.h"
#include "action_executor.h"

#include "event.h"
#include "event_bus.h"
#include "event_processor.h"
#include "id_generator.h"
#include "automation_context.h"

#include "message.h"
#include "message_manager.h"
#include "communication.h"
#include "message_id_generator.h"
#include "message_router.h"


// ============================================================
// REGISTRES
// ============================================================

LampRegistry lampRegistry;

GroupRegistry groupRegistry;

SceneRegistry sceneRegistry;

AutomationRegistry automationRegistry;

EventBus eventBus;

CommunicationBus communication;


// ============================================================
// CREATION MESSAGE
// ============================================================

void sendCommand(
    uint32_t destinationId,
    ActionType actionType,
    int32_t value
) {

    Message command = {

        generateMessageId(),

        0,                  // CORE
        destinationId,

        MessageType::COMMAND,

        millis(),

        static_cast<int32_t>(
            actionType
        ),

        value,

        0,

        MessageStatus::PENDING
    };


    sendMessage(
        communication,
        command
    );
}


// ============================================================
// SETUP
// ============================================================

void setup() {

    Serial.begin(115200);

    delay(1000);


    Serial.println();
    Serial.println(
        "=============================="
    );

    Serial.println(
        "      SMART LIGHTING V3.2"
    );

    Serial.println(
        "=============================="
    );


    // ========================================================
    // INITIALISATION
    // ========================================================

    initLampRegistry(
        lampRegistry
    );

    initGroupRegistry(
        groupRegistry
    );

    initSceneRegistry(
        sceneRegistry
    );

    initAutomationRegistry(
        automationRegistry
    );

    initEventBus(
        eventBus
    );

    initCommunication(
        communication
    );


    // ========================================================
    // LAMPES
    // ========================================================

    Lamp lamp1 = {

        {
            1,
            "LAMP_01",
            DeviceRole::LAMP,
            DeviceStatus::ONLINE,
            millis()
        },

        {
            false,
            0,
            false
        }
    };


    Lamp lamp2 = {

        {
            2,
            "LAMP_02",
            DeviceRole::LAMP,
            DeviceStatus::ONLINE,
            millis()
        },

        {
            false,
            0,
            false
        }
    };


    Lamp lamp3 = {

        {
            3,
            "LAMP_03",
            DeviceRole::LAMP,
            DeviceStatus::OFFLINE,
            millis()
        },

        {
            false,
            0,
            false
        }
    };


    addLamp(
        lampRegistry,
        lamp1
    );

    addLamp(
        lampRegistry,
        lamp2
    );

    addLamp(
        lampRegistry,
        lamp3
    );


    // ========================================================
    // GROUPE
    // ========================================================

    LampGroup entree = {

        1,
        "ENTREE",
        {},
        0
    };


    addGroup(
        groupRegistry,
        entree
    );


    addLampToGroup(
        groupRegistry,
        lampRegistry,
        1,
        1
    );


    addLampToGroup(
        groupRegistry,
        lampRegistry,
        1,
        2
    );


    addLampToGroup(
        groupRegistry,
        lampRegistry,
        1,
        3
    );


    // ========================================================
    // SCENE
    // ========================================================

    Scene soir = {

        1,
        "SOIR",
        {},
        0
    };


    addScene(
        sceneRegistry,
        soir
    );


    SceneAction scenePower = {

        1,
        CommandType::SET_GROUP_POWER,
        1
    };


    SceneAction sceneBrightness = {

        1,
        CommandType::SET_GROUP_BRIGHTNESS,
        40
    };


    addActionToScene(
        sceneRegistry,
        groupRegistry,
        1,
        scenePower
    );


    addActionToScene(
        sceneRegistry,
        groupRegistry,
        1,
        sceneBrightness
    );


    // ========================================================
    // TEST V3.2
    // ========================================================

    Serial.println();
    Serial.println(
        "===== TEST V3.2 ====="
    );


    // --------------------------------------------------------
    // 1. LAMP_01 ON
    // --------------------------------------------------------

    Serial.println();
    Serial.println(
        "[1] LAMP_01 -> ON"
    );


    sendCommand(
        1,
        ActionType::SET_LAMP_POWER,
        1
    );


    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // --------------------------------------------------------
    // 2. LAMP_01 BRIGHTNESS 75
    // --------------------------------------------------------

    Serial.println();
    Serial.println(
        "[2] LAMP_01 -> 75%"
    );


    sendCommand(
        1,
        ActionType::SET_LAMP_BRIGHTNESS,
        75
    );


    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // --------------------------------------------------------
    // 3. LAMP_01 AUTOMATIC ON
    // --------------------------------------------------------

    Serial.println();
    Serial.println(
        "[3] LAMP_01 -> AUTOMATIC"
    );


    sendCommand(
        1,
        ActionType::SET_LAMP_AUTOMATIC,
        1
    );


    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // --------------------------------------------------------
    // 4. GROUPE ENTREE -> ON
    // --------------------------------------------------------

    Serial.println();
    Serial.println(
        "[4] GROUPE ENTREE -> ON"
    );


    sendCommand(
        1,
        ActionType::SET_GROUP_POWER,
        1
    );


    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // --------------------------------------------------------
    // 5. GROUPE ENTREE -> 40%
    // --------------------------------------------------------

    Serial.println();
    Serial.println(
        "[5] GROUPE ENTREE -> 40%"
    );


    sendCommand(
        1,
        ActionType::SET_GROUP_BRIGHTNESS,
        40
    );


    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // --------------------------------------------------------
    // 6. SCENE SOIR
    // --------------------------------------------------------

    Serial.println();
    Serial.println(
        "[6] SCENE SOIR"
    );


    sendCommand(
        1,
        ActionType::EXECUTE_SCENE,
        0
    );


    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // ========================================================
    // ETAT FINAL
    // ========================================================

    Serial.println();
    Serial.println(
        "===== ETATS FINAUX ====="
    );


    Lamp* lamp01 = findLamp(
        lampRegistry,
        1
    );


    Lamp* lamp02 = findLamp(
        lampRegistry,
        2
    );


    Lamp* lamp03 = findLamp(
        lampRegistry,
        3
    );


    if (lamp01 != nullptr) {

        printLampState(
            *lamp01
        );
    }


    if (lamp02 != nullptr) {

        printLampState(
            *lamp02
        );
    }


    if (lamp03 != nullptr) {

        printLampState(
            *lamp03
        );
    }


    Serial.println();
    Serial.println(
        "=============================="
    );

    Serial.println(
        " SMART LIGHTING V3.2 READY"
    );

    Serial.println(
        "=============================="
    );
}


// ============================================================
// LOOP
// ============================================================

void loop() {

    updateDeviceStatus(
        lampRegistry
    );

    delay(1000);
}