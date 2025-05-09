#include QMK_KEYBOARD_H
#include "features/achordion.h"
#include "keymap_us_international.h"
#include "sendstring_us_international.h"


enum Layers
{
    ALPHA_LAYER,
    SYM_LAYER,
    NUM_LAYER,
    NAV_LAYER,
    WIN_NAV_LAYER,
    FN_LAYER,
    MEDIA_LAYER,
    GAMING_LAYER,
    ACCENT_LAYER,
    QMK_LAYER
};


enum CustomKeycodes
{
    DOT_ARROW = SAFE_RANGE,
    VIM_F,
    VIM_FF,
    VIM_T,
    VIM_TT,
    COPY,
    CUT,
    PASTE,
    UNDO,
    REDO,
    FIND,
    ESC_ALPHA_LAYER,
    RUN,
    ALTTAB,

    WIN_1,
    WIN_2,
    WIN_3,
    WIN_4,
    WIN_5,
    WIN_6,
    WIN_7,
    WIN_8,
    WIN_FULL,
    WIN_MIN,
    WIN_LEFT,
    WIN_RIGHT,
    WIN_SCL,
    WIN_SCR,

    ACC_E,
    ACC_A,
    ACC_I,
    ACC_O,
    ACC_U,
};

#define SYM_WIN_LAYER LT(0, KC_1)
#define NAV_HOLD      LT(NAV_LAYER, KC_SPC)

//////////////////////////////// KEY OVERRIDES ////////////////////////////////
const key_override_t space_ko         = ko_make_basic(MOD_MASK_SHIFT, NAV_HOLD, KC_TAB);
const key_override_t vimf_ko          = ko_make_basic(MOD_MASK_SHIFT, VIM_F, VIM_FF);
const key_override_t vimt_ko          = ko_make_basic(MOD_MASK_SHIFT, VIM_T, VIM_TT);
const key_override_t vim_undo_redo_ko = ko_make_with_layers(MOD_MASK_SHIFT, KC_U, LCTL(KC_R), 1 << NAV_LAYER);
const key_override_t vim_ctrlV_ko     = ko_make_with_layers(MOD_MASK_ALT, KC_V, LCTL(KC_V), 1 << NAV_LAYER);

// This globally defines all key overrides to be used
const key_override_t* key_overrides[] = {
    &space_ko,
    &vimf_ko,
    &vimt_ko,
    &vim_ctrlV_ko,
    &vim_undo_redo_ko,
};

//////////////////////////////// COMBOS ///////////////////////////////////////
#ifdef COMB
#undef COMB
#endif

#define COMB(name, action, ...) C_##name,
enum myCombos
{
#include "combos.def"
};
#undef COMB

#define COMB(name, action, ...) const uint16_t PROGMEM name##_combo[] = {__VA_ARGS__, COMBO_END};
#include "combos.def"
#undef COMB

#define COMB(name, action, ...) [C_##name] = COMBO(name##_combo, action),
combo_t key_combos[] = {
#include "combos.def"
};
#undef COMB

#ifdef COMBO_TERM_PER_COMBO
uint16_t get_combo_term(uint16_t combo_index, combo_t* combo)
{
    switch(combo_index)
    {
    case C_enter:
    case C_enter_gaming:
        return 50;
    }

    return COMBO_TERM;
}
#endif

bool combo_should_trigger(uint16_t combo_index, combo_t* combo, uint16_t keycode, keyrecord_t* record)
{
    switch(combo_index)
    {
    case C_ae:
        // Only trigger ae combo on the accent layer.
        if(!layer_state_is(ACCENT_LAYER))
        {
            return false;
        }
    }

    return true;
}

//////////////////////////////// TAP-HOLD /////////////////////////////////////
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t* record)
{
    switch(keycode)
    {
    case LGUI_T(KC_R):
    case RGUI_T(KC_E):
        return TAPPING_TERM + 100;
    default:
        return TAPPING_TERM;
    }
}


//////////////////////////////// ACHORDION ////////////////////////////////////

