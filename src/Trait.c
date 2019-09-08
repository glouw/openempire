#include "Trait.h"

#include "Graphics.h"

Trait Trait_Build(const Graphics file)
{
    static Trait zero;
    Trait trait = zero;
    trait.max_speed = Graphics_GetMaxSpeed(file);
    trait.accel = Graphics_GetAcceleration(file);
    trait.file_name = Graphics_GetString(file);
    trait.max_health = Graphics_GetHealth(file);
    trait.attack = Graphics_GetAttack(file);
    trait.is_rotatable = Graphics_GetRotatable(file);
    trait.is_single_frame = Graphics_GetSingleFrame(file);
    trait.is_walkable = Graphics_GetWalkable(file);
    trait.width = Graphics_GetWidth(file);
    trait.type = Graphics_GetType(file);
    trait.is_multi_state = Graphics_GetMultiState(file);
    trait.can_expire = Graphics_GetExpire(file);
    return trait;
}
