#include "CLogo.h"

#ifndef ARDUINO
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#else
#include <Arduino.h>
#endif


void logo_free(LogoData* logoData) {
    logo_reset(logoData);
    free(logoData->OriginalName);
    logoData->OriginalName = NULL;
}

void _logo_free_clients(LogoData* logoData) {
    if(logoData->Clients == NULL)
        return;
    for(uint i = 0; i < logoData->NumClients; i++) {
        free(logoData->Clients[i]);
    }
    free(logoData->Clients);
}

size_t _logo_encode_number(size_t n, uchar* buffer) {
    if(n == 0) {
        buffer[0] = 48;
        return 1;
    }
    const size_t size = (size_t)log10(n) + 1;
    size_t index = size - 1;
    for(;;)
    {
        buffer[index] = (n % 10) + 48;
        n /= 10;
        if(index == 0)
            break;
        index--;
    }
    return size;
}

size_t _logo_read_number(size_t* n, char* data) {
    size_t result = 0;
    char c = 48;
    size_t i = 0;
    while(c != LOGO_SEPARATOR) {
        result = result * 10 + c - 48;
        c = data[i++];
    }
    *n = result;
    return i;
}

size_t _logo_next_part(char* data) {
    uchar c = 0;
    size_t i = 0;
    while(c != LOGO_SEPARATOR) {
        c = data[i++];
    }
    return i;
}

void logo_send_raw(LogoData* logoData, MessageTypeSend messageType, char** parts, size_t partsLength, char* append) {
    uchar* data = (uchar*)malloc((logoData->BufferSize + 8) * sizeof(uchar));
    uchar* partsData = (uchar*)malloc(logoData->BufferSize * sizeof(uchar));

    data[0] = LOGO_START;
    size_t dataIndex = 1;

    partsData[0] = LOGO_SEPARATOR;
    partsData[1] = messageType;
    size_t partsDataIndex = 2;

    partsDataIndex += _logo_encode_number(partsLength, partsData + partsDataIndex);
    partsData[partsDataIndex++] = LOGO_SEPARATOR;

    for(size_t i = 0; i < partsLength; i++) {
        size_t partLength = strlen(parts[i]);
        partsDataIndex += _logo_encode_number(partLength, partsData + partsDataIndex);
        partsData[partsDataIndex++] = LOGO_SEPARATOR;
        for(size_t j = 0; j < partLength; j++) {
            partsData[partsDataIndex++] = parts[i][j];
        }
    }

    if(append != NULL) {
        size_t appendLength = strlen(append);
        for(size_t i = 0; i < appendLength; i++) {
            partsData[partsDataIndex++] = append[i];
        }
    }
    
    dataIndex += _logo_encode_number(partsDataIndex + dataIndex - 2, data + dataIndex);
    for(size_t i = 0; i < partsDataIndex; i++)
        data[dataIndex++] = partsData[i];

    LogoWrite(logoData, data, dataIndex);
    
    free(data);
    free(partsData);
}

void logo_send_message(LogoData* logoData, MessageTypeSend messageType, char* message, char** clients, size_t numClients) {
    char** _clients = (char**)malloc(++numClients * sizeof(char*));
    
    for(size_t i = 0; i < numClients; i++) {
        char* client = i == 0 ? logoData->Name : clients[i-1];
        size_t clientSize = strlen(client);
        _clients[i] = (char*)malloc((clientSize + 1) * sizeof(char));
        strcpy(_clients[i], client);
        _clients[i][clientSize] = 0;
    }

    logo_send_raw(logoData, messageType, _clients, numClients, message);

    for(size_t i = 0; i < numClients; i++)
        free(_clients[i]);
    free(_clients);
}

void logo_send_message_single(LogoData* logoData, MessageTypeSend messageType, char* message, char* client) {
    logo_send_message(logoData, messageType, message, &client, 1);
}

void logo_reset(LogoData* logoData) {
    _logo_free_clients(logoData);
    free(logoData->Name);
    logoData->Clients = NULL;
    logoData->Name = NULL;
    logoData->Connected = 0;
    logoData->NumClients = 0;
}

void logo_join(LogoData* logoData) {
    logo_send_raw(logoData, SND_JOIN, NULL, 0, logoData->OriginalName);
    while(!logoData->Connected) {
        logo_update(logoData);
    }
}

void logo_update_clients(LogoData* logoData) {
    logo_send_raw(logoData, SND_QUERY_CLIENTS, NULL, 0, NULL);
}

void logo_update(LogoData* logoData) {
    if(!LogoAvailable(logoData))
        return;
    char* data = (char*)malloc(logoData->BufferSize * sizeof(char));
    LogoRead(logoData, data, logoData->BufferSize);
    if(data[0] != LOGO_START)
        return;
    size_t length = 0;
    size_t dataIndex = _logo_read_number(&length, data + 1) + 1;
    MessageTypeReceive messageType = (MessageTypeReceive)data[dataIndex++];
    length--;
    size_t partLenght;
    switch(messageType) {
        case RCV_JOINED: {
            partLenght = _logo_next_part(data + dataIndex);
            length -= partLenght;
            dataIndex += partLenght;
            logoData->Name = (char*)malloc((length + 1) * sizeof(char));
            for(size_t i = 0; i < length; i++) {
                logoData->Name[i] = data[dataIndex++];
            }
            logoData->Name[length] = 0;
            length = 0;
            logo_update_clients(logoData);
            break;   
        }
        case RCV_CLIENTS: {
            _logo_free_clients(logoData);
            partLenght = _logo_read_number(&logoData->NumClients, data + dataIndex);
            length -= partLenght;
            dataIndex += partLenght;
            logoData->Clients = (char**)malloc(logoData->NumClients * sizeof(char*));
            for(uint i = 0; i < logoData->NumClients; i++) {
                size_t nameLength;
                dataIndex += _logo_read_number(&nameLength, data + dataIndex);
                logoData->Clients[i] = (char*)malloc((nameLength + 1) * sizeof(char));
                for(uint j = 0; j < nameLength; j++) {
                    logoData->Clients[i][j] = data[dataIndex++];
                }
                logoData->Clients[i][nameLength] = 0;
            }
            logoData->Connected = 1;
            break;
        }
        case RCV_MESSAGE:
        case RCV_COMMAND:
        case RCV_RESULT: {
            size_t senderLength;
            partLenght = _logo_next_part(data + dataIndex);
            length -= partLenght;
            dataIndex += partLenght;
            partLenght = _logo_read_number(&senderLength, data + dataIndex);
            length -= partLenght;
            dataIndex += partLenght;
            char* sender = (char*)malloc((senderLength + 1) * sizeof(char));
            for(uint i = 0; i < senderLength; i++) {
                sender[i] = data[dataIndex++];
                length--;
            }
            sender[senderLength] = 0;
            if(messageType == RCV_RESULT) {     //"OK: "
                dataIndex += 4;
                length -= 4;
            }
            char* message = (char*)malloc((length + 1) * sizeof(char));
            for(size_t i = 0; i < length; i++) {
                message[i] = data[dataIndex++];
            }
            message[length] = 0;
            length = 0;

            if(logoData->OnMessage != NULL)
                logoData->OnMessage(logoData, sender, messageType, message);
            
            free(sender);
            free(message);
            break;
        }
        default: {
            break;
        }
    }
    free(data);
}