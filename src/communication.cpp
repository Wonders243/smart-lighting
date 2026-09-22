#include <Arduino.h>

#include "communication.h"


void initCommunication(
    CommunicationBus& communication
) {
    communication.head = 0;
    communication.tail = 0;
    communication.count = 0;

    Serial.println("Communication initialise");
}


bool sendMessage(
    CommunicationBus& communication,
    Message message
) {

    if (communicationFull(communication)) {
        Serial.println("Communication pleine");
        return false;
    }

    message.status = MessageStatus::SENT;

    communication.messages[
        communication.tail
    ] = message;

    communication.tail++;

    if (communication.tail >= MAX_MESSAGES) {
        communication.tail = 0;
    }

    communication.count++;

    Serial.println("Message envoye");

    return true;
}


bool receiveMessage(
    CommunicationBus& communication,
    Message& message
) {

    if (communicationEmpty(communication)) {
        return false;
    }

    message = communication.messages[
        communication.head
    ];

    communication.head++;

    if (communication.head >= MAX_MESSAGES) {
        communication.head = 0;
    }

    communication.count--;

    return true;
}


bool communicationEmpty(
    const CommunicationBus& communication
) {
    return communication.count == 0;
}


bool communicationFull(
    const CommunicationBus& communication
) {
    return communication.count >= MAX_MESSAGES;
}


void printCommunicationStatus(
    const CommunicationBus& communication
) {

    Serial.println("===== COMMUNICATION =====");

    Serial.print("Messages : ");
    Serial.println(communication.count);

    Serial.print("Head     : ");
    Serial.println(communication.head);

    Serial.print("Tail     : ");
    Serial.println(communication.tail);

    Serial.println("=========================");
}