#include "cmdLineOpts.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char ip[18];
int port;
char fName[132];

void ReadCmdLine(int argc, char *argv[])
{
    int opt;

    while ((opt=getopt(argc, argv, ":a:p:f:")) != -1)
    {
        switch (opt)
        {
            case 'a':
                strcpy(ip, optarg);
                break;
            case 'f':
                strcpy(fName, optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
        }
    }
    // printf("cmdLine was read\n");
}

void GetSvrIp(char *iparg)
{
    strcpy(iparg, ip);
}
int GetSvrPort(void)
{
    return port;
}
void GetFileName(char *fn)
{
    strcpy(fn, fName);
}

