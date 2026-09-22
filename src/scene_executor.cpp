#include <Arduino.h>

#include "scene_executor.h"
#include "group_command_handler.h"


// ============================================================
// VERIFICATION D'UN ID
// ============================================================

static bool containsId(

    const uint32_t* ids,

    uint8_t count,

    uint32_t id
) {

    for (
        uint8_t i = 0;
        i < count;
        i++
    ) {

        if (
            ids[i] == id
        ) {

            return true;
        }
    }

    return false;
}


// ============================================================
// EXECUTION D'UNE SCENE
// ============================================================

SceneExecutionStatus executeScene(

    SceneRegistry& scenes,

    GroupRegistry& groups,

    LampRegistry& lamps,

    uint32_t sceneId,

    SceneExecutionResult& result
) {

    // --------------------------------------------------------
    // INITIALISATION
    // --------------------------------------------------------

    result.status =
        SceneExecutionStatus::FAILED;

    result.actionsExecuted = 0;

    result.actionsPartial = 0;

    result.actionsFailed = 0;

    result.lampsAffected = 0;

    result.lampsSkipped = 0;


    // --------------------------------------------------------
    // RECHERCHE SCENE
    // --------------------------------------------------------

    Scene* scene =
        findScene(
            scenes,
            sceneId
        );


    if (
        scene == nullptr
    ) {

        Serial.print(
            "Scene introuvable : "
        );

        Serial.println(
            sceneId
        );

        return SceneExecutionStatus::FAILED;
    }


    // --------------------------------------------------------
    // SCENE VIDE
    // --------------------------------------------------------

    if (
        scene->actionCount == 0
    ) {

        Serial.print(
            "Scene vide : "
        );

        Serial.println(
            scene->name
        );

        return SceneExecutionStatus::FAILED;
    }


    // --------------------------------------------------------
    // HEADER
    // --------------------------------------------------------

    Serial.println();

    Serial.println(
        "=============================="
    );

    Serial.print(
        " EXECUTION SCENE : "
    );

    Serial.println(
        scene->name
    );

    Serial.println(
        "=============================="
    );


    // --------------------------------------------------------
    // TABLEAUX POUR EVITER LES DOUBLONS
    // --------------------------------------------------------

    uint32_t affectedLampIds[
        MAX_GROUP_LAMPS * MAX_SCENE_ACTIONS
    ];

    uint8_t affectedCount = 0;


    uint32_t skippedLampIds[
        MAX_GROUP_LAMPS * MAX_SCENE_ACTIONS
    ];

    uint8_t skippedCount = 0;


    // ========================================================
    // EXECUTION DES ACTIONS
    // ========================================================

    for (
        uint8_t i = 0;
        i < scene->actionCount;
        i++
    ) {

        SceneAction& action =
            scene->actions[i];


        Serial.println();

        Serial.print(
            "Action #"
        );

        Serial.println(
            i
        );


        // ----------------------------------------------------
        // VERIFICATION GROUPE
        // ----------------------------------------------------

        LampGroup* group =
            findGroup(
                groups,
                action.groupId
            );


        if (
            group == nullptr
        ) {

            Serial.print(
                "Groupe introuvable : "
            );

            Serial.println(
                action.groupId
            );

            result.actionsFailed++;

            continue;
        }


        // ----------------------------------------------------
        // CREATION COMMANDE
        // ----------------------------------------------------

        Command command = {

            static_cast<uint32_t>(
                1000 + i
            ),

            action.groupId,

            action.commandType,

            action.value,

            CommandStatus::PENDING
        };


        // ----------------------------------------------------
        // EXECUTION
        // ----------------------------------------------------

        executeGroupCommand(

            groups,

            lamps,

            command
        );


        // ----------------------------------------------------
        // RESULTAT DE L'ACTION
        // --------------------------------------------------------

        switch (
            command.status
        ) {

            case CommandStatus::EXECUTED:

                result.actionsExecuted++;

                break;


            case CommandStatus::PARTIAL:

                result.actionsPartial++;

                break;


            case CommandStatus::FAILED:

                result.actionsFailed++;

                break;


            default:

                result.actionsFailed++;

                break;
        }


        // ----------------------------------------------------
        // COMPTAGE UNIQUE DES LAMPES
        // ----------------------------------------------------

        for (
            uint8_t j = 0;
            j < group->lampCount;
            j++
        ) {

            uint32_t lampId =
                group->lampIds[j];


            Lamp* lamp =
                findLamp(
                    lamps,
                    lampId
                );


            if (
                lamp == nullptr
            ) {

                continue;
            }


            // -----------------------------------------------
            // ONLINE
            // -----------------------------------------------

            if (
                lamp->device.status ==
                DeviceStatus::ONLINE
            ) {

                if (
                    !containsId(
                        affectedLampIds,
                        affectedCount,
                        lampId
                    )
                ) {

                    affectedLampIds[
                        affectedCount
                    ] = lampId;

                    affectedCount++;
                }
            }


            // -----------------------------------------------
            // OFFLINE
            // -----------------------------------------------

            else {

                if (
                    !containsId(
                        skippedLampIds,
                        skippedCount,
                        lampId
                    )
                ) {

                    skippedLampIds[
                        skippedCount
                    ] = lampId;

                    skippedCount++;
                }
            }
        }
    }


    // ========================================================
    // STATISTIQUES
    // ========================================================

    result.lampsAffected =
        affectedCount;

    result.lampsSkipped =
        skippedCount;


    // ========================================================
    // STATUT GLOBAL
    // ========================================================

    if (
        result.actionsFailed ==
        scene->actionCount
    ) {

        result.status =
            SceneExecutionStatus::FAILED;
    }

    else if (
        result.actionsPartial > 0 ||
        result.actionsFailed > 0
    ) {

        result.status =
            SceneExecutionStatus::PARTIAL;
    }

    else {

        result.status =
            SceneExecutionStatus::EXECUTED;
    }


    // ========================================================
    // AFFICHAGE
    // ========================================================

    Serial.println();

    Serial.println(
        "===== RESULTAT SCENE ====="
    );


    Serial.print(
        "Actions executees : "
    );

    Serial.println(
        result.actionsExecuted
    );


    Serial.print(
        "Actions partielles : "
    );

    Serial.println(
        result.actionsPartial
    );


    Serial.print(
        "Actions echouees : "
    );

    Serial.println(
        result.actionsFailed
    );


    Serial.print(
        "Lampes affectees : "
    );

    Serial.println(
        result.lampsAffected
    );


    Serial.print(
        "Lampes ignorees : "
    );

    Serial.println(
        result.lampsSkipped
    );


    Serial.print(
        "Statut scene : "
    );


    switch (
        result.status
    ) {

        case SceneExecutionStatus::EXECUTED:

            Serial.println(
                "EXECUTED"
            );

            break;


        case SceneExecutionStatus::PARTIAL:

            Serial.println(
                "PARTIAL"
            );

            break;


        case SceneExecutionStatus::FAILED:

            Serial.println(
                "FAILED"
            );

            break;
    }


    Serial.println(
        "=========================="
    );


    return result.status;
}