bool achordion_chord(uint16_t tap_hold_keycode,
                     keyrecord_t* tap_hold_record,
                     uint16_t other_keycode,
                     keyrecord_t* other_record)
{
    if(tap_hold_keycode == SYM_WIN_LAYER)
    {
        // if the other key is on the home row, then consider it a hold (this is where the buttons for switching windows are on WIN_NAV_LAYER)
        // this avoids having to hold for a long time when switching to windows that have the key on the same side as the layer switch key
        if(other_record->event.key.row % (MATRIX_ROWS / 2) == 1)
        {
            return true;
        }
    }
    if(tap_hold_keycode == NAV_HOLD)
        return true;
    if(other_keycode == OSM(MOD_LSFT))
        return true;
    return achordion_opposite_hands(tap_hold_record, other_record);
}

uint16_t achordion_timeout(uint16_t tap_hold_keycode)
{
    return 800;
}

bool achordion_eager_mod(uint8_t mod)
{
    switch(mod)
    {
    case MOD_LSFT:
    case MOD_RSFT:
    case MOD_LCTL:
    case MOD_RCTL:
        return true;  // Eagerly apply Shift and Ctrl mods.

    default:
        return false;
    }
}
uint16_t achordion_streak_timeout(uint16_t tap_hold_keycode)
{
    if(IS_QK_LAYER_TAP(tap_hold_keycode))
    {
        return 0;  // Disable streak detection on layer-tap keys.
    }

    // Otherwise, tap_hold_keycode is a mod-tap key.
    return 100;  // default
}

///////////////////////////////////////////////////////////////////////////////
bool is_alt_tab_active = false;  // ADD this near the beginning of keymap.c
uint16_t alt_tab_timer = 0;      // we will be using them soon.
                                 //
