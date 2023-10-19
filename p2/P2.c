/*---------------------------------
    File: P2.c
    Author: Rubén Bautista Barajas
    Date: 16/10/2023
-----------------------------------*/

#include "stub.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <puerto del servidor>\n", argv[0]);
        exit(1);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    start_server(server_ip, server_port);


    while (get_clock_lamport() == 0){
        continue;
    }

    // Espera a recibir un mensaje de P1
    struct message msg = receive_message();
    if (msg.action == READY_TO_SHUTDOWN) {
        printf("READY_TO_SHUTDOWN");

            // Envía una orden de apagado a P1
            send_message("P1", SHUTDOWN_NOW);

            // Espera a recibir un ACK de P1 antes de continuar
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
    close_socket();
    return 0;
}
