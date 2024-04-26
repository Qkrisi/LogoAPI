#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iostream>

#include "SocketLogoClient.hpp"

int LogoAvailable_CXX(LogoClient* logoClient)
{
    return ((SocketLogoClient*)logoClient)->_available();
}

size_t LogoRead_CXX(LogoClient* logoClient, char* buffer, size_t length)
{
    return ((SocketLogoClient*)logoClient)->_read(buffer, length);
}

void LogoWrite_CXX(LogoClient* logoClient, uchar* msg, size_t length)
{
    return ((SocketLogoClient*)logoClient)->_write(msg, length);
}

SocketLogoClient::~SocketLogoClient() {
    this->Stop();
}

int SocketLogoClient::Connect(const char* host, uint16_t port) {
    struct sockaddr_in address;
    this->SockFD = socket(AF_INET, SOCK_STREAM, 0);
    if(this->SockFD == -1)
        return 0;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);
    if(connect(this->SockFD, (struct sockaddr*)&address, sizeof(address)) != 0)
        return 0;
    this->Join();
    return 1;
}

int SocketLogoClient::Connect(const String& host, uint16_t port) {
    return this->Connect(host.c_str(), port);
}

void SocketLogoClient::Stop() {
    shutdown(this->SockFD, SHUT_RDWR);
    if(this->_available())
    {
        char _buffer[this->BytesAvailable];
        recv(this->SockFD, _buffer, this->BytesAvailable, 0);
    }
    close(this->SockFD);
    this->Reset();
}

int SocketLogoClient::_available() {
    ioctl(this->SockFD, FIONREAD, &this->BytesAvailable);
    return this->BytesAvailable > 0;
}

size_t SocketLogoClient::_read(char* buffer, size_t length) {
    return read(this->SockFD, buffer, std::min(length, (size_t)this->BytesAvailable));
}

void SocketLogoClient::_write(uchar* msg, size_t length) {
    write(this->SockFD, msg, length);
}
