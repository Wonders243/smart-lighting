#pragma once

#include <Arduino.h>

#include "message.h"

constexpr uint8_t MAX_MESSAGES = 20;

/*
 * Transport utilisé par la couche communication.
 *
 * V3.8 :
 * - SIMULATION est implémenté.
 * - ZIGBEE est réservé pour la prochaine étape.
 */
enum class CommunicationTransport {
    SIMULATION,
    ZIGBEE
};


/*
 * Etat de la communication.
 */
enum class CommunicationState {
    INITIALIZING,
    READY,
    ERROR
};


/*
 * File interne utilisée par le transport de simulation.
 *
 * Elle permet de tester toute la couche applicative
 * sans avoir encore de matériel Zigbee.
 */
struct CommunicationQueue {

    Message messages[MAX_MESSAGES];

    uint8_t head;
    uint8_t tail;
    uint8_t count;
};


/*
 * Couche de communication.
 *
 * Le reste du programme manipule uniquement cette structure.
 *
 * Le jour où Zigbee est intégré, son implémentation
 * pourra remplacer la simulation sans modifier :
 *
 * - MessageTracker
 * - MessageRouter
 * - Scenes
 * - Automations
 * - EventBus
 * - DeviceManager
 */
struct Communication {

    CommunicationTransport transport;

    CommunicationState state;

    uint32_t localDeviceId;

    CommunicationQueue queue;
};


/*
 * Initialisation.
 */
void initCommunication(
    Communication& communication,
    CommunicationTransport transport =
        CommunicationTransport::SIMULATION,
    uint32_t localDeviceId = 0
);


/*
 * Envoi d'un message.
 */
bool sendMessage(
    Communication& communication,
    Message message
);


/*
 * Réception d'un message.
 */
bool receiveMessage(
    Communication& communication,
    Message& message
);


/*
 * Informations sur la file.
 */
bool communicationEmpty(
    const Communication& communication
);

bool communicationFull(
    const Communication& communication
);


/*
 * Etat général.
 */
bool communicationReady(
    const Communication& communication
);


/*
 * Informations de debug.
 */
void printCommunicationStatus(
    const Communication& communication
);