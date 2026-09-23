#include <Arduino.h>

#include "simulation_transport.h"


SimulationTransport::SimulationTransport()
    : head(0),
      tail(0),
      count(0),
      ready(false) {
}


/*
 * ============================================================
 * BEGIN
 * ============================================================
 */

bool SimulationTransport::begin() {

    head = 0;

    tail = 0;

    count = 0;

    ready = true;


    Serial.println(
        "SimulationTransport : READY"
    );


    return true;
}


/*
 * ============================================================
 * SEND
 * ============================================================
 */

bool SimulationTransport::send(
    const Message& message
) {

    if (!ready) {
        return false;
    }


    if (full()) {

        Serial.println(
            "SimulationTransport : queue pleine"
        );

        return false;
    }


    messages[tail] =
        message;


    tail =
        (
            tail + 1
        ) % MAX_SIMULATION_MESSAGES;


    count++;


    return true;
}


/*
 * ============================================================
 * RECEIVE
 * ============================================================
 */

bool SimulationTransport::receive(
    Message& message
) {

    if (!ready) {
        return false;
    }


    if (empty()) {
        return false;
    }


    message =
        messages[head];


    head =
        (
            head + 1
        ) % MAX_SIMULATION_MESSAGES;


    count--;


    return true;
}


/*
 * ============================================================
 * STATUS
 * ============================================================
 */

bool SimulationTransport::isReady() const {

    return ready;
}


const char*
SimulationTransport::name() const {

    return "SIMULATION";
}


/*
 * ============================================================
 * QUEUE
 * ============================================================
 */

bool SimulationTransport::empty() const {

    return count == 0;
}


bool SimulationTransport::full() const {

    return count >=
        MAX_SIMULATION_MESSAGES;
}


uint8_t SimulationTransport::size() const {

    return count;
}