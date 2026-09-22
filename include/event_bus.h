#pragma once

#include <Arduino.h>

#include "event.h"

constexpr uint8_t MAX_EVENTS = 20;

struct EventBus {
    Event events[MAX_EVENTS];

    uint8_t head;
    uint8_t tail;
    uint8_t count;
};

void initEventBus(EventBus& bus);

bool publishEvent(
    EventBus& bus,
    const Event& event
);

bool consumeEvent(
    EventBus& bus,
    Event& event
);

bool isEventBusEmpty(
    const EventBus& bus
);

bool isEventBusFull(
    const EventBus& bus
);

void printEvent(
    const Event& event
);

void printEventBus(
    const EventBus& bus
);