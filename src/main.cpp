#include <Arduino.h>

#include "device_registry.h"
#include "group_manager.h"
#include "scene_manager.h"
#include "automation_manager.h"
#include "event_bus.h"
#include "communication.h"
#include "message_tracker.h"
#include "message_router.h"
#include "message_id_generator.h"
#include "device_manager.h"

LampRegistry lampRegistry;
GroupRegistry groupRegistry;
SceneRegistry sceneRegistry;
AutomationRegistry automationRegistry;
EventBus eventBus;

CommunicationBus communication;
MessageTracker messageTracker;

static void sendCommand(
    uint32_t destinationId,
    ActionType actionType,
    int32_t value
) {
    Message command = {
        generateMessageId(),

        0,
        destinationId,

        MessageType::COMMAND,

        millis(),

        static_cast<int32_t>(
            actionType
        ),

        value,

        0,

        MessageStatus::PENDING,

        ExecutionStatus::NOT_EXECUTED
    };

    /*
     * Le message est d'abord enregistré
     * dans le tracker.
     */
    trackMessage(
        messageTracker,
        command
    );

    /*
     * Puis envoyé dans le bus.
     */
    sendMessage(
        communication,
        command
    );
}

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println(
        "=============================="
    );
    Serial.println(
        "      SMART LIGHTING V3.5"
    );
    Serial.println(
        "=============================="
    );

    /*
     * INITIALISATION
     */

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

    initMessageTracker(
        messageTracker
    );

    /*
     * LAMPES
     */

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

    /*
     * GROUPE
     */

    LampGroup entrance = {
        1,
        "ENTREE",
        {},
        0
    };

    addGroup(
        groupRegistry,
        entrance
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

    /*
     * SCENE
     */

    Scene evening = {
        1,
        "SOIR",
        {},
        0
    };

    addScene(
        sceneRegistry,
        evening
    );

    SceneAction powerAction = {
        1,
        CommandType::SET_GROUP_POWER,
        1
    };

    addActionToScene(
        sceneRegistry,
        groupRegistry,
        1,
        powerAction
    );

    Serial.println();
    Serial.println(
        "===== TEST V3.5 ====="
    );

    /*
     * Envoi d'une commande.
     */
    sendCommand(
        1,
        ActionType::SET_LAMP_POWER,
        1
    );

    /*
     * Traitement COMMAND + ACK.
     */
    processMessages(
        communication,
        messageTracker,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );

    /*
     * Affichage du suivi.
     */
    printMessageTracker(
        messageTracker
    );

    Serial.println();
    Serial.println(
        "===== ETAT FINAL ====="
    );

    printLampRegistry(
        lampRegistry
    );

    Serial.println();
    Serial.println(
        "=============================="
    );

    Serial.println(
        " SMART LIGHTING V3.5 READY"
    );

    Serial.println(
        "=============================="
    );
}

void loop() {

    /*
     * Surveillance des lampes.
     */
    updateDeviceStatus(
        lampRegistry
    );

    /*
     * Surveillance des messages
     * en attente d'ACK.
     */
    updateMessageTimeouts(
        messageTracker
    );

    /*
     * Dans la vraie architecture,
     * processMessages() sera appelé
     * régulièrement ici.
     */

    delay(1000);
}