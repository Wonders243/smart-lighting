#pragma once

#include <Arduino.h>

#include "automation.h"
#include "scene_manager.h"


struct AutomationRegistry {
    Automation automations[MAX_AUTOMATIONS];
    uint8_t count;
};


void initAutomationRegistry(
    AutomationRegistry& registry
);


bool addAutomation(
    AutomationRegistry& registry,
    const Automation& automation
);


bool removeAutomation(
    AutomationRegistry& registry,
    uint32_t automationId
);


Automation* findAutomation(
    AutomationRegistry& registry,
    uint32_t automationId
);


bool addConditionToAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    const AutomationCondition& condition
);


bool removeConditionFromAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    uint8_t conditionIndex
);


bool addActionToAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    const AutomationAction& action
);


bool removeActionFromAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    uint8_t actionIndex
);


void printAutomation(
    const Automation& automation
);


void printAutomationRegistry(
    const AutomationRegistry& registry
);