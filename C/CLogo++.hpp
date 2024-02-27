#ifndef CLOGO_HPP
#define CLOGO_HPP

#ifndef ARDUINO
#include <string>
using String = std::string;
#else
#include <Arduino.h>
#endif

extern "C" {
#include "CLogo.h"
}

class LogoClient {
    protected:
        LogoData Data;
        void Reset();
    public:
        enum {MSGMODE_NONE, MSGMODE_CHAR, MSGMODE_STR} OnMessageMode = MSGMODE_NONE;
        union {
            void (*OnMessageChar)(LogoClient*, char*, MessageTypeReceive, char*);
            void (*OnMessageStr)(LogoClient*, const String&, MessageTypeReceive, const String&);
        } OnMessage;
        explicit LogoClient(char* name, size_t bufferSize = 2048);
        explicit LogoClient(String name, size_t bufferSize = 2048);
        LogoClient(char* name, void (*onMessage)(LogoClient*, char*, MessageTypeReceive, char*), size_t bufferSize = 2048);
        LogoClient(String name, void (*onMessage)(LogoClient*, const String&, MessageTypeReceive, const String&), size_t bufferSize = 2048);
        virtual ~LogoClient();
        void SendRaw(MessageTypeSend messageType, char** parts, size_t partsLength, char* append);
        void SendRaw(MessageTypeSend messageType, String* strings, size_t partsLength, const String& append);
        void SendMessage(MessageTypeSend messageType, char* message, char** clients, size_t numClients);
        void SendMessage(MessageTypeSend messageType, char* message, char* client);
        void SendMessage(MessageTypeSend messageType, const String& message, String* clients, size_t numClients);
        void SendMessage(MessageTypeSend messageType, const String& message, String client);
        void Join();
        void UpdateClients();
        void Update();

        int Connected();
        String GetName();
        String GetServerName();
        String GetClient(int clientIndex);
        size_t GetNumClients();
};

int LogoAvailable(LogoClient* logoClient);
size_t LogoRead(LogoClient* logoClient, char* buffer, size_t length);
void LogoWrite(LogoClient* logoClient, uchar* msg, size_t length);

char* _copy_str(const String& str);
void _message_proxy(LogoData* logoData, char* sender, MessageTypeReceive messageType, char* message);

#endif