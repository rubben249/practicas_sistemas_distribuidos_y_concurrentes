/*---------------------------------
    File: stub.c
    Author: Rubén Bautista Barajas
    Date: 10/10/2023
-----------------------------------*/

#include "stub.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static int socket_fd;
static struct sockaddr_in server_addr;
static int lamport_clock = 0;

int initialize_stub(const char *ip, int port) {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Error opening socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Inicializa el reloj de Lamport
    lamport_clock = 0;

    return 0;
}

void finalize_stub() {
    close(socket_fd);
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

    return msg;
}