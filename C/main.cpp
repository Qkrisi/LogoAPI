#include <iostream>

#include "src/CLogo++.hpp"
#include "src/Clients/SocketLogoClient.hpp"

void OnMessage(LogoClient* client, const String& sender, MessageTypeReceive messageType, const String& message) {
    std::cout << '[' << sender << "] (" << messageType << ") " << message << std::endl;
    client->SendMessage(SND_MESSAGE, "[ACK] " + message, sender);
}

int main() {
    SocketLogoClient client("CLogoTest", OnMessage);
    if(client.Connect("127.0.0.1", 51)) {
        std::cout << "Connected as " << client.GetName() << std::endl;
        for(;;)
            client.Update();
    }

    return 0;
}