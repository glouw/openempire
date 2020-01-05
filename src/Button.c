#include "Button.h"

#include "Buttons.h"
#include "Util.h"

static const char hotkeys[] = { 'Q', 'W', 'E', 'R', 'T', 'A', 'S', 'D', 'F', 'G', 'Z', 'X', 'C', 'V', 'B' };

Button Button_FromOverview(const Overview overview)
{
    const Buttons buttons = Buttons_FromMotive(overview.share.motive, overview.share.status.age);
    int32_t index = -1;
    if(overview.event.key_q) index =  0;
    if(overview.event.key_w) index =  1;
    if(overview.event.key_e) index =  2;
    if(overview.event.key_r) index =  3;
    if(overview.event.key_t) index =  4;
    if(overview.event.key_a) index =  5;
    if(overview.event.key_s) index =  6;
    if(overview.event.key_d) index =  7;
    if(overview.event.key_f) index =  8;
    if(overview.event.key_g) index =  9;
    if(overview.event.key_z) index = 10;
    if(overview.event.key_x) index = 11;
    if(overview.event.key_c) index = 12;
    if(overview.event.key_v) index = 13;
    if(overview.event.key_b) index = 14;
    static Button zero;
    return Buttons_IsIndexValid(buttons, index) ? buttons.button[index] : zero;
}

const char* Button_GetHotkeys(void)
{
    return hotkeys;
}

int32_t Button_GetHotkeysLen(void)
{
    return UTIL_LEN(hotkeys);
}
