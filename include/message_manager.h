#pragma once

#include <Arduino.h>

#include "message.h"

void printMessage(
    const Message& message
);

const char* messageTypeToString(
    MessageType type
);

const char* messageStatusToString(
    MessageStatus status
);