#include <Arduino.h>

#include "communication.h"


/*
 * ============================================================
 * SIMULATION TRANSPORT
 * ============================================================
 *
 * Pour V3.8, le transport de simulation utilise une
 * file circulaire.
 *
 * Plus tard :
 *
 * CommunicationTransport::ZIGBEE
 *
 * utilisera la stack Zigbee à la place de cette file.
 */


/*
 * ------------------------------------------------------------
 * Queue interne
 * ------------------------------------------------------------
 */

static bool queueSend(
    CommunicationQueue& queue,
    Message message
) {
    if (
        queue.count >=
        MAX_MESSAGES
    ) {
        return false;
    }


    queue.messages[
        queue.tail
    ] = message;


    queue.tail =
        (
            queue.tail + 1
        ) % MAX_MESSAGES;


    queue.count++;

    return true;
}


static bool queueReceive(
    CommunicationQueue& queue,
    Message& message
) {
    if (
        queue.count == 0
    ) {
        return false;
    }


    message =
        queue.messages[
            queue.head
        ];


    queue.head =
        (
            queue.head + 1
        ) % MAX_MESSAGES;


    queue.count--;

    return true;
}


/*
 * ============================================================
 * INITIALISATION
 * ============================================================
 */

void initCommunication(
    Communication& communication,
    CommunicationTransport transport,
    uint32_t localDeviceId
) {
    communication.transport =
        transport;

    communication.state =
        CommunicationState::INITIALIZING;

    communication.localDeviceId =
        localDeviceId;


    communication.queue.head = 0;
    communication.queue.tail = 0;
    communication.queue.count = 0;


    /*
     * V3.8 :
     *
     * seule la simulation est implémentée.
     */

    if (
        transport ==
        CommunicationTransport::SIMULATION
    ) {

        communication.state =
            CommunicationState::READY;

        Serial.println(
            "Communication : SIMULATION"
        );

        Serial.println(
            "Communication : READY"
        );

        return;
    }


    /*
     * Zigbee n'est pas encore implémenté.
     */

    if (
        transport ==
        CommunicationTransport::ZIGBEE
    ) {

        communication.state =
            CommunicationState::ERROR;

        Serial.println(
            "Communication : ZIGBEE"
        );

        Serial.println(
            "Zigbee non implemente dans V3.8"
        );

        return;
    }


    communication.state =
        CommunicationState::ERROR;
}


/*
 * ============================================================
 * SEND
 * ============================================================
 */

bool sendMessage(
    Communication& communication,
    Message message
) {
    if (
        communication.state !=
        CommunicationState::READY
    ) {
        Serial.println(
            "Communication non disponible"
        );

        return false;
    }


    /*
     * Le timestamp est ajouté automatiquement
     * si l'appelant ne l'a pas défini.
     */

    if (
        message.timestamp == 0
    ) {
        message.timestamp =
            millis();
    }


    /*
     * Etat initial.
     */

    message.status =
        MessageStatus::SENT;


    /*
     * Transport de simulation.
     */

    if (
        communication.transport ==
        CommunicationTransport::SIMULATION
    ) {

        if (
            !queueSend(
                communication.queue,
                message
            )
        ) {

            Serial.println(
                "Communication : queue pleine"
            );

            return false;
        }


        Serial.print(
            "Message envoye : "
        );

        Serial.println(
            message.id
        );

        return true;
    }


    /*
     * Zigbee futur.
     */

    if (
        communication.transport ==
        CommunicationTransport::ZIGBEE
    ) {

        Serial.println(
            "Erreur : transport Zigbee non disponible"
        );

        return false;
    }


    return false;
}


/*
 * ============================================================
 * RECEIVE
 * ============================================================
 */

bool receiveMessage(
    Communication& communication,
    Message& message
) {
    if (
        communication.state !=
        CommunicationState::READY
    ) {
        return false;
    }


    /*
     * Simulation.
     */

    if (
        communication.transport ==
        CommunicationTransport::SIMULATION
    ) {

        return queueReceive(
            communication.queue,
            message
        );
    }


    /*
     * Zigbee futur.
     */

    if (
        communication.transport ==
        CommunicationTransport::ZIGBEE
    ) {
        return false;
    }


    return false;
}


/*
 * ============================================================
 * STATUS
 * ============================================================
 */

bool communicationEmpty(
    const Communication& communication
) {
    return (
        communication.queue.count ==
        0
    );
}


bool communicationFull(
    const Communication& communication
) {
    return (
        communication.queue.count >=
        MAX_MESSAGES
    );
}


bool communicationReady(
    const Communication& communication
) {
    return (
        communication.state ==
        CommunicationState::READY
    );
}


/*
 * ============================================================
 * DEBUG
 * ============================================================
 */

void printCommunicationStatus(
    const Communication& communication
) {
    Serial.println();

    Serial.println(
        "===== COMMUNICATION ====="
    );


    Serial.print(
        "Transport : "
    );


    switch (
        communication.transport
    ) {

        case CommunicationTransport::SIMULATION:

            Serial.println(
                "SIMULATION"
            );

            break;


        case CommunicationTransport::ZIGBEE:

            Serial.println(
                "ZIGBEE"
            );

            break;
    }


    Serial.print(
        "Etat : "
    );


    switch (
        communication.state
    ) {

        case CommunicationState::INITIALIZING:

            Serial.println(
                "INITIALIZING"
            );

            break;


        case CommunicationState::READY:

            Serial.println(
                "READY"
            );

            break;


        case CommunicationState::ERROR:

            Serial.println(
                "ERROR"
            );

            break;
    }


    Serial.print(
        "Device local : "
    );

    Serial.println(
        communication.localDeviceId
    );


    Serial.print(
        "Messages en attente : "
    );

    Serial.println(
        communication.queue.count
    );


    Serial.println(
        "========================="
    );
}