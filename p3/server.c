/*---------------------------------
    File: server.c
    Author: Rubén Bautista Barajas
    Date: 21/11/2023
-----------------------------------*/

#include "proxy.h"

#define MAX_THREADS 600
#define QUEUE_SIZE 1024

// Variables globales
unsigned int counter = 0;
pthread_mutex_t counter_mutex;
pthread_mutex_t file_mutex;
sem_t reader_sem, writer_sem;
int reader_count = 0;
int writer_count = 0;
int reader_priority, writer_priority;
int waiting_writers = 0;
int waiting_readers = 0;

// Prototipo de funciones
void initialize_server();
void *handle_client(void *client_socket);
void read_counter_from_file();
void write_counter_to_file();
long get_time_in_microseconds();
void random_sleep();

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s --port PORT --priority writer/reader\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    const char *priority = argv[4];

    reader_priority = (strcmp(priority, "reader") == 0);
    writer_priority = !reader_priority;

    initialize_server();
    read_counter_from_file();

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Binding the socket to the address and port specified
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for connections
    if (listen(server_fd, QUEUE_SIZE) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", port);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        pthread_t thread_id;
        int *socket_ptr = malloc(sizeof(int));
        *socket_ptr = new_socket;

        // Limit the number of concurrent threads
        pthread_mutex_lock(&counter_mutex);
        while (writer_count + reader_count >= MAX_THREADS) {
            pthread_mutex_unlock(&counter_mutex);
            usleep(1000); // Sleep 1 ms to wait for threads to finish
            pthread_mutex_lock(&counter_mutex);
        }
        pthread_mutex_unlock(&counter_mutex);

        if (pthread_create(&thread_id, NULL, handle_client, socket_ptr) != 0) {
            perror("Failed to create thread");
            close(new_socket);
            free(socket_ptr);
        }
    }

    return 0;
}

void initialize_server() {
    pthread_mutex_init(&counter_mutex, NULL);
    pthread_mutex_init(&file_mutex, NULL);
    sem_init(&reader_sem, 0, 1);
    sem_init(&writer_sem, 0, 1);
}

void read_counter_from_file() {
    FILE *file = fopen("server_output.txt", "r");
    if (file) {
        if (fscanf(file, "%u", &counter) != 1) {
            perror("Failed to read counter from file");
        }
        fclose(file);
    }
}

void write_counter_to_file() {
    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen("server_output.txt", "w");
    if (file) {
        fprintf(file, "%u", counter);
        fclose(file);
    } else {
        perror("Failed to open file for writing");
    }
    pthread_mutex_unlock(&file_mutex);
}

long get_time_in_microseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
}

void random_sleep() {
    int time = (rand() % (150 - 75 + 1)) + 75;
    usleep(time * 1000); // sleep for 75-150 ms
}

void *handle_client(void *client_socket) {
    int sock = *((int *)client_socket);
    free(client_socket);

    struct request req;
    struct response res;
    long start_time, end_time;

    if (read(sock, &req, sizeof(req)) < 0) {
        perror("read");
        close(sock);
        return NULL;
    }

    start_time = get_time_in_microseconds();

    if (req.action == WRITE) {
        pthread_mutex_lock(&counter_mutex);
        waiting_writers++;
        while ((reader_priority && waiting_readers > 0) || (writer_count > 0)) {
            pthread_mutex_unlock(&counter_mutex);
            usleep(1000); // Sleep 1 ms
            pthread_mutex_lock(&counter_mutex);
        }
        waiting_writers--;
        writer_count++;
        pthread_mutex_unlock(&counter_mutex);

        // Write operation
        counter++;
        write_counter_to_file();
        printf("[%ld.%ld][ESCRITOR #%u] modifica contador con valor %u\n", start_time / 1000000, start_time % 1000000, req.id, counter);

        random_sleep();

        pthread_mutex_lock(&counter_mutex);
        writer_count--;
        pthread_mutex_unlock(&counter_mutex);

        res.action = WRITE;
        res.counter = counter;
    } else if (req.action == READ) {
        pthread_mutex_lock(&counter_mutex);
        waiting_readers++;
        while ((writer_priority && waiting_writers > 0) || (writer_count > 0)) {
            pthread_mutex_unlock(&counter_mutex);
            usleep(1000); // Sleep 1 ms
            pthread_mutex_lock(&counter_mutex);
        }
        waiting_readers--;
        reader_count++;
        if (reader_count == 1) {
            sem_wait(&writer_sem);
        }
        pthread_mutex_unlock(&counter_mutex);

        // Read operation
        printf("[%ld.%ld][LECTOR #%u] lee contador con valor %u\n", start_time / 1000000, start_time % 1000000, req.id, counter);

        random_sleep();

        pthread_mutex_lock(&counter_mutex);
        reader_count--;
        if (reader_count == 0) {
            sem_post(&writer_sem);
        }
        pthread_mutex_unlock(&counter_mutex);

        res.action = READ;
        res.counter = counter;
    }

    end_time = get_time_in_microseconds();
    res.latency_time = end_time - start_time;

    if (write(sock, &res, sizeof(res)) < 0) {
        perror("write");
    }

    close(sock);
    return NULL;
}