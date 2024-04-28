#ifndef CLOGO_H
#define CLOGO_H

#ifndef ARDUINO
#include <stdlib.h>
#else
#include <Arduino.h>
#endif

#define LOGO_START 0x07
#define LOGO_SEPARATOR 0x21

/// Enumeration of bytes to send to indicate the message type
typedef enum MessageTypeSend {
    /// Standard message
    SND_MESSAGE = 0x57,
    /// Command to execute (enable in the Net class!)
    SND_COMMAND = 0x52,
    /// Result of a function
    SND_RESULT = 0x4C,
    /// Request to join the session
    SND_JOIN = 0x61,
    /// Request the names of the connected clients
    SND_QUERY_CLIENTS = 0x55
} MessageTypeSend;

/// Enumeration of bytes that indicate the received message type
typedef enum MessageTypeReceive {
    /// Standard message
    RCV_MESSAGE = 0x77,
    /// Command to execute
    RCV_COMMAND = 0x72,
    /// Result of a function
    RCV_RESULT = 0x6C,
    /// Callback of a join request - data contains the name
    RCV_JOINED = 0x62,
    /// Callback of a client query - data contains the names of the connected clients
    RCV_CLIENTS = 0x76
} MessageTypeReceive;

/// Structure containing values required to communicate with the Imagine server
typedef struct LogoData {
    char* OriginalName;     //Requested name
    char* Name;             //The name given by Imagine (differs from OriginalName when duplicate)
    size_t BufferSize;
    int Connected;
    char** Clients;
    size_t NumClients;
    void (*OnMessage)(struct LogoData*, const char*, MessageTypeReceive, const char*);
    void* _logo_client;     //C++ proxy - instance of LogoClient
} LogoData;

/// To be implemented by the user - Gets wether data is available to be read from the TCP stream
/// @param logoData Pointer to the LogoData instance
/// @return 0 if no bytes are available to be read, 1 otherwise
int LogoAvailable_C(LogoData* logoData);

/// To be implemented by the user - Reads data from the TCP stream into a buffer
/// @param logoData Pointer to the LogoData instance
/// @param buffer The buffer to read the data into
/// @param length The length of the buffer
/// @return The number of bytes read
size_t LogoRead_C(LogoData* logoData, char* buffer, size_t length);

/// To be implemented by the user - Write data to the TCP stream
/// @param logoData Pointer to the LogoData instance
/// @param msg The data to send
/// @param length The length of the msg buffer
void LogoWrite_C(LogoData* logoData, const char* msg, size_t length);

/// Create and initialize an instance of LogoData
/// @param name Requested name of the client (null-terminated)
/// @return The created LogoData
LogoData create_logo_data(char* name);

/// Initialize an instance of LogoData to its default values
/// @param logoData Pointer to the LogoData instance
void logo_init(LogoData* logoData);

/// Free the memory allocations of an instance of LogoData
/// @param logoData Pointer to the LogoData instance
void logo_free(LogoData* logoData);

/// Free the memory allocations of the Clients pointer
/// @param logoData Pointer to the LogoData instance
void _logo_free_clients(LogoData* logoData);

/// Encode a number into a message to be sent
/// @param n The number to encode
/// @param buffer The buffer to copy the bytes into
/// @return The number of bytes written
size_t _logo_encode_number(size_t n, char* buffer);

/// Discard bytes until a separator
/// @param data The pointer to read the bytes from
/// @return The number of bytes discarded
size_t _logo_next_part(char* data);

/// Read remaining bytes as continuous number
/// @param n The pointer to read the number into
/// @param data The pointer to read the bytes from
/// @return The length of the number
size_t _logo_read_number(size_t* n, char* data);

/// Reset the values of the LogoData instance to be able to reconnect
/// @param logoData Pointer to the LogoData instance
void logo_reset(LogoData* logoData);

/// Send a raw message to the server
/// @param logoData Pointer to the LogoData instance
/// @param messageType The type of message to be sent
/// @param parts The parts to encode (<length>!<part>)
/// @param partsLength The number of parts to encode
/// @param append Null-terminated string to append to the end of the message
void logo_send_raw(LogoData* logoData, MessageTypeSend messageType, const char** parts, size_t partsLength, const char* append);

/// Send a message to specific clients
/// @param logoData Pointer to the LogoData instance
/// @param messageType The type of message to be sent
/// @param message Null-terminated string containing the message
/// @param clients Null-terminated strings representing the names of the clients to send the message to
/// @param numClients The length of the clients buffer (number of clients)
void logo_send_message(LogoData* logoData, MessageTypeSend messageType, const char* message, const char** clients, size_t numClients);

/// Send a message to a single client
/// @param logoData Pointer to the LogoData instance
/// @param messageType The type of message to be sent
/// @param message Null-terminated string containing the message
/// @param client Null-terminated string containing the name of the receiver client
void logo_send_message_single(LogoData* logoData, MessageTypeSend messageType, const char* message, const char* client);

/// Send a join command to the server
/// @param logoData Pointer to the LogoData instance
void logo_join(LogoData* logoData);

/// Query the names of the connected clients from the server
/// @param logoData Pointer to the LogoData instance
void logo_update_clients(LogoData* logoData);

/// Read incoming messages from the server
/// @param logoData Pointer to the LogoData instance
void logo_update(LogoData* logoData);

/// Get the name of the server
/// @param logoData Pointer to the LogoData instance
/// @return The name of the server (null-terminated)
const char* logo_server(LogoData* logoData);

/// Escapes a string to be able to be used as an argument in Imagine
/// @param str The string to escape
/// @param destination The buffer to copy the escaped string into (size is at least length*2+1)
/// @param length Length of str
/// @return Number of bytes written into destination
size_t logo_to_string_C(const char* str, char* destination, size_t length);

#endif
