#include "ESP8266LogoClient.hpp"

int LogoAvailable(LogoClient* logoClient)
{
	return ((ESP8266LogoClient*)logoClient)->_available();
}

size_t LogoRead(LogoClient* logoClient, char* buffer, size_t length)
{
	return ((ESP8266LogoClient*)logoClient)->_read(buffer, length);
}

void LogoWrite(LogoClient* logoClient, uchar* msg, size_t length)
{
	return ((ESP8266LogoClient*)logoClient)->_write(msg, length);
}

ESP8266LogoClient::~ESP8266LogoClient()
{
	this->Stop();
}

int ESP8266LogoClient::Connect(const char* host, uint16_t port)
{
	if(this->Client.connected())
		return 0;
	int result = this->Client.connect(host, port);
	if(result == 0)
		return 0;
	this->Join();
	return 1;
}

int ESP8266LogoClient::Connect(const String& host, uint16_t port)
{
	this->Connect(host.c_str(), port);
}

void ESP8266LogoClient::Stop()
{
	if(!this->Client.connected())
		return;
	this->Client.stop();
	this->Reset();
}

bool ESP8266LogoClient::GetSync()
{
	return this->Client.getSync();
}

void ESP8266LogoClient::SetSync(bool sync)
{
	this->Client.setSync(sync);
}

int ESP8266LogoClient::_available()
{
	return this->Client.available();
}

size_t ESP8266LogoClient::_read(char* buffer, size_t length)
{
	return this->Client.read(buffer, length);
}

void ESP8266LogoClient::_write(uchar* msg, size_t length)
{
	this->Client.write(msg, length);
}