bool process_record_user(uint16_t keycode, keyrecord_t* record)
{
    if(!process_achordion(keycode, record))
    {
        return false;
    }

    const uint8_t hold_mods    = get_mods();
    const uint8_t oneshot_mods = get_oneshot_mods();
    const uint8_t mods         = hold_mods | oneshot_mods;
    switch(keycode)
    {
    // These become dead keys with US International layout but i only want that on the accent layer, so just send a space after the dead key
    // if it is not the accent layer
    case KC_QUOT:
    case KC_GRV:
    case KC_DQUO:
    case KC_TILD:
    case KC_CIRC:
        if(record->event.pressed)
        {
            if(!layer_state_is(ACCENT_LAYER))
            {
                tap_code16(keycode);
                tap_code16(KC_SPC);
            }
            else
            {
                tap_code16(keycode);
            }
        }
        return false;
    case KC_COMM:
        if(record->event.pressed)
        {
            if((mods & MOD_MASK_SHIFT) && layer_state_is(ALPHA_LAYER))
            {
                del_oneshot_mods(MOD_MASK_SHIFT);
                unregister_mods(MOD_MASK_SHIFT);
                tap_code16(KC_QUOT);
                tap_code16(KC_SPC);
                register_mods(hold_mods);  // Restore mods.
            }
            else
            {
                tap_code16(KC_COMM);
            }
        }
        return false;
    // case NAV_SYMBOL_LAYER:
    //     if(record->event.pressed)
    //     {
    //         if(mods & MOD_MASK_SHIFT)
    //         {
    //             layer_move(NAV_LAYER);
    //             // remove shift otherwise it will affect the vim shortcuts
    //             unregister_mods(MOD_MASK_SHIFT);
    //             del_oneshot_mods(MOD_MASK_SHIFT);
    //         }
    //         else
    //         {
    //             layer_move(SYM_LAYER);
    //         }
    //     }
    //     return false;
    case SYM_WIN_LAYER:
        if(record->tap.count > 0)  // tap
        {
            if(record->event.pressed)
            {
                // tap press
                layer_move(SYM_LAYER);
            }
            else
            {
                // tap release
            }
        }
        else  // hold
        {
            // emulate a momentary layer hold key, like MO
            if(record->event.pressed)
            {
                // hold press
                layer_move(WIN_NAV_LAYER);
            }
            else
            {
                // hold release
                layer_move(ALPHA_LAYER);
            }
        }
        return false;
    case ESC_ALPHA_LAYER:
        if(record->event.pressed)
        {
            layer_move(ALPHA_LAYER);
            tap_code16(KC_ESC);
        }
        return false;
    case DOT_ARROW:
        if(record->event.pressed)
        {
            if(mods & MOD_MASK_SHIFT)
            {  // Is shift held?
                // Temporarily delete shift.
                del_oneshot_mods(MOD_MASK_SHIFT);
                unregister_mods(MOD_MASK_SHIFT);
                SEND_STRING("->");
                register_mods(hold_mods);  // Restore mods.
            }
            else
            {
                SEND_STRING(".");
            }
        }
        return false;
    case VIM_F:
        if(record->event.pressed)
        {
            SEND_STRING("f");
            layer_move(ALPHA_LAYER);
        }
        return false;
    case VIM_FF:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LSFT("f"));
            layer_move(ALPHA_LAYER);
        }
        return false;
    case VIM_T:
        if(record->event.pressed)
        {
            SEND_STRING("t");
            layer_move(ALPHA_LAYER);
        }
        return false;
    case VIM_TT:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LSFT("t"));
            layer_move(ALPHA_LAYER);
        }
        return false;
    case COPY:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL("c"));
        }
        return false;
    case CUT:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL("x"));
        }
        return false;
    case PASTE:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL("v"));
        }
        return false;
    case UNDO:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL("z"));
        }
        return false;
    case REDO:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL("y"));
        }
        return false;
    case FIND:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL("f"));
            layer_move(ALPHA_LAYER);
        }
        return false;
    case RUN:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LCTL(SS_LSFT(SS_LGUI(SS_LALT(" ")))));
            layer_move(ALPHA_LAYER);
        }
        return false;
    case ALTTAB:
        if(record->event.pressed)
        {
            if(!is_alt_tab_active)
            {
                is_alt_tab_active = true;
                register_code(KC_LALT);
            }
            alt_tab_timer = timer_read();
            register_code(KC_TAB);
        }
        else
        {
            unregister_code(KC_TAB);
        }
        return true;

    case WIN_1:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("1"));
        }
        return false;
    case WIN_2:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("2"));
        }
        return false;
    case WIN_3:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("3"));
        }
        return false;
    case WIN_4:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("4"));
        }
        return false;
    case WIN_5:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("5"));
        }
        return false;
    case WIN_6:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("6"));
        }
        return false;
    case WIN_7:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("7"));
        }
        return false;
    case WIN_8:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI("8"));
        }
        return false;
    case WIN_FULL:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI(SS_TAP(X_UP)));
        }
        return false;
    case WIN_MIN:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI(SS_TAP(X_DOWN)));
        }
        return false;
    case WIN_LEFT:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI(SS_TAP(X_LEFT)));
        }
        return false;
    case WIN_RIGHT:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI(SS_TAP(X_RIGHT)));
        }
        return false;
    case WIN_SCL:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_LEFT))));
        }
        return false;
    case WIN_SCR:
        if(record->event.pressed)
        {
            SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_RIGHT))));
        }
        return false;

    case ACC_E:
        if(record->event.pressed)
        {
            tap_code(KC_E);
            layer_move(ALPHA_LAYER);
        }
        return false;
    case ACC_A:
        if(record->event.pressed)
        {
            tap_code(KC_A);
            layer_move(ALPHA_LAYER);
        }
        return false;
    case ACC_O:
        if(record->event.pressed)
        {
            tap_code(KC_O);
            layer_move(ALPHA_LAYER);
        }
        return false;
    case ACC_U:
        if(record->event.pressed)
        {
            tap_code(KC_U);
            layer_move(ALPHA_LAYER);
        }
        return false;
    case ACC_I:
        if(record->event.pressed)
        {
            tap_code(KC_I);
            layer_move(ALPHA_LAYER);
        }
        return false;
    default:
        return true;
    }
    return true;
}

