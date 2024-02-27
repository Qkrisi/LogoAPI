#ifndef CLOGO_H
#define CLOGO_H

#ifndef ARDUINO
#include <stdlib.h>
#else
#include <Arduino.h>
#endif

#define LOGO_START 0x07
#define LOGO_SEPARATOR 0x21

typedef unsigned char uchar;

typedef enum MessageTypeSend {
    SND_MESSAGE = 0x57,
    SND_COMMAND = 0x52,
    SND_JOIN = 0x61, 
    SND_QUERY_CLIENTS = 0x55
} MessageTypeSend;

typedef enum MessageTypeReceive {
    RCV_MESSAGE = 0x77,
    RCV_COMMAND = 0x72,
    RCV_RESULT = 0x6C,
    RCV_JOINED = 0x62,
    RCV_CLIENTS = 0x76
} MessageTypeReceive;

typedef struct LogoData {
    char* OriginalName;
    char* Name;
    size_t BufferSize;
    int Connected;
    char** Clients;
    size_t NumClients;
    void (*OnMessage)(struct LogoData*, char*, MessageTypeReceive, char*);
    void* _logo_client;
} LogoData;

int LogoAvailable(LogoData* logoData);
size_t LogoRead(LogoData* logoData, char* buffer, size_t length);
void LogoWrite(LogoData* logoData, uchar* msg, size_t length);

void logo_free(LogoData* logoData);
void _logo_free_clients(LogoData* logoData);
size_t _logo_encode_number(size_t n, uchar* buffer);
size_t _logo_next_part(char* data);
size_t _logo_read_number(size_t* n, char* data);
void logo_reset(LogoData* logoData);
void logo_send_raw(LogoData* logoData, MessageTypeSend messageType, char** parts, size_t partsLength, char* append);
void logo_send_message(LogoData* logoData, MessageTypeSend messageType, char* message, char** clients, size_t numClients);
void logo_send_message_single(LogoData* logoData, MessageTypeSend messageType, char* message, char* client);
void logo_join(LogoData* logoData);
void logo_update_clients(LogoData* logoData);
void logo_update(LogoData* logoData);

#endif
