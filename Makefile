CC = avr-g++ --std=c++11 -mmcu=atmega328p
CMD = $(CC) -O2 -c
APP = app_aditbox.elf

%.o: %.cpp
	$(CMD) $<

%.elf: %.o
	$(CC) -o $@ $^

all: app_7seg1.elf \
app_aditbox.elf \
app_analog1.elf \
app_button1.elf \
app_button2.elf \
app_calc1.elf \
app_calc2.elf \
app_clock1.elf \
app_dotmatrix1.elf \
app_dotmatrix2.elf \
app_echoserv1.elf \
app_eedump.elf \
app_genppm.elf \
app_i2cscan1.elf \
app_icapt1.elf \
app_icapt2.elf \
app_infrared1.elf \
app_infrared2.elf \
app_ringtone1.elf \
app_ringtone2.elf \
app_ringtone3.elf \
app_ringtoneir1.elf \
app_tone1.elf \
app_melody1.elf \
app_ps2kb1.elf \
app_ps2kb2.elf \
app_ps2kb3.elf \
app_ps2kb4.elf \
app_ps2vga1.elf \
app_pcf8563.elf \
app_pwm2ppm.elf \
app_rgb1.elf \
app_sdls1.elf \
app_sdod1.elf \
app_uartloop1.elf \
app_uartloop2.elf \
app_vga1.elf \
app_vga2.elf \
app_vgacalc1.elf

app_7seg1.elf: app_7seg1.o
app_aditbox.elf: app_aditbox.o tft.o misc.o analog.o button.o
app_analog1.elf: app_analog1.o misc.o analog.o
app_button1.elf: app_button1.o misc.o
app_button2.elf: app_button2.o misc.o
app_calc1.elf: app_calc1.o tft.o misc.o analog.o button.o calc.o
app_calc2.elf: app_calc2.o misc.o calc.o
app_clock1.elf: app_clock1.o misc.o i2c.o lcd1602.o analog.o
app_dotmatrix1.elf: app_dotmatrix1.o
app_dotmatrix2.elf: app_dotmatrix2.o lcd1602.o
app_echoserv1.elf: app_echoserv1.o dns.o misc.o dhcp.o arp.o uip.o util.o network.o uip_ethernet.o uip_client.o uip_server.o uip_timer.o uip_udp.o
app_eedump.elf: app_eedump.o misc.o
app_genppm.elf: app_genppm.o
app_i2cscan1.elf: app_i2cscan1.o misc.o i2c.o
app_icapt1.elf: app_icapt1.o misc.o
app_icapt2.elf: app_icapt2.o misc.o
app_infrared1.elf: app_infrared1.o misc.o infrared.o
app_infrared2.elf: app_infrared2.o misc.o infrared.o
app_rgb1.elf: app_rgb1.o misc.o
app_ringtone1.elf: app_ringtone1.o
app_ringtone2.elf: app_ringtone2.o misc.o
app_ringtone3.elf: app_ringtone3.o misc.o lcd1602.o analog.o ringtone.o
app_ringtoneir1.elf: app_ringtoneir1.o misc.o lcd1602.o ringtone.o infrared.o
app_pcf8563.elf: app_pcf8563.o misc.o i2c.o
app_melody1.elf: app_melody1.o
app_ps2kb1.elf: app_ps2kb1.o misc.o keyboard.o vga.o
app_ps2kb2.elf: app_ps2kb2.o misc.o keyboard.o vga.o
app_ps2kb3.elf: app_ps2kb3.o misc.o vga.o
app_ps2kb4.elf: app_ps2kb4.o misc.o vga.o
app_ps2vga1.elf: app_ps2vga1.o vga.o misc.o
app_pwm2ppm.elf: app_pwm2ppm.o
app_sdls1.elf: app_sdls1.o fyle.o zd.o zdfile.o zd2card.o zdvolume.o misc.o
app_sdod1.elf: app_sdod1.o misc.o zd2card.o
app_tone1.elf: app_tone1.o
app_uartloop1.elf: app_uartloop1.o misc.o
app_uartloop2.elf: app_uartloop2.o misc.o
app_vga1.elf: app_vga1.o vga.o
app_vga2.elf: app_vga2.o
app_vgacalc1.elf: app_vgacalc1.o misc.o vga.o calc.o keyboard.o

