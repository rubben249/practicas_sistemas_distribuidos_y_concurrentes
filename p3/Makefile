CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -pthread

all: client server

client: client.c proxy.c
	$(CC) $(CFLAGS) -o client client.c proxy.c $(LDFLAGS)

server: server.c proxy.c
	$(CC) $(CFLAGS) -o server server.c proxy.c $(LDFLAGS)

clean:
	rm -f client server