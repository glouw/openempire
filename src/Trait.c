#include "Trait.h"

#include "Graphics.h"
#include "Interfac.h"

Trait Trait_Build(const Graphics file)
{
    static Trait zero;
    Trait trait = zero;
    trait.file_name = Graphics_GetString(file);
    trait.dimensions = Graphics_GetDimensions(file);
    trait.type = Graphics_GetType(file);
    trait.max_speed = Graphics_GetMaxSpeed(file);
    trait.max_health = Graphics_GetHealth(file);
    trait.attack = Graphics_GetAttack(file);
    trait.width = Graphics_GetWidth(file);
    trait.action = Graphics_GetAction(file);
    trait.is_single_frame = Graphics_GetSingleFrame(file);
    trait.is_walkable = Graphics_GetWalkable(file);
    trait.is_multi_state = Graphics_GetMultiState(file);
    trait.is_inanimate = Graphics_GetInanimate(file);
    trait.can_expire = Graphics_GetExpire(file);
    trait.is_detail = Graphics_GetDetail(file);
    trait.needs_midding = Graphics_GetMidding(file);
    return trait;
}
