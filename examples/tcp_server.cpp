#include "net.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <list>
#include <csignal>
#include <cstring>
#include <vector>

#ifdef CMAKE_PLATFORM_WINDOWS
#include <WinSock2.h>
#define poll WSAPoll
#define pollfd WSAPOLLFD
typedef ULONG nfds_t;
#endif // CMAKE_PLATFORM_WINDOWS

#ifdef CMAKE_PLATFORM_UNIX
#include <poll.h>
#endif // CMAKE_PLATFORM_UNIX

const int MAX_CLIENTS = 10;

bool doExit = false;

class Client
{
public:
    Client(int fd, const net::InAddr& addr) : mSock(fd), mAddr(addr) {}

    // move constructor
    Client(Client&& client) : mSock(std::move(client.mSock)), mAddr(client.mAddr) {}

    net::ClientStreamSocket mSock;
    net::InAddr mAddr;
};

net::ServerStreamSocket sock;

std::vector<Client> clients;

void doQuit()
{
    doExit = true;

    for (Client& client : clients)
    {
        client.mSock.Close();
    }

    // close server socket
    sock.Close();
}

void handleSigInt(int sig)
{
    std::cout << "Exit signal recevied\n";

    doQuit();
}

int main()
{
    net::ScopeInit init;

    net::InAddr addr;
    net::ParseInAddr("127.0.0.1:20000", addr);

    if (!sock.Listen(addr))
    {
        std::cerr << "Socket error: " << sock.GetError() << std::endl;
        return 0;
    }

    std::cout << "Listening on " << addr << std::endl;

    signal(SIGINT, &handleSigInt);

    pollfd fds[MAX_CLIENTS + 1];
    nfds_t nfds = 1;

    fds[0].fd = sock.Get();
    fds[0].events = POLLIN;

    char buf[256];

    while (!doExit)
    {
        poll(fds, nfds, -1);

        if (fds[0].revents & POLLIN)
        {
            int cltFd;
            net::InAddr cltAddr;
            if (sock.Accept(cltFd, cltAddr))
            {
                std::cout << "New connection: " << cltAddr << std::endl;

                if (nfds == MAX_CLIENTS + 1)
                {
                    std::cout << "Client limit exceeded!" << std::endl;
                    net::ClientStreamSocket css(cltFd);
                    css.Close();
                }
                else
                {
                    clients.push_back(Client(cltFd, cltAddr));
                    fds[nfds].fd = cltFd;
                    fds[nfds].events = POLLIN;
                    fds[nfds].revents = POLLIN;
                    ++nfds;
                }
            }
        }

        for (size_t idx = 1; idx < nfds; ++idx)
        {
            if (fds[idx].revents & POLLIN)
            {
                Client& client = clients[idx - 1];
                net::ClientStreamSocket& sock = client.mSock;

                int sz = sock.Recv(buf, sizeof(buf) - 1);
                if (sz < 0)
                {
                    std::cout << "Socket error: " << sock.GetError() << std::endl;
                }
                else if (sz > 0)
                {
                    buf[sz] = '\0';
                    std::cout << "> " << client.mAddr << " <: " << buf << std::endl;
                }
#ifdef CMAKE_PLATFORM_UNIX
                else if (sz == 0)
                {
                    std::cout << "Close: " << clients[idx - 1].mAddr << std::endl;
                    fds[idx].fd = -1;
                    fds[idx].revents = 0;
                    clients[idx - 1].mSock.Close();
                }
#endif // CMAKE_PLATFORM_UNIX
            }
            else if (fds[idx].revents & (POLLHUP | POLLERR))
            {
                std::cout << "Close: " << clients[idx - 1].mAddr << std::endl;
                fds[idx].fd = -1;
                fds[idx].revents = 0;
                clients[idx - 1].mSock.Close();
            }
        }
    }
}
