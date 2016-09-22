#include "net.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <hostname>" << std::endl;
        return 0;
    }

    net::InAddr addr;
    addr.ipPort = 80;

    size_t slashPos = 0;
    while (argv[1][slashPos] != '/' && argv[1][slashPos] != '\0') slashPos++;

    std::string host;
    std::string path = "/";
    if (argv[1][slashPos] == '/')
    {
        host = std::string(argv[1], slashPos);
        path = std::string(argv[1] + slashPos);
    }
    else
    {
        host == std::string(argv[1]);
    }

    if (net::GetAddrByName(host.c_str(), addr.ipAddr))
    {
        std::cout << "IP: " << addr << std::endl << std::endl;
        net::ClientStreamSocket sock;
        if (!sock.Connect(addr))
        {
            std::cerr << "Socket error: " << sock.GetError() << std::endl;
            return 0;
        }

        std::string data = "GET ";
        data.append(path);
        data.append(" HTTP/1.1\nHost: ");
        data.append(host);
        data.append("\n\n");

        std::cout << "Request: " << std::endl;
        std::cout << data;

        int snd = sock.Send(data.c_str(), data.size());
        if (snd < 0)
        {
            std::cerr << "Socket Error: " << sock.GetError();
        }

        std::cout << "Response: " << std::endl;
        char buf[256];
        buf[255] = '\0';
        int rcv;
        int wait = 10;

        while (wait--)
        {
            while (rcv = sock.Recv(buf, 255), rcv > 0)
            {
                if (rcv > 0 && rcv < sizeof(buf))
                {
                    buf[rcv] = '\0';
                }
                std::cout << buf;
            }
            if (rcv < 0)
            {
                std::cerr << "Socket Error: " << sock.GetError() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << std::endl;
    }
    else
    {
        std::cerr << "Could not resolve host: " << host << std::endl;
    }

    return 0;
}
