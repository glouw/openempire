#include "Button.h"

#include "Buttons.h"
#include "Util.h"

static const char hotkeys[] = {
    'Q', 'W', 'E', 'R', 'T',
    'A', 'S', 'D', 'F', 'G',
    'Z', 'X', 'C', 'V', 'B',
};

Button Button_FromOverview(const Overview overview)
{
    const Buttons buttons = Buttons_FromMotive(overview.incoming.motive, overview.incoming.status.age);
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
    return Buttons_IsIndexValid(buttons, index)
        ? buttons.button[index]
        : zero;
}

const char* Button_GetHotkeys(void)
{
    return hotkeys;
}

int32_t Button_GetHotkeysLen(void)
{
    return UTIL_LEN(hotkeys);
}

static Button Next(Button button, const Trigger trigger, const int32_t index)
{
    button.index = index;
    button.trigger = trigger;
    return button;
}

// TECH TREE BUTTON UPGRADES.
// BUTTON INDICES REFERENCE THE BASE BUTTON DEFINED IN BUTTONS.C
Button Button_Upgrade(Button button, const Bits bits)
{
    switch(button.icon_type)
    {
    case ICONTYPE_TECH:
        // TOWN CENTER.
        if(button.index == ICONTECH_AGE_2)
            if(Bits_Get(bits, TRIGGER_AGE_UP_2))
                button = Next(button, TRIGGER_AGE_UP_3, ICONTECH_AGE_3);
        if(button.index == ICONTECH_RESEARCH_MAN_AT_ARMS)
            if(Bits_Get(bits, TRIGGER_AGE_UP_3)
            && Bits_Get(bits, TRIGGER_UPGRADE_MILITIA))
                button = Next(button, TRIGGER_UPGRADE_MAN_AT_ARMS, ICONTECH_RESEARCH_LONG_SWORDSMAN);
        break;
    case ICONTYPE_UNIT:
        // BARRACKS.
        if(button.index == ICONUNIT_MILITIA)
        {
            if(Bits_Get(bits, TRIGGER_AGE_UP_2)
            && Bits_Get(bits, TRIGGER_UPGRADE_MILITIA))
                button = Next(button, TRIGGER_NONE, ICONUNIT_MAN_AT_ARMS);
            if(Bits_Get(bits, TRIGGER_AGE_UP_3)
            && Bits_Get(bits, TRIGGER_UPGRADE_MAN_AT_ARMS))
                button = Next(button, TRIGGER_NONE, ICONUNIT_LONG_SWORDSMAN);
        }
        if(button.index == ICONUNIT_SPEARMAN)
            if(Bits_Get(bits, TRIGGER_AGE_UP_3)
            && Bits_Get(bits, TRIGGER_UPGRADE_SPEARMAN))
                button = Next(button, TRIGGER_NONE, ICONUNIT_PIKEMAN);
        break;
    case ICONTYPE_NONE:
    case ICONTYPE_COMMAND:
    case ICONTYPE_BUILD:
    case ICONTYPE_COUNT:
        break;
    }
    return button;
}

bool Button_UseAttackMove(const Button button)
{
    return button.icon_type    == ICONTYPE_COMMAND
        && button.icon_command == ICONCOMMAND_ATTACK_MOVE;
}
