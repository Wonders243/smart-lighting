#include <Arduino.h>

#include "communication.h"

#include "simulation_transport.h"
#include "zigbee_transport.h"


/*
 * ============================================================
 * TRANSPORTS
 * ============================================================
 *
 * Ils vivent pendant toute la durée du programme.
 */

static SimulationTransport simulationTransport;

static ZigbeeTransport zigbeeTransport;


/*
 * ============================================================
 * INIT
 * ============================================================
 */

bool initCommunication(
    Communication& communication,
    CommunicationTransportType type,
    uint32_t localDeviceId
) {

    communication.type =
        type;

    communication.state =
        CommunicationState::INITIALIZING;

    communication.localDeviceId =
        localDeviceId;

    communication.transport =
        nullptr;


    /*
     * --------------------------------------------------------
     * SIMULATION
     * --------------------------------------------------------
     */

    if (
        type ==
        CommunicationTransportType::SIMULATION
    ) {

        communication.transport =
            &simulationTransport;


        if (
            simulationTransport.begin()
        ) {

            communication.state =
                CommunicationState::READY;


            Serial.println(
                "Communication : SIMULATION"
            );

            Serial.println(
                "Communication : READY"
            );


            return true;
        }
    }


    /*
     * --------------------------------------------------------
     * ZIGBEE
     * --------------------------------------------------------
     */

    if (
        type ==
        CommunicationTransportType::ZIGBEE
    ) {

        communication.transport =
            &zigbeeTransport;


        if (
            zigbeeTransport.begin()
        ) {

            communication.state =
                CommunicationState::READY;


            Serial.println(
                "Communication : ZIGBEE"
            );

            Serial.println(
                "Communication : READY"
            );


            return true;
        }


        communication.state =
            CommunicationState::ERROR;


        return false;
    }


    communication.state =
        CommunicationState::ERROR;


    return false;
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
        communication.transport ==
        nullptr
    ) {
        return false;
    }


    if (
        !communicationReady(
            communication
        )
    ) {
        return false;
    }


    /*
     * Timestamp automatique.
     */

    if (
        message.timestamp == 0
    ) {

        message.timestamp =
            millis();
    }


    message.status =
        MessageStatus::SENT;


    return communication.transport->send(
        message
    );
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
        communication.transport ==
        nullptr
    ) {
        return false;
    }


    if (
        !communicationReady(
            communication
        )
    ) {
        return false;
    }


    return communication.transport->receive(
        message
    );
}


/*
 * ============================================================
 * READY
 * ============================================================
 */

bool communicationReady(
    const Communication& communication
) {

    return (

        communication.state ==
        CommunicationState::READY

        &&

        communication.transport !=
        nullptr

        &&

        communication.transport->isReady()
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


    if (
        communication.transport !=
        nullptr
    ) {

        Serial.println(
            communication.transport->name()
        );

    }
    else {

        Serial.println(
            "NONE"
        );
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


    Serial.println(
        "========================="
    );
}