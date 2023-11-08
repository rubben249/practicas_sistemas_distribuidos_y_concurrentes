/*---------------------------------
    File: proxy.h
    Author: Rubén Bautista Barajas
    Date: 21/11/2023
-----------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#ifndef PROXY_H
#define PROXY_H

enum operations {
    WRITE = 0,
    READ = 1
};

struct request {
    enum operations action;
    unsigned int id;
};

struct response {
    enum operations action;
    unsigned int counter;
    long latency_time;
};

// Función para inicializar y ejecutar el cliente.
void run_client(const char *ip, int port, enum operations op, unsigned int id);

#endif // PROXY_H
