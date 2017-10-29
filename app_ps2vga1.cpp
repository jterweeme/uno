#include "misc.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "vga.h"
#include <avr/sleep.h>

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

static const uint8_t PS2_INVERTED_EXCLAMATION =   161; // ¡
static const uint8_t PS2_CENT_SIGN =          162; // ¢
static const uint8_t PS2_POUND_SIGN =         163; // £
static const uint8_t PS2_CURRENCY_SIGN =      164; // ¤
static const uint8_t PS2_YEN_SIGN   =         165; // ¥
static const uint8_t PS2_BROKEN_BAR  =                166; // ¦
static const uint8_t PS2_SECTION_SIGN =       167; // §
static const uint8_t PS2_DIAERESIS     =      168; // ¨
static const uint8_t PS2_COPYRIGHT_SIGN =     169; // ©
static const uint8_t PS2_FEMININE_ORDINAL =        170; // ª
static const uint8_t PS2_LEFT_DOUBLE_ANGLE_QUOTE = 171; // «
static const uint8_t PS2_NOT_SIGN      =      172; // ¬
static const uint8_t PS2_HYPHEN       =   173;
static const uint8_t PS2_REGISTERED_SIGN  =   174; // ®
static const uint8_t PS2_MACRON       =   175; // ¯
static const uint8_t PS2_DEGREE_SIGN   =      176; // °
static const uint8_t PS2_PLUS_MINUS_SIGN =    177; // ±
static const uint8_t PS2_SUPERSCRIPT_TWO   =  178; // ²
static const uint8_t PS2_SUPERSCRIPT_THREE  =     179; // ³
static const uint8_t PS2_ACUTE_ACCENT     =   180; // ´
static const uint8_t PS2_MICRO_SIGN       =   181; // µ
static const uint8_t PS2_PILCROW_SIGN =       182; // ¶
static const uint8_t PS2_MIDDLE_DOT =         183; // ·
static const uint8_t PS2_CEDILLA    =     184; // ¸
static const uint8_t PS2_SUPERSCRIPT_ONE =    185; // ¹
static const uint8_t PS2_MASCULINE_ORDINAL =      186; // º
static const uint8_t PS2_RIGHT_DOUBLE_ANGLE_QUOTE  =  187; // »
static const uint8_t PS2_FRACTION_ONE_QUARTER =   188; // ¼
static const uint8_t PS2_FRACTION_ONE_HALF   =    189; // ½
static const uint8_t PS2_FRACTION_THREE_QUARTERS = 190; // ¾
static const uint8_t PS2_INVERTED_QUESTION_MARK = 191; // ¿
static const uint8_t PS2_A_GRAVE = 192; // À
static const uint8_t PS2_A_ACUTE = 193; // Á
static const uint8_t PS2_A_CIRCUMFLEX = 194; // Â
static const uint8_t PS2_A_TILDE = 195; // Ã
static const uint8_t PS2_A_DIAERESIS = 196; // Ä
static const uint8_t PS2_A_RING_ABOVE = 197; // Å
static const uint8_t PS2_AE = 198; // Æ
static const uint8_t PS2_C_CEDILLA =          199; // Ç
static const uint8_t PS2_E_GRAVE =        200; // È
static const uint8_t PS2_E_ACUTE =        201; // É
static const uint8_t PS2_E_CIRCUMFLEX =       202; // Ê
static const uint8_t PS2_E_DIAERESIS =        203; // Ë
static const uint8_t PS2_I_GRAVE =        204; // Ì
static const uint8_t PS2_I_ACUTE =        205; // Í
static const uint8_t PS2_I_CIRCUMFLEX =       206; // Î
static const uint8_t PS2_I_DIAERESIS =        207; // Ï

