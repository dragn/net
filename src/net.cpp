#include "net.hpp"

#include <cstring>

#if CMAKE_PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#define close closesocket
#define LAST_ERROR WSAGetLastError()

#elif CMAKE_PLATFORM_UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#define LAST_ERROR errno
#define SOCKET_ERROR -1

#else
#error Unsupported system. Check that CMAKE_PLATFORM is defined.
#endif

static const char* GetStringForError(const int err)
{
    switch (err)
    {
        case EINTR:
            return ("Error: (EINTR) A blocking operation was interrupted.");

        case EACCES:
            return ("Error: (EACCES) Trying to access a socket in a way forbidden by its access permissions.");

        case EFAULT:
            return ("Error: (EFAULT) Invalid Pointer or Buffer Length too small.");

        case EINVAL:
            return ("Error: (EINVAL) Invalid argument supplied.");

        case EMFILE:
            return ("Error: (EMFILE) No more sockets are available.");

        case EWOULDBLOCK:
            return ("Warning: (EWOULDBLOCK) Operation on nonblocking socket cannot be completed immediately.");

        case EINPROGRESS:
            return ("Error: (EINPROGRESS) A blocking operation is currently executing.");

        case EALREADY:
            return ("Error: (EALREADY) A nonblocking operation is currently executing.");

        case ENOTSOCK:
            return ("Error: (ENOTSOCK) Socket operation on nonsocket.");

        case EDESTADDRREQ:
            return ("Error: (EDESTADDRREQ) Destination address required.");

        case EMSGSIZE:
            return ("Error: (EMSGSIZE) Send Data too big, or Receive Data larger than Receive Buffer.");

        case EPROTOTYPE:
            return ("Error: (EPROTOTYPE) Wrong Protocol type for socket.");

        case ENOPROTOOPT:
            return ("Error: (ENOPROTOOPT) Unknown, invalid or unsupported option or level specified in getsockopt or setsockopt.");

        case EPROTONOSUPPORT:
            return ("Error: (EPROTONOSUPPORT) The requested protocol is not supported.");

        case EOPNOTSUPP:
            return ("Error: (EOPNOTSUPP) The attempted operation is not supported for the type of object referenced.");

        case EAFNOSUPPORT:
            return ("Error: (EAFNOSUPPORT) The specified address family is not supported.");

        case EADDRINUSE:
            return ("Error: (EADDRINUSE) Local address is already in use.");

        case EADDRNOTAVAIL:
            return ("Error: (EADDRNOTAVAIL) The address is not valid in its context.");

        case ENETDOWN:
            return ("Error: (ENETDOWN) Network is down.");

        case ENETUNREACH:
            return ("Error: (ENETUNREACH) A socket operation was attempted to an unreachable network.");

        case ENETRESET:
            return ("Error: (ENETRESET) The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.");

        case ECONNABORTED:
            return ("Error: (ECONNABORTED) Connection aborted by Host, possibly due to a data transmission time-out or protocol error.");

        case ECONNRESET:
            return ("Error: (ECONNRESET) Connection forcibly closed by the remote host.");

        case ENOBUFS:
            return ("Error: (ENOBUFS) No buffer space is available for socket.");

        case EISCONN:
            return ("Error: (EISCONN) A connect request was made on an already-connected socket.");

        case ENOTCONN:
            return ("Error: (ENOTCONN) Socket is not connected.");

        case ETIMEDOUT:
            return ("Error: (ETIMEDOUT) Connection timed out.");

        case ECONNREFUSED:
            return ("Error: (ECONNREFUSED) Target refused Connection.");

        case EHOSTUNREACH:
            return ("Error: (EHOSTUNREACH) A socket operation was attempted to an unreachable host.");

#if CMAKE_PLATFORM_WINDOWS
        case WSAEINTR:
            return ("Error: (WSAEINTR) A blocking operation was interrupted by a call to WSACancelBlockingCall.");

        case WSAEACCES:
            return ("Error: (WSAEACCES) Trying to access a socket in a way forbidden by its access permissions.");

        case WSAEFAULT:
            return ("Error: (WSAEFAULT) Invalid Pointer or Buffer Length too small.");

        case WSAEINVAL:
            return ("Error: (WSAEINVAL) Invalid argument supplied.");

        case WSAEMFILE:
            return ("Error: (WSAEMFILE) No more sockets are available.");

        case WSAEWOULDBLOCK:
            return ("Warning: (WSAEWOULDBLOCK) Operation on nonblocking socket cannot be completed immediately.");

        case WSAEINPROGRESS:
            return ("Error: (WSAEINPROGRESS) A blocking operation is currently executing.");

        case WSAEALREADY:
            return ("Error: (WSAEALREADY) A nonblocking operation is currently executing.");

        case WSAENOTSOCK:
            return ("Error: (WSAENOTSOCK) Socket operation on nonsocket.");

        case WSAEDESTADDRREQ:
            return ("Error: (WSAEDESTADDRREQ) Destination address required.");

        case WSAEMSGSIZE:
            return ("Error: (WSAEMSGSIZE) Send Data too big, or Receive Data larger than Receive Buffer.");

        case WSAEPROTOTYPE:
            return ("Error: (WSAEPROTOTYPE) Wrong Protocol type for socket.");

        case WSAENOPROTOOPT:
            return ("Error: (WSAENOPROTOOPT) Unknown, invalid or unsupported option or level specified in getsockopt or setsockopt.");

        case WSAEPROTONOSUPPORT:
            return ("Error: (WSAEPROTONOSUPPORT) The requested protocol is not supported.");

        case WSAESOCKTNOSUPPORT:
            return ("Error: (WSAESOCKTNOSUPPORT) The specified socket type is not supported in this address family.");

        case WSAEOPNOTSUPP:
            return ("Error: (WSAEOPNOTSUPP) The attempted operation is not supported for the type of object referenced.");

        case WSAEPFNOSUPPORT:
            return ("Error: (WSAEPFNOSUPPORT) The specified Protocol family is not supported.");

        case WSAEAFNOSUPPORT:
            return ("Error: (WSAEAFNOSUPPORT) The specified address family is not supported.");

        case WSAEADDRINUSE:
            return ("Error: (WSAEADDRINUSE) Local address is already in use.");

        case WSAEADDRNOTAVAIL:
            return ("Error: (WSAEADDRNOTAVAIL) The address is not valid in its context.");

        case WSAENETDOWN:
            return ("Error: (WSAENETDOWN) Network is down.");

        case WSAENETUNREACH:
            return ("Error: (WSAENETUNREACH) A socket operation was attempted to an unreachable network.");

        case WSAENETRESET:
            return ("Error: (WSAENETRESET) The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.");

        case WSAECONNABORTED:
            return ("Error: (WSAECONNABORTED) Connection aborted by Host, possibly due to a data transmission time-out or protocol error.");

        case WSAECONNRESET:
            return ("Error: (WSAECONNRESET) Connection forcibly closed by the remote host.");

        case WSAENOBUFS:
            return ("Error: (WSAENOBUFS) No buffer space is available for socket.");

        case WSAEISCONN:
            return ("Error: (WSAEISCONN) A connect request was made on an already-connected socket.");

        case WSAENOTCONN:
            return ("Error: (WSAENOTCONN) Socket is not connected.");

        case WSAESHUTDOWN:
            return ("Error: (WSAESHUTDOWN) A Send or Receive was disallowed because the socket had already been shut down in that direction.");

        case WSAETIMEDOUT:
            return ("Error: (WSAETIMEDOUT) Connection timed out.");

        case WSAECONNREFUSED:
            return ("Error: (WSAECONNREFUSED) Target refused Connection.");

        case WSAEHOSTDOWN:
            return ("Error: (WSAEHOSTDOWN) A socket operation encountered a dead host.");

        case WSAEHOSTUNREACH:
            return ("Error: (WSAEHOSTUNREACH) A socket operation was attempted to an unreachable host.");

        case WSAEPROCLIM:
            return ("Error: (WSAEPROCLIM) Too many Apps using Sockets.");

        case WSASYSNOTREADY:
            return ("Error: (WSASYSNOTREADY) Network subsystem is unavailable.");

        case WSAVERNOTSUPPORTED:
            return ("Error: (WSAVERNOTSUPPORTED) Winsock.dll version out of range.");

        case WSANOTINITIALISED:
            return ("Error: (WSANOTINITIALISED) Successful WSAStartup not yet performed.");

        case WSAEDISCON:
            return ("Error: (WSAEDISCON) Graceful shutdown in progress.");

        case WSATYPE_NOT_FOUND:
            return ("Error: (WSATYPE_NOT_FOUND) The specified class was not found.");

        case WSAHOST_NOT_FOUND:
            return ("Error: (WSAHOST_NOT_FOUND) No such host is known.");

        case WSATRY_AGAIN:
            return ("Error: (WSATRY_AGAIN) Nonauthoritative host not found.");

        case WSANO_RECOVERY:
            return ("Error: (WSANO_RECOVERY) A nonrecoverable error occurred.");

        case WSANO_DATA:
            return ("Error: (WSANO_DATA) No data record of requested type.");

        case WSA_INVALID_HANDLE:
            return ("Error: (WSA_INVALID_HANDLE) Specified event object handle is invalid.");

        case WSA_INVALID_PARAMETER:
            return ("Error: (WSA_INVALID_PARAMETER) One or more parameters are invalid.");

        case WSA_IO_INCOMPLETE:
            return ("Error: (WSA_IO_INCOMPLETE) Overlapped I/O event object not in signaled state.");

        case WSA_IO_PENDING:
            return ("Error: (WSA_IO_PENDING) Overlapped operation will complete later.");

        case WSA_NOT_ENOUGH_MEMORY:
            return ("Error: (WSA_NOT_ENOUGH_MEMORY) Insufficient memory available.");

        case WSA_OPERATION_ABORTED:
            return ("Error: (WSA_OPERATION_ABORTED) Overlapped operation aborted.");

        case WSASYSCALLFAILURE:
            return ("Error: (WSASYSCALLFAILURE) Generic error code, returned under various conditions.");
#endif // CMAKE_PLATFORM_WINDOWS

        default:
            return "Unknown error";
    }
}

