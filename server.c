#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>awdawdawd
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(1); }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Binding failed");
        exit(1);}


    if (listen(serverSocket, 1) == -1) {
        perror("Listening failed");
        exit(1); }

    printf("Server listening on port %d...\n", SERVER_PORT);

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        perror("Accepting failed");
        exit(1); }

    printf("Client connected: %s\n", inet_ntoa(clientAddress.sin_addr));

    char message[BUFFER_SIZE];
    while (1) {
        memset(message, 0, sizeof(message));
        int bytesRead = recv(clientSocket, message, BUFFER_SIZE - 1, 0);
        if (bytesRead == -1) {
            perror("Receiving failed");
            exit(1); } else if (bytesRead == 0) {
            printf("Client disconnected\n");
            break; }

        printf("Received from client: %s\n", message);

        if (send(clientSocket, message, strlen(message), 0) == -1) {
            perror("Sending failed");
            exit(1); } }

    close(clientSocket);
    close(serverSocket);

    return 0; }
