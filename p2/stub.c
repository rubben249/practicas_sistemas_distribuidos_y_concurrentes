/*---------------------------------
    File: stub.c
    Author: Rubén Bautista Barajas
    Date: 10/10/2023
-----------------------------------*/

#include "stub.h"


#define MAX_CLIENTS 2

int conexion_socket;

static int socket_fd;
static struct sockaddr_in server_addr;
static int lamport_clock = 0;


void start_server(char *name, int port){
    setbuf(stdout, NULL);
    pthread_t thread[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    int i;

    conexion_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(conexion_socket < 0){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Server Socket successfully created...\n");
    }

    //Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(setsockopt(conexion_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        perror("Error setting socket options");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Socket options successfully set...\n");
    }   

    if(bind(conexion_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Socket successfully binded...\n");
    }
}

int initialize_stub(char *server_ip, int server_port) {
    // Crear un socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error al crear el socket");
        return -1;
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &(server_addr.sin_addr)) < 0) {
        perror("Error al configurar la dirección del servidor");
        return -1;
    }

    // Conectar al servidor
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr) < 0)) {
        perror("Error al conectar al servidor");
        return -1;
    }
    printf("Conexión al servidor establecida\n");

    return 0;  // Éxito
}

void close_socket() {
    close(socket_fd);
    exit(0);
    // Realiza la limpieza de recursos necesaria
}

int get_clock_lamport() {
    return lamport_clock;
}

void send_message(const char *destination, enum operations action) {
    struct message msg;
    memset(&msg, 0, sizeof(struct message));
    strncpy(msg.origin, "P2", sizeof(msg.origin));
    msg.action = action;
    msg.clock_lamport = lamport_clock;

    // Simula el envío del mensaje a través del socket
    sendto(socket_fd, &msg, sizeof(struct message), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Actualiza el reloj de Lamport después de enviar el mensaje
    lamport_clock++;
}

struct message receive_message() {
    struct message msg;
    memset(&msg, 0, sizeof(struct message));

    // Simula la recepción de un mensaje a través del socket
    recvfrom(socket_fd, &msg, sizeof(struct message), 0, NULL, NULL);

    // Actualiza el reloj de Lamport después de recibir el mensaje
    lamport_clock = (msg.clock_lamport > lamport_clock) ? msg.clock_lamport + 1 : lamport_clock + 1;
    lamport_clock++;

    return msg;
}