#include <Arduino.h>

#include "message_router.h"
#include "action.h"
#include "action_executor.h"


static bool executeCommandMessage(
    const Message& message,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
) {
    Action action = {
        static_cast<ActionType>(message.commandType),
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


static void printReceivedMessage(const Message& message) {

    Serial.println("===== MESSAGE =====");

    Serial.print("ID : ");
    Serial.println(message.id);

    Serial.print("Source : ");
    Serial.println(message.sourceId);

    Serial.print("Destination : ");
    Serial.println(message.destinationId);

    Serial.print("Type : ");

    switch (message.type) {

        case MessageType::COMMAND:
            Serial.println("COMMAND");
            break;

        case MessageType::EVENT:
            Serial.println("EVENT");
            break;

        case MessageType::STATE:
            Serial.println("STATE");
            break;

        case MessageType::HEARTBEAT:
            Serial.println("HEARTBEAT");
            break;

        case MessageType::ACK:
            Serial.println("ACK");
            break;
    }

    Serial.print("Timestamp : ");
    Serial.println(message.timestamp);

    Serial.print("Command : ");
    Serial.println(message.commandType);

    Serial.print("Value : ");
    Serial.println(message.value);

    Serial.print("Value 2 : ");
    Serial.println(message.value2);

    Serial.print("Status : ");

    switch (message.status) {

        case MessageStatus::PENDING:
            Serial.println("PENDING");
            break;

        case MessageStatus::SENT:
            Serial.println("SENT");
            break;

        case MessageStatus::DELIVERED:
            Serial.println("DELIVERED");
            break;

        case MessageStatus::FAILED:
            Serial.println("FAILED");
            break;
    }

    Serial.println("===================");
}


void processMessages(
    CommunicationBus& communication,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
) {
    Message message;

    while (receiveMessage(communication, message)) {

        Serial.println();
        Serial.println(">>> MESSAGE RECU");

        printReceivedMessage(message);

        if (message.type == MessageType::COMMAND) {

            Serial.println(">>> EXECUTION COMMANDE");

            bool success = executeCommandMessage(
                message,
                lamps,
                groups,
                scenes
            );

            if (success) {
                Serial.println("Commande executee");
            }
            else {
                Serial.println("Echec execution commande");
            }
        }
        else {
            Serial.println("Message non executable");
        }

        Serial.println();
    }
}