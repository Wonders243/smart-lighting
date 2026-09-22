#include <Arduino.h>

#include "message_id_generator.h"

uint32_t generateMessageId() {

    static uint32_t nextId = 1;

    return nextId++;
}