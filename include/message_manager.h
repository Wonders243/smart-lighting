#pragma once

#include <Arduino.h>

#include "message.h"


const char* messageTypeToString(
    MessageType type
);


const char* messageStatusToString(
    MessageStatus status
);


const char* executionStatusToString(
    ExecutionStatus status
);


void printMessage(
    const Message& message
);