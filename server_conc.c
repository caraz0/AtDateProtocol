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
#define BUFSIZE 1024

static void populateFields(int port);
static void bindTCPSocket();
static void unmarkPreviousConnections();
static void createLoopListener();
static void answerTCPConnection();


static int listenfd, connfd;
static pid_t childpid;
static time_t now;
static fd_set rset;
static struct sockaddr_in cliaddr, servaddr;

void runServer(unsigned int port)
{

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    populateFields(port);
    bindTCPSocket();
    unmarkPreviousConnections();
    createLoopListener();
}

static void populateFields(int port)
{
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
}

static void bindTCPSocket()
{
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 10);
}


static void unmarkPreviousConnections()
{
    FD_ZERO(&rset); 
}

static void createLoopListener()
{
    while (1)
    {
        answerTCPConnection();
    }
}


static void answerTCPConnection()
{
    socklen_t len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
    if ((childpid = fork()) == 0)
    {
        close(listenfd);
        debug("Conexion de cliente TCP\n");
        while (1)
        {
            
            time(&now);
            now = htonl(now + UNIX_OFFSET);
            int n = write(connfd, (time_t *)&now, sizeof(now));//send en tcp
            if (n < 0)
            {
                debug("Cliente cerro conexion\n");
                break;
            }
            if (childpid == 0)
                debug("Enviados %i bytes al cliente %i\n", n, cliaddr.sin_addr.s_addr);
            sleep(1);
        }
        close(connfd); //cierra el cliente
        exit(0);
    }
    close(connfd);
}

