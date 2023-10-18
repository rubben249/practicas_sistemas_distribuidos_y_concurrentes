CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -pthread

all: P1 P2 P3

P1: P1.c stub.c
	$(CC) $(CFLAGS) -o P1 P1.c stub.c $(LDFLAGS)

P2: P2.c stub.c
	$(CC) $(CFLAGS) -o P2 P2.c stub.c $(LDFLAGS)

P3: P3.c stub.c
	$(CC) $(CFLAGS) -o P3 P3.c stub.c $(LDFLAGS)

clean:
	rm -f P1 P2 P3
