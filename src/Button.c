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
    const int32_t index = Event_GetIndex(overview.event);
    const Buttons buttons = Buttons_FromMotive(overview.share.motive, overview.share.status.age);
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
