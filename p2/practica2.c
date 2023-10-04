/*---------------------------------
    File: client.c
    Author: Rubén Bautista Barajas
    Date: 01/10/2023
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

#define PORT 8080
#define MAX_MSG_SIZE 1024

int server_socket;
int client_id;

fd_set readmask;
struct timeval timeout;

void sigint_handler(int signum) {
    printf("\nClosing the client.\n");
    close(server_socket);
    exit(0);
}

int main(int argc, char **argv) {
    /*
    We check that the arguments are valid
    */
    if (argc != 4) {
        fprintf(stderr, "Use: %s <client_id> <ip_server> <port_server>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    client_id = atoi(argv[1]);
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

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
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
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
    
    /*
    Configure the SIGINT signal handler
    */
    signal(SIGINT, sigint_handler);

    printf("Connected to the server...\n");

    /*
    Send the message to the server
    */
    snprintf(message, sizeof(message), "Hello server! From client: %d\n", client_id);
    send(server_socket, message, strlen(message), 0);

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
        
        timeout.tv_sec=0; timeout.tv_usec=500000; 
        if (select(server_socket+1, &readmask, NULL, NULL, &timeout)==-1){
            exit(-1);
        }

        int constant = select(server_socket+1, &readmask, NULL, NULL, &timeout);

        if (FD_ISSET(STDIN_FILENO, &readmask)) {
            printf("> ");
            /*
            Read message from stdin
            */
            fgets(message, MAX_MSG_SIZE, stdin);
            
            /*
            Send the message
            */
            send(server_socket, message, strlen(message), 0);
        }

        /*
        Checks if there is data to read from the descriptor
        */
        if (FD_ISSET(server_socket, &readmask)){

            if(recv(server_socket, buffer, sizeof(buffer), 0)== 0){
                printf("Client disconnected\n");
                break;
            }
            /*
            Initialize the receive buffer to 0
            */
            memset(buffer, 0, sizeof(buffer));
            
            /*
            Receive response from the server
            */
            int r = recv(server_socket, (void*) buffer, sizeof(buffer), MSG_DONTWAIT);   
            printf("+++ %s", buffer);
        }        
    }

    sigint_handler(server_socket);
    return 0;
}