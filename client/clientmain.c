// Client side implementation of UDP client-server model 
#include "fileList.h"
#include "fileSender.h"
#include "cmdLineOpts.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>

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
        ShowCmdLineHelp();
        return 0;
    }
    ReadCmdLine(argc, argv);        // cmdLineOpts
    char ip[32];
    GetSvrIp(ip);
    printf("Sending to server @%s:%d\n", ip, GetSvrPort());
    InitFileSender();
    GetFileName(fileName);          // cmdLineOpts

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

        printf("sending file %s of %d bytes:\n",
                fileName, fileSize);
        SendFile(msgBuf, fileSize);         // fileSender
        sleep(1);    // avoid duplicate output time stamp names
    }
    CloseUdp(); 
    return 0; 
} 

int getFileList(char *fileName[1], char *fileNames[])
{
    int nFiles = 0;
    return nFiles;
}
