#include "cmdLineOpts.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static int port;
static uint32_t bufSize = 50 * 1024 * 1024;

void ShowCmdLineHelp()
{
    printf("switches (preceded by - or / and followed by blank)\n");
    printf("  -p  int      destination server port\n");
    printf("  -m  uint32_t file buffer size in bytes [def 50 mb\n");
    printf("  Example: UdpRcv -p 8080");
}

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
            case 'm':
                bufSize = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
        }
    }
    // printf("cmdLine was read\n");
}

uint32_t GetBufSize(void)
{
    return bufSize;
}

int GetSvrPort(void)
{
    return port;
}

