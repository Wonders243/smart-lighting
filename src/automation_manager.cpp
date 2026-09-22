#include <Arduino.h>

#include "automation_manager.h"


void initAutomationRegistry(
    AutomationRegistry& registry
) {
    registry.count = 0;
}


Automation* findAutomation(
    AutomationRegistry& registry,
    uint32_t automationId
) {
    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {
        if (
            registry.automations[i].id ==
            automationId
        ) {
            return &registry.automations[i];
        }
    }

    return nullptr;
}


bool addAutomation(
    AutomationRegistry& registry,
    const Automation& automation
) {
    if (
        registry.count >=
        MAX_AUTOMATIONS
    ) {
        Serial.println(
            "Nombre maximum d'automatisations atteint"
        );

        return false;
    }

    if (
        findAutomation(
            registry,
            automation.id
        ) != nullptr
    ) {
        Serial.print(
            "Automatisation deja existante : "
        );

        Serial.println(
            automation.id
        );

        return false;
    }

    registry.automations[
        registry.count
    ] = automation;

    registry.count++;

    Serial.print(
        "Automatisation ajoutee : "
    );

    Serial.println(
        automation.name
    );

    return true;
}


bool removeAutomation(
    AutomationRegistry& registry,
    uint32_t automationId
) {
    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {
        if (
            registry.automations[i].id ==
            automationId
        ) {
            for (
                uint8_t j = i;
                j < registry.count - 1;
                j++
            ) {
                registry.automations[j] =
                    registry.automations[j + 1];
            }

            registry.count--;

            Serial.print(
                "Automatisation supprimee : "
            );

            Serial.println(
                automationId
            );

            return true;
        }
    }

    return false;
}


bool addConditionToAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    const AutomationCondition& condition
) {
    Automation* automation =
        findAutomation(
            registry,
            automationId
        );

    if (automation == nullptr) {
        Serial.println(
            "Automatisation introuvable"
        );

        return false;
    }

    if (
        automation->conditionCount >=
        MAX_AUTOMATION_CONDITIONS
    ) {
        Serial.println(
            "Nombre maximum de conditions atteint"
        );

        return false;
    }

    automation->conditions[
        automation->conditionCount
    ] = condition;

    automation->conditionCount++;

    Serial.print(
        "Condition ajoutee a : "
    );

    Serial.println(
        automation->name
    );

    return true;
}


bool removeConditionFromAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    uint8_t conditionIndex
) {
    Automation* automation =
        findAutomation(
            registry,
            automationId
        );

    if (automation == nullptr) {
        return false;
    }

    if (
        conditionIndex >=
        automation->conditionCount
    ) {
        return false;
    }

    for (
        uint8_t i = conditionIndex;
        i < automation->conditionCount - 1;
        i++
    ) {
        automation->conditions[i] =
            automation->conditions[i + 1];
    }

    automation->conditionCount--;

    return true;
}


bool addActionToAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    const AutomationAction& action
) {
    Automation* automation =
        findAutomation(
            registry,
            automationId
        );

    if (automation == nullptr) {
        Serial.println(
            "Automatisation introuvable"
        );

        return false;
    }

    if (
        automation->actionCount >=
        MAX_AUTOMATION_ACTIONS
    ) {
        Serial.println(
            "Nombre maximum d'actions atteint"
        );

        return false;
    }

    automation->actions[
        automation->actionCount
    ] = action;

    automation->actionCount++;

    Serial.print(
        "Action ajoutee a : "
    );

    Serial.println(
        automation->name
    );

    return true;
}


bool removeActionFromAutomation(
    AutomationRegistry& registry,
    uint32_t automationId,
    uint8_t actionIndex
) {
    Automation* automation =
        findAutomation(
            registry,
            automationId
        );

    if (automation == nullptr) {
        return false;
    }

    if (
        actionIndex >=
        automation->actionCount
    ) {
        return false;
    }

    for (
        uint8_t i = actionIndex;
        i < automation->actionCount - 1;
        i++
    ) {
        automation->actions[i] =
            automation->actions[i + 1];
    }

    automation->actionCount--;

    return true;
}


