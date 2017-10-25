#include <avr/interrupt.h>
#include "misc.h"

#define F_CPU 16000000
#include <util/delay.h>
#include "infrared.h"

int main()
{
    int RECV_PIN = 10;
    IRrecv irrecv(RECV_PIN);
    decode_results results;
    Serial serial;
    serial.init();
    serial.write("Enabling IRin\r\n");
    irrecv.enableIRIn();
    serial.write("Enabled IRin\r\n");

    while (true)
    {
        if (irrecv.decode(&results))
        {
            switch (results.value)
            {
                case RC::POWER:
                    serial.write("Power button\r\n");
                    break;
                case RC::MODE:
                    serial.write("Mode button\r\n");
                    break;
                case RC::MUTE:
                    serial.write("MUTE\r\n");
                    break;
                case RC::PLAY:
                    serial.write("Play button\r\n");
                    break;
                case RC::PREV:
                    serial.write("Previous button\r\n");
                    break;
                case RC::NEXT:
                    serial.write("Next button\r\n");
                    break;
                case RC::EQ:
                    serial.write("EQ button\r\n");
                    break;
                case RC::MIN:
                    serial.write("-\r\n");
                    break;
                case RC::PLUS:
                    serial.write("+\r\n");
                    break;
                case RC::N0:
                    serial.write("0\r\n");
                    break;
                case RC::ROTATE:
                    serial.write("Rotate?\r\n");
                    break;
                case RC::USD:
                    serial.write("U/SD\r\n");
                    break;
                case RC::N1:
                    serial.write("1\r\n");
                    break;
                case RC::N2:
                    serial.write("2\r\n");
                    break;
                case RC::N3:
                    serial.write("3\r\n");
                    break;
                case RC::N4:
                    serial.write("4\r\n");
                    break;
                case RC::N5:
                    serial.write("5\r\n");
                    break;
                case RC::N6:
                    serial.write("6\r\n");
                    break;
                case RC::N7:
                    serial.write("7\r\n");
                    break;
                case RC::N8:
                    serial.write("8\r\n");
                    break;
                case RC::N9:
                    serial.write("9\r\n");
                    break;
                default:
                    serial.write("?\r\n");
            }
            irrecv.resume();
        }

        _delay_ms(100);
    }

    return 0;
}



