#pragma once

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

// See:
//   https://en.wikipedia.org/wiki/X_Macro
//
// Can be expanded to switch statements, enums, or arrays of integers or strings.
//
// Formatting goes:
//
//  FILE_X(name, file, priority, walkable)

#define FILE_PRIO_TERRAIN  (  1)
#define FILE_PRIO_SHADOW   (  2)
#define FILE_PRIO_GRAPHICS (  3)
#define FILE_PRIO_HIGHEST  (255)

#define FILE_X_INTERFAC \
    FILE_X(FILE_INTERFAC_NONE, /* ............... */ (-1),  FILE_PRIO_HIGHEST, true)

#define FILE_X_TERRAIN \
    FILE_X(FILE_TERRAIN_NONE, /* ................ */ (-1),  FILE_PRIO_HIGHEST, true)  \
    FILE_X(FILE_DIRT,  /* ....................... */ ( 0),  FILE_PRIO_TERRAIN, true)  \
    FILE_X(FILE_GRASS, /* ....................... */ ( 1),  FILE_PRIO_TERRAIN, true)  \
    FILE_X(FILE_WATER, /* ....................... */ ( 2),  FILE_PRIO_TERRAIN, false) \
    FILE_X(FILE_FARM,  /* ....................... */ ( 3),  FILE_PRIO_TERRAIN, true)

#define FILE_X_GRAPHICS \
    FILE_X(FILE_GRAPHICS_NONE, /* ............. */ (  -1), FILE_PRIO_HIGHEST , true)  \
    FILE_X(FILE_STONE_MINE,  /* ............... */ ( 334), FILE_PRIO_GRAPHICS, false) \
    FILE_X(FILE_TREE_STUMPS,   /* ............. */ ( 335), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_TREE_CHOPPED_DOWN,      /* .... */ ( 370), FILE_PRIO_GRAPHICS, false) \
    FILE_X(FILE_MALE_VILLAGER_FIGHTING, /* .... */ ( 430), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_DYING,    /* .... */ ( 431), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_STANDING, /* .... */ ( 432), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_DECAYING, /* .... */ ( 433), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_MOVING,   /* .... */ ( 434), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_CHOPPING, ( 452), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_CARRYING, ( 453), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_DYING,    ( 454), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_STANDING, ( 455), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_DECAYING, ( 456), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_FOREST_TREE_SHADOW, /* ........ */ ( 587), FILE_PRIO_SHADOW  , true)  \
    FILE_X(FILE_BERRY_BUSH,  /* ............... */ ( 698), FILE_PRIO_GRAPHICS, false) \
    FILE_X(FILE_WAYPOINT_FLAG, /* ............. */ ( 795), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_RIGHT_CLICK_RED_ARROWS, /* .... */ ( 796), FILE_PRIO_GRAPHICS, true)  \
    FILE_X(FILE_GOLD_MINE,   /* ............... */ (1297), FILE_PRIO_GRAPHICS, false) \
    FILE_X(FILE_FOREST_TREE, /* ............... */ (1410), FILE_PRIO_GRAPHICS, false)
