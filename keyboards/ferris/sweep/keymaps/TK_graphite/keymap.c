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
    NAV_SYMBOL_LAYER,
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
};

//////////////////////////////// KEY OVERRIDES ////////////////////////////////
const key_override_t space_ko         = ko_make_basic(MOD_MASK_SHIFT, KC_SPC, KC_TAB);
const key_override_t vimf_ko          = ko_make_basic(MOD_MASK_SHIFT, VIM_F, VIM_FF);
const key_override_t vimt_ko          = ko_make_basic(MOD_MASK_SHIFT, VIM_T, VIM_TT);
const key_override_t vim_undo_redo_ko = ko_make_with_layers(MOD_MASK_SHIFT, KC_U, LCTL(KC_R), 1 << NAV_LAYER);
const key_override_t vim_ctrlV_ko     = ko_make_with_layers(MOD_MASK_ALT, KC_V, LCTL(KC_V), 1 << NAV_LAYER);

// This globally defines all key overrides to be used
const key_override_t** key_overrides = (const key_override_t*[]){
    &space_ko,
    &vimf_ko,
    &vimt_ko,
    &vim_undo_redo_ko,
    &vim_ctrlV_ko,
    NULL  // Null terminate the array of overrides!
};

//////////////////////////////// COMBOS ///////////////////////////////////////
enum Combos
{
    ENTER_COMBO,
    ESC_COMBO,
    AE_COMBO,
    ESC_LAYER_COMBO,
};
const uint16_t PROGMEM enter_combo[]     = {KC_BSPC, KC_SPC, COMBO_END};
const uint16_t PROGMEM esc_combo[]       = {KC_BSPC, OSM(MOD_LSFT), COMBO_END};
const uint16_t PROGMEM esc_layer_combo[] = {KC_BSPC, TO(ALPHA_LAYER), COMBO_END};
const uint16_t PROGMEM ae_combo[]        = {KC_A, KC_E, COMBO_END};
combo_t key_combos[]                     = {
    [ENTER_COMBO]     = COMBO(enter_combo, KC_ENTER),
    [ESC_COMBO]       = COMBO(esc_combo, KC_ESC),
    [AE_COMBO]        = COMBO(ae_combo, US_AE),
    [ESC_LAYER_COMBO] = COMBO(esc_layer_combo, ESC_ALPHA_LAYER),
};

