#include <Arduino.h>

#include "zigbee_transport.h"


ZigbeeTransport::ZigbeeTransport()
    : ready(false) {
}


/*
 * ============================================================
 * BEGIN
 * ============================================================
 */

bool ZigbeeTransport::begin() {

    /*
     * V3.9 :
     *
     * Pas encore d'initialisation réelle.
     *
     * La vraie initialisation nécessitera :
     *
     * - ESP32-C6 ou ESP32-H2
     * - stack ESP-Zigbee
     * - rôle Zigbee
     * - configuration réseau
     * - endpoint
     */

    ready = false;


    Serial.println(
        "ZigbeeTransport : PREPARED"
    );


    Serial.println(
        "ZigbeeTransport : hardware integration pending"
    );


    return false;
}


/*
 * ============================================================
 * SEND
 * ============================================================
 */

bool ZigbeeTransport::send(
    const Message& message
) {

    (void)message;


    Serial.println(
        "ZigbeeTransport : send() non implemente"
    );


    return false;
}


/*
 * ============================================================
 * RECEIVE
 * ============================================================
 */

bool ZigbeeTransport::receive(
    Message& message
) {

    (void)message;


    return false;
}


/*
 * ============================================================
 * STATUS
 * ============================================================
 */

bool ZigbeeTransport::isReady() const {

    return ready;
}


const char*
ZigbeeTransport::name() const {

    return "ZIGBEE";
}