#include "cmdLineOpts.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char ip[18];
int port;
char fName[132];
int singleStepDbg = 0;

void ShowCmdLineHelp()
{
        printf("switches (preceded by - or / and followed by blank)\n");
        printf("  -a  char[] ip address of server\n");
        printf("  -p  int    destination server port\n");
        printf("  -f  char[] name of file to be sent, if name is preceded by @,\n");
        printf("             the file contains a list of files to be sent.\n");
        printf("  -s  none used for debugging - if present, enables single step"
               "           message sending" );
        printf("  Example: UdpXmt -a 192.168.1.29 -p 12111 -f @fileList.txt\n");
}
void ReadCmdLine(int argc, char *argv[])
{
    int opt;

    while ((opt=getopt(argc, argv, "a:p:f:s")) != -1)
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
            case 's':
                singleStepDbg = 1;
                break;
        }
    }
    printf("cmdLine was read, single step dbg=%d\n", singleStepDbg);
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
int GetSingleStepDbg(void)
{
    return singleStepDbg;
}