void printAutomation(
    const Automation& automation
) {
    Serial.println();
    Serial.println(
        "===== AUTOMATISATION ====="
    );

    Serial.print(
        "ID          : "
    );

    Serial.println(
        automation.id
    );

    Serial.print(
        "Nom         : "
    );

    Serial.println(
        automation.name
    );

    Serial.print(
        "Etat        : "
    );

    Serial.println(
        automation.enabled
            ? "ACTIVE"
            : "DESACTIVE"
    );

    Serial.print(
        "Logique     : "
    );

    Serial.println(
        automation.logic ==
            AutomationLogic::AND
            ? "AND"
            : "OR"
    );

    Serial.print(
        "Conditions  : "
    );

    Serial.println(
        automation.conditionCount
    );

    for (
        uint8_t i = 0;
        i < automation.conditionCount;
        i++
    ) {
        const AutomationCondition&
            condition =
                automation.conditions[i];

        Serial.print(
            "  Condition #"
        );

        Serial.print(i);

        Serial.print(
            " : "
        );

        switch (condition.type) {

            case AutomationConditionType::LIGHT_LEVEL:
                Serial.print(
                    "LUMINOSITE"
                );
                break;

            case AutomationConditionType::PRESENCE:
                Serial.print(
                    "PRESENCE"
                );
                break;

            case AutomationConditionType::TIME:
                Serial.print(
                    "HEURE"
                );
                break;
        }

        Serial.print(" ");

        switch (condition.op) {

            case AutomationOperator::LESS_THAN:
                Serial.print("<");
                break;

            case AutomationOperator::LESS_OR_EQUAL:
                Serial.print("<=");
                break;

            case AutomationOperator::GREATER_THAN:
                Serial.print(">");
                break;

            case AutomationOperator::GREATER_OR_EQUAL:
                Serial.print(">=");
                break;

            case AutomationOperator::EQUAL:
                Serial.print("=");
                break;
        }

        if (
            condition.type ==
            AutomationConditionType::TIME
        ) {
            uint32_t minutes =
                condition.value;

            uint32_t hour =
                minutes / 60;

            uint32_t minute =
                minutes % 60;

            if (hour < 10) {
                Serial.print("0");
            }

            Serial.print(hour);

            Serial.print(":");

            if (minute < 10) {
                Serial.print("0");
            }

            Serial.print(minute);
        }
        else {
            Serial.print(
                condition.value
            );
        }

        Serial.println();
    }

    Serial.print(
        "Actions     : "
    );

    Serial.println(
        automation.actionCount
    );

    for (
        uint8_t i = 0;
        i < automation.actionCount;
        i++
    ) {
        const AutomationAction&
            action =
                automation.actions[i];

        Serial.print(
            "  Action #"
        );

        Serial.print(i);

        Serial.print(
            " : "
        );

        switch (action.type) {

            case ActionType::EXECUTE_SCENE:
                Serial.print(
                    "EXECUTE_SCENE"
                );
                break;

            case ActionType::SET_LAMP_POWER:
                Serial.print(
                    "SET_LAMP_POWER"
                );
                break;

            case ActionType::SET_LAMP_BRIGHTNESS:
                Serial.print(
                    "SET_LAMP_BRIGHTNESS"
                );
                break;

            case ActionType::SET_LAMP_AUTOMATIC:
                Serial.print(
                    "SET_LAMP_AUTOMATIC"
                );
                break;

            case ActionType::SET_GROUP_POWER:
                Serial.print(
                    "SET_GROUP_POWER"
                );
                break;

            case ActionType::SET_GROUP_BRIGHTNESS:
                Serial.print(
                    "SET_GROUP_BRIGHTNESS"
                );
                break;
        }

        Serial.print(
            " -> ID="
        );

        Serial.print(
            action.targetId
        );

        Serial.print(
            " VALUE="
        );

        Serial.println(
            action.value
        );
    }

    Serial.print(
        "Delai       : "
    );

    Serial.print(
        automation.triggerDelayMs
    );

    Serial.println(
        " ms"
    );

    Serial.print(
        "Cooldown    : "
    );

    Serial.print(
        automation.cooldownMs
    );

    Serial.println(
        " ms"
    );

    Serial.print(
        "Mode        : "
    );

    Serial.println(
        automation.triggerMode ==
            AutomationTriggerMode::ONCE
            ? "ONCE"
            : "REPEAT"
    );

    Serial.println(
        "=========================="
    );
}


void printAutomationRegistry(
    const AutomationRegistry& registry
) {
    Serial.println();
    Serial.println(
        "===== REGISTRE AUTOMATISATIONS ====="
    );

    Serial.print(
        "Nombre : "
    );

    Serial.println(
        registry.count
    );

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {
        printAutomation(
            registry.automations[i]
        );
    }

    Serial.println(
        "===================================="
    );
}