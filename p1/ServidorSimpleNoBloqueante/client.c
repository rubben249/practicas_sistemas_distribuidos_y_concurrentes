/*---------------------------------
    File: client.c
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
    printf("\nClosing the client.\n");
    close(server_socket);
    exit(0);
}

int main(int argc, char **argv) {

    setbuf(stdout, NULL);
    signal(SIGINT, sigint_handler);

    struct sockaddr_in server_addr;
    char message[MAX_MSG_SIZE];
    char buffer[MAX_MSG_SIZE];

    /*
    Create the client socket
    */
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully created...\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    /*
    Convert IP address from text to binary
    */
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        exit(EXIT_FAILURE);
    }

    /*
    Connect to server
    */
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to the server...\n");

    while (1) {
       

        /*
        Reset the mask
        */
        FD_ZERO(&readmask);
        
        /*
        We assign the new descriptor
        */
        FD_SET(server_socket, &readmask);

        FD_SET(STDIN_FILENO, &readmask);
 
        /*
        Timeout de 0.5 seg of.
        */
        timeout.tv_sec=0; timeout.tv_usec=500000; 
        
        if (select(server_socket+1, &readmask, NULL, NULL, &timeout)==-1)
        exit(-1);

        /*
        Checks if there is data to read from the descriptor
        */
        if (FD_ISSET(server_socket, &readmask)){
            /*
            Initialize the receive buffer to 0
            */
            memset(buffer, 0, sizeof(buffer));

            /*
            Receive response from the server
            */
            int r = recv(server_socket, (void*) buffer, sizeof(buffer), MSG_DONTWAIT);
            if(r== 0){
                printf("Client disconnected\n");
                break;
            }

            
            printf("+++ %s", buffer);
        } 

         printf(">");
        /*
        Read message from stdin
        */
        fgets(message, MAX_MSG_SIZE, stdin);
        send(server_socket, message, strlen(message), 0);
    }
    sigint_handler(server_socket);

    return 0;
}