#pragma once

#include <Arduino.h>
#include "message.h"

constexpr uint8_t MAX_MESSAGES = 20;

struct CommunicationBus {

    Message messages[MAX_MESSAGES];

    uint8_t head;
    uint8_t tail;
    uint8_t count;
};


void initCommunication(
    CommunicationBus& communication
);


bool sendMessage(
    CommunicationBus& communication,
    Message message
);


bool receiveMessage(
    CommunicationBus& communication,
    Message& message
);


bool communicationEmpty(
    const CommunicationBus& communication
);


bool communicationFull(
    const CommunicationBus& communication
);


void printCommunicationStatus(
    const CommunicationBus& communication
);