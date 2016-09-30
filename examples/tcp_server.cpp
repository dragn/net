#include "net.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <pthread.h>
#include <list>
#include <csignal>
#include <cstring>

class Client
{
public:
    Client() : close(false) {}

    net::ClientStreamSocket sock;
    net::InAddr addr;
    pthread_t thread;

    bool close;
    char buf[256];

    void Handle()
    {
        while (!close)
        {
            std::cout << "recv " << std::endl;
            int rcv = sock.Recv(buf, 255);
            std::cout << "rcv " << rcv << std::endl;
            if (rcv > 0)
            {
                buf[rcv] = '\0';
                std::cout << addr << ": " << buf << std::endl;
                if (strcmp(buf, "bye") == 0)
                {
                    close = true;
                }
            }
            else if (rcv < 0)
            {
                close = true;
            }
        }
        std::cout << "Client closed " << addr << std::endl;
    }
};

static std::list<Client*> clients;
static pthread_mutex_t clients_mutex;
static pthread_t client_sweep_thread;
static bool doExit = false;

static void* handle(void* arg)
{
    Client* client = reinterpret_cast<Client*>(arg);
    client->Handle();
    return NULL;
}

static void* client_sweep(void*)
{
    while (!doExit)
    {
        pthread_mutex_lock(&clients_mutex);

        auto iter = clients.begin();
        while (iter != clients.end())
        {
            if (pthread_tryjoin_np((*iter)->thread, NULL) == 0)
            {
                std::cout << "Client " << (*iter)->addr << " disconnected. Closing...\n";
                delete *iter;
                iter = clients.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        pthread_mutex_unlock(&clients_mutex);
    }
}

net::ServerStreamSocket sock;

void doQuit()
{
    doExit = true;

    pthread_join(client_sweep_thread, NULL);

    // shutdown all clients
    pthread_mutex_lock(&clients_mutex);

    auto iter = clients.begin();
    while (iter != clients.end())
    {
        (*iter)->sock.Close();
        (*iter)->close = true;
        if (pthread_join((*iter)->thread, NULL) == 0)
        {
            std::cout << "Client " << (*iter)->addr << " disconnected. Closing...\n";
            delete *iter;
            iter = clients.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

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

    pthread_create(&client_sweep_thread, NULL, &client_sweep, NULL);

    signal(SIGINT, &handleSigInt);

    bool exit = false;

    while (true)
    {
        Client* client = new Client;
        if (!sock.Accept(client->sock, client->addr))
        {
            std::cerr << "Socket error: " << sock.GetError() << std::endl;
            return 0;
        }

        pthread_mutex_lock(&clients_mutex);

        clients.push_back(client);
        std::cout << "Incoming connection from " << addr << std::endl;
        pthread_create(&clients.back()->thread, NULL, &handle, clients.back());

        pthread_mutex_unlock(&clients_mutex);
    }

    doExit = true;
    pthread_join(client_sweep_thread, NULL);
}
