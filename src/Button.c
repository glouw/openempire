#include "Button.h"

#include "Buttons.h"
#include "Util.h"

static const Trigger research_long_swordsman[] = {
    TRIGGER_UPGRADE_MILITIA,
    TRIGGER_AGE_UP_3
};
static const Trigger research_two_handed_swordsman[] = {
    TRIGGER_UPGRADE_MAN_AT_ARMS,
    TRIGGER_AGE_UP_4
};
static const Trigger research_champion[] = {
    TRIGGER_UPGRADE_LONG_SWORDSMAN,
    TRIGGER_AGE_UP_4
};
static const Trigger icon_age_3[] = {
    TRIGGER_AGE_UP_2
};
static const Trigger icon_age_4[] = {
    TRIGGER_AGE_UP_3
};
static const Trigger icon_man_at_arms[] = {
    TRIGGER_UPGRADE_MILITIA
};
static const Trigger icon_long_swordsman[] = {
    TRIGGER_UPGRADE_MAN_AT_ARMS
};
static const Trigger icon_two_handed_swordsman[] = {
    TRIGGER_UPGRADE_LONG_SWORDSMAN
};
static const Trigger icon_champion[] = {
    TRIGGER_UPGRADE_TWO_HANDED_SWORDSMAN
};
static const char hotkeys[] = {
    'Q', 'W', 'E', 'R', 'T', 'A', 'S', 'D', 'F', 'G', 'Z', 'X', 'C', 'V', 'B'
};

static bool Triggered(const Bits bits, const Trigger triggers[], const int32_t len)
{
    for(int32_t i = 0; i < len; i++)
        if(!Bits_Get(bits, triggers[i]))
            return false;
    return true;
}

#define TRIGGERED(bits, triggers) Triggered(bits, triggers, UTIL_LEN(triggers))

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

Button Next(Button button, const Trigger trigger, const int32_t index)
{
    button.index = index;
    button.trigger = trigger;
    return button;
}

// BUTTON INDEX REFERENCES DEFAULT BUTTON STARTING POINT DEFINED IN BUTTONS.C
Button Button_Upgrade(Button button, const Bits bits)
{
    switch(button.icon_type)
    {
    case ICONTYPE_TECH:
        if(button.index == ICONTECH_AGE_2)
        {
            if(TRIGGERED(bits, icon_age_3))
                button = Next(button, TRIGGER_AGE_UP_3, ICONTECH_AGE_3);
            if(TRIGGERED(bits, icon_age_4))
                button = Next(button, TRIGGER_AGE_UP_4, ICONTECH_AGE_4);
        }
        if(button.index == ICONTECH_RESEARCH_MAN_AT_ARMS)
        {
            if(TRIGGERED(bits, research_long_swordsman))
                button = Next(button, TRIGGER_UPGRADE_MAN_AT_ARMS, ICONTECH_RESEARCH_LONG_SWORDSMAN);
            if(TRIGGERED(bits, research_two_handed_swordsman))
                button = Next(button, TRIGGER_UPGRADE_LONG_SWORDSMAN, ICONTECH_RESEARCH_TWO_HANDED_SWORDSMAN);
            if(TRIGGERED(bits, research_champion))
                button = Next(button, TRIGGER_UPGRADE_TWO_HANDED_SWORDSMAN, ICONTECH_RESEARCH_CHAMPION);
        }
        break;
    case ICONTYPE_UNIT:
        if(button.index == ICONUNIT_MILITIA)
        {
            if(TRIGGERED(bits, icon_man_at_arms))
                button = Next(button, TRIGGER_NONE, ICONUNIT_MAN_AT_ARMS);
            if(TRIGGERED(bits, icon_long_swordsman))
                button = Next(button, TRIGGER_NONE, ICONUNIT_LONG_SWORDSMAN);
            if(TRIGGERED(bits, icon_two_handed_swordsman))
                button = Next(button, TRIGGER_NONE, ICONUNIT_TWO_HANDED_SWORDSMAN);
            if(TRIGGERED(bits, icon_champion))
                button = Next(button, TRIGGER_NONE, ICONUNIT_CHAMPION);
        }
        break;
    }
    return button;
}
