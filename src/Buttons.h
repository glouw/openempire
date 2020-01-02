#pragma once

#include "Button.h"

#include "Motive.h"
#include "Age.h"

#include <stdint.h>

typedef struct
{
    const Button* button;
    int32_t count;
}
Buttons;

Buttons Buttons_FromMotive(const Motive, const Age);

bool Buttons_IsIndexValid(const Buttons, const int32_t index);
