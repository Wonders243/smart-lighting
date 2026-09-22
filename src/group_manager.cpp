#include <Arduino.h>

#include "group_manager.h"


// ============================================================
// INITIALISATION
// ============================================================

void initGroupRegistry(
    GroupRegistry& registry
) {

    registry.count = 0;
}


// ============================================================
// RECHERCHE D'UN GROUPE
// ============================================================

LampGroup* findGroup(
    GroupRegistry& registry,
    uint32_t groupId
) {

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        if (
            registry.groups[i].id ==
            groupId
        ) {

            return &registry.groups[i];
        }
    }

    return nullptr;
}


// ============================================================
// VERIFICATION D'APPARTENANCE
// ============================================================

bool lampBelongsToGroup(
    const LampGroup& group,
    uint32_t lampId
) {

    for (
        uint8_t i = 0;
        i < group.lampCount;
        i++
    ) {

        if (
            group.lampIds[i] ==
            lampId
        ) {

            return true;
        }
    }

    return false;
}


// ============================================================
// AJOUT D'UN GROUPE
// ============================================================

bool addGroup(
    GroupRegistry& registry,
    const LampGroup& group
) {

    // --------------------------------------------------------
    // Registre plein
    // --------------------------------------------------------

    if (
        registry.count >=
        MAX_GROUPS
    ) {

        Serial.println(
            "Impossible d'ajouter le groupe : registre plein"
        );

        return false;
    }


    // --------------------------------------------------------
    // ID déjà utilisé
    // --------------------------------------------------------

    if (
        findGroup(
            registry,
            group.id
        ) != nullptr
    ) {

        Serial.print(
            "Groupe deja existant : "
        );

        Serial.println(
            group.id
        );

        return false;
    }


    // --------------------------------------------------------
    // Vérification du nombre de lampes
    // --------------------------------------------------------

    if (
        group.lampCount >
        MAX_GROUP_LAMPS
    ) {

        Serial.println(
            "Impossible d'ajouter le groupe : trop de lampes"
        );

        return false;
    }


    // --------------------------------------------------------
    // Ajout
    // --------------------------------------------------------

    registry.groups[
        registry.count
    ] = group;

    registry.count++;


    Serial.print(
        "Groupe ajoute : "
    );

    Serial.println(
        group.name
    );


    return true;
}


// ============================================================
// SUPPRESSION D'UN GROUPE
// ============================================================

bool removeGroup(
    GroupRegistry& registry,
    uint32_t groupId
) {

    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        if (
            registry.groups[i].id ==
            groupId
        ) {

            // ------------------------------------------------
            // Décalage des groupes suivants
            // ------------------------------------------------

            for (
                uint8_t j = i;
                j + 1 < registry.count;
                j++
            ) {

                registry.groups[j] =
                    registry.groups[j + 1];
            }


            registry.count--;


            Serial.print(
                "Groupe supprime : "
            );

            Serial.println(
                groupId
            );


            return true;
        }
    }


    Serial.print(
        "Impossible de supprimer le groupe : "
    );

    Serial.println(
        groupId
    );


    return false;
}


// ============================================================
// AJOUT D'UNE LAMPE DANS UN GROUPE
// ============================================================

