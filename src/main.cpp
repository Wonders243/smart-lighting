#include <Arduino.h>

#include "roles.h"

#include "device.h"
#include "device_manager.h"
#include "message_manager.h"

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
// REGISTRES GLOBAUX
// ============================================================

LampRegistry lampRegistry;
GroupRegistry groupRegistry;
SceneRegistry sceneRegistry;
AutomationRegistry automationRegistry;

EventBus eventBus;
CommunicationBus communication;


// ============================================================
// SETUP
// ============================================================

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("      SMART LIGHTING V3.1");
    Serial.println("==============================");


    // --------------------------------------------------------
    // INITIALISATION
    // --------------------------------------------------------

    initLampRegistry(lampRegistry);
    initGroupRegistry(groupRegistry);
    initSceneRegistry(sceneRegistry);
    initAutomationRegistry(automationRegistry);

    initEventBus(eventBus);
    initCommunication(communication);


    // --------------------------------------------------------
    // CREATION DES LAMPES
    // --------------------------------------------------------

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


    addLamp(lampRegistry, lamp1);
    addLamp(lampRegistry, lamp2);
    addLamp(lampRegistry, lamp3);


    // --------------------------------------------------------
    // GROUPE ENTREE
    // --------------------------------------------------------

    LampGroup entree = {
        1,
        "ENTREE",
        {},
        0
    };

    addGroup(groupRegistry, entree);

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


    // --------------------------------------------------------
    // SCENE SOIR
    // --------------------------------------------------------

    Scene soir = {
        1,
        "SOIR",
        {},
        0
    };

    addScene(sceneRegistry, soir);


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
    // TEST V3.1
    // ========================================================

    Serial.println();
    Serial.println("===== TEST V3.1 MESSAGE -> ACTION =====");


    Message command = {
        generateMessageId(),

        0,                  // CORE
        1,                  // LAMP_01

        MessageType::COMMAND,

        millis(),

        static_cast<int32_t>(
            ActionType::SET_LAMP_POWER
        ),

        1,                  // ON

        0,

        MessageStatus::PENDING
    };


    Serial.println("Envoi commande : LAMP_01 -> ON");

    sendMessage(
        communication,
        command
    );


    printCommunicationStatus(
        communication
    );


    // --------------------------------------------------------
    // ROUTAGE + EXECUTION
    // --------------------------------------------------------

    processMessages(
        communication,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // --------------------------------------------------------
    // VERIFICATION
    // --------------------------------------------------------

    Lamp* result = findLamp(
        lampRegistry,
        1
    );

    if (result != nullptr) {

        Serial.println();
        Serial.println("===== ETAT APRES MESSAGE =====");

        printLampState(*result);
    }


    // --------------------------------------------------------
    // FIN
    // --------------------------------------------------------

    Serial.println();
    Serial.println("==============================");
    Serial.println(" SMART LIGHTING V3.1 READY");
    Serial.println("==============================");
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