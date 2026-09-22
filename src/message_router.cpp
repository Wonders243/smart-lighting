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
        commandType >=
            static_cast<int32_t>(
                ActionType::EXECUTE_SCENE
            )
        &&
        commandType <=
            static_cast<int32_t>(
                ActionType::SET_GROUP_BRIGHTNESS
            );
}

static ExecutionStatus executeCommandMessage(
    const Message& message,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
) {
    if (!isValidActionType(message.commandType)) {
        Serial.println(
            "Type de commande invalide"
        );

        return ExecutionStatus::FAILED;
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
    ExecutionStatus executionStatus
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

    return ack;
}

void processMessages(
    CommunicationBus& communication,
    MessageTracker& tracker,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
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

        printMessage(message);

        /*
         * COMMAND
         */
        if (
            message.type ==
            MessageType::COMMAND
        ) {
            Serial.println(
                ">>> EXECUTION COMMANDE"
            );

            ExecutionStatus result =
                executeCommandMessage(
                    message,
                    lamps,
                    groups,
                    scenes
                );

            Serial.print(
                "Resultat execution : "
            );

            Serial.println(
                executionStatusToString(
                    result
                )
            );

            Message ack =
                createAck(
                    message,
                    result
                );

            if (
                sendMessage(
                    communication,
                    ack
                )
            ) {
                Serial.println(
                    "ACK genere"
                );
            }
        }

        /*
         * ACK
         */
        else if (
            message.type ==
            MessageType::ACK
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

            Serial.print(
                "Resultat : "
            );

            Serial.println(
                executionStatusToString(
                    message.executionStatus
                )
            );

            processAck(
                tracker,
                message
            );
        }

        /*
         * AUTRES MESSAGES
         */
        else {
            Serial.println(
                "Message non executable"
            );
        }

        Serial.println();
    }
}