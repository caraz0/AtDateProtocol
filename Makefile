all: server_conc.o tcp_client.o udp_client.o main

CC = gcc
CFLAGS=-Wall -g

debug: debug.c debug.h
	$(CC) -c $(CFLAGS) -o debug.o debug.c

server_conc: server_conc.c server_conc.h
	$(CC) -c $(CFLAGS) -o server_conc.o server_conc.c

tcp_client: tcp_client.c tcp_client.h
	$(CC) -c $(CFLAGS) -o tcp_client.o tcp_client.c

udp_client: udp_client.c udp_client.h
	$(CC) -c $(CFLAGS) -o udp_client.o udp_client.c

main: debug server_conc tcp_client udp_client atdate.c
	$(CC) $(CFLAGS) -o atdate atdate.c debug.o udp_client.o tcp_client.o server_conc.o

clean:
	rm -f *.o atdate tcp_client.o debug.o udp_client.o server_conc.o 