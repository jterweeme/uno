#include "keyboard.h"

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

uint8_t PS2Keyboard::CharBuffer = 0;
uint8_t PS2Keyboard::UTF8next = 0;

uint8_t PS2Keyboard::get_scan_code()
{
    return _buf.get();
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
    EICRA |= 1<<ISC00 | 1<<ISC01;   // external interrupt 0 falling edge?
    EIMSK |= 1<<INT0;               // enable interrupt0
}

extern uint32_t overflows0;

void PS2Keyboard::isr()
{
    static uint32_t prev_ticks = 0;
    uint8_t val = PINB & 1<<0;

    if (overflows0 - prev_ticks > 250)
        _bitset.reset();

    prev_ticks = overflows0;
    _bitset.addBit(val);

    if (_bitset.count() == 11)
    {
        _buf.push(_bitset.incoming());
        _bitset.reset();
    }
}

char PS2Keyboard::get_iso8859_code()
{
    static uint8_t state=0;
    char c;

    while (1)
    {
        uint8_t s = get_scan_code();

        if (!s)
            return 0;

        if (s == 0xF0)
        {
            state |= BREAK;
        }
        else if (s == 0xE0)
        {
            state |= MODIFIER;
        }
        else
        {
            if (state & BREAK)
            {
                if (s == 0x12)
                    state &= ~SHIFT_L;
                else if (s == 0x59)
                    state &= ~SHIFT_R;
                else if (s == 0x14)
                    state &= ~CTRL;
                else if (s == 0x11 && (state & MODIFIER))
                    state &= ~ALTGR;

                state &= ~(BREAK | MODIFIER);
                continue;
            }

            if (s == 0x12)
            {
                state |= SHIFT_L;
                continue;
            }
            else if (s == 0x59)
            {
                state |= SHIFT_R;
                continue;
            }
            else if (s == 0x14)
            {
                state |= CTRL;
                continue;
            }
            else if (s == 0x11 && (state & MODIFIER))
            {
                state |= ALTGR;
            }

            c = 0;

            if (state & MODIFIER)
            {
                switch (s)
                {
                    case 0x70:
                        c = PS2_INSERT;
                        break;
                    case 0x6C:
                        c = PS2_HOME;
                        break;
                    case 0x7D:
                        c = PS2_PAGEUP;
                        break;
                    case 0x71:
                        c = PS2_DELETE;
                        break;
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
            }
            else if ((state & ALTGR) && keymap->uses_altgr)
            {
                if (s < PS2_KEYMAP_SIZE)
                    c = pgm_read_byte(keymap->altgr + s);
            }
            else if (state & (SHIFT_L | SHIFT_R))
            {
                if (s < PS2_KEYMAP_SIZE)
                    c = pgm_read_byte(keymap->shift + s);
            }
            else
            {
                if (s < PS2_KEYMAP_SIZE)
                    c = pgm_read_byte(keymap->noshift + s);
            }

            if (state & CTRL)
            {
                if (c >= 'A' && c <= 'Z')
                    c = c - 'A' + 1;
                else if (c >= 'a' && c <= 'z')
                    c = c - 'a' + 1;
                else if (c == PS2_ENTER)
                    c = PS2_LINEFEED;
            }

            state &= ~(BREAK | MODIFIER);

            if (c)
                return c;
        }
    }
}



