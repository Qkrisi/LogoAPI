#include "CLogo++.hpp"

#ifndef ARDUINO
#include <cstring>
#include <string>
#include <iostream>
#else
#include <Arduino.h>
#endif

extern "C"
{
    #include "CLogo.h"
    int LogoAvailable(LogoData* logoData) {
        return LogoAvailable((LogoClient*)logoData->_logo_client);
    }
    size_t LogoRead(LogoData* logoData, char* buffer, size_t length) {
        return LogoRead((LogoClient*)logoData->_logo_client, buffer, length);
    }
    void LogoWrite(LogoData* logoData, uchar* msg, size_t length) {
        LogoWrite((LogoClient*)logoData->_logo_client, msg, length);
    }
}

LogoClient::LogoClient(char *name, size_t bufferSize) {
    this->Data.OriginalName = name;
    this->Data.BufferSize = bufferSize;
    this->Data._logo_client = (void*)this;
    this->Data.OnMessage = _message_proxy;
}

LogoClient::LogoClient(String name, size_t bufferSize) : LogoClient(_copy_str(name), bufferSize) {
}

LogoClient::LogoClient(char *name, void (*onMessage)(LogoClient*, char*, MessageTypeReceive, char*), size_t bufferSize) : LogoClient(name, bufferSize) {
    this->OnMessage.OnMessageChar = onMessage;
    this->OnMessageMode = MSGMODE_CHAR;
}

LogoClient::LogoClient(String name, void (*onMessage)(LogoClient*, const String&, MessageTypeReceive, const String&), size_t bufferSize) : LogoClient(name, bufferSize) {
    this->OnMessage.OnMessageStr = onMessage;
    this->OnMessageMode = MSGMODE_STR;
}

LogoClient::~LogoClient() {
    logo_free(&this->Data);
}

void LogoClient::Reset() {
    logo_reset(&this->Data);
}

void LogoClient::SendRaw(MessageTypeSend messageType, char** parts, size_t partsLength, char* append) {
    logo_send_raw(&this->Data, messageType, parts, partsLength, append);
}

void LogoClient::SendRaw(MessageTypeSend messageType, String* parts, size_t partsLength, const String& append) {
    auto partsBuffer = (char**)malloc(partsLength * sizeof(char*));
    for(size_t i = 0; i < partsLength; i++) {
        partsBuffer[i] = _copy_str(parts[i]);
    }
    char* appendBuffer = _copy_str(append);
    this->SendRaw(messageType, partsBuffer, partsLength, appendBuffer);
    for(size_t i = 0;i < partsLength; i++)
        free(partsBuffer[i]);
    free(partsBuffer);
    free(appendBuffer);
}

void LogoClient::SendMessage(MessageTypeSend messageType, char* message, char** clients, size_t numClients) {
    logo_send_message(&this->Data, messageType, message, clients, numClients);
}

void LogoClient::SendMessage(MessageTypeSend messageType, char *message, char *client) {
    this->SendMessage(messageType, message, &client, 1);
}

void LogoClient::SendMessage(MessageTypeSend messageType, const String& message, String* clients, size_t numClients) {
    auto clientsBuffer = (char**)malloc(numClients * sizeof(char*));
    for(size_t i = 0; i < numClients; i++) {
        clientsBuffer[i] = _copy_str(clients[i]);
    }
    char* messageBuffer = _copy_str(message);
    this->SendMessage(messageType, messageBuffer, clientsBuffer, numClients);
    for(size_t i = 0;i < numClients; i++)
        free(clientsBuffer[i]);
    free(clientsBuffer);
    free(messageBuffer);
}

void LogoClient::SendMessage(MessageTypeSend messageType, const String& message, String client) {
    this->SendMessage(messageType, message, &client, 1);
}

void LogoClient::Join() {
    logo_join(&this->Data);
}

void LogoClient::UpdateClients() {
    logo_update_clients(&this->Data);
}

void LogoClient::Update() {
    logo_update(&this->Data);
}

int LogoClient::Connected() {
    return this->Data.Connected;
}

String LogoClient::GetName() {
    return String(this->Data.Name);
}

String LogoClient::GetServerName() {
    return this->GetClient(0);
}

String LogoClient::GetClient(int clientIndex) {
    if(clientIndex < 0 || clientIndex >= this->GetNumClients())
        return "";
    return String(this->Data.Clients[clientIndex]);
}

size_t LogoClient::GetNumClients() {
    return this->Data.NumClients;
}


char* _copy_str(const String& str) {
    char* buffer = (char*)malloc((str.length() + 1) * sizeof(char*));
    strcpy(buffer, str.c_str());
    return buffer;
}

void _message_proxy(LogoData* logoData, char* sender, MessageTypeReceive messageType, char* message) {
    if(logoData->_logo_client == NULL)
        return;
    auto client = (LogoClient*)logoData->_logo_client;
    switch(client->OnMessageMode) {
        case LogoClient::MSGMODE_CHAR:
            client->OnMessage.OnMessageChar(client, sender, messageType, message);
            break;
        case LogoClient::MSGMODE_STR:
            client->OnMessage.OnMessageStr(client, String(sender), messageType, String(message));
            break;
        case LogoClient::MSGMODE_NONE:
            break;
    }
}
