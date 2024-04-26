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

/// Provides a wrapper object for LogoData
class LogoClient {
    protected:
        LogoData Data;
        void Reset();
    public:
        /// The type of OnMessage event to call
        enum {
            /// Don't call the event
            MSGMODE_NONE,
            /// Call it with a character buffer
            MSGMODE_CHAR,
            /// Call it with a std::string,
            MSGMODE_STR
        } OnMessageMode = MSGMODE_NONE;

        /// Stores the function pointer to the OnMessage event
        union {
            /// Event for MSGMODE_CHAR
            void (*OnMessageChar)(LogoClient*, char*, MessageTypeReceive, char*);

            /// Event for MSGMODE_STR
            void (*OnMessageStr)(LogoClient*, const String&, MessageTypeReceive, const String&);
        } OnMessage;


        explicit LogoClient(char* name, size_t bufferSize = 2048);
        explicit LogoClient(String name, size_t bufferSize = 2048);
        LogoClient(char* name, void (*onMessage)(LogoClient*, char*, MessageTypeReceive, char*), size_t bufferSize = 2048);
        LogoClient(String name, void (*onMessage)(LogoClient*, const String&, MessageTypeReceive, const String&), size_t bufferSize = 2048);
        virtual ~LogoClient();
        void SendRaw(MessageTypeSend messageType, char** parts, size_t partsLength, char* append);
        void SendRaw(MessageTypeSend messageType, String* parts, size_t partsLength, const String& append);
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

int LogoAvailable_CXX(LogoClient* logoClient);
size_t LogoRead_CXX(LogoClient* logoClient, char* buffer, size_t length);
void LogoWrite_CXX(LogoClient* logoClient, uchar* msg, size_t length);

char* _copy_str(const String& str);
void _message_proxy(LogoData* logoData, char* sender, MessageTypeReceive messageType, char* message);

String logo_to_string_CXX(const String& str);

#endif