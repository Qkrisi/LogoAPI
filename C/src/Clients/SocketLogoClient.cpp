#ifdef WINSOCK
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iostream>
#include <cstring>
#endif

#include "SocketLogoClient.hpp"

int LogoAvailable_CXX(LogoClient* logoClient)
{
    return ((SocketLogoClient*)logoClient)->_available();
}

size_t LogoRead_CXX(LogoClient* logoClient, char* buffer, size_t length)
{
    return ((SocketLogoClient*)logoClient)->_read(buffer, length);
}

void LogoWrite_CXX(LogoClient* logoClient, const char* msg, size_t length)
{
    return ((SocketLogoClient*)logoClient)->_write(msg, length);
}

SocketLogoClient::~SocketLogoClient() {
    this->Stop();
}

int SocketLogoClient::Connect(const char* host, uint16_t port) {
    struct sockaddr_in address;
#ifdef WINSOCK
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 0;
#endif
    this->SockFD = socket(
        AF_INET,
        SOCK_STREAM,
#ifdef WINSOCK
        IPPROTO_TCP
#else
        0
#endif
    );
    if (this->SockFD == -1)
    {
#ifdef WINSOCK
        WSACleanup();
#endif
        return 0;
    }
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);
    if (connect(this->SockFD, (struct sockaddr*)&address, sizeof(address)) != 0)
    {
#ifdef WINSOCK
        WSACleanup();
#endif
        return 0;
    }
    this->Join();
    return 1;
}

int SocketLogoClient::Connect(const String& host, uint16_t port) {
    return this->Connect(host.c_str(), port);
}

void SocketLogoClient::Stop() {
    shutdown(
        this->SockFD,
#ifdef WINSOCK
        SD_BOTH
#else
        SHUT_RDWR
#endif
    );
    if(this->_available())
    {
        char* _buffer = (char*)malloc(this->BytesAvailable * sizeof(char));
        recv(this->SockFD, _buffer, this->BytesAvailable, 0);
        free(_buffer);
    }
#ifdef WINSOCK
    closesocket(this->SockFD);
    WSACleanup();
#else
    close(this->SockFD);
#endif
    this->Reset();
}

int SocketLogoClient::_available() {
#ifdef WINSOCK
    ioctlsocket(this->SockFD, FIONREAD, &this->BytesAvailable);
#else
    ioctl(this->SockFD, FIONREAD, &this->BytesAvailable);
#endif
    return this->BytesAvailable > 0;
}

size_t SocketLogoClient::_read(char* buffer, size_t length) {
#ifdef WINSOCK
    return recv(this->SockFD, buffer, min(length, this->BytesAvailable), 0);
#else
    return read(this->SockFD, buffer, std::min(length, (size_t)this->BytesAvailable));
#endif
}

void SocketLogoClient::_write(const char* msg, size_t length) {
#ifdef WINSOCK
    send(this->SockFD, msg, length, 0);
#else
    write(this->SockFD, msg, length);
#endif
}
