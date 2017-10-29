#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <avr/pgmspace.h>
#include "misc.h"

static const uint8_t PS2_TAB = 9;
static const uint8_t PS2_ENTER = 13;
static const uint8_t PS2_LINEFEED = 10;
static const uint8_t PS2_BACKSPACE = 127;
static const uint8_t PS2_ESC = 27;
static const uint8_t PS2_INSERT = 0;
static const uint8_t PS2_DELETE = 127;
static const uint8_t PS2_HOME = 0;
static const uint8_t PS2_END = 0;
static const uint8_t PS2_PAGEUP = 25;
static const uint8_t PS2_PAGEDOWN = 26;
static const uint8_t PS2_UPARROW = 11;
static const uint8_t PS2_LEFTARROW = 8;
static const uint8_t PS2_DOWNARROW = 12;
static const uint8_t PS2_RIGHTARROW = 21;
static const uint8_t PS2_F1 = 0;
static const uint8_t PS2_F2 = 0;
static const uint8_t PS2_F3 = 0;
static const uint8_t PS2_F4 = 0;
static const uint8_t PS2_F5 = 0;
static const uint8_t PS2_F6 = 0;
static const uint8_t PS2_F7 = 0;
static const uint8_t PS2_F8 = 0;
static const uint8_t PS2_F9 = 0;
static const uint8_t PS2_F10 = 0;
static const uint8_t PS2_F11 = 0;
static const uint8_t PS2_F12 = 0;
static const uint8_t PS2_SCROLL = 0;








static const uint8_t PS2_KEYMAP_SIZE = 136;

struct PS2Keymap_t {
    uint8_t noshift[PS2_KEYMAP_SIZE];
    uint8_t shift[PS2_KEYMAP_SIZE];
    uint8_t uses_altgr;
    uint8_t altgr[PS2_KEYMAP_SIZE];
};

extern const PROGMEM PS2Keymap_t PS2Keymap_US;
static const PS2Keymap_t *keymap = &PS2Keymap_US;


