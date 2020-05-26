// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include "fileSender.h"
#include "cmdLineOpts.h"
#include "fileList.h"

int main(int argc, char *argv[])
{ 
    int fileSize;
    int indx;
    int argLen;
    int optNum;
    int optSel;
    int optErr;
    struct stat st;
    char *msgBuf;
    FILE *fptr;
    char fileName[132];
    char fileNames[100][132];

    printf("\n\nUdpXmt - Linux UDP client file transmitter v20200522\n\n");
    if (argc < 2)
    {
        printf("switches (preceded by - or / and followed by blank)\n");
        printf("  -a  char[] ip address of server\n");
        printf("  -p  int    destination server port\n");
        printf("  -f  char[] name of file to be sent, if name is preceded by @,\n");
        printf("             the file contains a list of files to be sent.\n");
        printf("  Example: UdpXmt -a 192.168.1.29 12111 @fileList.txt");
        return 0;
    }
    ReadCmdLine(argc, argv);        // cmdLineOpts
    SetServerAddr();                // fileSender
    GetFileName(fileName);          // cmdLineOpts
    // printf("\noptions:\n"
    //         "  Server address: %s\n"
    //         "            port: %d\n"
    //         "  File          : %s\n",
    //         svrIp, svrPort, fileName);

    int nFiles = ReadFileList(fileName);    // fileList
    int iFile;
    // send files to server until name length is 0
    for (iFile=0; iFile<nFiles; iFile++)
    {
        GetNextFileName(fileName);          // fileList
        // printf("Enter file name : ");
        // scanf("%s" , fileName);
        // if (strlen(fileName) == 0)
        // {
        //     break;
        // }
        if (fileName[0] == 0)
        {
            printf("File list terminated unexpectedly\n");
            break;
        }
        stat(fileName, &st);
        fileSize = st.st_size;
        msgBuf = malloc(fileSize * sizeof(char));
        fptr = fopen(fileName, "rb");
        fread(msgBuf, fileSize, 1, fptr);

        printf("sending file of %d bytes:\n", fileSize);
    
        SendFile(msgBuf, fileSize);         // fileSender
    }
    CloseUdp(); 
    return 0; 
} 

int getFileList(char *fileName[1], char *fileNames[])
{
    int nFiles = 0;
    return nFiles;
}