bool combo_should_trigger(uint16_t combo_index, combo_t* combo, uint16_t keycode, keyrecord_t* record)
{
    switch(combo_index)
    {
    case AE_COMBO:
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
    case NAV_SYMBOL_LAYER:
        if(record->event.pressed)
        {
            if(mods & MOD_MASK_SHIFT)
            {
                layer_move(NAV_LAYER);
                // remove shift otherwise it will affect the vim shortcuts
                unregister_mods(MOD_MASK_SHIFT);
                del_oneshot_mods(MOD_MASK_SHIFT);
            }
            else
            {
                layer_move(SYM_LAYER);
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
            KC_Q,        KC_L,         KC_D,         KC_W,  KC_Z,      KC_SCLN,        KC_F,              KC_O,          KC_U,    KC_J,
            KC_N, LGUI_T(KC_R), LALT_T(KC_T), LCTL_T(KC_S), KC_G,      KC_Y,    RCTL_T(KC_H),     LALT_T(KC_A),  RGUI_T(KC_E),   KC_I,
            KC_B,        KC_X,         KC_M,         KC_C,  KC_V,      KC_K,           KC_P,         DOT_ARROW,       KC_COMM, KC_MINS,

                                          OSM(MOD_LSFT), KC_BSPC,      KC_SPC, NAV_SYMBOL_LAYER),


	[SYM_LAYER] = LAYOUT_split_3x5_2(
            KC_CIRC, KC_TILD, KC_HASH, KC_COLN, KC_GRV,      KC_PIPE,          KC_QUES, KC_SLSH, KC_BSLS, KC_NO,
            KC_AMPR, KC_ASTR, KC_LBRC, KC_LPRN, KC_LCBR,     KC_RCBR,          KC_RPRN, KC_RBRC, KC_DQUO, KC_PLUS,
            KC_DLR,  KC_LT,   KC_GT,   KC_EXLM, KC_PERC,     KC_AT,   TO(ACCENT_LAYER), KC_EQL,  KC_QUOT, TO(FN_LAYER),

                            TO(ALPHA_LAYER), KC_BSPC,        KC_SPC,   TO(NUM_LAYER)),

    [NUM_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_P7, KC_P8, KC_P9, KC_NO,
            KC_PDOT, KC_PSLS, KC_PAST, KC_PMNS, KC_PPLS,    KC_P0, KC_P4, KC_P5, KC_P6, KC_EQL,
            KC_NO,   KC_NO,   KC_NO,   KC_COMM, KC_NO,      KC_NO,   KC_P1, KC_P2, KC_P3, KC_NO,

                               TO(ALPHA_LAYER), KC_BSPC,    KC_SPC, TO(WIN_NAV_LAYER)),

    [NAV_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,      KC_Y,        KC_P,    VIM_F,  KC_LCBR,      LCTL(KC_U),    KC_P2,   KC_P3,   KC_P4, KC_NO,
            KC_W,       KC_B,        KC_E,    VIM_T,  KC_RCBR,      LCTL(KC_D),    KC_LEFT, KC_DOWN, KC_UP, KC_RGHT,
            LSFT(KC_V), LCTL(KC_V),  KC_V,  KC_CIRC,  KC_DLR,       KC_NO,         KC_COMM, KC_SCLN, KC_NO, KC_ESC,

                                    TO(ALPHA_LAYER),  KC_LALT,      KC_LSFT, KC_LCTL),

    [WIN_NAV_LAYER] = LAYOUT_split_3x5_2(
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
            WIN_1, WIN_2, WIN_3, WIN_4, KC_NO,      KC_NO, WIN_5, WIN_6, WIN_7, WIN_8,
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

                     TO(ALPHA_LAYER), KC_LSFT,      ALTTAB, KC_LCTL),

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
            KC_Q, KC_W, KC_E, KC_R, KC_T,      KC_Y, KC_U, KC_I, KC_O, KC_P,
            KC_A, KC_S, KC_D, KC_F, KC_G,      KC_H, KC_J, KC_K, KC_L, KC_NO,
            KC_Z, KC_X, KC_C, KC_V, KC_B,      KC_N, KC_M, KC_NO, KC_NO, KC_ESC,
                         KC_COMM, KC_SPC,      ALTTAB, TO(ALPHA_LAYER)),

    [ACCENT_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_NO, KC_O,  KC_U,  KC_NO,
            KC_CIRC, KC_DQUO, KC_QUOT, KC_GRV,  KC_NO,      KC_NO, KC_NO, KC_A,  KC_E,  KC_I,
            US_SS,   KC_NO,   KC_NO,   US_CCED, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

                             TO(ALPHA_LAYER), KC_BSPC,      KC_SPC, KC_NO),

    [QMK_LAYER] = LAYOUT_split_3x5_2(
            QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, QK_RBT,
            KC_NO,   KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
            EE_CLR,  KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

                         TO(ALPHA_LAYER), KC_NO,      KC_NO, KC_NO)

};
// clang-format on

void keyboard_post_init_user(void)
{
    // Initialize RGB to static black
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_BLACK);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
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
    case FN_LAYER:
        rgblight_setrgb_at(RGB_YELLOW, 0);
        break;
    case MEDIA_LAYER:
        rgblight_setrgb_at(RGB_PINK, 0);
        break;
    case GAMING_LAYER:
        rgblight_setrgb_at(RGB_TEAL, 0);
        break;
    }
}


#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif  // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
