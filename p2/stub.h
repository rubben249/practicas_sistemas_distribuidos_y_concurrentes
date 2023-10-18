/*---------------------------------
    File: stub.h
    Author: Rubén Bautista Barajas
    Date: 16/10/2023
-----------------------------------*/

#ifndef STUB_H
#define STUB_H

enum operations {
    READY_TO_SHUTDOWN = 0,
    SHUTDOWN_NOW,
    SHUTDOWN_ACK
};

struct message {
    char origin[20];
    enum operations action;
    unsigned int clock_lamport;
};

int initialize_stub(const char *ip, int port);

void finalize_stub();

int get_clock_lamport();

void send_message(const char *destination, enum operations action);

struct message receive_message();

#endif
