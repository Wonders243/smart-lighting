#include <Arduino.h>

#include "id_generator.h"

uint32_t generateEventId() {
    static uint32_t nextId = 1;

    return nextId++;
}