#include <iostream>
#include <time.h>

#include "src/Clients/SocketLogoClient.hpp"

void OnMessage(LogoClient* client, const String& sender, MessageTypeReceive messageType, const String& message) {
    std::cout << '[' << sender << "] (" << messageType << ") " << message << std::endl;
    client->SendMessage(SND_MESSAGE, "[ACK] " + message, sender);
}

int main() {
    SocketLogoClient logoClient("CLogoTest", OnMessage);
    if(!logoClient.Connect("127.0.0.1", 51)) {
        std::cout << "Sikertelen csatlakozás" << std::endl;
        return 0;
    }

    logoClient.SendMessage(SND_COMMAND, "lap1't1'e 50", logoClient.GetServerName());
    clock_t start = clock();
    int elapsed = 0;
    while(elapsed < 10)
    {
        logoClient.Update();
        elapsed = (clock() - start) / CLOCKS_PER_SEC;
    }
    logoClient.Stop();

    return 0;
}