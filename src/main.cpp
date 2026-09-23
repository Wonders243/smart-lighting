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
#include "message_deduplicator.h"


/*
 * ============================================================
 * REGISTRES
 * ============================================================
 */

LampRegistry lampRegistry;
GroupRegistry groupRegistry;
SceneRegistry sceneRegistry;
AutomationRegistry automationRegistry;
EventBus eventBus;

CommunicationBus communication;
MessageTracker messageTracker;
MessageDeduplicator messageDeduplicator;


/*
 * ============================================================
 * VARIABLES DE TEST
 * ============================================================
 */

uint32_t timeoutMessageId = 0;
uint32_t duplicateMessageId = 0;


/*
 * ============================================================
 * MODE DE SIMULATION
 * ============================================================
 *
 * true :
 *   les messages sont volontairement perdus.
 *
 * false :
 *   communication normale.
 */

bool simulateCommunicationLoss = false;


/*
 * ============================================================
 * ENVOI D'UNE COMMANDE
 * ============================================================
 */

static uint32_t sendCommand(
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
     * Enregistrer le message dans le tracker
     * avant l'envoi.
     */

    if (
        !trackMessage(
            messageTracker,
            command
        )
    ) {

        Serial.println(
            "Erreur : impossible de suivre le message."
        );

        return 0;
    }


    /*
     * Envoyer le message.
     */

    if (
        !sendMessage(
            communication,
            command
        )
    ) {

        Serial.println(
            "Erreur : impossible d'envoyer le message."
        );

        return 0;
    }


    return command.id;
}


/*
 * ============================================================
 * SIMULATION DE PERTE DE COMMUNICATION
 * ============================================================
 */

static void simulateLostMessages() {

    Message lostMessage;


    while (
        receiveMessage(
            communication,
            lostMessage
        )
    ) {

        Serial.print(
            "Message volontairement perdu : "
        );

        Serial.println(
            lostMessage.id
        );
    }
}


/*
 * ============================================================
 * COMMUNICATION NORMALE
 * ============================================================
 */

static void processNormalCommunication() {

    processMessages(
        communication,
        messageTracker,
        messageDeduplicator,
        lampRegistry,
        groupRegistry,
        sceneRegistry
    );
}


/*
 * ============================================================
 * SETUP
 * ============================================================
 */

void setup() {

    Serial.begin(115200);

    delay(1000);


    /*
     * ========================================================
     * TITRE
     * ========================================================
     */

    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        "       SMART LIGHTING V3.7"
    );

    Serial.println(
        "================================"
    );


    /*
     * ========================================================
     * INITIALISATION
     * ========================================================
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

    initMessageDeduplicator(
        messageDeduplicator
    );


    /*
     * ========================================================
     * LAMPES
     * ========================================================
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
     * ========================================================
     * GROUPE ENTREE
     * ========================================================
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
     * ========================================================
     * SCENE SOIR
     * ========================================================
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


    /*
     * ========================================================
     * TEST 1
     * ========================================================
     *
     * COMMAND
     *    ↓
     * EXECUTION
     *    ↓
     * ACK
     *    ↓
     * TRACKER
     *    ↓
     * DELIVERED
     */

    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        " TEST 1 : COMMAND + ACK"
    );

    Serial.println(
        "================================"
    );


    simulateCommunicationLoss = false;


    uint32_t normalMessageId =
        sendCommand(
            1,
            ActionType::SET_LAMP_POWER,
            1
        );


    if (
        normalMessageId != 0
    ) {

        /*
         * COMMAND → EXECUTION → ACK
         */

        processNormalCommunication();


        /*
         * ACK → TRACKER
         */

        processNormalCommunication();
    }


    Serial.println();

    Serial.println(
        "===== TRACKER TEST 1 ====="
    );


    printMessageTracker(
        messageTracker
    );


    /*
     * ========================================================
     * TEST 2
     * ========================================================
     *
     * COMMAND
     *    ↓
     * PERDU
     *    ↓
     * TIMEOUT
     *    ↓
     * RETRY #1
     *    ↓
     * PERDU
     *    ↓
     * TIMEOUT
     *    ↓
     * RETRY #2
     *    ↓
     * PERDU
     *    ↓
     * TIMEOUT
     *    ↓
     * FAILED
     */

    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        " TEST 2 : TIMEOUT + RETRIES"
    );

    Serial.println(
        "================================"
    );


    simulateCommunicationLoss = true;


    timeoutMessageId =
        sendCommand(
            1,
            ActionType::SET_LAMP_POWER,
            0
        );


    Serial.print(
        "Message timeout : "
    );

    Serial.println(
        timeoutMessageId
    );


    /*
     * Premier message perdu.
     */

    simulateLostMessages();
}


