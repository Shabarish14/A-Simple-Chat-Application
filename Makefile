C = gcc
CFLAGS= -std=gnu99 -pthread -Wall -Wpedantic -pedantic -g
CFLAGS2= -o
all: client server
client:
	$(C) $(CFLAGS) client.c ${CFLAGS2} client.x

server:
	$(C) $(CFLAGS) server.c ${CFLAGS2} server.x

clean:
	rm -v *.x
