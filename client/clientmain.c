// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/stat.h>
#include "cmdLineOpts.h"
#include "fileList.h"

int main(int argc, char *argv[])
{ 
    int sockfd; 
    int fileSize;
    int indx;
    int argLen;
    int optNum;
    int optSel;
    int optErr;
    struct stat st;
    char *msgBuf;
    char svrIp[18];
    int svrPort;
    struct sockaddr_in	 servaddr; 
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
    ReadCmdLine(argc, argv);
    GetSvrIp(svrIp);
    svrPort = GetSvrPort();
    servaddr.sin_port = htons(svrPort);
    GetFileName(fileName);
    printf("\noptions:\n"
            "  Server address: %s\n"
            "            port: %d\n"
            "  File          : %s\n",
            svrIp, svrPort, fileName);

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(svrIp);

    int nFiles = ReadFileList(fileName);
    int iFile;
    // send files to server until name length is 0
    for (iFile=0; iFile<nFiles; iFile++)
    {
        GetNextFileName(fileName);
        // printf("Enter file name : ");
        // scanf("%s" , fileName);
        // if (strlen(fileName) == 0)
        // {
        //     break;
        // }
        stat(fileName, &st);
        fileSize = st.st_size;
        msgBuf = malloc(fileSize * sizeof(char));
        fptr = fopen(fileName, "rb");
        fread(msgBuf, fileSize, 1, fptr);

        printf("sending msg of %d bytes:\n", fileSize);
    
        sendto(sockfd, (const char *)msgBuf, fileSize, 
            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr)); 
        printf("message sent okay.\n"); 
    }
    // n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
    //             MSG_WAITALL, (struct sockaddr *) &servaddr, 
    //             &len); 
    // buffer[n] = '\0'; 
    // printf("Server : %s\n", buffer); 

    close(sockfd); 
    return 0; 
} 

int getFileList(char *fileName[1], char *fileNames[])
{
    int nFiles = 0;
    return nFiles;
}
