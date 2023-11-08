/*---------------------------------
    File: client.c
    Author: Rubén Bautista Barajas
    Date: 21/11/2023
-----------------------------------*/

#include "proxy.h"

// Estructura para pasar los argumentos a los hilos.
typedef struct {
    char *ip;
    int port;
    enum operations op;
    unsigned int id;
} ThreadArgs;

void *client_thread(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    run_client(thread_args->ip, thread_args->port, thread_args->op, thread_args->id);
    free(thread_args);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <ip> <port> <operation: read/write> <num_clients>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    enum operations op = (strcmp(argv[3], "write") == 0) ? WRITE : READ;
    int num_clients = atoi(argv[4]);

    pthread_t *threads = malloc(num_clients * sizeof(pthread_t));
    if (!threads) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_clients; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (!args) {
            perror("Memory allocation failed");
            free(threads);
            return EXIT_FAILURE;
        }

        args->ip = ip;
        args->port = port;
        args->op = op;
        args->id = i;

        if (pthread_create(&threads[i], NULL, client_thread, args) != 0) {
            perror("Failed to create thread");
            free(args);
            continue;
        }
    }

    for (int i = 0; i < num_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return EXIT_SUCCESS;
}