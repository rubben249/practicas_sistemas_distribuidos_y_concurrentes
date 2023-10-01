/*---------------------------------
    File: server.c
    Author: Rubén Bautista Barajas
    Date: 01/10/2023
-----------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <getopt.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_CLIENTS 100
#define MAX_MSG_SIZE 1024

int server_socket;
int client_c = 0;
pthread_t threads_c[MAX_CLIENTS];

struct ClientData {
    int socket;
    int id;
};

fd_set readmask;
struct timeval timeout;

void sigint_handler(int signum) {
    printf("\nShutting down the server.\n");
    for (int i = 0; i < client_c; i++) {
        pthread_cancel(threads_c[i]);
    }
    close(server_socket);
    exit(0);
}

void *handle_client(void *arg) {
    struct ClientData *client_data = (struct ClientData *)arg;
    int client_socket = client_data->socket;
    int client_id = client_data->id;

    char buffer[MAX_MSG_SIZE];

    /*
    Generate a random delay between 0.5 and 2 seconds
    */
    srand(time(NULL) + client_id);
    double random_delay = (double)rand() / RAND_MAX * (1.5) + 0.5;
    usleep((random_delay * 1000000));

    /*
    Send a message to the client
    */
    char response[MAX_MSG_SIZE];
    snprintf(response, sizeof(response), "Hello server! From client: %d\n", client_id);
    send(client_socket, response, strlen(response), 0);

    /*
    Receive and display messages from the client
    */
    while (1) {
        int r = recv(client_socket, buffer, sizeof(buffer), 0);
        if (r <= 0) {
            printf("Client %d disconected.\n", client_id);
            close(client_socket);
            break;
        }
        buffer[r] = '\0';
        printf("+++ %s", client_id, buffer);
    }

    free(client_data);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Use: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int PORT = atoi(argv[1]);

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
    Ports reuse
    */
    const int controler = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &controler, sizeof(int)) < 0) {
        perror("Error setting SO_REUSEADDR");
    }

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
    if (listen(server_socket, MAX_CLIENTS) < 0) {
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

    /*
    Configure the SIGINT signal handler
    */
    
    signal(SIGINT, sigint_handler);

    char message[MAX_MSG_SIZE];
    char buffer[MAX_MSG_SIZE];

    while (1) {
        if ( client_c< MAX_CLIENTS) {
            int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
            if (client_socket < 0) {
                perror("Conexion error");
                exit(EXIT_FAILURE);
            }

            struct ClientData *client_data = (struct ClientData *)malloc(sizeof(struct ClientData));
            client_data->socket = client_socket;
            client_data->id = client_c + 1;

            pthread_create(&threads_c[client_c], NULL, handle_client, (void *)client_data);
            client_c++;
        }
    }
    return 0;
}