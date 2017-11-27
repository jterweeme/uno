#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

#define Ethernet UIPEthernet
#define EthernetClient UIPClient
#define EthernetServer UIPServer
#define EthernetUDP UIPUDP

unsigned long millis(void);

__extension__ typedef int __guard __attribute__((mode (__DI__)));
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);

extern "C" void __cxa_pure_virtual(void);

class Prynt;

class Pryntable
{
public:
    virtual size_t printTo(Prynt& p) const = 0;
};

static const uint8_t DEC = 10;

class Prynt
{
private:
    int write_error;
    size_t printNumber(unsigned long, uint8_t);
    //size_t printFloat(double, uint8_t);
protected:
    void setWriteError(int err = 1) { write_error = err; }
public:
    Prynt() : write_error(0) {}

    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }

    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);

    //size_t print(const __FlashStringHelper *);
    //size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    //size_t print(double, int = 2);
    size_t print(const Pryntable&);
    //size_t println(const __FlashStringHelper *);
    //size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(uint8_t, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    //size_t println(double, int = 2);
    size_t println(const Pryntable&);
    size_t println(void);
};

class IPAddrezz : public Pryntable
{
private:
    union {
        uint8_t a8[4];  // IPv4 address
        uint32_t a32;
    } _address;

    uint8_t* raw_address() { return _address.a8; }
public:
    IPAddrezz() { memset(_address.a8, 0, sizeof(_address)); }
    IPAddrezz(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddrezz(uint32_t address) { _address.a32 = address; }
    IPAddrezz(const uint8_t *address) { memcpy(_address.a8, address, sizeof(_address)); }
    operator uint32_t() { return _address.a32; }
    bool operator==(const IPAddrezz& addr) { return _address.a32 == addr._address.a32; };
    bool operator==(const uint8_t* addr);
    uint8_t operator[](int index) const { return _address.a8[index]; };
    uint8_t& operator[](int index) { return _address.a8[index]; };
    IPAddrezz& operator=(const uint8_t *address);
    IPAddrezz& operator=(uint32_t address);
    virtual size_t printTo(Prynt& p) const;

    //friend class EthernetClass;
    friend class UDeeP;
    friend class Klient;
    //friend class Server;
    //friend class DhcpClass;
    friend class DNSClient;
};

class Ztream : public Prynt
{
protected:
    unsigned long _timeout;
    unsigned long _startMillis;  // used for timeout measurement
    int timedRead();    // private method to read stream with timeout
    int timedPeek();    // private method to peek stream with timeout
    int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout
public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    Ztream() {_timeout=1000;}
    void setTimeout(unsigned long timeout);
    bool find(char *target);   // reads data from the stream until the target string is found
    bool find(char *target, size_t length);
    bool findUntil(char *target, char *terminator);
    bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);
    long parseInt(); // returns the first valid (long) integer value from the current position.
    float parseFloat();               // float version of parseInt
    size_t readBytes( char *buffer, size_t length); // read chars from stream into buffer
    size_t readBytesUntil( char terminator, char *buffer, size_t length);
    //String readString();
    //String readStringUntil(char terminator);
protected:
    long parseInt(char skipChar); // as above but the given skipChar is ignored
    float parseFloat(char skipChar);  // as above but the given skipChar is ignored
};

class Klient : public Ztream
{
public:
    virtual int connect(IPAddrezz ip, uint16_t port) =0;
    virtual int connect(const char *host, uint16_t port) =0;
    virtual size_t write(uint8_t) =0;
    virtual size_t write(const uint8_t *buf, size_t size) =0;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int read(uint8_t *buf, size_t size) = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    virtual void stop() = 0;
    virtual uint8_t connected() = 0;
    virtual operator bool() = 0;
protected:
    uint8_t* rawIPAddress(IPAddrezz& addr) { return addr.raw_address(); };
};

#endif
