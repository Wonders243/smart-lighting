#pragma once

#include <Arduino.h>

#include "communication_transport.h"


enum class CommunicationTransportType {

    SIMULATION,

    ZIGBEE
};


enum class CommunicationState {

    INITIALIZING,

    READY,

    ERROR
};


struct Communication {

    CommunicationTransportType type;

    CommunicationState state;

    uint32_t localDeviceId;

    CommunicationTransportInterface*
        transport;
};


/*
 * Initialisation.
 */
bool initCommunication(
    Communication& communication,
    CommunicationTransportType type,
    uint32_t localDeviceId
);


/*
 * Envoi.
 */
bool sendMessage(
    Communication& communication,
    Message message
);


/*
 * Réception.
 */
bool receiveMessage(
    Communication& communication,
    Message& message
);


/*
 * Etat.
 */
bool communicationReady(
    const Communication& communication
);


/*
 * Debug.
 */
void printCommunicationStatus(
    const Communication& communication
);