static bool SetSocketBlockingEnabled(int fd, bool blocking)
{
    if (fd < 0)
        return false;

#if CMAKE_PLATFORM_WINDOWS
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else // CMAKE_PLATFORM_WINDOWS
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif // CMAKE_PLATFORM_WINDOWS
}

namespace net
{
extern bool ParseIP4Addr(const char* str, IP4Addr& outAddr)
{
    uint32_t a, b, c, d;
    if (sscanf(str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
        return false;
    outAddr.bytes.a = (uint8_t)a;
    outAddr.bytes.b = (uint8_t)b;
    outAddr.bytes.c = (uint8_t)c;
    outAddr.bytes.d = (uint8_t)d;
    return true;
}

extern bool ParseInAddr(const char* str, InAddr& outAddr)
{
    uint32_t a, b, c, d, port;
    if (sscanf(str, "%u.%u.%u.%u:%u", &a, &b, &c, &d, &port) != 5)
        return false;
    outAddr.ipAddr.bytes.a = (uint8_t)a;
    outAddr.ipAddr.bytes.b = (uint8_t)b;
    outAddr.ipAddr.bytes.c = (uint8_t)c;
    outAddr.ipAddr.bytes.d = (uint8_t)d;
    outAddr.ipPort = (IPPort)port;
    return true;
}

extern void InAddrToStr(const InAddr& addr, char* str)
{
    sprintf(str, "%hhu.%hhu.%hhu.%hhu:%hu", addr.ipAddr.bytes.a, addr.ipAddr.bytes.b, addr.ipAddr.bytes.c, addr.ipAddr.bytes.d, addr.ipPort);
}

extern bool InAddrEquals(const InAddr& a, const InAddr& b)
{
    return a.ipAddr.addr == b.ipAddr.addr && a.ipPort == b.ipPort;
}

std::ostream& operator<<(std::ostream& os, const IP4Addr& addr)
{
    return os << +addr.bytes.a << "." << +addr.bytes.b << "." << +addr.bytes.c << "." << +addr.bytes.d;
}

std::ostream& operator<<(std::ostream& os, const InAddr& addr)
{
    return os << addr.ipAddr << ":" << addr.ipPort;
}

std::wostream& operator<<(std::wostream& os, const IP4Addr& addr)
{
    os << +addr.bytes.a << "." << +addr.bytes.b << "." << +addr.bytes.c << "." << +addr.bytes.d;
    return os;
}

std::wostream& operator<<(std::wostream& os, const InAddr& addr)
{
    os << addr.ipAddr << ":" << addr.ipPort;
    return os;
}

bool DatagramSocket::Bind(const IPPort& port, bool blocking /* = false */)
{
    if (mState != eDatagramSocketState::New)
    {
        mError = "Invalid state";
        return false;
    }

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (mSocket == -1)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (bind(mSocket, (sockaddr*)&addr, sizeof(addr)) != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    if (!blocking)
    {
        if (!SetSocketBlockingEnabled(mSocket, false))
        {
            mError = "Unable to set non-blocking mode";
            return false;
        }
    }

    mState = eDatagramSocketState::Bound;
    return true;
}

int DatagramSocket::SendTo(const InAddr& addr, const char* data, size_t dataSize)
{
    if (mState != eDatagramSocketState::Bound)
    {
        mError = "Invalid state";
        return -1;
    }

    sockaddr_in to;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    memcpy(&to.sin_addr, &addr.ipAddr, sizeof(addr.ipAddr));
    to.sin_port = htons(addr.ipPort);

    int result = sendto(mSocket, data, dataSize, 0, (sockaddr*)&to, sizeof(to));

    if (result == SOCKET_ERROR)
    {
        mError = GetStringForError(LAST_ERROR);
        return -1;
    }

    return result;
}

int DatagramSocket::RecvFrom(InAddr& outAddr, char* outData, size_t dataSize)
{
    if (mState != eDatagramSocketState::Bound)
    {
        mError = "Invalid state";
        return -1;
    }

    sockaddr_in from;
    socklen_t fromSize = sizeof(from);

    int result = recvfrom(mSocket, outData, dataSize, 0, (sockaddr*)&from, &fromSize);

    if (result == SOCKET_ERROR)
    {
#if CMAKE_PLATFORM_WINDOWS
        // Ignore WinSock's ECONNRESET.
        // "On a UDP-datagram socket this error indicates a previous send operation resulted in an ICMP Port Unreachable message."
        if (LAST_ERROR == WSAEWOULDBLOCK || LAST_ERROR == WSAECONNRESET)
            return 0;
#elif CMAKE_PLATFORM_UNIX
        if (LAST_ERROR == EWOULDBLOCK)
            return 0;
#endif
        mError = GetStringForError(LAST_ERROR);
        return -1;
    }

    outAddr.ipPort = (IPPort)ntohs(from.sin_port);
    memcpy(&outAddr.ipAddr, &from.sin_addr, sizeof(IP4Addr));

    return result;
}

bool DatagramSocket::Close()
{
    if (mState != eDatagramSocketState::Bound)
    {
        mError = "Invalid state";
        return false;
    }

    if (close(mSocket) != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    mState = eDatagramSocketState::Closed;
    mError = "";
    return true;
}

bool DatagramSocket::Reset()
{
    if (mState != eDatagramSocketState::Closed)
    {
        mError = "Invalid state";
        return false;
    }
    mState = eDatagramSocketState::New;
    mSocket = -1;

    return true;
}

DatagramSocket::~DatagramSocket()
{
    if (mState == eDatagramSocketState::Bound)
    {
        Close();
    }
}

ClientStreamSocket::~ClientStreamSocket()
{
    if (mState == eClientStreamSocketState::Connected)
    {
        Close();
    }
}

bool ClientStreamSocket::Connect(const InAddr& addr)
{
    if (mState != eClientStreamSocketState::New)
    {
        mError = "Invalid state";
        return false;
    }

    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mSocket == -1)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }
    sockaddr_in to;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    memcpy(&to.sin_addr, &addr.ipAddr, sizeof(addr.ipAddr));
    to.sin_port = htons(addr.ipPort);

    int result = connect(mSocket, (sockaddr*)&to, sizeof(to));
    if (result != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    if (!SetSocketBlockingEnabled(mSocket, false))
    {
        mError = "Unable to set non-blocking mode";
        return false;
    }

    mState = eClientStreamSocketState::Connected;
    mError = "";
    return true;
}

bool ClientStreamSocket::Close()
{
    if (mState != eClientStreamSocketState::Connected && mState != eClientStreamSocketState::New)
    {
        mError = "Invalid state";
        return false;
    }

    if (mState == eClientStreamSocketState::New)
    {
        mState = eClientStreamSocketState::Closed;
        return true;
    }


    if (close(mSocket) != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    mSocket = -1;

    mState = eClientStreamSocketState::Closed;
    mError = "";
    return true;
}

bool ClientStreamSocket::Reset()
{
    if (mState != eClientStreamSocketState::Closed)
    {
        mError = "Invalid state";
        return false;
    }
    mState = eClientStreamSocketState::New;
    mSocket = -1;

    return true;
}

int ClientStreamSocket::Send(const char* data, size_t dataSize)
{
    if (mState != eClientStreamSocketState::Connected)
    {
        mError = "Invalid state";
        return -1;
    }

    int result = send(mSocket, data, dataSize, 0);
    if (result < 0)
    {
        const char* error = GetStringForError(LAST_ERROR);
        Close(); // socket is probably disconnected, try to close and switch state
        mError = error;
        return -1;
    }

    return result;
}

int ClientStreamSocket::Recv(char* outData, size_t dataSize)
{
    if (mState != eClientStreamSocketState::Connected)
    {
        mError = "Invalid state";
        return -1;
    }

    int result = recv(mSocket, outData, dataSize, 0);
    if (result == 0)
    {
        // for a TCP socket recv returns 0 when remote end closed the connection
        // close the socket and update state
        Close();

        return 0;
    }
    else if (result < 0)
    {
#if CMAKE_PLATFORM_WINDOWS
        if (LAST_ERROR == WSAEWOULDBLOCK) return 0;
#elif CMAKE_PLATFORM_UNIX
        if (LAST_ERROR == EWOULDBLOCK) return 0;
#endif

        const char* error = GetStringForError(LAST_ERROR);
        Close(); // socket is probably disconnected, try to close and switch state
        mError = error;
        return -1;
    }

    return result;
}

bool ServerStreamSocket::Listen(const InAddr& addr)
{
    if (GetState() != eServerStreamSocketState::New)
    {
        mError = "Invalid state";
        return false;
    }

    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mSocket == -1)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    sockaddr_in to;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    memcpy(&to.sin_addr, &addr.ipAddr, sizeof(addr.ipAddr));
    to.sin_port = htons(addr.ipPort);

    int result = bind(mSocket, (sockaddr*)&to, sizeof(to));
    if (result != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    if (listen(mSocket, 64) != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    if (!SetSocketBlockingEnabled(mSocket, false))
    {
        mError = "Unable to set non-blocking mode";
        return false;
    }

    mState = eServerStreamSocketState::Listening;
    mError = "";
    mAddr = addr;

    return true;
}

bool ServerStreamSocket::Accept(ClientStreamSocket& sock, InAddr& addr)
{
    int sock_fd;

    if (Accept(sock_fd, addr))
    {
        sock.mState = eClientStreamSocketState::Connected;
        sock.mSocket = sock_fd;
        return true;
    }

    return false;
}

bool ServerStreamSocket::Accept(int& sock, InAddr& addr)
{
    if (mState != eServerStreamSocketState::Listening)
    {
        mError = "Invalid state";
        return false;
    }

    sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    socklen_t len = sizeof(sa);

    // set listening socket address
    memcpy(&sa.sin_addr, &mAddr.ipAddr, sizeof(mAddr.ipAddr));
    sa.sin_port = htons(mAddr.ipPort);

    sock = accept(mSocket, (sockaddr*) &sa, &len);

    if (sock == -1)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    if (!SetSocketBlockingEnabled(sock, false))
    {
        mError = "Unable to set non-blocking mode";
        return false;
    }

    memcpy(&addr.ipAddr, &sa.sin_addr, sizeof(sa.sin_addr));
    addr.ipPort = ntohs(sa.sin_port);

    mError = "";

    return true;
}

bool ServerStreamSocket::Close()
{
    if (mState != eServerStreamSocketState::Listening)
    {
        mError = "Invalid state";
        return false;
    }

    if (close(mSocket) != 0)
    {
        mError = GetStringForError(LAST_ERROR);
        return false;
    }

    mState = eServerStreamSocketState::Closed;
    mError = "";

    return true;
}

bool GetAddrByName(const char* name, IP4Addr& outAddr)
{
    bool success = false;

    struct addrinfo* pQueryResults = nullptr;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(name, 0, 0, &pQueryResults) == 0)
    {
        for (struct addrinfo* item = pQueryResults; item != nullptr; item = item->ai_next)
        {
            if (item->ai_family == AF_INET)
            {
                struct sockaddr_in* ipv4Address = (struct sockaddr_in*)item->ai_addr;
                if (ipv4Address != nullptr)
                {
                    outAddr.addr = ipv4Address->sin_addr.s_addr;
                    success = true;
                    break;
                }
            }
        }
    }

    if (pQueryResults != nullptr)
    {
        freeaddrinfo(pQueryResults);
    }

    return success;
}

#if CMAKE_PLATFORM_WINDOWS

#define WINSOCK_VERSION_LO 2
#define WINSOCK_VERSION_HI 2

bool Init()
{
    WORD Version;
    int Error;
    WSADATA WSAData;

    /*---- Initialise WinSock ----*/
    Version = MAKEWORD(WINSOCK_VERSION_LO, WINSOCK_VERSION_HI);
    Error = WSAStartup(Version, &WSAData);
    if (Error == 0)
    {
        /*---- Check correct Version of Winsock is supported ----*/
        if ((LOBYTE(WSAData.wVersion) == WINSOCK_VERSION_LO) && (HIBYTE(WSAData.wVersion) == WINSOCK_VERSION_HI))
        {
            /*---- WinSock Initialised Successfully ----*/
            return true;
        }

        /*---- Wrong Version of WinSock, so Clean Up ----*/
        WSACleanup();
    }

    return false;
}

void Close()
{
    WSACleanup();
}

#endif // CMAKE_PLATFORM_WINDOWS

#if CMAKE_PLATFORM_UNIX

bool Init()
{
    // nothing to do on Unix systems
    return true;
}

void Close()
{
    // nothing to do on Unix systems
}

#endif // CMAKE_PLATFORM_LINUX

} // namespace net
