#include <stdarg.h>
#include <stdio.h>

int enabled = 0;

void debug(char *msg, ...)
{
    va_list arg;
    va_start(arg, msg);
    if (enabled == 1)
        vfprintf(stdout, msg, arg);
    va_end(arg);
}

void setDebugStatus(int status)
{
    enabled = status;
}

int getDebugStatus()
{
    return enabled;
}
