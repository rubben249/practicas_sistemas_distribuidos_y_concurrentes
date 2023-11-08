/*---------------------------------
    File: proxy.c
    Author: Rubén Bautista Barajas
    Date: 21/11/2023
-----------------------------------*/

#include "proxy.h"

// Utilidad para crear un socket y conectarlo al servidor.
int create_socket_and_connect(const char *ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Cannot create socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    return sockfd;
}

void run_client(const char *ip, int port, enum operations op, unsigned int id) {
    int sockfd = create_socket_and_connect(ip, port);
    if (sockfd < 0) {
        exit(EXIT_FAILURE);
    }

    struct request req;
    req.action = op;
    req.id = id;

    // Send the request to the server.
    if (write(sockfd, &req, sizeof(req)) < 0) {
        perror("Write failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Wait for the response from the server.
    struct response resp;
    if (read(sockfd, &resp, sizeof(resp)) < 0) {
        perror("Read failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Output the response.
    printf("[Client %u] Operation: %s, Counter: %u, Latency: %ld ns\n",
           id,
           op == READ ? "Read" : "Write",
           resp.counter,
           resp.latency_time);

    close(sockfd);
}
