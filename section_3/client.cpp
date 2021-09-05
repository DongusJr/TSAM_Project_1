#include  <stdio.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <iostream>
#include  <netdb.h>
#include  <unistd.h>
#include  <string.h>

// Function to make sure there are enough arguements
void checkIfArguementsAreValid(int argc){
    if (argc != 3) {
        std::cout << "Should run like this: ./client <ip address> <ip port>" << std::endl;
        exit(0);
    }
}

// Function to send a message to the server
void sendToServer(int sockfd, std::string userBuffer, char* hostname) {
    if (send(sockfd, userBuffer.c_str(), sizeof(userBuffer), 0) == -1) {
        std::cout << stderr << " Error, could not send message to " << hostname << std::endl;
        exit(0);
    }
}

// Function to recieve and unpack message sent from server
void recieveMessage(int sockfd, uint32_t messageSize, int bufferSize, std::string* message){
    int numberOfPackets = messageSize / bufferSize; 
    numberOfPackets += (messageSize % bufferSize) ? 1 : 0;      // Round properly 
    int i = 0;
    char recieveBuffer[bufferSize] = {0};   // Store in buffer, then transfer to message
    while (i < numberOfPackets) {
        recv(sockfd, recieveBuffer, sizeof(recieveBuffer), 0);  // Recieve message into buffer
        *message += recieveBuffer;
        i++;
        memset(recieveBuffer, 0, bufferSize);  // Reset the buffer
    } 
}

int main(int argc, char* argv[]){
    struct sockaddr_in server_addr;
    std::string userBuffer;           // User input each loop
    const int bufferSize = 1024;      // Size of recieve buffer

    checkIfArguementsAreValid(argc);
    char* hostname = argv[1];
    int port = atoi(argv[2]);
    
    // set server address
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
        // recieve input from user, send to server
        std::getline(std::cin, userBuffer);
        sendToServer(sockfd, userBuffer, hostname);

        // Get the size of the message
        uint32_t messageSize;
        recv(sockfd, &messageSize, sizeof(long), 0);
        messageSize = ntohl(messageSize);
        
        // Unpack the message
        std::string message;
        recieveMessage(sockfd, messageSize, bufferSize, &message);
        
        std::cout << message << std::endl;
    }
    close(sockfd);
    return 0;
}