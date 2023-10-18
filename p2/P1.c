/*---------------------------------
    File: P1.c
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

    // Inicializa el reloj de Lamport para P1
    int local_lamport_clock = 0;

    // Espera a que el reloj alcance un valor específico
    while (get_clock_lamport() < local_lamport_clock) {
        continue;
    }

    // Por ejemplo, notificar que P1 está listo para apagarse
    send_message("P2", READY_TO_SHUTDOWN);

    // Espera recibir un ACK de P2 antes de continuar
    struct message msg = receive_message();
    if (msg.action == SHUTDOWN_ACK) {
        printf("P1 ha recibido un ACK de P2. Puede apagarse.\n");
    }

    // Finaliza el stub
    finalize_stub();
    return 0;
}