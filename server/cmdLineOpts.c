#include "cmdLineOpts.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int port;

void ReadCmdLine(int argc, char *argv[])
{
    int opt;

    while ((opt=getopt(argc, argv, ":a:p:f:")) != -1)
    {
        switch (opt)
        {
            // case 'a':
            //     strcpy(ip, optarg);
            //     break;
            // case 'f':
            //     strcpy(fName, optarg);
            //     break;
            case 'p':
                port = atoi(optarg);
                break;
        }
    }
    // printf("cmdLine was read\n");
}

int GetSvrPort(void)
{
    return port;
}

