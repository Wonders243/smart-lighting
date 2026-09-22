#include <Arduino.h>

#include "message_router.h"
#include "message_manager.h"
#include "action.h"
#include "action_executor.h"
#include "message_id_generator.h"


static bool isValidActionType(
    int32_t commandType
) {

    return
        commandType >= static_cast<int32_t>(
            ActionType::EXECUTE_SCENE
        )
        &&
        commandType <= static_cast<int32_t>(
            ActionType::SET_GROUP_BRIGHTNESS
        );
}


static bool executeCommandMessage(
    const Message& message,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
) {

    if (!isValidActionType(message.commandType)) {

        Serial.println(
            "Type de commande invalide"
        );

        return false;
    }


    Action action = {
        static_cast<ActionType>(
            message.commandType
        ),

        message.destinationId,

        message.value
    };


    return executeAction(
        action,
        scenes,
        groups,
        lamps
    );
}


static Message createAck(
    const Message& command,
    bool success
) {

    Message ack = {

        generateMessageId(),

        command.destinationId,
        command.sourceId,

        MessageType::ACK,

        millis(),

        command.commandType,

        success ? 1 : 0,

        static_cast<int32_t>(
            command.id
        ),

        MessageStatus::PENDING
    };

    return ack;
}


void processMessages(
    CommunicationBus& communication,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
) {

    Message message;


    while (receiveMessage(
        communication,
        message
    )) {

        Serial.println();
        Serial.println(
            ">>> MESSAGE RECU"
        );

        printMessage(message);


        // ====================================================
        // COMMAND
        // ====================================================

        if (message.type == MessageType::COMMAND) {

            Serial.println(
                ">>> EXECUTION COMMANDE"
            );


            bool success =
                executeCommandMessage(
                    message,
                    lamps,
                    groups,
                    scenes
                );


            Message ack =
                createAck(
                    message,
                    success
                );


            if (success) {

                Serial.println(
                    "Commande executee"
                );

                message.status =
                    MessageStatus::DELIVERED;
            }

            else {

                Serial.println(
                    "Echec execution commande"
                );

                message.status =
                    MessageStatus::FAILED;
            }


            // ------------------------------------------------
            // ACK
            // ------------------------------------------------

            if (sendMessage(
                communication,
                ack
            )) {

                Serial.println(
                    "ACK genere"
                );
            }
        }


        // ====================================================
        // ACK
        // ====================================================

        else if (
            message.type == MessageType::ACK
        ) {

            Serial.println(
                ">>> ACK RECU"
            );

            Serial.print(
                "Message original : "
            );

            Serial.println(
                message.value2
            );


            if (message.value == 1) {

                Serial.println(
                    "Resultat : SUCCES"
                );
            }

            else {

                Serial.println(
                    "Resultat : ECHEC"
                );
            }
        }


        // ====================================================
        // AUTRES MESSAGES
        // ====================================================

        else {

            Serial.println(
                "Message non executable"
            );
        }
    }
}