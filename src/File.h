#pragma once

#include "Type.h"
#include "Action.h"

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

#define FILE_NONE (-1)

#define FILE_PRIO_TERRAIN (1)
#define FILE_PRIO_SHADOW (2)
#define FILE_PRIO_DECAY (3)
#define FILE_PRIO_BUILDING (4)
#define FILE_PRIO_UNIT (5)
#define FILE_PRIO_HIGHEST (255)

#define FILE_DIMENSIONS_1X1 {1,1}
#define FILE_DIMENSIONS_2X1 {2,1}
#define FILE_DIMENSIONS_2X2 {2,2}
#define FILE_DIMENSIONS_3X3 {3,3}
#define FILE_DIMENSIONS_4X4 {4,4}
#define FILE_DIMENSIONS_5X5 {5,5}
#define FILE_DIMENSIONS_6X6 {6,6}

// For more details about X-Macros: https://en.wikipedia.org/wiki/X_Macro
// This X-Macro is used in: Interfac.[ch]. Graphics.[ch], Terrain.[ch], Registrar.c
// To get an idea of how the X-Macro expands in Graphics.c, for instance, run: gcc -E Graphics.c | clang-format
//  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                             |           |                   |                  | W  |                       |       |         |        |         |  S  |  M  |  E  |  I  |                    |                 | D
//                                                             |           |                   |                  | A  |                       |       |         |        |         |  I  |  U  |  X  |  N  |                    |                 | E
//                                                             |           |                   |                  | K  |                       |       |         |        |         |  N  |  L  |  P  |  A  |                    |                 | T
//                                                             |           |                   |                  | A  |                       |       |         |        |         |  G  |  T  |  I  |  N  |                    |                 | A
//                                                             |           |                   |                  | B  |                       |       |         |        |         |  L  |  I  |  R  |  I  |                    |                 | I
//                                                             |           |                   |                  | L  |                       |       |         |        |         |  E  |  S  |  E  |  M  |                    |                 | L
//                                                             |           |                   |                  | E  |                       |       |         |        |         |  F  |  T  |     |  A  |                    |                 |
//                                                             |           |                   |                  |    |                       |       |         |        |         |  R  |  A  |     |  T  |                    |                 |
//                                                             |           |                   |                  |    |                       |       |         |        |         |  A  |  T  |     |  E  |                    |                 |
//                                                             |           |                   |                  |    |                       | MAX   |         |        |         |  M  |  E  |     |     |                    |                 |
//  NAME                                                       | FILE      | UPGRADE           | PRIO             |    | TYPE                  | SPEED | HEALTH  | ATTACK | WIDTH   |  E  |     |     |     | DIMENSIONS         | ACTION          |
//  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define FILE_X_INTERFAC \
    FILE_X(FILE_INTERFAC_NONE,                                   FILE_NONE, FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_INTERFAC_BUILDING_ICONS,                              (66), FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_INTERFAC_COMMAND_ICONS,                               (78), FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_INTERFAC_TECH_ICONS,                                  (83), FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_INTERFAC_UNIT_ICONS,                                  (84), FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_INTERFAC_HUD_0,                                      (158), FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1))
