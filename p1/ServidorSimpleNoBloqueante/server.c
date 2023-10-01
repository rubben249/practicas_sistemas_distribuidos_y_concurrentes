/*---------------------------------
    File: server.c
    Author: Rubén Bautista Barajas
    Date: 27/9/2023
-----------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/select.h>
#include <unistd.h>

#define PORT 8080
#define MAX_MSG_SIZE 1024

fd_set readmask;
struct timeval timeout;

int server_socket;

void sigint_handler(int signum) {
    printf("\nShutting down the server.\n");
    close(server_socket);
    exit(0);
}

int main() {
    setbuf(stdout, NULL);
    signal(SIGINT, sigint_handler);

    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    /*
    Create the server socket
    */
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    printf("Socket successfully created...\n");

    /*
     Binding the socket to the client address and port
     */
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket binding error");
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully binded...\n");

    /*
    Listen for incoming connections
    */
   if (listen(server_socket, 1) < 0) {
        perror("Listening error");
        exit(EXIT_FAILURE);
    }

    /*
     Accept incoming connection
    */
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    char message[MAX_MSG_SIZE];
    char buffer[MAX_MSG_SIZE];

    while (1) {
        
        /*
        Reset the mask
        */
        FD_ZERO(&readmask);
        
        /*
        We assign the new descriptor
        */
        FD_SET(client_socket, &readmask);

        FD_SET(STDIN_FILENO, &readmask);
        
        if (select(client_socket+1, &readmask, NULL, NULL, &timeout)==-1)
        exit(-1);

        /*
        Checks if there is data to read from the descriptor
        */
        if (FD_ISSET(client_socket, &readmask)){
            /*
            Initialize the receive buffer to 0
            */
            memset(buffer, 0, sizeof(buffer));
            int r = recv(client_socket, (void*) buffer, sizeof(buffer), MSG_DONTWAIT);        
            /*
            Receive response from the server
            */
            
            printf("+++ %s", buffer);
        }
              
        printf(">");
        /*
        Read message from stdin
        */
        fgets(message, MAX_MSG_SIZE, stdin);
        send(client_socket, message, strlen(message), 0);

    }
    sigint_handler(client_socket);
    return 0;
}