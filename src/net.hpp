#pragma once

#include <ostream>
#include <cstdint>

namespace net
{

typedef uint16_t IPPort;

/*
    IPv4 address representation
*/
union IP4Addr
{
    struct
    {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
    } bytes;
    uint32_t addr;
};

/*
    Internet address representation
*/
struct InAddr
{
    IP4Addr ipAddr;
    IPPort ipPort;
};

namespace eDatagramSocketState
{
enum Type
{
    New,
    Bound,
    Closed
};
}

namespace eClientStreamSocketState
{
enum Type
{
    New,
    Connected,
    Closed
};
}

namespace eServerStreamSocketState
{
enum Type
{
    New,
    Listening,
    Closed
};
}

/*
    Construct IP4Addr from string representation "X.X.X.X"
*/
extern bool ParseIP4Addr(const char* str, IP4Addr& outAddr);

/*
    Construct InAddr from string representation "X.X.X.X:X"
*/
extern bool ParseInAddr(const char* str, InAddr& outAddr);

/*
    Get string representation for the InAddr: "X.X.X.X:X"
    The user should make sure that the buffer is long enough
*/
extern void InAddrToStr(const InAddr& addr, char* str);

/*
    InAddr compare. Returns true if addresses are equal byte-to-byte.
*/
extern bool InAddrEquals(const InAddr& a, const InAddr& b);

/*
    Resolve IPv4 address by hostname
*/
extern bool GetAddrByName(const char* name, IP4Addr& outAddr);

/*
    Cross-platform interface over UDP socket.
*/
class DatagramSocket
{
public:
    DatagramSocket()
        : mState(eDatagramSocketState::New)
        , mSocket(-1)
        , mError(nullptr)
    {
    }
    ~DatagramSocket();

    /*
        Bind this socket to the specified port.
        Use port 0 to take any free port.
        Valid in state New.
        Returns false, if some error occured. Use GetError() to get error description.
    */
    bool Bind(const IPPort& port, bool blocking = false);

    /*
        Close the socket.
        Valid in state Bound.
        Returns false, if some error occured. Use GetError() to get error description.
    */
    bool Close();

    /*
        Reset socket state back to New.
        Valid in state Closed.
        Returns false, if some error occured. Use GetError() to get error description.
    */
    bool Reset();

    /*
        Send data to remote socket.
        Valid in state Bound.

        addr: internet address to send to
        data: pointer to data buffer
        dataSize: number of bytes to send

        Returns the number of actual bytes written or negative number when error occurred.
        Use GetError() to get error description.
    */
    int SendTo(const InAddr& addr, const char* data, size_t dataSize);

    /*
        Receive data from this socket.
        Valid in state Bound.

        outAddr: remote address of the received packet will be stored here
        outData: pointer to buffer to write data to
        dataSize: the size of the buffer, the function is guaranteed not to overwrite the buffer

        Returns the number of bytes received or negative number when error occurred.
        Use GetError() to get error description.
    */
    int RecvFrom(InAddr& outAddr, char* outData, size_t dataSize);

    /*
        Get the current state of the socket
    */
    const eDatagramSocketState::Type& GetState() const
    {
        return mState;
    }

    /*
        Get the description of the last error
    */
    const char* GetError() const
    {
        return mError;
    }

private:
    eDatagramSocketState::Type mState;
    int mSocket;
    const char* mError;

}; // class DatagramSocket

/*
    Client-side TCP socket interface
*/
class ClientStreamSocket
{
    friend class ServerStreamSocket;

public:
    ClientStreamSocket()
        : mState(eClientStreamSocketState::New)
        , mSocket(-1)
        , mError(nullptr)
    {}

    /*
        Construct from existing socket
    */
    ClientStreamSocket(int sock_fd)
        : mState(eClientStreamSocketState::Connected)
        , mSocket(sock_fd)
        , mError(nullptr)
    {}

