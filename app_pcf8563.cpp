#include <stdio.h>
#include "misc.h"
#include "i2c.h"

int main()
{
    Serial serial;
    serial.init();
    I2CBus bus;
    bus.init(72);
    PCF8563 pcf(&bus);
    pcf.gettime();
    char buf[10];
    snprintf(buf, 10, "%d\r\n", pcf.sec());
    serial.write(buf);

    while (true)
        ;

    return 0;
}


