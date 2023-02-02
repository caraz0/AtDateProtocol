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
static int connectToHost();
static void startLoopListener();

void runTCPClient(char host[], unsigned int port)
{
    server = gethostbyname(host);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no hay host: %s\n", host);
        exit(0);
    }

    populateFields(port);

    if (connectToHost() < 0)
    {
        perror("ERROR de conexión");
        exit(0);
    }

    debug("Conexion con el host con exito");
    startLoopListener();
}

static void populateFields(int port)
{
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(port);
}

static int connectToHost()
{
    return connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
}

static void startLoopListener()
{
    while (1)
    {
        int n = read(sockfd, &packet, sizeof(packet));
        if (n == -1)
        {
            perror("ERROR de lectura del socket");
            exit(0);
        }

        if (n > 0)
        {
            debug("Recibidos %i bytes del servoidor\n", n);
            packet = ntohl(packet) - UNIX_OFFSET;
            debug("Time: %s", ctime(&packet));

        }
    }
}