class PS2Keyboard
{
public:
    static const uint8_t N1 = 0x16;
    static const uint8_t N2 = 0x1e;
    static const uint8_t N3 = 0x26;
    static const uint8_t N4 = 0x25;
    static const uint8_t N5 = 0x2e;
    static const uint8_t N6 = 0x36;
    static const uint8_t N7 = 0x3d;
    static const uint8_t N8 = 0x3e;
    static const uint8_t N9 = 0x46;
    static const uint8_t N0 = 0x45;
    static const uint8_t NUM1 = 0x69;
    static const uint8_t NUM2 = 0x72;
    static const uint8_t NUM3 = 0x7a;
    static const uint8_t PLUS = 0x79;
    static const uint8_t MINUS = 0x7b;
    static const uint8_t SLASH = 0x4a;
    static const uint8_t ENTER = 0x5a;
    static const uint8_t ESC = 0x76;
    static const uint8_t NUMLOCK = 0x77;
    static const uint8_t STAR = 0x7c;
private:    // not used so far
    static const uint8_t PS2_INVERTED_EXCLAMATION =   161;
    static const uint8_t PS2_CENT_SIGN =          162;
    static const uint8_t PS2_POUND_SIGN =         163;
    static const uint8_t PS2_CURRENCY_SIGN =      164;
    static const uint8_t PS2_YEN_SIGN = 165;
    static const uint8_t PS2_BROKEN_BAR = 166;
    static const uint8_t PS2_SECTION_SIGN = 167;
    static const uint8_t PS2_DIAERESIS     = 168;
    static const uint8_t PS2_COPYRIGHT_SIGN = 169;
    static const uint8_t PS2_FEMININE_ORDINAL = 170;
    static const uint8_t PS2_LEFT_DOUBLE_ANGLE_QUOTE = 171;
    static const uint8_t PS2_NOT_SIGN      = 172;
    static const uint8_t PS2_HYPHEN       = 173;
    static const uint8_t PS2_REGISTERED_SIGN = 174;
    static const uint8_t PS2_MACRON       =   175;
    static const uint8_t PS2_DEGREE_SIGN   =      176;
    static const uint8_t PS2_PLUS_MINUS_SIGN =    177;
    static const uint8_t PS2_SUPERSCRIPT_TWO   =  178;
    static const uint8_t PS2_SUPERSCRIPT_THREE  =     179;
    static const uint8_t PS2_ACUTE_ACCENT     =   180;
    static const uint8_t PS2_MICRO_SIGN       =   181;
    static const uint8_t PS2_PILCROW_SIGN =       182;
    static const uint8_t PS2_MIDDLE_DOT =         183;
    static const uint8_t PS2_CEDILLA    =     184;
    static const uint8_t PS2_SUPERSCRIPT_ONE =    185;
    static const uint8_t PS2_MASCULINE_ORDINAL =      186;
    static const uint8_t PS2_RIGHT_DOUBLE_ANGLE_QUOTE  =  187;
    static const uint8_t PS2_FRACTION_ONE_QUARTER =   188;
    static const uint8_t PS2_FRACTION_ONE_HALF   =    189;
    static const uint8_t PS2_FRACTION_THREE_QUARTERS = 190;
    static const uint8_t PS2_INVERTED_QUESTION_MARK = 191;
    static const uint8_t PS2_A_GRAVE = 192;
    static const uint8_t PS2_A_ACUTE = 193;
    static const uint8_t PS2_A_CIRCUMFLEX = 194;
    static const uint8_t PS2_A_TILDE = 195;
    static const uint8_t PS2_A_DIAERESIS = 196;
    static const uint8_t PS2_A_RING_ABOVE = 197;
    static const uint8_t PS2_AE = 198;
    static const uint8_t PS2_C_CEDILLA =          199;
    static const uint8_t PS2_E_GRAVE =        200;
    static const uint8_t PS2_E_ACUTE =        201;
    static const uint8_t PS2_E_CIRCUMFLEX =       202;
    static const uint8_t PS2_E_DIAERESIS =        203;
    static const uint8_t PS2_I_GRAVE =        204;
    static const uint8_t PS2_I_ACUTE =        205;
    static const uint8_t PS2_I_CIRCUMFLEX =       206;
    static const uint8_t PS2_I_DIAERESIS =        207;
private: // not used so far
    static const uint8_t PS2_ETH = 208;
    static const uint8_t PS2_N_TILDE = 209;
    static const uint8_t PS2_O_GRAVE = 210;
    static const uint8_t PS2_O_ACUTE = 211;
    static const uint8_t PS2_O_CIRCUMFLEX = 212;
    static const uint8_t PS2_O_TILDE = 213;
    static const uint8_t PS2_O_DIAERESIS =        214;
    static const uint8_t PS2_MULTIPLICATION =     215;
    static const uint8_t PS2_O_STROKE =           216;
    static const uint8_t PS2_U_GRAVE =        217;
    static const uint8_t PS2_U_ACUTE =        218;
    static const uint8_t PS2_U_CIRCUMFLEX =       219;
    static const uint8_t PS2_U_DIAERESIS =        220;
    static const uint8_t PS2_Y_ACUTE =        221;
    static const uint8_t PS2_THORN =          222;
    static const uint8_t PS2_SHARP_S =        223;
    static const uint8_t PS2_a_GRAVE =        224;
    static const uint8_t PS2_a_ACUTE =        225;
    static const uint8_t PS2_a_CIRCUMFLEX =       226;
    static const uint8_t PS2_a_TILDE =        227;
    static const uint8_t PS2_a_DIAERESIS =        228;
    static const uint8_t PS2_a_RING_ABOVE =       229;
    static const uint8_t PS2_ae =             230;
    static const uint8_t PS2_c_CEDILLA =          231;
    static const uint8_t PS2_e_GRAVE =        232;
    static const uint8_t PS2_e_ACUTE =        233;
    static const uint8_t PS2_e_CIRCUMFLEX =       234;
    static const uint8_t PS2_e_DIAERESIS =        235;
    static const uint8_t PS2_i_GRAVE =        236;
    static const uint8_t PS2_i_ACUTE =        237;
    static const uint8_t PS2_i_CIRCUMFLEX =       238;
    static const uint8_t PS2_i_DIAERESIS =        239;
    static const uint8_t PS2_eth =            240;
    static const uint8_t PS2_n_TILDE =        241;
    static const uint8_t PS2_o_GRAVE =        242;
    static const uint8_t PS2_o_ACUTE =        243;
    static const uint8_t PS2_o_CIRCUMFLEX = 244;
    static const uint8_t PS2_o_TILDE = 245;
    static const uint8_t PS2_o_DIAERESIS = 246;
    static const uint8_t PS2_DIVISION = 247;
    static const uint8_t PS2_o_STROKE = 248;
    static const uint8_t PS2_u_GRAVE = 249;
    static const uint8_t PS2_u_ACUTE = 250;

    static const uint8_t PS2_u_CIRCUMFLEX = 251;
    static const uint8_t PS2_u_DIAERESIS = 252;
    static const uint8_t PS2_y_ACUTE = 253;
    static const uint8_t PS2_thorn = 254;
    static const uint8_t PS2_y_DIAERESIS = 255;
public:
    static const uint8_t BREAK    = 0x01;
    static const uint8_t MODIFIER = 0x02;
    static const uint8_t SHIFT_L =  0x04;
    static const uint8_t SHIFT_R =  0x08;
    static const uint8_t ALTGR =    0x10;
    static const uint8_t CTRL =     0x20;
public:
    static volatile uint8_t head, tail;
    static uint8_t CharBuffer;
    static uint8_t UTF8next;
private:
    MyBitset _bitset;
    CircBuf _buf;
public:
    PS2Keyboard();
    bool available();
    int read();
    void isr();
    void ps2interrupt();
    char get_iso8859_code();
    uint8_t get_scan_code();
};


#endif


