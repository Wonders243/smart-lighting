#pragma once

#include <stdint.h>

class FakeHardwareSerial {
public:
    template <typename T>
    void print(const T&) {}

    template <typename T>
    void println(const T&) {}

    void println() {}
};

extern FakeHardwareSerial Serial;

uint32_t millis();