static const uint8_t PS2_ETH = 208; // Ð
static const uint8_t PS2_N_TILDE = 209; // Ñ
static const uint8_t PS2_O_GRAVE = 210; // Ò
static const uint8_t PS2_O_ACUTE = 211; // Ó
static const uint8_t PS2_O_CIRCUMFLEX =       212; // Ô
static const uint8_t PS2_O_TILDE =        213; // Õ
static const uint8_t PS2_O_DIAERESIS =        214; // Ö
static const uint8_t PS2_MULTIPLICATION =     215; // ×
static const uint8_t PS2_O_STROKE =           216; // Ø
static const uint8_t PS2_U_GRAVE =        217; // Ù
static const uint8_t PS2_U_ACUTE =        218; // Ú
static const uint8_t PS2_U_CIRCUMFLEX =       219; // Û
static const uint8_t PS2_U_DIAERESIS =        220; // Ü
static const uint8_t PS2_Y_ACUTE =        221; // Ý
static const uint8_t PS2_THORN =          222; // Þ
static const uint8_t PS2_SHARP_S =        223; // ß
static const uint8_t PS2_a_GRAVE =        224; // à
static const uint8_t PS2_a_ACUTE =        225; // á
static const uint8_t PS2_a_CIRCUMFLEX =       226; // â
static const uint8_t PS2_a_TILDE =        227; // ã
static const uint8_t PS2_a_DIAERESIS =        228; // ä
static const uint8_t PS2_a_RING_ABOVE =       229; // å
static const uint8_t PS2_ae =             230; // æ
static const uint8_t PS2_c_CEDILLA =          231; // ç
static const uint8_t PS2_e_GRAVE =        232; // è
static const uint8_t PS2_e_ACUTE =        233; // é
static const uint8_t PS2_e_CIRCUMFLEX =       234; // ê
static const uint8_t PS2_e_DIAERESIS =        235; // ë
static const uint8_t PS2_i_GRAVE =        236; // ì
static const uint8_t PS2_i_ACUTE =        237; // í
static const uint8_t PS2_i_CIRCUMFLEX =       238; // î
static const uint8_t PS2_i_DIAERESIS =        239; // ï
static const uint8_t PS2_eth =            240; // ð
static const uint8_t PS2_n_TILDE =        241; // ñ
static const uint8_t PS2_o_GRAVE =        242; // ò
static const uint8_t PS2_o_ACUTE =        243; // ó
static const uint8_t PS2_o_CIRCUMFLEX = 244; // ô
static const uint8_t PS2_o_TILDE = 245; // õ
static const uint8_t PS2_o_DIAERESIS = 246; // ö
static const uint8_t PS2_DIVISION = 247; // ÷
static const uint8_t PS2_o_STROKE = 248; // ø
static const uint8_t PS2_u_GRAVE = 249; // ù
static const uint8_t PS2_u_ACUTE = 250; // ú

static const uint8_t PS2_u_CIRCUMFLEX = 251; // û
static const uint8_t PS2_u_DIAERESIS = 252; // ü
static const uint8_t PS2_y_ACUTE = 253; // ý
static const uint8_t PS2_thorn = 254; // þ
static const uint8_t PS2_y_DIAERESIS = 255; // ÿ


static const uint8_t PS2_KEYMAP_SIZE = 136;

typedef struct {
    uint8_t noshift[PS2_KEYMAP_SIZE];
    uint8_t shift[PS2_KEYMAP_SIZE];
    uint8_t uses_altgr;
    uint8_t altgr[PS2_KEYMAP_SIZE];
} PS2Keymap_t;

extern const PROGMEM PS2Keymap_t PS2Keymap_US;
static const PS2Keymap_t *keymap = &PS2Keymap_US;
static const uint8_t BUFFER_SIZE = 45;

class PS2Keyboard
{
public:
    static volatile uint8_t head, tail;
    static uint8_t CharBuffer;
    static uint8_t UTF8next;
public:
    PS2Keyboard();
    static bool available();
    static int read();
    static void ps2interrupt();
    static char get_iso8859_code();
    static inline uint8_t get_scan_code();
};

static volatile uint8_t buffer[BUFFER_SIZE];

uint8_t PS2Keyboard::CharBuffer = 0;
uint8_t PS2Keyboard::UTF8next = 0;
volatile uint8_t PS2Keyboard::head = 0;
volatile uint8_t PS2Keyboard::tail = 0;

static uint32_t overflows0 = 0;

ISR(TIMER0_OVF_vect)
{
    overflows0++;
}

ISR(INT0_vect)
{
    static uint8_t bitcount=0;
    static uint8_t incoming=0;
    static uint32_t prev_ticks = 0;
    uint8_t val = PINB & 1<<0;

    uint32_t ticks = overflows0;
    
    if (ticks - prev_ticks > 250)
    {
        bitcount = 0;
        incoming = 0;
    }

    prev_ticks = ticks;
    uint8_t n = bitcount - 1;

    if (n <= 7)
        incoming |= val << n;

    if (++bitcount == 11)
    {
        uint8_t i = PS2Keyboard::head + 1;

        if (i >= BUFFER_SIZE)
            i = 0;

        if (i != PS2Keyboard::tail)
        {
            buffer[i] = incoming;
            PS2Keyboard::head = i;
        }

        bitcount = incoming = 0;
    }
}



inline uint8_t PS2Keyboard::get_scan_code()
{
    uint8_t i = tail;

    if (i == head)
        return 0;

    if (++i >= BUFFER_SIZE)
        i = 0;

    uint8_t c = buffer[i];
    tail = i;
    return c;
}

static const uint8_t BREAK    = 0x01;
static const uint8_t MODIFIER = 0x02;
static const uint8_t SHIFT_L =  0x04;
static const uint8_t SHIFT_R =  0x08;
static const uint8_t ALTGR =    0x10;
static const uint8_t CTRL =     0x20;

const PROGMEM PS2Keymap_t PS2Keymap_US = {
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
    0, 0, 'z', 's', 'a', 'w', '2', 0,
    0, 'c', 'x', 'd', 'e', '4', '3', 0,
    0, ' ', 'v', 'f', 't', 'r', '5', 0,
    0, 'n', 'b', 'h', 'g', 'y', '6', 0,
    0, 0, 'm', 'j', 'u', '7', '8', 0,
    0, ',', 'k', 'i', 'o', '0', '9', 0,
    0, '.', '/', 'l', ';', 'p', '-', 0,
    0, 0, '\'', 0, '[', '=', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
    0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
  // with shift
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
    0, 0, 'Z', 'S', 'A', 'W', '@', 0,
    0, 'C', 'X', 'D', 'E', '$', '#', 0,
    0, ' ', 'V', 'F', 'T', 'R', '%', 0,
    0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
    0, 0, 'M', 'J', 'U', '&', '*', 0,
    0, '<', 'K', 'I', 'O', ')', '(', 0,
    0, '>', '?', 'L', ':', 'P', '_', 0,
    0, 0, '"', 0, '{', '+', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
    0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
    0
};

