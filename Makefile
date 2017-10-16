CC = avr-g++ --std=c++11 -mmcu=atmega328p
CMD = $(CC) -O2 -c

all:
	$(CMD) app_calc1.cpp misc.h
	$(CMD) app_eedump.cpp
	$(CMD) app_vga1.cpp
	$(CMD) app_vga2.cpp
	$(CMD) tft.cpp
	$(CMD) misc.cpp
	$(CC) -o app_aditbox app_aditbox.o tft.o misc.o
	$(CC) -o app_calc1 app_calc1.o tft.o misc.o
	$(CC) -o app_eedump app_eedump.o misc.o
	$(CC) -o app_vga1 app_vga1.o
	$(CC) -o app_vga2 app_vga2.o

clean:
	rm -vf *.o app_calc1 app_vga1 app_vga2 app_eedump

upload:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U app_vga2


