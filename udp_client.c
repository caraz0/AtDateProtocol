#include <stdio.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#include "debug.h"

#define UNIX_OFFSET 2208988800L

static time_t packet;
static struct hostent *server;
static struct sockaddr_in serveraddr;
static int sockfd;

static void populateFields(int port);
static int sendDatagram();
static char *getTimeFormmated(time_t timeToFormat);
static int listenToPacket();

void runUDPClient(char host[], unsigned int port)
{

    server = gethostbyname(host);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no hay host: %s\n", host);
        exit(0);
    }

    populateFields(port);

    if (sendDatagram())
    {
        perror("ERROR al escribir el socket");
        exit(0);
    }
    debug("Enviado datagrama vacio al servidor\n");

    int n = listenToPacket();
    if (n == -1)
    {
        perror("ERROR al leer el socket");
        exit(0);
    }
    printf("Recibidos %i bytes del servidor\n", n);
    printf("Time: %s\n", getTimeFormmated(packet));
    close(sockfd);
}

static void populateFields(int port)
{
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(port);
}

static int sendDatagram()
{
    return sendto(sockfd, 0, 0, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
}

static char *getTimeFormmated(time_t timeToFormat)
{
    timeToFormat = ntohl(packet) - UNIX_OFFSET;
    return ctime(&timeToFormat);
}

static int listenToPacket()
{
    socklen_t len = sizeof(serveraddr);
    return recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&serveraddr, &len);
}