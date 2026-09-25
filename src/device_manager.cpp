#include <Arduino.h>

#include "device_manager.h"
#include "event_bus.h"

static uint32_t nextDeviceEventId = 1;

static void publishDeviceStatusEvent(
    EventBus* eventBus,
    EventType type,
    const Device& device,
    uint32_t timestamp
) {
    if (eventBus == nullptr) {
        return;
    }

    Event event = {
        nextDeviceEventId++,
        type,
        device.id,
        device.status == DeviceStatus::ONLINE ? 1 : 0,
        0,
        timestamp
    };

    publishEvent(*eventBus, event);
}

void updateDeviceSeen(
    Device& device,
    EventBus* eventBus
) {
    const bool wasOffline = device.status == DeviceStatus::OFFLINE;
    const uint32_t now = millis();

    device.lastSeen = now;

    device.status =
        DeviceStatus::ONLINE;

    if (wasOffline) {
        publishDeviceStatusEvent(
            eventBus,
            EventType::LAMP_ONLINE,
            device,
            now
        );
    }
}


void updateDeviceStatus(
    LampRegistry& registry,
    EventBus* eventBus
) {

    uint32_t now = millis();


    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        Device& device =
            registry.lamps[i].device;


        if (
            device.status ==
            DeviceStatus::ONLINE
        ) {

            if (
                now - device.lastSeen >
                DEVICE_TIMEOUT
            ) {

                device.status =
                    DeviceStatus::OFFLINE;

                publishDeviceStatusEvent(
                    eventBus,
                    EventType::LAMP_OFFLINE,
                    device,
                    now
                );


                Serial.print(
                    "Device OFFLINE : "
                );

                Serial.println(
                    device.name
                );
            }
        }
    }
}
