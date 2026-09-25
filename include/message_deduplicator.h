#pragma once

#include <Arduino.h>

#include "message.h"

constexpr uint8_t MAX_PROCESSED_MESSAGES = 20;

struct ProcessedMessage {
    uint32_t sourceId;
    uint32_t messageId;

    ExecutionStatus executionStatus;

    uint32_t processedAt;

    bool valid;
};

struct MessageDeduplicator {
    ProcessedMessage messages[MAX_PROCESSED_MESSAGES];

    uint8_t count;
    uint8_t nextIndex;
};

void initMessageDeduplicator(
    MessageDeduplicator& deduplicator
);

ProcessedMessage* findProcessedMessage(
    MessageDeduplicator& deduplicator,
    uint32_t sourceId,
    uint32_t messageId
);

bool isMessageProcessed(
    MessageDeduplicator& deduplicator,
    uint32_t sourceId,
    uint32_t messageId
);

bool registerProcessedMessage(
    MessageDeduplicator& deduplicator,
    uint32_t sourceId,
    uint32_t messageId,
    ExecutionStatus executionStatus
);

void printMessageDeduplicator(
    const MessageDeduplicator& deduplicator
);