#define FILE_X_TERRAIN \
    FILE_X(FILE_TERRAIN_NONE,                                    FILE_NONE, FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_DIRT,                                                  (0), FILE_GRAPHICS_NONE, FILE_PRIO_TERRAIN,  (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_GRASS,                                                 (1), FILE_GRAPHICS_NONE, FILE_PRIO_TERRAIN,  (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_WATER,                                                 (2), FILE_GRAPHICS_NONE, FILE_PRIO_TERRAIN,  (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FARM,                                                  (3), FILE_GRAPHICS_NONE, FILE_PRIO_TERRAIN,  (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1))
#define FILE_X_GRAPHICS \
    FILE_X(FILE_GRAPHICS_NONE,                                   FILE_NONE, FILE_GRAPHICS_NONE, FILE_PRIO_HIGHEST,  (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_SMALLER_EXPLOSION_SMOKE,                              (21), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_SMOKE,               (0),      (0),    (0),    (0),     (0),  (0),  (1),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_BARRACKS,                          (38), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_BARRACKS,            (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_BARRACKS,                          (42), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_BARRACKS,            (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_NORTH_EUROPE_CASTLE_SHADOW,                           (96), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_4X4, ACTION_NONE,      (1)) \
    FILE_X(FILE_NORTH_EUROPE_CASTLE,                                 (100), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_CASTLE,              (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_4X4, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_FIRE_SMALL_A,                                        (148), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_SMALL_B,                                        (149), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_SMALL_C,                                        (150), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_MEDIUM_A,                                       (151), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_MEDIUM_B,                                       (152), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_LARGE_A,                                        (153), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_LARGE_B,                                        (154), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_LARGE_C,                                        (155), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FIRE_LARGE_D,                                        (156), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_FIRE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_KNIGHT_FIGHTING,                                     (190), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_KNIGHT,              (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_KNIGHT_FALLING,                                      (191), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_KNIGHT,              (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_KNIGHT_IDLE,                                         (192), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_KNIGHT,            (180),    (100),   (13), (3800),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_KNIGHT_DECAYING,                                     (193), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_KNIGHT,              (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_KNIGHT_MOVING,                                       (194), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_KNIGHT,              (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_MILL_SHADOW,                      (216), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_MILL_ANIMATION,                   (219), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_MILL,                             (223), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_MILL,                (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_MILL_SHADOW,                      (227), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (1)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_MILL_ANIMATION,                   (230), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (1)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_MILL,                             (234), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_MILL,                (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_SHADOW,                            (268), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_SHADOW,               (270), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_SHADOW,               (279), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_SHADOW,               (287), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_TOP,                               (269), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_TOWN_CENTER,         (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_TOP,                  (275), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_TOWN_CENTER,         (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_TOP,                  (283), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_TOWN_CENTER,         (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_FLOORING,             (291), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_TOP,                  (295), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_TOWN_CENTER,         (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_RUBBLE_1X1,                                          (299), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_RUBBLE,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_RUBBLE_2X2,                                          (300), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_RUBBLE,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (0), FILE_DIMENSIONS_2X2, ACTION_NONE,      (1)) \
    FILE_X(FILE_RUBBLE_3X3,                                          (301), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_RUBBLE,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (0), FILE_DIMENSIONS_3X3, ACTION_NONE,      (1)) \
    FILE_X(FILE_RUBBLE_4X4,                                          (302), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_RUBBLE,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (0), FILE_DIMENSIONS_4X4, ACTION_NONE,      (1)) \
    FILE_X(FILE_MILITIA_FIGHTING,                                    (321), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_MILITIA,             (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_MILITIA_FALLING,                                     (322), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_MILITIA,             (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_MILITIA_IDLE,                                        (323), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_MILITIA,           (120),    (100),    (5), (2500),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_MILITIA_DECAYING,                                    (324), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_MILITIA,             (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_MILITIA_MOVING,                                      (325), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_MILITIA,             (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_RUBBLE_5X5,                                          (304), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_RUBBLE,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (0), FILE_DIMENSIONS_5X5, ACTION_NONE,      (1)) \
    FILE_X(FILE_STONE_MINE,                                          (334), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (0), TYPE_STONE_MINE,          (0), (999999),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (0)) \
    FILE_X(FILE_TREE_STUMPS,                                         (335), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_RUBBLE,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_TEUTONIC_KNIGHT_FIGHTING,                            (359), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_TEUTONIC_KNIGHT,     (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_TEUTONIC_KNIGHT_FALLING,                             (360), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_TEUTONIC_KNIGHT,     (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_TEUTONIC_KNIGHT_IDLE,                                (361), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_TEUTONIC_KNIGHT,   (100),    (100),   (18), (2500),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_TEUTONIC_KNIGHT_DECAYING,                            (362), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_TEUTONIC_KNIGHT,     (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_TEUTONIC_KNIGHT_MOVING,                              (363), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_TEUTONIC_KNIGHT,     (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_COMMAND,   (0)) \
    FILE_X(FILE_FEMALE_VILLAGER_FIGHTING,                            (402), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_FEMALE_VILLAGER_FALLING,                             (403), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_FEMALE_VILLAGER_IDLE,                                (404), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,          (120),    (100),    (5), (2500),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_FEMALE_VILLAGER_DECAYING,                            (405), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_FEMALE_VILLAGER_MOVING,                              (406), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_FIGHTING,                              (430), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_FALLING,                               (431), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_IDLE,                                  (432), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,          (120),    (100),    (5), (2500),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_DECAYING,                              (433), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_MOVING,                                (434), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_CHOPPING,                   (452), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_CARRYING,                   (453), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_FALLING,                    (454), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_IDLE,                       (455), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_VILLAGER,          (120),    (100),    (5), (2500),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_MALE_VILLAGER_WOODCUTTER_DECAYING,                   (456), FILE_GRAPHICS_NONE, FILE_PRIO_DECAY,    (1), TYPE_VILLAGER,            (0),      (0),    (0),    (0),     (0),  (1),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_BUILD,     (0)) \
    FILE_X(FILE_AGE_1_HOUSE,                                         (568), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_HOUSE,               (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_HOUSE,                            (569), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_HOUSE,               (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_3_NORTH_EUROPE_HOUSE,                            (573), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_HOUSE,               (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_FOREST_TREE_SHADOW,                                  (587), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_BERRY_BUSH,                                          (698), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (0), TYPE_BERRY_BUSH,          (0), (999999),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (0)) \
    FILE_X(FILE_AGE_1_BARRACKS,                                      (729), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_BARRACKS,            (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_3X3, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_WAYPOINT_FLAG,                                       (795), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_RIGHT_CLICK_RED_ARROWS,                              (796), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (1), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (1),  (0), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT,            (798), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A,  (802), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B,  (806), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_MILL_DONKEY,                                   (811), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_NONE,                (0),      (0),    (0),    (0),     (0),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (1)) \
    FILE_X(FILE_AGE_1_MILL,                                          (812), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_MILL,                (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_NORTH_EUROPE_STONE_MINING_CAMP,                      (813), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_MINING_CAMP,         (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_NORTH_EUROPE_LUMBER_CAMP,                            (817), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_LUMBER_CAMP,         (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X2, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_WONDER_BRITONS,                                      (832), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_WONDER,              (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_6X6, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_ROOF_LEFT,                         (870), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_A,               (871), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_B,               (872), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT,            (873), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A,  (877), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B,  (881), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_OUTPOST,                                      (1259), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_OUTPOST,             (0),    (100),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_UNIT_TECH, (0)) \
    FILE_X(FILE_AGE_1_OUTPOST_SHADOW,                               (1274), FILE_GRAPHICS_NONE, FILE_PRIO_SHADOW,   (1), TYPE_SHADOW,              (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_GOLD_MINE,                                          (1297), FILE_GRAPHICS_NONE, FILE_PRIO_UNIT,     (0), TYPE_GOLD_MINE,           (0), (999999),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (0)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_ROOF_RITE,                        (1370), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_A,              (1371), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_B,              (1372), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE,           (1373), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A, (1377), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B, (1381), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE,           (1397), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_2X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A, (1401), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B, (1405), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (1), TYPE_TOWN_CENTER,         (0),      (0),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (1)) \
    FILE_X(FILE_FOREST_TREE,                                        (1410), FILE_GRAPHICS_NONE, FILE_PRIO_BUILDING, (0), TYPE_TREE,                (0), (999999),    (0),    (0),     (1),  (0),  (0),  (1), FILE_DIMENSIONS_1X1, ACTION_NONE,      (0))