/*
 * ============================================================
 * LOOP
 * ============================================================
 */

void loop() {

    /*
     * ========================================================
     * TEST 2
     * ========================================================
     */

    if (
        simulateCommunicationLoss
    ) {

        /*
         * Tous les messages sont volontairement perdus.
         */

        simulateLostMessages();

    }
    else {

        /*
         * Communication normale.
         */

        processNormalCommunication();
    }


    /*
     * ========================================================
     * TIMEOUT / RETRY
     * ========================================================
     */

    updateMessageTimeouts(
        messageTracker,
        communication
    );


    /*
     * ========================================================
     * VERIFICATION FIN TEST 2
     * ========================================================
     */

    PendingMessage* timeoutMessage =
        findPendingMessage(
            messageTracker,
            timeoutMessageId
        );


    if (
        timeoutMessage != nullptr &&
        timeoutMessage->completed &&
        simulateCommunicationLoss
    ) {

        Serial.println();

        Serial.println(
            "================================"
        );

        Serial.println(
            " TEST 2 TERMINE"
        );

        Serial.println(
            "================================"
        );


        printPendingMessage(
            *timeoutMessage
        );


        /*
         * Arrêt de la perte totale.
         */

        simulateCommunicationLoss = false;


        /*
         * Petite pause.
         */

        delay(1000);


        /*
         * ====================================================
         * TEST 3
         * ====================================================
         *
         * COMMAND
         *    ↓
         * EXECUTION
         *    ↓
         * ACK PERDU
         *    ↓
         * TIMEOUT
         *    ↓
         * RETRY
         *    ↓
         * DOUBLON
         *    ↓
         * PAS DE DEUXIEME EXECUTION
         *    ↓
         * NOUVEL ACK
         *    ↓
         * DELIVERED
         */

        Serial.println();

        Serial.println(
            "================================"
        );

        Serial.println(
            " TEST 3 : ACK PERDU + DOUBLON"
        );

        Serial.println(
            "================================"
        );


        /*
         * Envoi de la commande.
         */

        duplicateMessageId =
            sendCommand(
                1,
                ActionType::SET_LAMP_POWER,
                1
            );


        /*
         * ----------------------------------------------------
         * COMMAND
         * ----------------------------------------------------
         *
         * Le COMMAND est exécuté.
         *
         * L'ACK est volontairement perdu.
         */

        processMessages(
            communication,
            messageTracker,
            messageDeduplicator,
            lampRegistry,
            groupRegistry,
            sceneRegistry,
            true
        );


        Serial.println();

        Serial.println(
            "ACK perdu. Attente du timeout..."
        );
    }


    /*
     * ========================================================
     * TEST 3 : RETRY + DOUBLON
     * ========================================================
     */

    PendingMessage* duplicateMessage =
        findPendingMessage(
            messageTracker,
            duplicateMessageId
        );


    if (
        duplicateMessage != nullptr &&
        !duplicateMessage->completed &&
        duplicateMessage->retryCount > 0
    ) {

        /*
         * ----------------------------------------------------
         * RETRY
         * ----------------------------------------------------
         *
         * Le même message revient avec le même ID.
         */

        processNormalCommunication();


        /*
         * Le routeur doit :
         *
         *   reconnaître le doublon
         *        ↓
         *   ne PAS exécuter la commande
         *        ↓
         *   renvoyer un ACK
         *
         * processMessages() traite ensuite cet ACK.
         */


        /*
         * ----------------------------------------------------
         * RESULTAT TEST 3
         * ----------------------------------------------------
         */

        if (
            duplicateMessage->completed
        ) {

            Serial.println();

            Serial.println(
                "================================"
            );

            Serial.println(
                " RESULTAT TEST 3"
            );

            Serial.println(
                "================================"
            );


            printPendingMessage(
                *duplicateMessage
            );


            /*
             * Etat du déduplicateur.
             */

            printMessageDeduplicator(
                messageDeduplicator
            );


            /*
             * Etat final des lampes.
             */

            Serial.println();

            Serial.println(
                "===== ETAT FINAL LAMPES ====="
            );


            printLampRegistry(
                lampRegistry
            );


            /*
             * Empêcher le test de recommencer.
             */

            duplicateMessageId = 0;
        }
    }


    delay(100);
}