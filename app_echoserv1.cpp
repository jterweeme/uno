/*
ethernet example met HanRun HR911105A

UNO P12 = SO / SPI MISO
UNO P11 = SI / SPI MOSI
UNO P13 = SCK / SPI SCK
UNO P10 = CS / SPI SS

*/

#include "uip_ethernet.h"
#include "uip_server.h"
#include "uip_client.h"

#define F_CPU 16000000UL
#include <util/delay.h>

#include <stdio.h>

#include "misc.h"

EthernetServer server = EthernetServer(80);
Serial *g_serial;

int main()
{
    Serial s;
    g_serial = &s;
    s.init();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    IPAddrezz myIP(192,168,178,32);
    Ethernet.begin(mac,myIP);
    server.begin();

    while (true)
    {
#if 0
        size_t size;

        if (EthernetClient client = server.available())
        {
            if (client)
            {
                while((size = client.available()) > 0)
                {
                    uint8_t* msg = (uint8_t*)malloc(size);
                    size = client.read(msg,size);
                    char onzin[50];
                    snprintf(onzin, 50, "%u\r\n", size);
                    s.write(onzin);
                    s.write((char *)msg);
                    client.write(msg,size);
                    free(msg);
                }
            }
        }
#else
        EthernetClient client = server.available();

        if (client)
        {
            s.write("new client\r\n");
            bool currentLineIsBlank = true;

            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
                    s.write(c);

                    if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin

          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            //int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            //client.print(sensorReading);
            client.println("<br />");       
          }
          client.println("</html>");
           break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    _delay_ms(1);
    
            // close the connection:
            client.stop();
            s.write("client disonnected\r\n");
        }
#endif

    }

    return 0;
}




