#pragma once

#include <Arduino.h>
#include "action.h"

constexpr uint8_t MAX_AUTOMATIONS = 10;
constexpr uint8_t MAX_AUTOMATION_CONDITIONS = 5;
constexpr uint8_t MAX_AUTOMATION_ACTIONS = 5;

enum class AutomationConditionType {
    LIGHT_LEVEL,
    PRESENCE,
    TIME
};

enum class AutomationOperator {
    LESS_THAN,
    LESS_OR_EQUAL,
    GREATER_THAN,
    GREATER_OR_EQUAL,
    EQUAL
};

enum class AutomationLogic {
    AND,
    OR
};

enum class AutomationTriggerMode {
    ONCE,
    REPEAT
};

struct AutomationCondition {
    AutomationConditionType type;
    AutomationOperator op;
    int32_t value;
};

struct AutomationAction {
    ActionType type;
    uint32_t targetId;
    int32_t value;
};
struct Automation {
    uint32_t id;
    const char* name;

    bool enabled;

    AutomationCondition conditions[
        MAX_AUTOMATION_CONDITIONS
    ];

    uint8_t conditionCount;

    AutomationLogic logic;

    // =====================================
    // ACTIONS
    // =====================================

    AutomationAction actions[
        MAX_AUTOMATION_ACTIONS
    ];

    uint8_t actionCount;

    uint32_t sceneId;

    // =====================================
    // ETAT
    // =====================================

    bool conditionState;

    uint32_t conditionSince;

    uint32_t lastTriggered;

    // =====================================
    // TEMPORISATION
    // =====================================

    uint32_t triggerDelayMs;

    uint32_t cooldownMs;

    AutomationTriggerMode triggerMode;
};