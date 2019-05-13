#include "Stack.h"

#include "Util.h"

Stack Stack_Build(const int32_t max)
{
    Unit** const reference = UTIL_ALLOC(Unit*, max);
    UTIL_CHECK(reference);
    const Stack stack = { reference, 0, max };
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
