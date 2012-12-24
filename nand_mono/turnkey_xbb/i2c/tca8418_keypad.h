#ifndef _TCA8418_KEYPAD_H
#define _TCA8418_KEYPAD_H

// Key value definition. Used as a combination with key definition in define.h
#define KEY_VALUE_NONE          0
#define KEY_VALUE_FM            0x21
#define KEY_VALUE_OPERA         0x22
#define KEY_VALUE_PLAY          0x23
#define KEY_VALUE_MUSIC         0x24
#define KEY_VALUE_FICTION       0x25
#define KEY_VALUE_PREV          0x26
#define KEY_VALUE_CITY          0x27
#define KEY_VALUE_COMIC         0x28
#define KEY_VALUE_NEXT          0x29
#define KEY_VALUE_SONG          0x2A
#define KEY_VALUE_STORY         0x2B
#define KEY_VALUE_FORWARD       0x2C
#define KEY_VALUE_LECTURE       0x2D
#define KEY_VALUE_MISC          0x2E
#define KEY_VALUE_BACKWARD      0x2F
#define KEY_VALUE_HEALTH        0x30

extern uint8 tca8418_get_real_key (void);
void tca8418_init(void);

#if (TCA8418_TEST == FEATURE_ON)
void tca8418_test();
#endif

#endif
