#include <Arduino.h>

#include "automation_engine.h"
#include "action_executor.h"
#include "message_manager.h"


bool evaluateCondition(
    const AutomationCondition& condition,
    const AutomationContext& context
) {
    int32_t actualValue = 0;

    switch (condition.type) {

        case AutomationConditionType::LIGHT_LEVEL:
            actualValue =
                context.lightLevel;
            break;

        case AutomationConditionType::PRESENCE:
            actualValue =
                context.presenceDetected
                    ? 1
                    : 0;
            break;

        case AutomationConditionType::TIME:
            actualValue =
                context.hour * 60 +
                context.minute;
            break;
    }

    switch (condition.op) {

        case AutomationOperator::LESS_THAN:
            return actualValue <
                   condition.value;

        case AutomationOperator::LESS_OR_EQUAL:
            return actualValue <=
                   condition.value;

        case AutomationOperator::GREATER_THAN:
            return actualValue >
                   condition.value;

        case AutomationOperator::GREATER_OR_EQUAL:
            return actualValue >=
                   condition.value;

        case AutomationOperator::EQUAL:
            return actualValue ==
                   condition.value;
    }

    return false;
}


bool evaluateAutomation(
    const Automation& automation,
    const AutomationContext& context
) {
    if (
        automation.conditionCount == 0
    ) {
        return false;
    }

    if (
        automation.logic ==
        AutomationLogic::AND
    ) {
        for (
            uint8_t i = 0;
            i < automation.conditionCount;
            i++
        ) {
            if (
                !evaluateCondition(
                    automation.conditions[i],
                    context
                )
            ) {
                return false;
            }
        }

        return true;
    }

    for (
        uint8_t i = 0;
        i < automation.conditionCount;
        i++
    ) {
        if (
            evaluateCondition(
                automation.conditions[i],
                context
            )
        ) {
            return true;
        }
    }

    return false;
}


void processAutomations(
    AutomationRegistry& registry,
    SceneRegistry& scenes,
    GroupRegistry& groups,
    LampRegistry& lamps,
    const AutomationContext& context
) {
    uint32_t now = millis();

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {
        Automation& automation =
            registry.automations[i];

        if (!automation.enabled) {
            continue;
        }

        bool conditionResult =
            evaluateAutomation(
                automation,
                context
            );


        /*
         * ==========================
         * CONDITION FALSE
         * ==========================
         */

        if (!conditionResult) {

            if (automation.conditionState) {

                Serial.print(
                    "Condition terminee : "
                );

                Serial.println(
                    automation.name
                );
            }

            automation.conditionState = false;
            automation.conditionSince = 0;

            continue;
        }


        /*
         * ==========================
         * NOUVELLE CONDITION TRUE
         * ==========================
         */

        if (!automation.conditionState) {

            automation.conditionState = true;
            automation.conditionSince = now;

            Serial.print(
                "Condition detectee : "
            );

            Serial.println(
                automation.name
            );

            continue;
        }


        /*
         * ==========================
         * DELAI
         * ==========================
         */

        if (
            now - automation.conditionSince <
            automation.triggerDelayMs
        ) {
            continue;
        }


        /*
         * ==========================
         * COOLDOWN
         * ==========================
         */

        if (
            automation.lastTriggered != 0 &&
            now - automation.lastTriggered <
            automation.cooldownMs
        ) {
            continue;
        }


        /*
         * ==========================
         * MODE ONCE
         * ==========================
         */

        if (
            automation.triggerMode ==
            AutomationTriggerMode::ONCE
        ) {
            if (
                automation.lastTriggered >=
                automation.conditionSince
            ) {
                continue;
            }
        }


        /*
         * ==========================
         * DECLENCHEMENT
         * ==========================
         */

        Serial.println();

        Serial.print(
            "AUTOMATISATION DECLENCHEE : "
        );

        Serial.println(
            automation.name
        );


        /*
         * ==========================
         * EXECUTION DES ACTIONS
         * ==========================
         */

        uint8_t actionsExecuted = 0;
        uint8_t actionsPartial = 0;
        uint8_t actionsFailed = 0;


        for (
            uint8_t actionIndex = 0;
            actionIndex < automation.actionCount;
            actionIndex++
        ) {

            const AutomationAction&
                automationAction =
                    automation.actions[
                        actionIndex
                    ];


            Serial.print(
                "Action #"
            );

            Serial.println(
                actionIndex
            );


            Action action = {

                automationAction.type,

                automationAction.targetId,

                automationAction.value
            };


            ExecutionStatus result =
                executeAction(
                    action,
                    scenes,
                    groups,
                    lamps
                );


            Serial.print(
                "Resultat action : "
            );

            Serial.println(
                executionStatusToString(
                    result
                )
            );


            switch (result) {

                case ExecutionStatus::EXECUTED:
                    actionsExecuted++;
                    break;

                case ExecutionStatus::PARTIAL:
                    actionsPartial++;
                    break;

                case ExecutionStatus::FAILED:
                    actionsFailed++;
                    break;
            }
        }


        /*
         * ==========================
         * RESULTAT
         * ==========================
         */

        Serial.println();

        Serial.println(
            "===== RESULTAT AUTOMATISATION ====="
        );


        Serial.print(
            "Actions executees : "
        );

        Serial.println(
            actionsExecuted
        );


        Serial.print(
            "Actions partielles : "
        );

        Serial.println(
            actionsPartial
        );


        Serial.print(
            "Actions echouees : "
        );

        Serial.println(
            actionsFailed
        );


        Serial.println(
            "==================================="
        );


        automation.lastTriggered = now;


        /*
         * ==========================
         * MODE ONCE
         * ==========================
         */

        if (
            automation.triggerMode ==
            AutomationTriggerMode::ONCE
        ) {

            Serial.println(
                "Mode ONCE : attente du retour a FALSE"
            );
        }
    }
}