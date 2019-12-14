#pragma once

#include "Civ.h"
#include "File.h"
#include "Type.h"
#include "Point.h"
#include "Action.h"

#include <stdbool.h>

typedef enum
{
#define FILE_X(name, file, upgrade, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, dimensions, action, detail) name = file,
    FILE_X_GRAPHICS
#undef FILE_X
}
Graphics;

const char* Graphics_GetString(const Graphics);

uint8_t Graphics_GetHeight(const Graphics);

bool Graphics_GetWalkable(const Graphics);

Type Graphics_GetType(const Graphics);

int32_t Graphics_GetMaxSpeed(const Graphics);

int32_t Graphics_GetHealth(const Graphics);

int32_t Graphics_GetAttack(const Graphics);

int32_t Graphics_GetWidth(const Graphics);

bool Graphics_GetSingleFrame(const Graphics);

bool Graphics_GetMultiState(const Graphics);

bool Graphics_GetExpire(const Graphics);

Point Graphics_GetDimensions(const Graphics);

bool Graphics_GetInanimate(const Graphics);

Action Graphics_GetAction(const Graphics);

bool Graphics_GetDetail(const Graphics);

Graphics Graphics_GetUpgrade(const Graphics, const Civ);
