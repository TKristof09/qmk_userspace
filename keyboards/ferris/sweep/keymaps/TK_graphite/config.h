#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD
#undef WS2812_DI_PIN
#define WS2812_DI_PIN 25
#undef RGBLED_NUM
#define RGBLED_NUM 2
#undef RGBLED_SPLIT
#define RGBLED_SPLIT \
    {                \
        1, 1         \
    }

#define MOUSEKEY_INTERVAL    16
#define MOUSEKEY_MAX_SPEED   7
#define MOUSEKEY_TIME_TO_MAX 60

#define TAPPING_TERM 300
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define QUICK_TAP_TERM 0
#define ACHORDION_STREAK
