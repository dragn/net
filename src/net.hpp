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
        , mError(nullptr)
        , mSocket(-1)
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
public:
    ClientStreamSocket()
        : mState(eClientStreamSocketState::New)
        , mError(nullptr)
        , mSocket(-1)
    {
    };
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
        Use GetError() to get error description.
    */
    int Recv(char* outData, size_t dataSize);

    /*
        Get the current state of the socket
    */
    const eClientStreamSocketState::Type& GetState() const
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
    eClientStreamSocketState::Type mState;
    int mSocket;
    const char* mError;

}; // class ClientStreamSocket

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