    /*
        Move constructor
    */
    ClientStreamSocket(ClientStreamSocket&& other)
        : mState(other.mState)
        , mSocket(other.mSocket)
        , mError(nullptr)
    {
        // avoid socket close on destruction
        other.mState = eClientStreamSocketState::Closed;
        other.mSocket = -1;
    }

    ~ClientStreamSocket();

    /*
        Try to connect the socket to the specified address.
        Valid in state New. On successful result switches state to Connected.
        Returns false, if some error occured. Use GetError() to get error description.
    */
    bool Connect(const InAddr& addr);

    /*
        Close the socket.
        Valid in state New or Connected. Switches state to Closed.
        Returns false, if some error occured. Use GetError() to get error description.
    */
    bool Close();

    /*
        Reset socket state back to New.
        Valid in state Closed.
        Returns false, if some error occured. Use GetError() to get error description.
    */
    bool Reset();

    /*
        Send data to remote socket.
        Valid in state Connected.

        data: pointer to data buffer
        dataSize: number of bytes to send

        Returns the number of actual bytes written or negative number when error occurred.
        Use GetError() to get error description.
    */
    int Send(const char* data, size_t dataSize);

    /*
        Receive data from this socket.
        Valid in state Connected.

        outData: pointer to buffer to write data to
        dataSize: the size of the buffer; the function is guaranteed not to overwrite the buffer

        Returns the number of bytes received or negative number when error occurred.
        May return 0 when recv led to connection being closed or operation would block (check the state).
        Use GetError() to get error description.
    */
    int Recv(char* outData, size_t dataSize);

    /*
        Get underlying socket number
    */
    int Get() const
    {
        return mSocket;
    }

    /*
        Get the current state of the socket
    */
    const eClientStreamSocketState::Type& GetState() const
    {
        return mState;
    }

    /*
        Check for closed state
    */
    bool IsClosed() const
    {
        return mState == eClientStreamSocketState::Closed;
    }

    /*
        Get the description of the last error
    */
    const char* GetError() const
    {
        return mError;
    }

private:
    eClientStreamSocketState::Type mState;
    int mSocket;
    const char* mError;

}; // class ClientStreamSocket

class ServerStreamSocket
{
public:
    ServerStreamSocket()
        : mState(eServerStreamSocketState::New)
        , mSocket(-1)
        , mError(nullptr)
    {}

    /*
        Bind and listen for incoming connections on provided address
        Valid in state New.

        Returns true if socket successfully created and bound or false if error occurred.
        Use GetError() to get error description.
    */
    bool Listen(const InAddr& addr);

    /*
        Accepts incoming connections. Returns true on successful accept.
        Setups provided client socket to be used with new connection.
        The call will block until there are incoming connections.

        Returns false on error.
        Use GetError() to get error description.
    */
    bool Accept(ClientStreamSocket& sock, InAddr& addr);
    bool Accept(int& sock, InAddr& addr);

    /*
        Close underlying socket
    */
    bool Close();

    /*
        Get the current state of the socket
    */
    const eServerStreamSocketState::Type& GetState() const
    {
        return mState;
    }

    /*
        Get the description of the last error
    */
    const char* GetError() const
    {
        return mError;
    }

    /*
        Get underlying socket number
    */
    int Get() const
    {
        return mSocket;
    }

private:
    InAddr mAddr;
    int mSocket;
    eServerStreamSocketState::Type mState;
    const char* mError;
}; // class ServerStreamSocket

/*
   std::ostream operators
*/
std::ostream& operator<<(std::ostream& os, const IP4Addr& addr);
std::ostream& operator<<(std::ostream& os, const InAddr& addr);

/*
    std::wostream operators
*/
std::wostream& operator<<(std::wostream& os, const IP4Addr& addr);
std::wostream& operator<<(std::wostream& os, const InAddr& addr);

/*
    Global platform-specific networking initialization
*/
extern bool Init();

/*
    Global platform-specific networking deinitialization
*/
extern void Close();

class ScopeInit
{
public:
    ScopeInit()
    {
        net::Init();
    }
    ~ScopeInit()
    {
        net::Close();
    }
};

} // namespace net
