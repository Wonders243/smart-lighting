#pragma once

#include <Arduino.h>

#if defined(DEVICE_LAMP_A) && defined(DEVICE_LAMP_B)
    #error "Un seul identifiant de lampe peut être défini"
#endif

#if defined(DEVICE_LAMP_A)
    constexpr uint32_t DEVICE_LOCAL_ID = 1;
    constexpr const char* DEVICE_LOCAL_NAME = "LAMP_A";
#elif defined(DEVICE_LAMP_B)
    constexpr uint32_t DEVICE_LOCAL_ID = 2;
    constexpr const char* DEVICE_LOCAL_NAME = "LAMP_B";
#else
    constexpr uint32_t DEVICE_LOCAL_ID = 0;
    constexpr const char* DEVICE_LOCAL_NAME = "UNASSIGNED";
#endif
