#ifndef SOCKETLOGOCLIENT_HPP
#define SOCKETLOGOCLIENT_HPP

#ifdef CLIENTSFOLDER
#include "../CLogo++.hpp"
#else
#include "CLogo++.hpp"
#endif

class SocketLogoClient : public LogoClient {
    private:
        int SockFD;
        unsigned long BytesAvailable;
    public:
        using LogoClient::LogoClient;
        ~SocketLogoClient();
        int Connect(const char* host, uint16_t port = 51);
        int Connect(const String& host, uint16_t port = 51);
        void Stop();
        int _available();
        size_t _read(char* buffer, size_t length);
        void _write(const char* msg, size_t length);
};

#endif
