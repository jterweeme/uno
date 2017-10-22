/*
Plays an RTTTL ringtone on Arduino Uno Pin 11 using timer2 without interrupt

CS22 CS21 CS20
000 No clock source
001 no prescaling
010 clk/8
011 clk/32
100 clk/64
101 clk/128
110 clk/256
111 clk/1024
*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <ctype.h>

#define F_CPU 16000000
#include <util/delay.h>
#include "lcd1602.h"
#include "analog.h"
#include "ringtone.h"

const char tones[] PROGMEM =
    "Indiana Jones:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,"
    "1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,"
    "8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6\n"
    "AuldLS:d=4,o=6,b=101:g5,c,8c,c,e,d,8c,d,8e,8d,c,8c,"
    "e,g,2a,a,g,8e,e,c,d,8c,d,8e,8d,c,8a5,a5,g5,2c\n"
    "Walk of Life:d=4,o=6,b=80:8d.6,4d.6,32c.6,32d.6,16f.6,"
    "32d.6,8c6,8a#.5,2a#5,32c.6,32d.6,8f.6,4f.6,32c.6,32d.6,16f.6,32d.6,8c6,8a#.5,"
    "4a#.5,32c.6,32d.6,16f.6,32d.6,32c.6,32a#.5,8f.6,4f.6,32c.6,32d.6,16f.6,32d.6,"
    "8c6,8a#.5,2a#5,32c.6,32d.6,8f.6,4f.6,32c.6,32d.6,16f.6,32d.6,8c6,8a#.5,4a#.5,"
    "32c.6,32d.6,16f.6,32d.6,32c.6,32a#.5,8f6,8f6,\n"
    "Wilhelmus:d=4,o=5,b=90:d,g,g,8a,8b,8c6,8a,b,8a,8b,c6,b,8a,8g,a,2g,p,d,g,"
    "g,8a,8b,8c6,8a,b,8a,8b,c6,b,8a,8g,a,2g,p,8b,8c6,2d6,e6,2d6,c6,b,8a,8b,c6,b,"
    "a,g,2a,p,d,8g,8f#,8g,8a,b,2a,g,f#,d,8e,8f#,g,g,f#,2g\n"
    "Good Bad:d=4,o=6,b=40:32b5,32e6,32b5,32e6,"
    "8b.5,16g.5,16a.5,4e5,32b5,32e6,32b5,32e6,8b.5,16g.5,16a.5,4d6,32b5,32e6,"
    "32b5,32e6,8b.5,16g.5,32f#.5,32e.5,4d5,32b5,32e6,32b5,32e6,8b.5,16a.5,4e5,\n"
    "James Bond:d=4,o=6,b=112:16c.5,32d.5,32d.5,16d.5,8d.5,16c.5,16c.5,16c.5,"
    "16c.5,32d#.5,32d#.5,16d#.5,8d#.5,16d.5,16d.5,16d.5,16c.5,32d.5,32d.5,16d.5,"
    "8d.5,16c.5,16c.5,16c.5,16c.5,32d#.5,32d#.5,16d#.5,8d#.5,16d.5,16d.5,16d.5,"
    "16c.5,32d.5,32d.5,16d.5,8d.5,16c.5,16c.5,16c.5,16c.5,32d#.5,32d#.5,16d#.5,"
    "8d#.5,16d.5,16d.5,16c.5,16b.5,2a#5,16g.5,16f.5,2g.5\n"
    "Bob de Bouwer:d=4,o=5,b=140:g6,16g,16p,8e6,d6,b,16c,16p,8p,16c,16p,8p,16d,16p,8p,"
    "16d,16p,8p,d6,16g,16p,8c6,b,g,16c,16p,8p,16d,16p,8p,16g,16p,8p,16g,16p,8p,"
    "g6,16g,16p,8e6,d6,b,16c,16p,8p,16c,16p,8p,16d,16p,8p,16d,16p,8p,d6,16g,16p,"
    "8c6,b,g,16c,16p,8p,16d,16p,8p,16g\n"
    "Godfather:d=4,o=5,b=160:8g,8c6,8d#6,8d6,8c6,8d#6,8c6,8d6,c6,8g#,8a#,2g,8p,"
    "8g,8c6,8d#6,8d6,8c6,8d#6,8c6,8d6,c6,8g,8f#,2f,8p,8f,8g#,8b,2d6,8p,8f,8g#,"
    "8b,2c6,8p,8c,8d#,8a#,8g#,g,8a#,8g#,8g#,8g,8g,8b4,2c\n"
    "Final Countdown:d=16,o=5,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,"
    "4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b\n"
    "Barbie Girl:d=8,o=5,b=125:g#,e,g#,c#6,4a,4p,f#,d#,f#,b,"
    "4g#,f#,e,4p,e,c#,4f#,4c#,4p,f#,e,4g#,4f#\n"
    "Back to the Future:d=16,o=5,b=200:4g.,p,4c.,p,2f#.,p,g.,"
    "p,a.,p,8g,p,8e,p,8c,p,4f#,p,g.,p,a.,p,8g.,p,8d.,p,8g.,p,"
    "8d.6,p,4d.6,p,4c#6,p,b.,p,c#.6,p,2d.6\n"
    "test:d=16,o=5,b=200:16c,8d,8e,4f,g,4a6,4b\n"
    "Macarena:d=8,o=5,b=180:f,f,f,4f,f,f,f,f,f,f,f,a,c,c,4f,f,f,4f,f,f,f,"
    "f,f,f,d,c,4p,4f,f,f,4f,f,f,f,f,f,f,f,a,4p,2c.6,4a,c6,a,f,4p,2p\n"
    "Rule Britania:d=8,o=5,b=100: e.,e,f,4f,e,f.,16e,d.,16c,2b4,4g,4f,"
    "16e,16c,16f,16d,g,f,4e,d.,16c,4c\n"
    "USA National Anthem:d=8,o=5,b=120:e.,d,4c,4e,4g,4c6.,p,e6.,d6,4c6,"
    "4e,4f#,4g.,p,4g,4e6.,d6,4c6,2b,a,4b,c6.,16p,4c6,4g,4e,32p,4c";

int main()
{
    LCD1602 lcd;
    lcd.init();
    lcd.write("Back to the future");
    Timer2 t2;
    Ringtone player(&t2);
    Analog adc;
    adc.init();
    DFKeypad dfk(&adc);

    while (true)
    {
        uint8_t x = dfk.read();
        
        switch (x)
        {
            case DFKeypad::SELECT:
                player.play_P(tones, 0);
                break;
        }
    }

    return 0;
}





