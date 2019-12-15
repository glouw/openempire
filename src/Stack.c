#include "Stack.h"

#include "Util.h"

Stack Stack_Build(const int32_t max)
{
    Unit** const reference = UTIL_ALLOC(Unit*, max);
    const Stack stack = { reference, 0, max };
    return stack;
}

void Stack_Append(Stack* const stack, Unit* const unit)
{
    if(stack->count == stack->max)
    {
        stack->max *= 2;
        Unit** const reference = UTIL_REALLOC(stack->reference, Unit*, stack->max);
        stack->reference = reference;
    }
    stack->reference[stack->count++] = unit;
}

void Stack_Free(const Stack stack)
{
    free(stack.reference);
}

bool Stack_IsWalkable(const Stack stack)
{
    for(int32_t i = 0; i < stack.count; i++)
    {
        const Trait trait = stack.reference[i]->trait;
        if(!trait.is_walkable)
            return false;
    }
    return true;
}

int32_t Stack_GetMaxPathIndex(const Stack stack, Unit* const unit)
{
    int32_t max = 0;
    for(int32_t i = 0; i < stack.count; i++)
    {
        Unit* const other = stack.reference[i];
        if(Unit_InPlatoon(unit, other)
        && other->path_index > max)
            max = other->path_index;
    }
    return max;
}
