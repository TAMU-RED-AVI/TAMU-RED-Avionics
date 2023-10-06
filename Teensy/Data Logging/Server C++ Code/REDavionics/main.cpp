#include <iostream>
#include <winsock2.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    struct sockaddr_in server, client;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize." << std::endl;
        return 1;
    }

    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1234);

    if (bind(ListenSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        return 1;
    }

    listen(ListenSocket, 1);
    std::cout << "Waiting for connections..." << std::endl;

    int clientSize = sizeof(struct sockaddr_in);
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&client, &clientSize);
    if (ClientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        return 1;
    }

    std::cout << "Connected." << std::endl;

    char buffer[1024];
    int bytesReceived;
    int totalBytes = 0;
    time_t startTime = time(NULL);
    while (true) {
        bytesReceived = recv(ClientSocket, buffer, sizeof(buffer), 0);
        totalBytes += bytesReceived;

        if (difftime(time(NULL), startTime) >= 1) {
            std::cout << "Data rate: " << totalBytes << " bytes/second" << std::endl;
            totalBytes = 0;
            startTime = time(NULL);
        }
    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
