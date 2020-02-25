#pragma once

#include "Unit.h"
#include "Overview.h"
#include "Color.h"
#include "Trigger.h"
#include "Restore.h"
#include "Parts.h"
#include "Grid.h"
#include "Grid.h"
#include "Motive.h"
#include "Field.h"
#include "Map.h"
#include "Points.h"
#include "Packet.h"
#include "Registrar.h"
#include "Stack.h"
#include "Share.h"

typedef struct
{
    Unit* unit;
    Stack* stack;
    Color color;
    Share stamp[COLOR_COUNT];
    int32_t count;
    int32_t max;
    int32_t size;
    int32_t command_group_next;
    int32_t select_count;
    int32_t cpu_count;
    int32_t repath_index;
}
Units;

Units Units_New(const int32_t size, const int32_t cpu_count, const int32_t max, const Color, const Civ);

void Units_Free(const Units);

Stack Units_GetStackCart(const Units, const Point);

Field Units_Field(const Units, const Map);

void Units_ResetTiled(const Units);

Units Units_Generate(Units, const Map, const Grid, const Registrar, const int32_t users);

Units Units_Caretake(Units, const Registrar, const Grid, const Map, const Field);

Units Units_Float(Units, const Units, const Registrar, const Overview, const Grid, const Map, const Motive);

Units Units_PacketService(Units, const Registrar, const Packet, const Grid, const Map, const Field);

uint64_t Units_Xor(const Units);

Point Units_GetFirstTownCenterPan(const Units, const Grid);

Units Units_UnpackRestore(Units, const Restore, const Grid);

Restore Units_PackRestore(const Units, const int32_t cycles);

void Units_FreeAllPaths(const Units);
