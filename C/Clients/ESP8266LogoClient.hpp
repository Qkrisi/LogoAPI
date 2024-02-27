#ifndef ESP8266LOGOCLIENT_HPP
#define ESP8266LOGOCLIENT

#include <ESP8266WiFi.h>
#include "CLogo++.hpp"

class ESP8266LogoClient : public LogoClient
{
	private:
		WiFiClient Client;
	public:
		using LogoClient::LogoClient;
		~ESP8266LogoClient();
		int Connect(const char* host, uint16_t port);
		int Connect(const String& host, uint16_t port);
		void Stop();
		bool GetSync();
		void SetSync(bool sync);
		int _available();
		size_t _read(char* buffer, size_t length);
		void _write(uchar* msg, size_t length);
};

#endif
