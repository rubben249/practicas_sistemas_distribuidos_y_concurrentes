/*---------------------------------
    File: P1.c
    Author: Rubén Bautista Barajas
    Date: 16/10/2023
-----------------------------------*/

#include "stub.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <puerto del servidor>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    if(initialize_stub(server_ip, server_port)){
        fprintf(stderr, "Error al inicializar el stub.\n");
        exit(1);
    }

    while (get_clock_lamport() == 0){
        continue;
    }

    send_message("P2", READY_TO_SHUTDOWN);

    while (get_clock_lamport() == 2){
        continue;
    }
       
    // Espera recibir la orden de P2 para apagarse
    struct message msg = receive_message();

    while (get_clock_lamport() == 3){
        continue;
    }
    
    // Si el mensaje es SHUTDOWN_NOW, envía un ACK a P2
    if (msg.action == SHUTDOWN_NOW) {
        printf("P3 ha mandado un ACK a P2. Puede apagarse.\n");
        send_message("P2", SHUTDOWN_ACK);
    }

    // Cierre edl stub
    close_socket();
    return 0;
}