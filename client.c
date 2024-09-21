#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5555

int main(void) {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    if((clientSocket = socket(PF_INET, SOCK_STREAM, 0))==0) {
        perror("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    printf("Client Socket created successfully....\n");

    // cleaning the serverAddr and then assigning values
    memset(&serverAddr,'\0',sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //hardcoding it rn

    if(connect(clientSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0) {
        perror("Error connecting to server\n");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server successfully....\n");

    //cleaning buffer
    memset(buffer,'\0',sizeof(buffer));
    if (recv(clientSocket, buffer, 1024, 0)<0) {
        perror("Error receiving data\n");
        exit(EXIT_FAILURE);
    }
    printf("Data received from server successfully....\n");
    printf("Received data: %s\n", buffer);
    printf("Closing connection....\n");

    return 0;
}
