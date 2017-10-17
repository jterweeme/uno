CC = avr-g++ --std=c++11 -mmcu=atmega328p
CMD = $(CC) -O2 -c

all:
	$(CMD) app_7seg1.cpp
	$(CMD) app_calc1.cpp misc.h
	$(CMD) app_eedump.cpp
	$(CMD) app_dotmatrix1.cpp
	$(CMD) app_dotmatrix2.cpp
	$(CMD) app_ps2kb1.cpp
	$(CMD) app_uartloop1.cpp
	$(CMD) app_uartloop2.cpp
	$(CMD) app_vga1.cpp
	$(CMD) app_vga2.cpp
	$(CMD) tft.cpp
	$(CMD) misc.cpp
	$(CC) -o app_dotmatrix2 app_dotmatrix2.o
	$(CC) -o app_7seg1 app_7seg1.o
	$(CC) -o app_calc1 app_calc1.o tft.o misc.o
	$(CC) -o app_dotmatrix1 app_dotmatrix1.o
	$(CC) -o app_eedump app_eedump.o misc.o
	$(CC) -o app_ps2kb1 app_ps2kb1.o
	$(CC) -o app_uartloop1 app_uartloop1.o misc.o
	$(CC) -o app_uartloop2 app_uartloop2.o misc.o
	$(CC) -o app_vga1 app_vga1.o
	$(CC) -o app_vga2 app_vga2.o

clean:
	rm -vf *.o *.gch
	rm -vf app_calc1 app_ps2kb1 app_uartloop1 app_uartloop2 app_vga1 app_vga2 app_eedump

upload:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U app_dotmatrix2


