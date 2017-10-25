#include <stdio.h>
#include "misc.h"
#include "i2c.h"
#include "lcd1602.h"
#include "analog.h"

#define F_CPU 16000000
#include <util/delay.h>

void dayOfWeek(char *dag, uint8_t day)
{
    switch (day)
    {
        case 0:
            snprintf(dag, 3, "Ma");
            break;
        case 1:
            snprintf(dag, 3, "Di");
            break;
        case 2:
            snprintf(dag, 3, "Wo");
            break;
        case 3:
            snprintf(dag, 3, "Do");
            break;
        case 4:
            snprintf(dag, 3, "Vr");
            break;
        case 5:
            snprintf(dag, 3, "Za");
            break;
        case 6:
            snprintf(dag, 3, "Zo");
            break;
        default:
            snprintf(dag, 3, "??");
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
        char dag[5];
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
        }

        pcf.gettime();

        if (pcf.alarm())
            t2.toneA(800);
        else
            t2.noToneA();

        lcd.setCursor(0, 0);
        dayOfWeek(dag, pcf.dayOfWeek());

        snprintf(buf, 16, "%s %02d/%02d/20%d%d", dag,
            pcf.day(), pcf.month(), pcf.decade(), pcf.yearMod10());

        lcd.write(buf);
        lcd.setCursor(0, 1);
        snprintf(buf, 16, "%02d:%02d:%02d", pcf.hour(), pcf.min(), pcf.sec());
        lcd.write(buf);
        _delay_ms(250);
    }

    return 0;
}