void matrix_scan_user(void)
{
    if(is_alt_tab_active)
    {
        if(timer_elapsed(alt_tab_timer) > 1000)
        {
            unregister_code(KC_LALT);
            is_alt_tab_active = false;
        }
    }
    achordion_task();
}
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[ALPHA_LAYER] = LAYOUT_split_3x5_2(
            KC_Q,         KC_L,         KC_D,         KC_W,             KC_Z,                           KC_SCLN,        KC_F,          KC_O,          KC_U,          KC_J,
            MEH_T(KC_N),  LGUI_T(KC_R), LALT_T(KC_T), LCTL_T(KC_S),     LT(NUM_LAYER, KC_G),            KC_Y,    RCTL_T(KC_H),  LALT_T(KC_A),  RGUI_T(KC_E),   MEH_T(KC_I),
            KC_B,         KC_X,         KC_M,         KC_C,             KC_V,                           KC_K,           KC_P,     DOT_ARROW,       KC_COMM,        KC_MINS,

                                                         OSM(MOD_LSFT), KC_BSPC,                        NAV_HOLD,  SYM_WIN_LAYER),


	[SYM_LAYER] = LAYOUT_split_3x5_2(
            KC_CIRC, KC_TILD, KC_HASH, KC_COLN, KC_GRV,      KC_SCLN, KC_PERC, KC_SLSH, KC_BSLS, KC_NO,
            KC_AMPR, KC_ASTR, KC_LBRC, KC_LPRN, KC_LCBR,     KC_RCBR, KC_RPRN, KC_RBRC, KC_DQUO, KC_PLUS,
            KC_DLR,  KC_LT,   KC_GT,   KC_EXLM, KC_PIPE,     KC_AT,   KC_QUES, KC_EQL,  KC_QUOT, TO(FN_LAYER),

                            TO(ALPHA_LAYER), KC_BSPC,        NAV_HOLD,   TO(ACCENT_LAYER)),

    [NUM_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_PPLS,   KC_7, KC_8, KC_9, KC_NO,
            KC_DOT,  KC_PSLS, KC_PAST, KC_PMNS, KC_PPLS,    KC_0,      KC_4, KC_5, KC_6, KC_EQL,
            KC_NO,   KC_NO,   KC_COMM, KC_COMM, KC_NO,      KC_PMNS,   KC_1, KC_2, KC_3, KC_NO,

                               TO(ALPHA_LAYER), KC_BSPC,    NAV_HOLD, TO(SYM_LAYER)),

    [NAV_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,      KC_Y,        KC_P,  KC_LSFT,  KC_LCBR,      LCTL(KC_U),    KC_P2,   KC_P3,   KC_P4, KC_NO,
            KC_W,       KC_B,        KC_E,  KC_LCTL,  KC_RCBR,      LCTL(KC_D),    KC_LEFT, KC_DOWN, KC_UP, KC_RGHT,
            LSFT(KC_V), LCTL(KC_V),  KC_V,  KC_CIRC,  KC_DLR,       KC_NO,         KC_COMM, KC_SCLN, KC_NO, KC_ESC,

                                    TO(ALPHA_LAYER),  KC_LALT,      KC_NO, KC_NO),

    [WIN_NAV_LAYER] = LAYOUT_split_3x5_2(
            KC_NO, WIN_MIN,  WIN_FULL,  KC_NO,  KC_NO,      KC_NO,   KC_NO, KC_NO, KC_NO, KC_NO,
            WIN_1, WIN_2,    WIN_3,     WIN_4,  WIN_SCL,    WIN_SCR, WIN_5, WIN_6, WIN_7, WIN_8,
            KC_NO, WIN_LEFT, WIN_RIGHT, ALTTAB, KC_NO,      KC_NO,   KC_NO, KC_NO, KC_NO, KC_NO,

                            LCTL(KC_LSFT), RUN,      KC_NO, KC_NO),

    [FN_LAYER] = LAYOUT_split_3x5_2(
            TO(QMK_LAYER),   KC_NO,  KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_F7, KC_F8, KC_F9, KC_F12,
            UNDO,            CUT,    COPY,    PASTE,   FIND,       KC_NO, KC_F4, KC_F5, KC_F6, KC_F11,
            KC_NO,           KC_NO,  KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_F1, KC_F2, KC_F3, KC_F10,

                                    TO(ALPHA_LAYER),   RUN,        TO(GAMING_LAYER), TO(MEDIA_LAYER)),

    [MEDIA_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_VOLU, KC_NO,   KC_NO,      KC_NO, KC_NO,   KC_MS_BTN3, KC_NO,   KC_NO,
            KC_MUTE, KC_MPRV, KC_MPLY, KC_MNXT, KC_NO,      KC_NO, KC_MS_L, KC_MS_D,    KC_MS_U, KC_MS_R,
            KC_NO,   KC_NO,   KC_VOLD, KC_NO,   KC_NO,      KC_NO, KC_WH_L, KC_WH_D,    KC_WH_U, KC_WH_R,

                          TO(ALPHA_LAYER), KC_MS_BTN1,      KC_MS_BTN2, KC_NO),

    [GAMING_LAYER] = LAYOUT_split_3x5_2(
            KC_Q, KC_L, KC_D, KC_W, KC_Z,      TO(ALPHA_LAYER),  KC_F, KC_O, KC_U, KC_J,
            KC_N, KC_R, KC_T, KC_S, KC_G,      KC_Y,     KC_H, KC_A, KC_E, KC_I,
            KC_B, KC_X, KC_M, KC_C, KC_V,      KC_K,     KC_P, ALTTAB, KC_SLSH, KC_ESC,
                         KC_COMM, KC_BSPC,     KC_SPC,  LT(NUM_LAYER, KC_ENTER)),

    [ACCENT_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_NO, ACC_O,  ACC_U,  KC_NO,
            KC_DQUO, KC_CIRC, KC_QUOT, KC_GRV,  KC_NO,      KC_NO, KC_NO, ACC_A,  ACC_E,  ACC_I,
            US_SS,   KC_NO,   KC_NO,   US_CCED, KC_NO,      KC_NO, KC_NO, KC_NO,  KC_NO,  KC_NO,

                             TO(ALPHA_LAYER), KC_BSPC,      KC_SPC, OSM(MOD_LSFT)),

    [QMK_LAYER] = LAYOUT_split_3x5_2(
            QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, QK_RBT,
            KC_NO,   KC_NO, KC_NO, KC_NO, UG_TOGG,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
            EE_CLR,  KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

                         TO(ALPHA_LAYER), KC_NO,      KC_NO, KC_NO)

};
// clang-format on

