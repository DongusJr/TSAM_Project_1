#include  <stdio.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <iostream>
#include  <netdb.h>
#include  <unistd.h>
#include  <string.h>

void checkIfArguementsAreValid(int argc){
    if (argc != 3) {
        std::cout << "Should run like this: ./client {host} {port}" << std::endl;
        exit(0);
    }
}



void sendToServer(int sockfd, std::string userBuffer, char* hostname) {
    if (send(sockfd, userBuffer.c_str(), sizeof(userBuffer), 0) == -1) {
        std::cout << stderr << " Error, could not send message to " << hostname << std::endl;
        exit(0);
    }
}

void recieveMessage(int sockfd, uint32_t messageSize, int bufferSiize, std::string* message){
    int numberOfPackets = messageSize / bufferSiize;
    numberOfPackets += (messageSize % bufferSiize) ? 1 : 0;
    int i = 0;
    char recieveBuffer[bufferSiize] = {0};
    while (i < numberOfPackets) {
        recv(sockfd, recieveBuffer, sizeof(recieveBuffer), 0);
        *message += recieveBuffer;
        i++;
        memset(recieveBuffer, 0, bufferSiize);
    } 
}

int main(int argc, char* argv[]){
    struct sockaddr_in server_addr;
    std::string userBuffer;           // User input each loop
    const int bufferSiize = 1024;

    checkIfArguementsAreValid(argc);
    char* hostname = argv[1];
    int port = atoi(argv[2]);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Create a client socket
    
    int sockfd = socket(server_addr.sin_family, SOCK_STREAM, 0);

    // connect to host
    
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        std::cout << stderr << " Error, could not connect to " << hostname << std::endl;
        exit(0);
    }
    while (true) {
        std::getline(std::cin, userBuffer);
        sendToServer(sockfd, userBuffer, hostname);

        uint32_t messageSize;
        recv(sockfd, &messageSize, sizeof(long), 0);
        messageSize = ntohl(messageSize);
        
        std::string message;        
        recieveMessage(sockfd, messageSize, bufferSiize, &message);
        
        std::cout << message << std::endl;
    }
    close(sockfd);
    return 0;
}