#pragma once

#include <Arduino.h>

#include "automation_manager.h"
#include "scene_executor.h"
#include "group_manager.h"
#include "device_registry.h"


struct AutomationContext {
    uint16_t lightLevel;
    bool presenceDetected;
    uint16_t hour;
    uint16_t minute;
};

bool evaluateCondition(
    const AutomationCondition& condition,
    const AutomationContext& context
);

bool evaluateAutomation(
    const Automation& automation,
    const AutomationContext& context
);

void processAutomations(
    AutomationRegistry& automations,
    SceneRegistry& scenes,
    GroupRegistry& groups,
    LampRegistry& lamps,
    const AutomationContext& context
);