#include <Arduino.h>

#include "roles.h"
#include "device_registry.h"
#include "device_manager.h"
#include "lamp_controller.h"
#include "group_manager.h"
#include "scene_manager.h"
#include "scene_executor.h"
#include "automation_manager.h"
#include "automation_engine.h"
#include "action_executor.h"

LampRegistry lampRegistry;
GroupRegistry groupRegistry;
SceneRegistry sceneRegistry;
AutomationRegistry automationRegistry;


// ============================================================
// OUTIL DE TEST
// ============================================================

void processContext(
    const char* title,
    AutomationContext context
) {
    Serial.println();
    Serial.println(title);

    Serial.print("Contexte : ");
    Serial.print(context.lightLevel);
    Serial.print(" lux | ");

    Serial.print(
        context.presenceDetected
            ? "presence"
            : "absence"
    );

    Serial.print(" | ");

    if (context.hour < 10) {
        Serial.print("0");
    }

    Serial.print(context.hour);
    Serial.print(":");

    if (context.minute < 10) {
        Serial.print("0");
    }

    Serial.println(context.minute);

    processAutomations(
        automationRegistry,
        sceneRegistry,
        groupRegistry,
        lampRegistry,
        context
    );
}


// ============================================================
// SETUP
// ============================================================

void setup() {

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("      SMART LIGHTING V2");
    Serial.println("      AUTOMATION V2.7");
    Serial.println("==============================");


    // ========================================================
    // INITIALISATION
    // ========================================================

    initLampRegistry(
        lampRegistry
    );

    initGroupRegistry(
        groupRegistry
    );

    initSceneRegistry(
        sceneRegistry
    );

    initAutomationRegistry(
        automationRegistry
    );


    // ========================================================
    // LAMPES
    // ========================================================

    Lamp lamp1 = {
        {
            1,
            "LAMP_01",
            DeviceRole::LAMP,
            DeviceStatus::ONLINE,
            millis()
        },
        {
            false,
            100,
            false
        }
    };

    Lamp lamp2 = {
        {
            2,
            "LAMP_02",
            DeviceRole::LAMP,
            DeviceStatus::ONLINE,
            millis()
        },
        {
            false,
            70,
            false
        }
    };

    Lamp lamp3 = {
        {
            3,
            "LAMP_03",
            DeviceRole::LAMP,
            DeviceStatus::OFFLINE,
            0
        },
        {
            false,
            100,
            true
        }
    };

    addLamp(
        lampRegistry,
        lamp1
    );

    addLamp(
        lampRegistry,
        lamp2
    );

    addLamp(
        lampRegistry,
        lamp3
    );


    // ========================================================
    // GROUPE ENTREE
    // ========================================================

    LampGroup entree = {
        1,
        "ENTREE",
        {},
        0
    };

    addGroup(
        groupRegistry,
        entree
    );

    addLampToGroup(
        groupRegistry,
        lampRegistry,
        1,
        1
    );

    addLampToGroup(
        groupRegistry,
        lampRegistry,
        1,
        2
    );

    addLampToGroup(
        groupRegistry,
        lampRegistry,
        1,
        3
    );


    // ========================================================
    // SCENE SOIR
    // ========================================================

    Scene soir = {
        1,
        "SOIR",
        {},
        0
    };

    addScene(
        sceneRegistry,
        soir
    );

    SceneAction powerOn = {
        1,
        CommandType::SET_GROUP_POWER,
        1
    };

    SceneAction brightness = {
        1,
        CommandType::SET_GROUP_BRIGHTNESS,
        40
    };

    addActionToScene(
        sceneRegistry,
        groupRegistry,
        1,
        powerOn
    );

    addActionToScene(
        sceneRegistry,
        groupRegistry,
        1,
        brightness
    );


    // ========================================================
    // AUTOMATISATION 1
    //
    // SOIR_PRESENCE
    //
    // luminosite <= 30
    // AND
    // presence = 1
    //
    // MODE ONCE
    // DELAI 3 secondes
    // ========================================================

    Automation automation1 = {
        1,
        "SOIR_PRESENCE",

        true,

        {},

        0,

        AutomationLogic::AND,

        // Actions
        {},

        0,

        // Scene de compatibilite
        1,

        // Etat
        false,
        0,
        0,

        // Timing
        3000,
        10000,

        // Mode
        AutomationTriggerMode::ONCE
    };

    addAutomation(
        automationRegistry,
        sceneRegistry,
        automation1
    );


    // ========================================================
    // CONDITIONS AUTOMATISATION 1
    // ========================================================

    AutomationCondition lightCondition = {
        AutomationConditionType::LIGHT_LEVEL,
        AutomationOperator::LESS_OR_EQUAL,
        30
    };

    AutomationCondition presenceCondition = {
        AutomationConditionType::PRESENCE,
        AutomationOperator::EQUAL,
        1
    };

    addConditionToAutomation(
        automationRegistry,
        1,
        lightCondition
    );

    addConditionToAutomation(
        automationRegistry,
        1,
        presenceCondition
    );


    // ========================================================
    // ACTION DIRECTE 1
    //
    // LAMP_01 -> 25 %
    // ========================================================

    AutomationAction directBrightness = {
        ActionType::SET_LAMP_BRIGHTNESS,
        1,
        25
    };

    addActionToAutomation(
        automationRegistry,
        1,
        directBrightness
    );


    // ========================================================
    // ACTION DIRECTE 2
    //
    // LAMP_01 -> ON
    // ========================================================

    AutomationAction directPower = {
        ActionType::SET_LAMP_POWER,
        1,
        1
    };

    addActionToAutomation(
        automationRegistry,
        1,
        directPower
    );


    // ========================================================
    // AUTOMATISATION 2
    //
    // SOIR_HORAIRE
    //
    // luminosite <= 20
    // OR
    // heure >= 21:00
    //
    // MODE REPEAT
    // DELAI 2 secondes
    // COOLDOWN 8 secondes
    // ========================================================

    Automation automation2 = {
        2,
        "SOIR_HORAIRE",

        true,

        {},

        0,

        AutomationLogic::OR,

        // Actions
        {},

        0,

        // Scene
        1,

        // Etat
        false,
        0,
        0,

        // Timing
        2000,
        8000,

        // Mode
        AutomationTriggerMode::REPEAT
    };

    addAutomation(
        automationRegistry,
        sceneRegistry,
        automation2
    );


    // ========================================================
    // CONDITIONS AUTOMATISATION 2
    // ========================================================

    AutomationCondition darkCondition = {
        AutomationConditionType::LIGHT_LEVEL,
        AutomationOperator::LESS_OR_EQUAL,
        20
    };

    AutomationCondition timeCondition = {
        AutomationConditionType::TIME,
        AutomationOperator::GREATER_OR_EQUAL,
        21 * 60
    };

    addConditionToAutomation(
        automationRegistry,
        2,
        darkCondition
    );

    addConditionToAutomation(
        automationRegistry,
        2,
        timeCondition
    );


    // ========================================================
    // ACTION AUTOMATISATION 2
    //
    // EXECUTION DE LA SCENE SOIR
    // ========================================================

    AutomationAction sceneAction = {
        ActionType::EXECUTE_SCENE,
        1,
        0
    };

    addActionToAutomation(
        automationRegistry,
        2,
        sceneAction
    );


    // ========================================================
    // AFFICHAGE DU REGISTRE
    // ========================================================

    printAutomationRegistry(
        automationRegistry
    );


    // ========================================================
    // TEST 1
    //
    // AUCUNE CONDITION
    // ========================================================

    processContext(
        "===== TEST 1 =====",
        {
            100,
            false,
            18,
            0
        }
    );

    delay(1000);


    // ========================================================
    // TEST 2
    //
    // SOIR_PRESENCE
    //
    // 20 lux + presence
    // ========================================================

    processContext(
        "===== TEST 2 =====",
        {
            20,
            true,
            18,
            0
        }
    );

    Serial.println(
        "Attente 3 secondes..."
    );

    delay(3000);


    // ========================================================
    // TEST 2B
    // DECLENCHEMENT
    // ========================================================

    processContext(
        "===== TEST 2B =====",
        {
            20,
            true,
            18,
            0
        }
    );


    // ========================================================
    // TEST 3
    //
    // ONCE :
    // la condition reste vraie
    // mais aucune repetition
    // ========================================================

    Serial.println();
    Serial.println(
        "===== TEST 3 ====="
    );

    Serial.println(
        "Condition toujours vraie"
    );

    delay(5000);

    processContext(
        "TEST 3B",
        {
            20,
            true,
            18,
            0
        }
    );


    // ========================================================
    // TEST 4
    //
    // RETOUR FALSE
    // ========================================================

    processContext(
        "===== TEST 4 =====",
        {
            100,
            false,
            18,
            0
        }
    );

    delay(1000);


    // ========================================================
    // TEST 5
    //
    // NOUVEAU TRUE
    // ========================================================

    processContext(
        "===== TEST 5 =====",
        {
            20,
            true,
            18,
            0
        }
    );

    Serial.println(
        "Attente 3 secondes..."
    );

    delay(3000);


    // ========================================================
    // TEST 5B
    // ========================================================

    processContext(
        "===== TEST 5B =====",
        {
            20,
            true,
            18,
            0
        }
    );


    // ========================================================
    // TEST 6
    //
    // REPEAT
    // ========================================================

    Serial.println();
    Serial.println(
        "===== TEST 6 ====="
    );

    Serial.println(
        "REPEAT : condition toujours vraie"
    );

    delay(5000);

    processContext(
        "TEST 6B",
        {
            20,
            true,
            18,
            0
        }
    );


    // ========================================================
    // TEST 7
    //
    // COOLDOWN
    // ========================================================

    Serial.println();
    Serial.println(
        "===== TEST 7 ====="
    );

    Serial.println(
        "Attente 5 secondes..."
    );

    delay(5000);

    processContext(
        "TEST 7B",
        {
            20,
            true,
            18,
            0
        }
    );


    // ========================================================
    // TEST 8
    //
    // COOLDOWN TERMINE
    // ========================================================

    Serial.println();
    Serial.println(
        "===== TEST 8 ====="
    );

    Serial.println(
        "Attente 5 secondes..."
    );

    delay(5000);

    processContext(
        "TEST 8B",
        {
            20,
            true,
            18,
            0
        }
    );


    // ========================================================
    // ETAT FINAL
    // ========================================================

    Serial.println();
    Serial.println(
        "===== ETAT FINAL ====="
    );

    printLampState(
        lampRegistry.lamps[0]
    );

    printLampState(
        lampRegistry.lamps[1]
    );

    printLampState(
        lampRegistry.lamps[2]
    );


    // ========================================================
    // FIN
    // ========================================================

    Serial.println();
    Serial.println(
        "=============================="
    );

    Serial.println(
        " SMART LIGHTING V2.7 READY"
    );

    Serial.println(
        "=============================="
    );
}


// ============================================================
// LOOP
// ============================================================

void loop() {

    updateDeviceStatus(
        lampRegistry
    );

    delay(1000);
}