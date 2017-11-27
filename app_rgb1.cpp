#include "misc.h"
#include <avr/sleep.h>

int main()
{
    Timer0 t0;
    Timer2 t2;
    t0.toneA(0);
    t0.toneB(10);
    t2.toneA(10);  //r

    while (true)
        sleep_mode();

    return 0;
}


