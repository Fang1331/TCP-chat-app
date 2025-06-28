#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

struct Client
{
    SOCKET socket;
    int id;
};

std::vector<Client> clients;
std::mutex clientMutex;
int clientCounter = 0;

void handleClient(Client client)
{
    char buffer[1024];

    std::cout << "Client " << client.id << " connected.\n";

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(client.socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0)
        {
            std::cout << "Client " << client.id << " disconnected.\n";
            closesocket(client.socket);
            return;
        }

        buffer[bytesReceived] = '\0';
        std::string command(buffer);

        if (command == "LIST")
        {
            std::string list;
            std::lock_guard<std::mutex> lock(clientMutex);
            for (const auto &c : clients)
            {
                if (c.id != client.id)
                {
                    list += "Client " + std::to_string(c.id) + " is connected.\n";
                }
            }
            send(client.socket, list.c_str(), list.size(), 0);
        }
        else if (command.rfind("SEND", 0) == 0)
        {
            // expected format: SEND <id> <message>
            size_t idStart = command.find(" ") + 1;
            size_t msgStart = command.find(" ", idStart) + 1;

            int targetId = std::stoi(command.substr(idStart, msgStart - idStart - 1));
            std::string msg = command.substr(msgStart);

            std::lock_guard<std::mutex> lock(clientMutex);
            for (const auto &c : clients)
            {
                if (c.id == targetId)
                {
                    std::string taggedMsg = "Message from Client " + std::to_string(client.id) + ": " + msg;
                    send(c.socket, taggedMsg.c_str(), taggedMsg.size(), 0);
                }
            }
        }
        else
        {
            std::string echo = "Echo: " + command;
            send(client.socket, echo.c_str(), echo.size(), 0);
        }
    }
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    std::cout << "Server listening on port 8080...\n";

    while (true)
    {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &addrLen);

        Client newClient{clientSocket, ++clientCounter};
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            clients.push_back(newClient);
        }

        std::thread t(handleClient, newClient);
        t.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
