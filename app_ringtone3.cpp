/*
Plays an RTTTL ringtone on Arduino Uno Pin 11 (PB2/OC2A/MOSI)
using timer2 without interrupt

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
    "Indy Jones:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,"
    "1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,"
    "8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6\n"

    "AuldLS:d=4,o=6,b=101:g5,c,8c,c,e,d,8c,d,8e,8d,c,8c,"
    "e,g,2a,a,g,8e,e,c,d,8c,d,8e,8d,c,8a5,a5,g5,2c\n"

    "WalkOfLife:d=4,o=6,b=80:8d.6,4d.6,32c.6,32d.6,16f.6,"
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

    "BobBouwer:d=4,o=5,b=140:g6,16g,16p,8e6,d6,b,16c,16p,8p,16c,16p,8p,16d,16p,8p,"
    "16d,16p,8p,d6,16g,16p,8c6,b,g,16c,16p,8p,16d,16p,8p,16g,16p,8p,16g,16p,8p,"
    "g6,16g,16p,8e6,d6,b,16c,16p,8p,16c,16p,8p,16d,16p,8p,16d,16p,8p,d6,16g,16p,"
    "8c6,b,g,16c,16p,8p,16d,16p,8p,16g\n"

    "Godfather:d=4,o=5,b=160:8g,8c6,8d#6,8d6,8c6,8d#6,8c6,8d6,c6,8g#,8a#,2g,8p,"
    "8g,8c6,8d#6,8d6,8c6,8d#6,8c6,8d6,c6,8g,8f#,2f,8p,8f,8g#,8b,2d6,8p,8f,8g#,"
    "8b,2c6,8p,8c,8d#,8a#,8g#,g,8a#,8g#,8g#,8g,8g,8b4,2c\n"

    "FCountdown:d=16,o=5,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,"
    "4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b\n"

    "BarbieGirl:d=8,o=5,b=125:g#,e,g#,c#6,4a,4p,f#,d#,f#,b,"
    "4g#,f#,e,4p,e,c#,4f#,4c#,4p,f#,e,4g#,4f#\n"

    "BackFuture:d=16,o=5,b=200:4g.,p,4c.,p,2f#.,p,g.,"
    "p,a.,p,8g,p,8e,p,8c,p,4f#,p,g.,p,a.,p,8g.,p,8d.,p,8g.,p,"
    "8d.6,p,4d.6,p,4c#6,p,b.,p,c#.6,p,2d.6\n"

    "test:d=16,o=5,b=200:16c,8d,8e,4f,g,4a6,4b\n"

    "Postman Pat:d=16,o=5,b=100:f#,p,a,p,8b,8p,f#,p,a,p,8b,8p,f#,"
    "p,a,p,b,p,d6,d6,c#6,c#6,a,p,4b.,8p,32f#,g,p,a,p,b,p,g,p,8f#.,"
    "8e,8p,32f#,g,p,a,p,32b.,32b.,g,p,8f#.,8e,8p,"
    "32f#,g,p,a,p,b,p,g,p,f#,p,e,p,d,p,c#,p,2d\n"

    "Let it be:d=8,o=5,b=100:16e6,d6,4c6,16e6,g6,a6,g.6,16g6,g6,e6,16d6,c6,16a,g,"
    "4e.6,4p,e6,16e6,f.6,e6,e6,d6,16p,16e6,16d6,d6,2c.6.\n"

    "Macarena:d=8,o=5,b=180:f,f,f,4f,f,f,f,f,f,f,f,a,c,c,4f,f,f,4f,f,f,f,"
    "f,f,f,d,c,4p,4f,f,f,4f,f,f,f,f,f,f,f,a,4p,2c.6,4a,c6,a,f,4p,2p\n"

    "RuleBritan:d=8,o=5,b=100: e.,e,f,4f,e,f.,16e,d.,16c,2b4,4g,4f,"
    "16e,16c,16f,16d,g,f,4e,d.,16c,4c\n"

    "Heman:d=8,o=6,b=160:g,g,g,4g,4e,g,a,g,f,4g,4c,g,a,g,f,4g,4e,c,2d.,4p,"
    "4e.,4a5,4c,e,f#,e,d,4e,4a5,e,f#,e,d,4e,4a5,g5,2a5\n"

    "Flintstons:d=8,o=5,b=200:g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,"
    "4d#,2f,2p,4g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,4d#,2c#\n"

    "USAnthem:d=8,o=5,b=120:e.,d,4c,4e,4g,4c6.,p,e6.,d6,4c6,"
    "4e,4f#,4g.,p,4g,4e6.,d6,4c6,2b,a,4b,c6.,16p,4c6,4g,4e,32p,4c";

int main()
{
    LCD1602 lcd;
    lcd.init();
    Timer2 t2;
    Ringtone player(&t2, tones);
    player.next();
    lcd.clear();

    for (uint8_t i = 0; i < 10; i++)
        lcd.write(player._title[i]);

    Analog adc;
    adc.init();
    DFKeypad dfk(&adc);

    while (true)
    {
        uint8_t x = dfk.read();
        
        switch (x)
        {
            case DFKeypad::SELECT:
                player.play_P();
                break;
            case DFKeypad::DOWN:
                lcd.clear();
                _delay_ms(250);
                player.skip();
                player.next();


                for (uint8_t i = 0; i < 10; i++)
                    lcd.write(player._title[i]);

                break;
            case DFKeypad::UP:
                player.prev();
                break;
            case DFKeypad::RIGHT:
                lcd.clear();
                break;
        }
    }

    return 0;
}





