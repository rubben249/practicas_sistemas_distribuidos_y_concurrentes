/*---------------------------------
    File: P3.c
    Author: Rubén Bautista Barajas
    Date: 16/10/2023
-----------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stub.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <puerto del servidor>\n", argv[0]);
        exit(1);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    if (initialize_stub(server_ip, server_port) != 0) {
        fprintf(stderr, "Error al inicializar el stub.\n");
        exit(1);
    }

    // Inicializa el reloj de Lamport para P3
    int local_lamport_clock = 0;

    // Espera a que el reloj alcance un valor específico
    while (get_clock_lamport() < local_lamport_clock) {
        continue;
    }

    // Espera a recibir un mensaje de P1
    struct message msg = receive_message();
    if (msg.action == READY_TO_SHUTDOWN) {
        printf("P3 ha recibido un mensaje de P1. Enviando ACK de apagado a P2.\n");

        // Envía un ACK de apagado a P2
        send_message("P2", SHUTDOWN_ACK);

        // Espera a recibir un mensaje de P2
        struct message p2_msg = receive_message();
        if (p2_msg.action == SHUTDOWN_NOW) {
            // Finaliza el stub
            finalize_stub();

            return 0;
        }
    }

    // Finaliza el stub
    finalize_stub();

    return 0;
}