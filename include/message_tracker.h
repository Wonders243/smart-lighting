#pragma once

#include <Arduino.h>

#include "message.h"
#include "communication.h"

constexpr uint8_t MAX_PENDING_MESSAGES = 20;

constexpr uint32_t MESSAGE_TIMEOUT = 5000;

constexpr uint8_t MAX_MESSAGE_RETRIES = 2;

struct PendingMessage {
    Message message;

    bool waitingForAck;
    bool completed;
    bool timedOut;

    uint8_t retryCount;

    uint32_t sentAt;
    uint32_t completedAt;
};

struct MessageTracker {
    PendingMessage messages[MAX_PENDING_MESSAGES];
    uint8_t count;
};

void initMessageTracker(
    MessageTracker& tracker
);

bool trackMessage(
    MessageTracker& tracker,
    const Message& message
);

PendingMessage* findPendingMessage(
    MessageTracker& tracker,
    uint32_t messageId
);

bool processAck(
    MessageTracker& tracker,
    const Message& ack
);

/*
 * Vérifie les timeouts et indique
 * quels messages doivent être renvoyés.
 *
 * Les messages à retransmettre sont
 * replacés dans le CommunicationBus
 * par le routeur / main.
 */
bool updateMessageTimeouts(
    MessageTracker& tracker,
    CommunicationBus& communication
);

void printPendingMessage(
    const PendingMessage& message
);

void printMessageTracker(
    const MessageTracker& tracker
);