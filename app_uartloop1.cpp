// Reads from serialport, writes back changing uppercase to lowercase and
// vice versa using polling method

#include "misc.h"

bool isUpper(char c)
{
    return c >= 65 && c <= 90;
}

bool isLower(char c)
{
    return c >= 97 && c <= 122;
}

char convert(char c)
{
    if (isUpper(c))
        return c + 32;

    if (isLower(c))
        return c - 32;

    return c;
}

int main()
{
    Serial serial;
    serial.init();
    serial.enableRead();

    while (true)
        serial.myPutc(convert(serial.readByte()));

    return 0;
}


