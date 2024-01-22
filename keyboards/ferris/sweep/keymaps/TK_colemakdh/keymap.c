#include QMK_KEYBOARD_H

enum Layers
{
    ALPHA_LAYER,
    SYM_LAYER,
    NUM_LAYER,
    NAV_LAYER,
    FN_LAYER,
    MEDIA_LAYER,
    GAMING_LAYER,
    QMK_LAYER
};


enum CustomKeycodes
{
    ARROW_MACRO = SAFE_RANGE,
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
};


const key_override_t space_ko = ko_make_basic(MOD_MASK_SHIFT, KC_SPC, KC_TAB);
const key_override_t dot_ko = ko_make_with_layers(MOD_MASK_SHIFT, KC_DOT, ARROW_MACRO, 0x1);

// This globally defines all key overrides to be used
const key_override_t **key_overrides = (const key_override_t *[]){
    &space_ko,
    &dot_ko,
    NULL // Null terminate the array of overrides!
};


bool process_record_user(uint16_t keycode, keyrecord_t* record)
{
    switch(keycode)
    {
    case ARROW_MACRO:
        if(record->event.pressed)
        {
            SEND_STRING("->");
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
        }
        return false;
    default:
        return true;
    }
    return true;
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[ALPHA_LAYER] = LAYOUT_split_3x5_2(
            KC_Q,        KC_W,        KC_F,        KC_P, KC_B,      KC_J,        KC_L,        KC_U,        KC_Y, KC_SCLN,
            KC_A, WIN_T(KC_R), ALT_T(KC_S), CTL_T(KC_T), KC_G,      KC_M, CTL_T(KC_N), ALT_T(KC_E), WIN_T(KC_I), KC_O,
            KC_Z,        KC_X,        KC_C,        KC_D, KC_V,      KC_K,        KC_H,     KC_COMM,      KC_DOT, KC_SLSH,

                                             KC_LSFT, KC_BSPC,      KC_SPC, TO(SYM_LAYER)),

	[SYM_LAYER] = LAYOUT_split_3x5_2(
            KC_CIRC, KC_TILD, KC_HASH, KC_COLN, KC_GRV,      KC_PIPE, KC_QUES, KC_UNDS, KC_BSLS, KC_NO,
            KC_AMPR, KC_ASTR, KC_LBRC, KC_LPRN, KC_LCBR,     KC_RCBR, KC_RPRN, KC_RBRC, KC_DQUO, KC_PLUS,
            KC_DLR,  KC_LT,   KC_GT,   KC_EXLM, KC_PERC,     KC_AT,   KC_MINS, KC_EQL,  KC_QUOT, TO(FN_LAYER),

                            TO(ALPHA_LAYER), KC_SPC,         TO(NAV_LAYER),   TO(NUM_LAYER)),

    [NUM_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_P7, KC_P8, KC_P9, KC_NO,
            KC_PDOT, KC_PSLS, KC_PAST, KC_PMNS, KC_PPLS,    KC_NO, KC_P4, KC_P5, KC_P6, KC_PEQL,
            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_P1, KC_P2, KC_P3, KC_NO,

                        TO(ALPHA_LAYER), TO(FN_LAYER),      KC_P0, TO(NAV_LAYER)),

    [NAV_LAYER] = LAYOUT_split_3x5_2(
            KC_NO, KC_P,  KC_Y,    VIM_T,  KC_LCBR,      KC_U,    KC_P2,   KC_P3,   KC_P4, KC_NO,
            KC_W,  KC_B,  KC_E,    VIM_F,  KC_RCBR,      KC_D,    KC_LEFT, KC_DOWN, KC_UP, KC_RGHT,
            KC_NO, KC_NO, KC_CIRC, KC_DLR, KC_NO,        KC_NO,   KC_COMM, KC_NO,   KC_NO, KC_TRNS,

                            TO(ALPHA_LAYER),  KC_V,      KC_LSFT, KC_SCLN),

    [FN_LAYER] = LAYOUT_split_3x5_2(
            TO(QMK_LAYER),   KC_NO,  KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_F7, KC_F8, KC_F9, KC_F12,
            UNDO,            CUT,    COPY,    PASTE,   FIND,       KC_NO, KC_F4, KC_F5, KC_F6, KC_F11,
            KC_NO,           KC_NO,  KC_NO,   KC_NO,   KC_NO,      KC_NO, KC_F1, KC_F2, KC_F3, KC_F10,

                                    TO(ALPHA_LAYER),   KC_NO,      TO(GAMING_LAYER), TO(MEDIA_LAYER)),

    [MEDIA_LAYER] = LAYOUT_split_3x5_2(
            KC_NO,   KC_NO,   KC_VOLU, KC_NO,   KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
            KC_MUTE, KC_MPRV, KC_MPLY, KC_MNXT, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
            KC_NO,   KC_NO,   KC_VOLD, KC_NO,   KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

                               TO(ALPHA_LAYER), KC_NO,      KC_NO, KC_NO),

    [GAMING_LAYER] = LAYOUT_split_3x5_2(
            KC_Q, KC_W, KC_E, KC_R, KC_T,      KC_Y, KC_U, KC_I, KC_O, KC_P,
            KC_A, KC_S, KC_D, KC_F, KC_G,      KC_H, KC_J, KC_K, KC_L, KC_NO,
            KC_Z, KC_X, KC_C, KC_V, KC_B,      KC_N, KC_M, KC_NO, KC_NO, KC_NO,
                 TO(ALPHA_LAYER), KC_SPC,      KC_COMM, KC_NO),

    [QMK_LAYER] = LAYOUT_split_3x5_2(
            QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, QK_RBT,
            KC_NO,   KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
            EE_CLR,  KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

                         TO(ALPHA_LAYER), KC_NO,      KC_NO, KC_NO)

};
// clang-format on

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif  // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)

