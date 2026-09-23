#include <Arduino.h>

#include "message_router.h"
#include "message_manager.h"
#include "action_executor.h"
#include "message_id_generator.h"


static bool isValidCommandType(
    int32_t commandType
) {
    return (
        commandType >=
            static_cast<int32_t>(
                ActionType::EXECUTE_SCENE
            )
        &&
        commandType <=
            static_cast<int32_t>(
                ActionType::SET_GROUP_BRIGHTNESS
            )
    );
}


static void sendAck(
    CommunicationBus& communication,
    const Message& command,
    ExecutionStatus executionStatus,
    bool dropAck
) {
    Message ack = {
        generateMessageId(),

        command.destinationId,
        command.sourceId,

        MessageType::ACK,

        millis(),

        command.commandType,

        static_cast<int32_t>(
            executionStatus
        ),

        static_cast<int32_t>(
            command.id
        ),

        MessageStatus::PENDING,

        executionStatus
    };


    /*
     * Simulation de perte de l'ACK.
     *
     * L'ACK est généré logiquement mais n'est pas
     * placé dans le bus de communication.
     */

    if (dropAck) {

        Serial.println();

        Serial.print(
            "ACK volontairement perdu : "
        );

        Serial.println(
            ack.id
        );

        Serial.print(
            "ACK correspondant au message : "
        );

        Serial.println(
            command.id
        );

        return;
    }


    /*
     * Communication normale.
     */

    sendMessage(
        communication,
        ack
    );

    Serial.println(
        "ACK genere"
    );
}


void processMessages(
    CommunicationBus& communication,
    MessageTracker& tracker,
    MessageDeduplicator& deduplicator,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes,
    bool dropNextAck
) {
    Message message;


    while (
        receiveMessage(
            communication,
            message
        )
    ) {

        Serial.println();

        Serial.println(
            ">>> MESSAGE RECU"
        );

        printMessage(
            message
        );


        /*
         * ====================================================
         * ACK
         * ====================================================
         */

        if (
            message.type ==
            MessageType::ACK
        ) {

            Serial.println(
                ">>> ACK RECU"
            );

            processAck(
                tracker,
                message
            );

            continue;
        }


        /*
         * ====================================================
         * COMMAND
         * ====================================================
         */

        if (
            message.type !=
            MessageType::COMMAND
        ) {
            continue;
        }


        /*
         * ====================================================
         * VALIDATION
         * ====================================================
         */

        if (
            !isValidCommandType(
                message.commandType
            )
        ) {

            Serial.println(
                "CommandType invalide"
            );

            sendAck(
                communication,
                message,
                ExecutionStatus::FAILED,
                dropNextAck
            );

            /*
             * Le flag ne doit être utilisé qu'une fois.
             */

            dropNextAck = false;

            continue;
        }


        /*
         * ====================================================
         * DETECTION DE DOUBLON
         * ====================================================
         */

        ProcessedMessage* processed =
            findProcessedMessage(
                deduplicator,
                message.id
            );


        if (
            processed != nullptr
        ) {

            Serial.println();

            Serial.println(
                ">>> DOUBLON DETECTE"
            );

            Serial.print(
                "Message deja execute : "
            );

            Serial.println(
                message.id
            );

            Serial.print(
                "Execution precedente : "
            );

            Serial.println(
                executionStatusToString(
                    processed->executionStatus
                )
            );


            /*
             * IMPORTANT :
             *
             * On NE réexécute PAS la commande.
             *
             * On renvoie simplement le résultat
             * déjà obtenu.
             */

            sendAck(
                communication,
                message,
                processed->executionStatus,
                false
            );

            continue;
        }


        /*
         * ====================================================
         * CREATION ACTION
         * ====================================================
         */

        Action action = {
            static_cast<ActionType>(
                message.commandType
            ),

            message.destinationId,

            message.value
        };


        /*
         * ====================================================
         * EXECUTION
         * ====================================================
         */

        Serial.println(
            ">>> EXECUTION COMMANDE"
        );


        ExecutionStatus result =
            executeAction(
                action,
                scenes,
                groups,
                lamps
            );


        Serial.print(
            "Resultat execution : "
        );

        Serial.println(
            executionStatusToString(
                result
            )
        );


        /*
         * ====================================================
         * MEMORISATION
         * ====================================================
         */

        registerProcessedMessage(
            deduplicator,
            message.id,
            result
        );


        /*
         * ====================================================
         * ACK
         * ====================================================
         */

        sendAck(
            communication,
            message,
            result,
            dropNextAck
        );


        /*
         * Le flag ne doit être consommé qu'une fois.
         */

        dropNextAck = false;
    }
}