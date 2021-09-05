#include  <stdio.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <iostream>
#include  <netdb.h>
#include  <unistd.h>

int main(int argc, char* argv[]){
    char* hostname = argv[1];
    int port = atoi(argv[2]);
    struct hostent *host;

    // Hostname stuff
    host = gethostbyname(hostname);
    if (host == NULL) {
        std::cout << stderr << " Error, no such host\n";
        exit(0);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    // socket()
    int sockfd = socket(server_addr.sin_family, SOCK_STREAM, 0);

    // connect()
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        std::cout << stderr << " Error, could not connect to " << hostname << std::endl;
        exit(0);
    }
    // send()
    char* buffer = (char*) "SYS ls";
    if (send(sockfd, buffer, sizeof(buffer), 0) == -1) {
        std::cout << stderr << " Error, could not send message to " << hostname << std::endl;
        exit(0);
    }
    // recv()
    // close()
    close(sockfd);
    return 0;
}