analog.o: analog.cpp analog.h
app_7seg1.o: app_7seg1.cpp
app_aditbox.o: app_aditbox.cpp misc.h
app_analog1.o: app_analog1.cpp misc.h
app_button1.o: app_button1.cpp misc.h
app_button2.o: app_button2.cpp misc.h
app_calc1.o: app_calc1.cpp misc.h analog.h button.h
app_calc2.o: app_calc2.cpp calc.h
app_clock1.o: app_clock1.cpp misc.h
app_dotmatrix1.o: app_dotmatrix1.cpp
app_dotmatrix2.o: app_dotmatrix2.cpp
app_echoserv1.o: app_echoserv1.cpp uip_client.h uip_server.h uip_ethernet.h
app_eedump.o: app_eedump.cpp misc.h
app_genppm.o: app_genppm.cpp
app_i2cscan1.o: app_i2cscan1.cpp
app_icapt1.o: app_icapt1.cpp misc.h
app_icapt2.o: app_icapt2.cpp misc.h
app_infrared1.o: app_infrared1.cpp misc.h infrared.h
app_infrared2.o: app_infrared2.cpp misc.h infrared.h
app_ringtone1.o: app_ringtone1.cpp
app_ringtone2.o: app_ringtone2.cpp
app_ringtone3.o: app_ringtone3.cpp lcd1602.h analog.h ringtone.h misc.h
app_ringtoneir1.o: app_ringtoneir1.cpp lcd1602.h ringtone.h misc.h infrared.h
app_tone1.o: app_tone1.cpp
app_melody1.o: app_melody1.cpp
app_pcf8563.o: app_pcf8563.cpp
app_ps2kb1.o: app_ps2kb1.cpp misc.h keyboard.h
app_ps2kb2.o: app_ps2kb2.cpp misc.h keyboard.h
app_ps2kb3.o: app_ps2kb3.cpp misc.h keyboard.h
app_ps2kb4.o: app_ps2kb4.cpp misc.h
app_ps2vga1.o: app_ps2vga1.cpp misc.h vga.h
app_pwm2ppm.o: app_pwm2ppm.cpp
app_rgb1.o: app_rgb1.cpp misc.h
app_uartloop1.o: app_uartloop1.cpp misc.h
app_uartloop2.o: app_uartloop2.cpp misc.h
app_vga1.o: app_vga1.cpp vga.h
app_vga2.o: app_vga2.cpp
app_vgacalc1.o: app_vgacalc1.cpp vga.h misc.h calc.h
arp.o: arp.cpp arp.h
button.o: button.cpp button.h
calc.o: calc.cpp calc.h
dhcp.o: dhcp.cpp dhcp.h
dns.o: dns.cpp dns.h
infrared.o: infrared.cpp infrared.h
keyboard.o: keyboard.cpp keyboard.h
misc.o: misc.cpp misc.h
network.o: network.cpp network.h
tft.o: tft.cpp tft.h
i2c.o: i2c.cpp i2c.h
ringtone.o: ringtone.cpp ringtone.h misc.h
lcd1602.o: lcd1602.cpp lcd1602.h
uip.o: uip.cpp
uip_client.o: uip_client.cpp uip_client.h
uip_ethernet.o: uip_ethernet.cpp uip_ethernet.h
uip_server.o: uip_server.cpp uip_server.h
uip_timer.o: uip_timer.cpp uip_timer.h
uip_udp.o: uip_udp.cpp uip_udp.h
util.o: util.cpp util.h
vga.o: vga.cpp vga.h
zd2card.o: zd2card.cpp zd2card.h

clean:
	rm -vf *.o *.gch *.elf

rebuild: clean all

arduino:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U $(APP)


