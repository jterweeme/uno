// Reads from serialport, writes back changing uppercase to lowercase and
// vice versa using polling method

#include "misc.h"

int main()
{
    Serial serial;
    serial.init();
    serial.enableRead();

    while (true)
        serial.write(CType::convert(serial.readByte()));

    return 0;
}


