

class EthernetServer
{
public:
    EthernetServer(uint16_t);
    void begin();
};

void EthernetServer::begin()
{
    listen(_port);
    EthernetClass::tick();
}

EthernetServer server = EthernetServer(1000);

int main()
{
    uint8_t mac[6] = {0,1,2,3,4,5};
    IPAddress myIP(192,168,200,56);
    Ethernet.begin(mac, myIP);
    server.begin();

    while (true)
    {
        size_t size;

        if (EthernetClient client = server.available())
        {
            if (client)
            {
                while ((size = client.available()) > 0)
                {
                    uint8_t *msg = (uint8_t *)malloc(size);
                    size = client.read(msg, size);
                    client.write(msg, size);
                    free(msg);
                }
            }
        }
    }

    return 0;
}


