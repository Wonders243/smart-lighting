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
#include "action.h"

// ============================================================
// REGISTRES SYSTEME
// ============================================================

LampRegistry lampRegistry;
GroupRegistry groupRegistry;
SceneRegistry sceneRegistry;
AutomationRegistry automationRegistry;
EventBus eventBus;

CommunicationBus communication;
MessageTracker messageTracker;


// ============================================================
// ENVOI D'UNE COMMANDE
// ============================================================

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

    // Enregistrement dans le tracker
    if (
        !trackMessage(
            messageTracker,
            command
        )
    ) {
        Serial.println(
            "Impossible de suivre la commande"
        );

        return;
    }

    // Envoi dans le bus de communication
    if (
        !sendMessage(
            communication,
            command
        )
    ) {
        Serial.println(
            "Echec envoi commande"
        );
    }
}


// ============================================================
// TEST TIMEOUT / RETRY
// ============================================================

static void createTimeoutTest() {

    Serial.println();
    Serial.println(
        "================================"
    );

    Serial.println(
        "     TEST TIMEOUT / RETRY"
    );

    Serial.println(
        "================================"
    );

    Message command = {
        generateMessageId(),

        0,
        999,

        MessageType::COMMAND,

        millis(),

        static_cast<int32_t>(
            ActionType::SET_LAMP_POWER
        ),

        1,

        0,

        MessageStatus::PENDING,

        ExecutionStatus::NOT_EXECUTED
    };

    // Enregistrement dans le tracker
    if (
        !trackMessage(
            messageTracker,
            command
        )
    ) {
        Serial.println(
            "Impossible de suivre la commande"
        );

        return;
    }

    // Envoi de la commande
    if (
        !sendMessage(
            communication,
            command
        )
    ) {
        Serial.println(
            "Echec envoi commande"
        );

        return;
    }

    /*
     * On retire volontairement le message
     * du bus sans l'exécuter.
     *
     * Cela simule un destinataire qui
     * ne répond pas.
     */

    Message ignored;

    if (
        receiveMessage(
            communication,
            ignored
        )
    ) {
        Serial.println(
            "Message volontairement ignore."
        );
    }

    Serial.println(
        "Attente du timeout..."
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
        "      SMART LIGHTING V3.6"
    );

    Serial.println(
        "=============================="
    );


    // ========================================================
    // INITIALISATION DES REGISTRES
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

    initMessageTracker(
        messageTracker
    );


    // ========================================================
    // CREATION DES LAMPES
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
    // CREATION DU GROUPE
    // ========================================================

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


    // ========================================================
    // CREATION DE LA SCENE
    // ========================================================

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


    // ========================================================
    // TEST 1 : COMMAND + ACK
    // ========================================================

    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        "      TEST COMMAND + ACK"
    );

    Serial.println(
        "================================"
    );


    sendCommand(
        1,
        ActionType::SET_LAMP_POWER,
        1
    );


    /*
     * Traitement du COMMAND.
     *
     * Le routeur va :
     *
     * COMMAND
     *    ↓
     * executeAction()
     *    ↓
     * ACK
     */

    processMessages(
        communication,
        messageTracker,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    // ========================================================
    // AFFICHAGE DU TRACKER APRES ACK
    // ========================================================

    printMessageTracker(
        messageTracker
    );


    // ========================================================
    // TEST 2 : TIMEOUT / RETRY
    // ========================================================

    createTimeoutTest();


    // ========================================================
    // ETAT INITIAL
    // ========================================================

    Serial.println();

    Serial.println(
        "===== ETAT INITIAL ====="
    );

    printLampRegistry(
        lampRegistry
    );


    Serial.println();

    Serial.println(
        "=============================="
    );

    Serial.println(
        " SMART LIGHTING V3.6 READY"
    );

    Serial.println(
        "=============================="
    );
}


// ============================================================
// LOOP
// ============================================================

void loop() {

    /*
     * Mise à jour de l'état des appareils.
     */

    updateDeviceStatus(
        lampRegistry
    );


    /*
     * Vérification des messages en attente.
     *
     * Si un message dépasse MESSAGE_TIMEOUT :
     *
     *     TIMEOUT
     *        ↓
     *     RETRY #1
     *        ↓
     *     RETRY #2
     *        ↓
     *     FAILED
     */

    updateMessageTimeouts(
        messageTracker,
        communication
    );


    /*
     * Traitement des messages présents
     * dans le bus.
     *
     * IMPORTANT :
     * Le retry est remis dans le bus par
     * updateMessageTimeouts().
     *
     * Dans ce test, le destinataire 999
     * n'existe pas et le message sera donc
     * ignoré pour permettre de tester les
     * timeouts.
     */

    processMessages(
        communication,
        messageTracker,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );


    /*
     * Affichage périodique du tracker
     * pour suivre l'évolution du retry.
     */

    static uint32_t lastTrackerPrint = 0;

    if (
        millis() - lastTrackerPrint >= 1000
    ) {
        lastTrackerPrint = millis();

        Serial.println();

        Serial.println(
            "----- TRACKER -----"
        );

        printMessageTracker(
            messageTracker
        );
    }


    delay(100);
}