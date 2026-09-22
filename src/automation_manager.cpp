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
    SceneRegistry& scenes,
    const Automation& automation
) {
    if (
        registry.count >=
        MAX_AUTOMATIONS
    ) {
        Serial.println(
            "Impossible d'ajouter l'automatisation : registre plein"
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

    if (
        automation.conditionCount >
        MAX_AUTOMATION_CONDITIONS
    ) {
        Serial.println(
            "Impossible d'ajouter l'automatisation : trop de conditions"
        );

        return false;
    }

    if (
        findScene(
            scenes,
            automation.sceneId
        ) == nullptr
    ) {
        Serial.print(
            "Scene introuvable pour l'automatisation : "
        );

        Serial.println(
            automation.sceneId
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

bool enableAutomation(
    AutomationRegistry& registry,
    uint32_t automationId
) {
    Automation* automation =
        findAutomation(
            registry,
            automationId
        );

    if (automation == nullptr) {
        return false;
    }

    automation->enabled = true;

    automation->conditionState = false;
    automation->conditionSince = 0;

    Serial.print(
        "Automatisation activee : "
    );

    Serial.println(
        automation->name
    );

    return true;
}

bool disableAutomation(
    AutomationRegistry& registry,
    uint32_t automationId
) {
    Automation* automation =
        findAutomation(
            registry,
            automationId
        );

    if (automation == nullptr) {
        return false;
    }

    automation->enabled = false;

    automation->conditionState = false;
    automation->conditionSince = 0;

    Serial.print(
        "Automatisation desactivee : "
    );

    Serial.println(
        automation->name
    );

    return true;
}

static void printCondition(
    const AutomationCondition& condition
) {
    switch (condition.type) {

        case AutomationConditionType::LIGHT_LEVEL:
            Serial.print("LUMINOSITE ");
            break;

        case AutomationConditionType::PRESENCE:
            Serial.print("PRESENCE ");
            break;

        case AutomationConditionType::TIME:
            Serial.print("HEURE ");
            break;
    }

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
        uint16_t totalMinutes =
            condition.value;

        uint16_t hour =
            totalMinutes / 60;

        uint16_t minute =
            totalMinutes % 60;

        if (hour < 10) {
            Serial.print("0");
        }

        Serial.print(hour);

        Serial.print(":");

        if (minute < 10) {
            Serial.print("0");
        }

        Serial.println(minute);
    }
    else {
        Serial.println(
            condition.value
        );
    }
}

void printAutomation(
    const Automation& automation
) {
    Serial.println();
    Serial.println(
        "===== AUTOMATISATION ====="
    );

    Serial.print("ID          : ");
    Serial.println(
        automation.id
    );

    Serial.print("Nom         : ");
    Serial.println(
        automation.name
    );

    Serial.print("Etat        : ");

    Serial.println(
        automation.enabled
            ? "ACTIVE"
            : "INACTIVE"
    );

    Serial.print("Logique     : ");

    Serial.println(
        automation.logic ==
        AutomationLogic::AND
            ? "AND"
            : "OR"
    );

    Serial.print("Scene       : ");

    Serial.println(
        automation.sceneId
    );

    Serial.print("Conditions  : ");

    Serial.println(
        automation.conditionCount
    );

    for (
        uint8_t i = 0;
        i < automation.conditionCount;
        i++
    ) {
        Serial.print(
            "  Condition #"
        );

        Serial.print(i);

        Serial.print(" : ");

        printCondition(
            automation.conditions[i]
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