// turn off power led
void keyboard_pre_init_user(void)
{
    // Set our LED pin as output
    setPinOutput(24);
    // Turn the LED off
    // (Due to technical reasons, high is off and low is on)
    writePinHigh(24);
}
void keyboard_post_init_user(void)
{
    // Initialize RGB to static black
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_BLACK);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}
void oneshot_mods_changed_user(uint8_t mods)
{
    if(mods & MOD_MASK_SHIFT)
    {
        setPinOutput(24);
        // Turn the LED on
        // (Due to technical reasons, high is off and low is on)
        writePinLow(24);
    }
    else
    {
        if(!is_caps_word_on())
        {
            setPinOutput(24);
            // Turn the LED off
            // (Due to technical reasons, high is off and low is on)
            writePinHigh(24);
        }
    }
}
void caps_word_set_user(bool active)
{
    if(active)
    {
        // Do something when Caps Word activates.
        setPinOutput(24);
        // Turn the LED on
        // (Due to technical reasons, high is off and low is on)
        writePinLow(24);
    }
    else
    {
        // Do something when Caps Word deactivates.
        setPinOutput(24);
        // Turn the LED off
        // (Due to technical reasons, high is off and low is on)
        writePinHigh(24);
    }
}
void housekeeping_task_user(void)
{
    switch(get_highest_layer(layer_state | default_layer_state))
    {
    case ALPHA_LAYER:
        // Default layer
        rgblight_setrgb_at(RGB_BLACK, 0);
        break;
    case SYM_LAYER:
        rgblight_setrgb_at(RGB_RED, 0);
        break;
    case NUM_LAYER:
        rgblight_setrgb_at(RGB_GREEN, 0);
        break;
    case NAV_LAYER:
        rgblight_setrgb_at(RGB_BLUE, 0);
        break;
    case WIN_NAV_LAYER:
        rgblight_setrgb_at(RGB_PURPLE, 0);
        break;
    case FN_LAYER:
        rgblight_setrgb_at(RGB_YELLOW, 0);
        break;
    case MEDIA_LAYER:
        rgblight_setrgb_at(RGB_PINK, 0);
        break;
    case GAMING_LAYER:
        rgblight_setrgb_at(RGB_TEAL, 0);
        break;
    case ACCENT_LAYER:
        rgblight_setrgb_at(RGB_ORANGE, 0);
        break;
    case QMK_LAYER:
        rgblight_setrgb_at(RGB_WHITE, 0);
        break;
    }
}
