/*
PCF8563 debuggen op seriele terminal
*/

#include <stdio.h>
#include "misc.h"
#include "i2c.h"

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static inline void hexByte(Serial &s, uint8_t n)
{
    s.write(nibble(n >> 4));
    s.write(nibble(n & 0xf));
}

static void dayOfWeek(char *dag, uint8_t day)
{
    switch (day)
    {
        case 0:
            ::strncpy(dag, "Ma", 3);
            break;
        case 1:
            ::strncpy(dag, "Di", 3);
            break;
        case 2:
            ::strncpy(dag, "Wo", 3);
            break;
        case 3:
            ::strncpy(dag, "Do", 3);
            break;
        case 4:
            ::strncpy(dag, "Vr", 3);
            break;
        case 5:
            ::strncpy(dag, "Za", 3);
            break;
        case 6:
            ::strncpy(dag, "Zo", 3);
            break;
        default:
            ::strncpy(dag, "??", 3);
            break;
    }
}

int main()
{
    Serial serial;
    serial.init();
    I2CBus bus;
    bus.init(72);
    PCF8563 pcf(&bus);
    
    // set time
#if 0
    pcf._vl_seconds &= 0x80;
    pcf._vl_seconds |= 0;   // 0 secs
    pcf._min &= 0x80;
    pcf._min |= 3<<4 | 7;   // 37 mins
    pcf._hours &= 0x40;
    pcf._hours |= 14;       // 14 hours
    pcf._weekday &= 0xf8;
    pcf._weekday |= 2;
    pcf._days &= 0xc0;
    pcf._days |= 10;
#endif
    pcf._hour_alarm &= 0xc0;
    pcf._hour_alarm |= 22;
    pcf._minute_alarm &= 0x80;
    pcf._minute_alarm |= 3<<4;
#if 1
    pcf.write();
#endif

    while (true)
    {
        pcf.gettime();
        hexByte(serial, pcf._control_status_1);
        serial.write(' ');
        hexByte(serial, pcf._control_status_2);
        serial.write(' ');
        hexByte(serial, pcf._vl_seconds);
        serial.write(' ');
        hexByte(serial, pcf._min);
        serial.write(' ');
        hexByte(serial, pcf._hours);
        serial.write(' ');
        hexByte(serial, pcf._days);
        serial.write(' ');
        hexByte(serial, pcf._weekday);
        serial.write(' ');
        hexByte(serial, pcf._century_months);
        serial.write(' ');
        hexByte(serial, pcf._years);
        serial.write(' ');
        hexByte(serial, pcf._minute_alarm);
        serial.write(' ');
        hexByte(serial, pcf._hour_alarm);
        serial.write(' ');
        hexByte(serial, pcf._day_alarm);
        serial.write(' ');
        hexByte(serial, pcf._weekday_alarm);
        serial.write(' ');
        hexByte(serial, pcf._clkout_control);
        serial.write(' ');
        hexByte(serial, pcf._timer_control);
        serial.write(' ');
        hexByte(serial, pcf._timer);
        serial.write("\r\n");
        char buf[30];
        dayOfWeek(buf, pcf.dayOfWeek());
        serial.write(buf);
        serial.write(' ');
        serial.write('0' + (pcf._days >> 4 & 3));
        serial.write('0' + (pcf._days & 0xf));
        serial.write('/');
        serial.write('0' + (pcf._century_months & 0x1f));
        serial.write('/');
        serial.write('2');
        serial.write('0');
        serial.write('0' + (pcf._years >> 4));
        serial.write('0' + (pcf._years & 0xf));
        serial.write(' ');
        serial.write('0' + (pcf._hours >> 4 & 0x3));
        serial.write('0' + (pcf._hours & 0xf));
        serial.write(':');
        serial.write('0' + (pcf._min >> 4 & 0x7));
        serial.write('0' + (pcf._min & 0xf));
        serial.write(':');
        serial.write('0' + (pcf._vl_seconds >> 4 & 0x7));
        serial.write('0' + (pcf._vl_seconds & 0xf));
        serial.write("\r\n");
        serial.write('0' + (pcf._hour_alarm >> 4 & 0x3));
        serial.write('0' + (pcf._hour_alarm & 0xf));
        serial.write(':');
        serial.write('0' + (pcf._minute_alarm >> 4 & 0x7));
        serial.write('0' + (pcf._minute_alarm & 0xf));
        serial.write("\r\n\r\n");

        for (volatile uint32_t i = 0; i < 0xfffff; i++)
            ;
    }

    return 0;
}


