#include "net.hpp"

#include <ev++.h>
#include <iostream>

class Client
{
public:
    Client(int sock_fd, const net::InAddr& addr) :
        sock(sock_fd),
        addr(addr)
    {
        watcher.set<Client, &Client::Read>(this);
        watcher.start(sock.Get(), ev::READ);

        std::cout << "Client connected: " << addr << std::endl;
    }

    char buf[256];

    void Read(ev::io &w, int revents)
    {
        int sz = sock.Recv(buf, 256);
        if (sz > 0)
        {
            std::cout << sz << " bytes received" << std::endl;
            std::cout << addr << ": " << buf;
        }
        else
        {
            if (sz != 0)
            {
                std::cerr << "Socket error: " << sock.GetError() << std::endl;
            }
            std::cout << "Client disconnected: " << addr << std::endl;
            sock.Close();
            delete this;
        }
    }

private:
    net::ClientStreamSocket sock;
    net::InAddr addr;
    ev::io watcher;
};

class Server
{
public:
    net::InAddr addr;

    bool Listen(const net::InAddr& addr)
    {
        if (!sock.Listen(addr))
        {
            std::cerr << "Socket error: " << sock.GetError() << std::endl;
            return false;
        }

        watcher.set<Server, &Server::Accept>(this);
        watcher.start(sock.Get(), ev::READ);

        std::cout << "Listening on " << addr << std::endl;

        return true;
    }

    void Accept(ev::io& w, int revents)
    {
        int client_sock;
        net::InAddr addr;

        if (sock.Accept(client_sock, addr))
        {
            Client* client = new Client(client_sock, addr);
        }
    }

    void Close()
    {
        sock.Close();
    }

private:
    net::ServerStreamSocket sock;
    ev::io watcher;
};

Server serv;

static void sig_cb(ev::sig& w, int signal)
{
    std::cout << "Exit signal received" << std::endl;

    serv.Close();

    w.loop.break_loop();
}

int main()
{
    net::ScopeInit init;

    net::InAddr addr;
    net::ParseInAddr("127.0.0.1:20000", addr);

    if (serv.Listen(addr))
    {
        ev::sig sig_watcher;
        sig_watcher.set<sig_cb>();
        sig_watcher.start(SIGINT);

        ev_run(EV_DEFAULT, 0);
    }
}
