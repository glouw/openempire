#include "Stack.h"

#include "Util.h"

Stack Stack_Build(const int32_t max)
{
    static Point zero;
    Unit** const reference = UTIL_ALLOC(Unit*, max);
    UTIL_CHECK(reference);
    const Stack stack = { reference, zero, 0, max };
    return stack;
}

void Stack_Append(Stack* const stack, Unit* const unit)
{
    if(stack->count == stack->max)
    {
        stack->max *= 2;
        Unit** const reference = UTIL_REALLOC(stack->reference, Unit*, stack->max);
        UTIL_CHECK(reference);
        stack->reference = reference;
    }
    stack->reference[stack->count++] = unit;
}

void Stack_Free(const Stack stack)
{
    free(stack.reference);
}

static int32_t CompareByY(const void* a, const void* b)
{
    Unit* const aa = *((Unit**) a);
    Unit* const bb = *((Unit**) b);
    const Point pa = Point_ToIso(aa->cart_grid_offset);
    const Point pb = Point_ToIso(bb->cart_grid_offset);
    return pa.y < pb.y;
}

void Stack_Sort(const Stack stack)
{
    if(stack.count > 1)
        qsort(stack.reference, stack.count, sizeof(*stack.reference), CompareByY);
}

bool Stack_IsWalkable(const Stack stack)
{
    for(int32_t i = 0; i < stack.count; i++)
        if(!Graphics_IsWalkable(stack.reference[i]->file))
            return false;
    return true;
}

void Stack_UpdateCenterOfMass(Stack* const stack)
{
    static Point zero;
    Point out = zero;
    int32_t count = 0;
    for(int32_t i = 0; i < stack->count; i++)
    {
        Unit* const unit = stack->reference[i];
        if(!State_IsDead(unit->state))
        {
            out = Point_Add(out, unit->cell);
            count++;
        }
    }
    stack->center_of_mass = (count > 0) ? Point_Div(out, count) : out;
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
