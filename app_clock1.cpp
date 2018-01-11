/*
Alarm clock met DFRobot dot matrix display
en PCF8563 RTC

PCF8563

SDA: A4
SCL: A5

RS: D8
EN: D9
Data1: D4
Data2: D5
Data3: D6
Data4: D7
*/

#include <stdio.h>
#include "misc.h"
#include "i2c.h"
#include "lcd1602.h"
#include "analog.h"

#define F_CPU 16000000
#include <util/delay.h>

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
    bus.init(72);   // brr = 72
    PCF8563 pcf(&bus);
    Analog adc;
    adc.init();
    LCD1602 lcd;
    lcd.init();
    lcd.begin(16, 2);
    lcd.clear();
    DFKeypad kb(&adc);
    Timer2 t2;

    while (true)
    {
        char buf[16];
        uint8_t x = kb.read();

        switch (x)
        {
            case DFKeypad::SELECT:
                lcd.backlight();
                break;
            case DFKeypad::LEFT:
                serial.write("Clear alarm!\r\n");
                pcf.alarm(false);
                pcf.write();
                break;
            case DFKeypad::UP:
                lcd.blink();
                break;
            case DFKeypad::DOWN:
                lcd.backlight();
                lcd.clear();
                break;
        }

        pcf.gettime();

        if (pcf.alarm())
            t2.toneA(800);
        else
            t2.noToneA();

        lcd.home();
        dayOfWeek(buf, pcf.dayOfWeek());
        lcd.write(buf);

        snprintf(buf, 16, " %02d/%02d/20%d%d   ", 
            pcf.day(), pcf.month(), pcf.decade(), pcf.yearMod10());

        lcd.write(buf);
        lcd.setCursor(0, 1);
        snprintf(buf, 16, "%02d:%02d:%02d        ", pcf.hour(), pcf.min(), pcf.sec());
        lcd.write(buf);
        _delay_ms(250);
    }

    return 0;
}


