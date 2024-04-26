#ifndef SOCKETLOGOCLIENT_HPP
#define SOCKETLOGOCLIENT_HPP

#include "../CLogo++.hpp"

class SocketLogoClient : public LogoClient {
    private:
        int SockFD;
        int BytesAvailable;
    public:
        using LogoClient::LogoClient;
        ~SocketLogoClient();
        int Connect(const char* host, uint16_t port = 51);
        int Connect(const String& host, uint16_t port = 51);
        void Stop();
        int _available();
        size_t _read(char* buffer, size_t length);
        void _write(uchar* msg, size_t length);
};

#endif
