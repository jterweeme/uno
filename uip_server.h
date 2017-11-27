#ifndef UIPSERVER_H
#define UIPSERVER_H

#include "uip_client.h"
#include "util.h"

class Zerver : public Prynt
{
public:
    virtual void begin() =0;
};

class UIPServer : public Zerver
{
public:
    UIPServer(uint16_t);
    UIPClient available();
    void begin();
    size_t write(uint8_t);
    size_t write(const uint8_t *buf, size_t size);
    using Prynt::write;
private:
    uint16_t _port;
};

#endif
