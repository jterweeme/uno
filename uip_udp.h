#ifndef UIPUDP_H
#define UIPUDP_H

#include "util.h"
#include "uip.h"
#include "uip_client.h"

#define UIP_UDP_MAXDATALEN 1500
#define UIP_UDP_PHYH_LEN UIP_LLH_LEN+UIP_IPUDPH_LEN
#define UIP_UDP_MAXPACKETSIZE UIP_UDP_MAXDATALEN+UIP_UDP_PHYH_LEN

typedef struct {
  memaddress out_pos;
  memhandle packet_next;
  memhandle packet_in;
  memhandle packet_out;
  bool send;
} uip_udp_userdata_t;

class UDeeP : public Ztream
{
public:
    virtual uint8_t begin(uint16_t) =0;   // initialize, start listening on specified port
    virtual void stop() =0;  // Finish with the UDP socket
    virtual int beginPacket(IPAddrezz ip, uint16_t port) =0;
    virtual int beginPacket(const char *host, uint16_t port) =0;
    virtual int endPacket() =0;
    virtual size_t write(uint8_t) =0;
    virtual size_t write(const uint8_t *buffer, size_t size) =0;
    virtual int parsePacket() =0;
    virtual int available() =0;
    virtual int read() =0;
    virtual int read(unsigned char* buffer, size_t len) =0;
    virtual int read(char* buffer, size_t len) =0;
    virtual int peek() =0;
    virtual void flush() =0;  // Finish reading the current packet
    virtual IPAddrezz remoteIP() =0;
    virtual uint16_t remotePort() =0;
protected:
    uint8_t* rawIPAddress(IPAddrezz& addr) { return addr.raw_address(); };
};

class UIPUDP : public UDeeP
{
private:
    struct uip_udp_conn *_uip_udp_conn;
    uip_udp_userdata_t appdata;
public:
    UIPUDP();  // Constructor
    uint8_t begin(uint16_t);
    void stop();  // Finish with the UDP socket
    int beginPacket(IPAddrezz ip, uint16_t port);
    int beginPacket(const char *host, uint16_t port);
    int endPacket();
    size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  using Prynt::write;
  int parsePacket();
  int available();
  int read();
  int read(unsigned char* buffer, size_t len);
  int read(char* buffer, size_t len) { return read((unsigned char*) buffer, len); }
  int peek();
  void flush();	// Finish reading the current packet
  IPAddrezz remoteIP();
  uint16_t remotePort();
private:
  friend void uipudp_appcall(void);
  friend class UIPEthernetClass;
  static void _send(uip_udp_userdata_t *data);
};

#endif
