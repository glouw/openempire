#pragma once

#include "Type.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    int32_t id;
    int32_t data_offset;
    int32_t size;
}
File;

void File_Print(const File);

File File_Load(FILE* const);

#define FILE_NONE_MAX_SPEED (0)
#define FILE_NONE_ACCEL (0)

#define FILE_VILLAGER_MAX_SPEED (1500)
#define FILE_VILLAGER_ACCEL      (500)

#define FILE_PRIO_TERRAIN  (  1)
#define FILE_PRIO_SHADOW   (  2)
#define FILE_PRIO_GRAPHICS (  3)
#define FILE_PRIO_HIGHEST  (255)

// For more details about X-Macros:
//   https://en.wikipedia.org/wiki/X_Macro
//
// NOTE: This X-Macro is used in:
//   Interfac.[ch]
//   Graphics.[ch]
//   Terrain.[ch]
//   Registrar.c
//
// To get an idea of how the X-Macro expands in Graphics.c, for instance, run:
//
//   gcc -E Graphics.c | clang-format

// ------------------------------------------------- FILE ---- PRIORITY ---------- WALKABKLE - TYPE ------------------ MAX_SPEED -------------- ACCEL
#define FILE_X_INTERFAC \
    FILE_X(FILE_INTERFAC_NONE,                       (  -1),   FILE_PRIO_HIGHEST,  (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL)

#define FILE_X_TERRAIN \
    FILE_X(FILE_TERRAIN_NONE,                        (  -1),   FILE_PRIO_HIGHEST,  (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_DIRT,                                (   0),   FILE_PRIO_TERRAIN,  (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_GRASS,                               (   1),   FILE_PRIO_TERRAIN,  (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_WATER,                               (   2),   FILE_PRIO_TERRAIN,  (false),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_FARM,                                (   3),   FILE_PRIO_TERRAIN,  (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL)

#define FILE_X_GRAPHICS \
    FILE_X(FILE_GRAPHICS_NONE,                       (  -1),   FILE_PRIO_HIGHEST,  (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_STONE_MINE,                          ( 334),   FILE_PRIO_GRAPHICS, (false),    TYPE_STONE_MINE,        FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_TREE_STUMPS,                         ( 335),   FILE_PRIO_GRAPHICS, (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_TREE_CHOPPED_DOWN,                   ( 370),   FILE_PRIO_GRAPHICS, (false),    TYPE_TREE_CHOPPED_DOWN, FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_FIGHTING,              ( 430),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_DYING,                 ( 431),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_STANDING,              ( 432),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_DECAYING,              ( 433),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_MOVING,                ( 434),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_CHOPPING,   ( 452),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_CARRYING,   ( 453),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_DYING,      ( 454),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_STANDING,   ( 455),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_DECAYING,   ( 456),   FILE_PRIO_GRAPHICS, (true ),    TYPE_VILLAGER,          FILE_VILLAGER_MAX_SPEED, FILE_VILLAGER_ACCEL) \
    FILE_X(FILE_FOREST_TREE_SHADOW,                  ( 587),   FILE_PRIO_SHADOW,   (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_BERRY_BUSH,                          ( 698),   FILE_PRIO_GRAPHICS, (false),    TYPE_BERRY_BUSH,        FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_WAYPOINT_FLAG,                       ( 795),   FILE_PRIO_GRAPHICS, (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_RIGHT_CLICK_RED_ARROWS,              ( 796),   FILE_PRIO_GRAPHICS, (true ),    TYPE_NONE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_GOLD_MINE,                           (1297),   FILE_PRIO_GRAPHICS, (false),    TYPE_GOLD_MINE,         FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL) \
    FILE_X(FILE_FOREST_TREE,                         (1410),   FILE_PRIO_GRAPHICS, (false),    TYPE_TREE,              FILE_NONE_MAX_SPEED,     FILE_NONE_ACCEL)