char PS2Keyboard::get_iso8859_code()
{
    static uint8_t state=0;
    char c;

    while (1)
    {
        uint8_t s = get_scan_code();
        if (!s) return 0;
        if (s == 0xF0) {
            state |= BREAK;
        } else if (s == 0xE0) {
            state |= MODIFIER;
        } else {
            if (state & BREAK) {
                if (s == 0x12) {
                    state &= ~SHIFT_L;
                } else if (s == 0x59) {
                    state &= ~SHIFT_R;
                } else if (s == 0x14) {
                    state &= ~CTRL;
                } else if (s == 0x11 && (state & MODIFIER)) {
                    state &= ~ALTGR;
                }
                state &= ~(BREAK | MODIFIER);
                continue;
            }
            if (s == 0x12) {
                state |= SHIFT_L;
                continue;
            } else if (s == 0x59) {
                state |= SHIFT_R;
                continue;
            } else if (s == 0x14) {
                state |= CTRL;
                continue;
            } else if (s == 0x11 && (state & MODIFIER)) {
                state |= ALTGR;
            }
            c = 0;
            if (state & MODIFIER) {
                switch (s) {
                  case 0x70: c = PS2_INSERT;      break;
                  case 0x6C: c = PS2_HOME;        break;
                  case 0x7D: c = PS2_PAGEUP;      break;
                  case 0x71: c = PS2_DELETE;      break;
                  case 0x69: c = PS2_END;         break;
                  case 0x7A: c = PS2_PAGEDOWN;    break;
                  case 0x75: c = PS2_UPARROW;     break;
                  case 0x6B: c = PS2_LEFTARROW;   break;
                  case 0x72: c = PS2_DOWNARROW;   break;
                  case 0x74: c = PS2_RIGHTARROW;  break;
                  case 0x4A: c = '/';             break;
                  case 0x5A: c = PS2_ENTER;       break;
                  default: break;
                }
            } else if ((state & ALTGR) && keymap->uses_altgr) {
                if (s < PS2_KEYMAP_SIZE)
                    c = pgm_read_byte(keymap->altgr + s);
            } else if (state & (SHIFT_L | SHIFT_R)) {
                if (s < PS2_KEYMAP_SIZE)
                    c = pgm_read_byte(keymap->shift + s);
            } else {
                if (s < PS2_KEYMAP_SIZE)
                    c = pgm_read_byte(keymap->noshift + s);
            }
            if (state & CTRL) {
                if (c >= 'A' && c <= 'Z')
                    c = c - 'A' + 1;
                else if (c >= 'a' && c <= 'z')
                    c = c - 'a' + 1;
                else if (c == PS2_ENTER)
                    c = PS2_LINEFEED;
            }
            state &= ~(BREAK | MODIFIER);
            if (c) return c;
        }
    }
}

bool PS2Keyboard::available()
{
    if (CharBuffer || UTF8next)
        return true;

    CharBuffer = get_iso8859_code();

    if (CharBuffer)
        return true;

    return false;
}

int PS2Keyboard::read()
{
    uint8_t result = UTF8next;

    if (result)
    {
        UTF8next = 0;
    }
    else
    {
        result = CharBuffer;

        if (result)
            CharBuffer = 0;
        else
            result = get_iso8859_code();

        if (result >= 128)
        {
            UTF8next = (result & 0x3F) | 0x80;
            result = ((result >> 6) & 0x1F) | 0xC0;
        }
    }

    if (!result)
        return -1;

    return result;
}

PS2Keyboard::PS2Keyboard()
{
    DDRD &= ~(1<<DDD3 | 1<<DDD2);
    PORTD |= 1<<PORTD3 | 1<<PORTD2;
    DDRB &= ~(1<<DDB0);
    PORTB |= 1<<PORTB0;
    head = tail = 0;
    EICRA |= 1<<ISC00 | 1<<ISC01;   // external interrupt 1 falling edge?
    EIMSK |= 1<<INT0;               // enable interrupt1
}

#if 0
Serial *g_serial;

ISR(USART_RX_vect)
{
    VGA::g_vga->write(UDR0);
}
#endif

int main()
{
    sei();
#if 0
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;     // clk/64
    TCNT0 = 0;
    TIMSK0 = 1<<TOIE0;              // timer0 overflow generates interrupt
#endif
    PS2Keyboard keyboard;
    VGA vga;
    vga.init();
    vga.write("Keyboard Test:");
    vga.gotoxy(0, 1);

    while (true)
    {
        if (keyboard.available())
            vga.write(keyboard.read());

        //sleep_mode();
    }

    return 0;
}