bool addLampToGroup(
    GroupRegistry& groups,
    LampRegistry& lamps,
    uint32_t groupId,
    uint32_t lampId
) {

    // --------------------------------------------------------
    // Recherche du groupe
    // --------------------------------------------------------

    LampGroup* group =
        findGroup(
            groups,
            groupId
        );


    if (group == nullptr) {

        Serial.print(
            "Groupe introuvable : "
        );

        Serial.println(
            groupId
        );

        return false;
    }


    // --------------------------------------------------------
    // Vérification de la lampe
    // --------------------------------------------------------

    Lamp* lamp =
        findLamp(
            lamps,
            lampId
        );


    if (lamp == nullptr) {

        Serial.print(
            "Lampe introuvable : "
        );

        Serial.println(
            lampId
        );

        return false;
    }


    // --------------------------------------------------------
    // Vérification du doublon
    // --------------------------------------------------------

    if (
        lampBelongsToGroup(
            *group,
            lampId
        )
    ) {

        Serial.print(
            "Lampe deja presente dans le groupe : "
        );

        Serial.println(
            lampId
        );

        return false;
    }


    // --------------------------------------------------------
    // Groupe plein
    // --------------------------------------------------------

    if (
        group->lampCount >=
        MAX_GROUP_LAMPS
    ) {

        Serial.println(
            "Impossible d'ajouter la lampe : groupe plein"
        );

        return false;
    }


    // --------------------------------------------------------
    // Ajout
    // --------------------------------------------------------

    group->lampIds[
        group->lampCount
    ] = lampId;

    group->lampCount++;


    Serial.print(
        "Lampe "
    );

    Serial.print(
        lamp->device.name
    );

    Serial.print(
        " ajoutee au groupe "
    );

    Serial.println(
        group->name
    );


    return true;
}


// ============================================================
// RETRAIT D'UNE LAMPE D'UN GROUPE
// ============================================================

bool removeLampFromGroup(
    GroupRegistry& groups,
    uint32_t groupId,
    uint32_t lampId
) {

    LampGroup* group =
        findGroup(
            groups,
            groupId
        );


    if (group == nullptr) {

        Serial.print(
            "Groupe introuvable : "
        );

        Serial.println(
            groupId
        );

        return false;
    }


    // --------------------------------------------------------
    // Recherche de la lampe
    // --------------------------------------------------------

    for (
        uint8_t i = 0;
        i < group->lampCount;
        i++
    ) {

        if (
            group->lampIds[i] ==
            lampId
        ) {

            // -----------------------------------------------
            // Décalage
            // -----------------------------------------------

            for (
                uint8_t j = i;
                j + 1 < group->lampCount;
                j++
            ) {

                group->lampIds[j] =
                    group->lampIds[j + 1];
            }


            group->lampCount--;


            Serial.print(
                "Lampe "
            );

            Serial.print(
                lampId
            );

            Serial.print(
                " retiree du groupe "
            );

            Serial.println(
                group->name
            );


            return true;
        }
    }


    Serial.print(
        "Lampe "
    );

    Serial.print(
        lampId
    );

    Serial.println(
        " absente du groupe"
    );


    return false;
}


// ============================================================
// AFFICHAGE D'UN GROUPE
// ============================================================

void printGroup(
    const LampGroup& group
) {

    Serial.println();

    Serial.println(
        "===== GROUPE ====="
    );


    Serial.print(
        "ID          : "
    );

    Serial.println(
        group.id
    );


    Serial.print(
        "Nom         : "
    );

    Serial.println(
        group.name
    );


    Serial.print(
        "Nombre      : "
    );

    Serial.println(
        group.lampCount
    );


    Serial.print(
        "Lampes      : "
    );


    if (
        group.lampCount == 0
    ) {

        Serial.println(
            "Aucune"
        );
    }

    else {

        for (
            uint8_t i = 0;
            i < group.lampCount;
            i++
        ) {

            Serial.print(
                group.lampIds[i]
            );


            if (
                i + 1 <
                group.lampCount
            ) {

                Serial.print(
                    ", "
                );
            }
        }

        Serial.println();
    }


    Serial.println(
        "================="
    );
}


// ============================================================
// AFFICHAGE DU REGISTRE
// ============================================================

void printGroupRegistry(
    const GroupRegistry& registry
) {

    Serial.println();

    Serial.println(
        "===== REGISTRE GROUPES ====="
    );


    Serial.print(
        "Nombre de groupes : "
    );

    Serial.println(
        registry.count
    );


    for (
        uint8_t i = 0;
        i < registry.count;
        i++
    ) {

        printGroup(
            registry.groups[i]
        );
    }


    Serial.println(
        "============================"
    );
}