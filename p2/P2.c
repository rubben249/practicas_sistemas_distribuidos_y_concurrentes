/*---------------------------------
    File: P2.c
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

    // Inicializa el reloj de Lamport para P2
    int local_lamport_clock = 1;

    // Espera a que el reloj alcance un valor específico
    while (get_clock_lamport() < local_lamport_clock) {
        continue;
    }

    // Espera a recibir un mensaje de P1
    struct message msg = receive_message();
    if (msg.action == READY_TO_SHUTDOWN) {
        printf("P2 ha recibido un mensaje de P1. Enviando orden de apagado a P1.\n");

        // Envía una orden de apagado a P1
        send_message("P1", SHUTDOWN_NOW);

        // Espera a recibir un ACK de P3 antes de continuar
        struct message ack_msg = receive_message();
        if (ack_msg.action == SHUTDOWN_ACK) {
            printf("P2 ha recibido un ACK de P1. Enviando orden de apagado a P3.\n");

            // Envía una orden de apagado a P3
            send_message("P3", SHUTDOWN_NOW);

            // Espera a recibir un ACK de P3 antes de continuar
            struct message ack_msg_p3 = receive_message();
            if (ack_msg_p3.action == SHUTDOWN_ACK) {
                printf("P2 ha recibido un ACK de P3. Los clientes fueron correctamente apagados en t(lamport) = %u\n", get_clock_lamport());
            }
        }
    }

    // Finaliza el stub
    finalize_stub();
    return 0;
}
