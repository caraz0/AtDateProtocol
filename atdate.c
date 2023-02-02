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
#include <setjmp.h>

#include "udp_client.h"
#include "tcp_client.h"
#include "server_conc.h"
#include "debug.h"

#define DEFAULT_PORT 37
#define PORT_SIZE
#define MAX_DNS_NAME 253

static jmp_buf catch; 

static char mode[] = "cu", host[MAX_DNS_NAME];
static int s_select = 0;
static unsigned int port = DEFAULT_PORT;

void sigint_handler(int signo)
{
   
    puts("SIGINT received...");
    exit(0);
}

void throwError()
{
    longjmp(catch, 42);
}

void checkParameters(int argc, char *argv[]);
void runProgramAndDebugMessages();

void checkHostParameter()
{
    if (strlen(optarg) > 32)
    {
        printf("Argumento demasiado largo, maximo 32 caracteres\n");
        throwError();
    }
}

void checkModeParameter()
{
    if (strlen(optarg) > 2)
    {
        printf("Argumento demasiado largo, maximo 2 caracteres\n");
        throwError();
    }

    if (strcmp(optarg, "cu") != 0 && strcmp(optarg, "ct") != 0 && strcmp(optarg, "s") != 0)
    {
        printf("El parametro -mode debe ser cu/ct/s\n");
        throwError();
    }
}

void checkPortParameter()
{
    if (strlen(optarg) > 8)
    {
        printf("Argumento demasiado largo, maximo 8 caracteres\n");
        throwError();
    }
}

void unknowParameterAction()
{
    if ((optopt == 's') || (optopt == 'm') || (optopt == 'p'))
        fprintf(stderr, "Error: la opcion -%c requiere un argumento\n", optopt);
    else if (isprint(optopt))
        fprintf(stderr, "Error: argumento `-%c' no valido\n", optopt);
    else
        fprintf(stderr, "Error: argumento `\\x%x' no valido.\n", optopt);
}

void checkIfHostIsEmpty()
{
    if (s_select == 0)
    {
        printf("Debes rellenar el parametro -host en modo consulta\n");
        throwError();
    }
}

int modeIsNotHost()
{
    return strcmp(mode, "s") != 0;
}


int main(int argc, char *argv[])
{

    if (setjmp(catch))
        return 1;
    else 
        checkParameters(argc, argv);

    runProgramAndDebugMessages();
}

void checkParameters(int argc, char *argv[])
{

    int opt;
    while ((opt = getopt(argc, argv, "s:p:dm:")) != -1)
    {

        switch (opt)
        {
        case 's':
            checkHostParameter();
            strcpy(host, optarg);
            s_select = 1;
            break;
        case 'm':
            checkModeParameter();
            strcpy(mode, optarg);
            break;
        case 'p':
            checkPortParameter();
            port = atoi(optarg);
            break;
        case 'd':
            setDebugStatus(1);
            break;
        case '?':
            unknowParameterAction();
            throwError();
            break;
        default:
            fprintf(stderr, "Error: argumento %d no valido\n", optind);
            throwError();
        }
    }

    if (modeIsNotHost())
        checkIfHostIsEmpty();
}

void runProgramAndDebugMessages()
{

    if (strcmp(mode, "cu") == 0)
    {

        debug("\nCaracteristicas seleccionadas\n");
        debug("---------------------------\n");
        debug("Servidor a conectar: %s\n", host);
        debug("Modo del programa: %s\n", mode);
        debug("Puerto a usar: %i\n", port);
        debug("Modo depuracion: %i\n", getDebugStatus());
        debug("---------------------------\n");

        runUDPClient(host, port);
    }
    else if (strcmp(mode, "ct") == 0)
    {

        debug("\nCaracteristicas seleccionadas\n");
        debug("---------------------------\n");
        debug("Servidor a conectar: %s\n", host);
        debug("Modo del programa: %s\n", mode);
        debug("Puerto a usar: %i\n", port);
        debug("Modo depuracion: %i\n", getDebugStatus());
        debug("---------------------------\n");

        runTCPClient(host, port);
    }
    else if (strcmp(mode, "s") == 0)
    {

        debug("\nCaracteristicas seleccionadas\n");
        debug("---------------------------\n");
        debug("Modo servidor\n");
        debug("Puerto a escuchar: %i\n", port);
        debug("Modo depuracion: %i\n", getDebugStatus());
        debug("---------------------------\n");

        runServer(port);
    }
}