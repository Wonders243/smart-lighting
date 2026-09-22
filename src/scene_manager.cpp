#include <Arduino.h>

#include "scene_manager.h"


// ============================================================
// INITIALISATION
// ============================================================

void initSceneRegistry(
    SceneRegistry& registry
) {

    registry.count = 0;
}


// ============================================================
// RECHERCHE D'UNE SCENE
// ============================================================

Scene* findScene(
    SceneRegistry& registry,
    uint32_t sceneId
) {

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        if (
            registry.scenes[i].id ==
            sceneId
        ) {

            return &registry.scenes[i];
        }
    }

    return nullptr;
}


// ============================================================
// AJOUT D'UNE SCENE
// ============================================================

bool addScene(
    SceneRegistry& registry,
    const Scene& scene
) {

    if (
        registry.count >=
        MAX_SCENES
    ) {

        Serial.println(
            "Impossible d'ajouter la scene : registre plein"
        );

        return false;
    }


    if (
        findScene(
            registry,
            scene.id
        ) != nullptr
    ) {

        Serial.print(
            "Scene deja existante : "
        );

        Serial.println(
            scene.id
        );

        return false;
    }


    if (
        scene.actionCount >
        MAX_SCENE_ACTIONS
    ) {

        Serial.println(
            "Impossible d'ajouter la scene : trop d'actions"
        );

        return false;
    }


    registry.scenes[
        registry.count
    ] = scene;

    registry.count++;


    Serial.print(
        "Scene ajoutee : "
    );

    Serial.println(
        scene.name
    );


    return true;
}


// ============================================================
// SUPPRESSION D'UNE SCENE
// ============================================================

bool removeScene(
    SceneRegistry& registry,
    uint32_t sceneId
) {

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        if (
            registry.scenes[i].id ==
            sceneId
        ) {

            for (
                uint8_t j = i;
                j + 1 < registry.count;
                j++
            ) {

                registry.scenes[j] =
                    registry.scenes[j + 1];
            }


            registry.count--;


            Serial.print(
                "Scene supprimee : "
            );

            Serial.println(
                sceneId
            );


            return true;
        }
    }


    Serial.print(
        "Scene introuvable : "
    );

    Serial.println(
        sceneId
    );


    return false;
}


// ============================================================
// AJOUT D'UNE ACTION
// ============================================================

bool addActionToScene(
    SceneRegistry& scenes,
    GroupRegistry& groups,
    uint32_t sceneId,
    const SceneAction& action
) {

    Scene* scene =
        findScene(
            scenes,
            sceneId
        );


    if (scene == nullptr) {

        Serial.print(
            "Scene introuvable : "
        );

        Serial.println(
            sceneId
        );

        return false;
    }


    // --------------------------------------------------------
    // Vérification du groupe
    // --------------------------------------------------------

    LampGroup* group =
        findGroup(
            groups,
            action.groupId
        );


    if (group == nullptr) {

        Serial.print(
            "Groupe introuvable : "
        );

        Serial.println(
            action.groupId
        );

        return false;
    }


    // --------------------------------------------------------
    // Vérification du nombre d'actions
    // --------------------------------------------------------

    if (
        scene->actionCount >=
        MAX_SCENE_ACTIONS
    ) {

        Serial.println(
            "Impossible d'ajouter l'action : scene pleine"
        );

        return false;
    }


    // --------------------------------------------------------
    // Ajout
    // --------------------------------------------------------

    scene->actions[
        scene->actionCount
    ] = action;

    scene->actionCount++;


    Serial.print(
        "Action ajoutee a la scene : "
    );

    Serial.println(
        scene->name
    );


    return true;
}


// ============================================================
// RETRAIT D'UNE ACTION
// ============================================================

bool removeActionFromScene(
    SceneRegistry& scenes,
    uint32_t sceneId,
    uint8_t actionIndex
) {

    Scene* scene =
        findScene(
            scenes,
            sceneId
        );


    if (scene == nullptr) {

        Serial.print(
            "Scene introuvable : "
        );

        Serial.println(
            sceneId
        );

        return false;
    }


    if (
        actionIndex >=
        scene->actionCount
    ) {

        Serial.println(
            "Index d'action invalide"
        );

        return false;
    }


    for (
        uint8_t i = actionIndex;
        i + 1 < scene->actionCount;
        i++
    ) {

        scene->actions[i] =
            scene->actions[i + 1];
    }


    scene->actionCount--;


    Serial.print(
        "Action retiree de la scene : "
    );

    Serial.println(
        scene->name
    );


    return true;
}


// ============================================================
// AFFICHAGE D'UNE ACTION
// ============================================================

static void printSceneAction(
    const SceneAction& action
) {

    Serial.print(
        "Groupe ID     : "
    );

    Serial.println(
        action.groupId
    );


    Serial.print(
        "Commande      : "
    );


    switch (
        action.commandType
    ) {

        case CommandType::SET_GROUP_POWER:

            Serial.println(
                "POWER"
            );

            Serial.print(
                "Valeur        : "
            );

            Serial.println(
                action.value != 0
                    ? "ON"
                    : "OFF"
            );

            break;


        case CommandType::SET_GROUP_BRIGHTNESS:

            Serial.println(
                "BRIGHTNESS"
            );

            Serial.print(
                "Valeur        : "
            );

            Serial.print(
                action.value
            );

            Serial.println(
                "%"
            );

            break;


        default:

            Serial.println(
                "INCONNUE"
            );

            break;
    }
}


// ============================================================
// AFFICHAGE D'UNE SCENE
// ============================================================

void printScene(
    const Scene& scene
) {

    Serial.println();

    Serial.println(
        "===== SCENE ====="
    );


    Serial.print(
        "ID          : "
    );

    Serial.println(
        scene.id
    );


    Serial.print(
        "Nom         : "
    );

    Serial.println(
        scene.name
    );


    Serial.print(
        "Actions     : "
    );

    Serial.println(
        scene.actionCount
    );


    for (
        uint8_t i = 0;
        i < scene.actionCount;
        i++
    ) {

        Serial.println();

        Serial.print(
            "Action #"
        );

        Serial.println(
            i
        );


        printSceneAction(
            scene.actions[i]
        );
    }


    Serial.println(
        "================="
    );
}


// ============================================================
// AFFICHAGE DU REGISTRE
// ============================================================

void printSceneRegistry(
    const SceneRegistry& registry
) {

    Serial.println();

    Serial.println(
        "===== REGISTRE SCENES ====="
    );


    Serial.print(
        "Nombre de scenes : "
    );

    Serial.println(
        registry.count
    );


    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        printScene(
            registry.scenes[i]
        );
    }


    Serial.println(
        "==========================="
    );
}