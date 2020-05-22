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

#define PORT	 8080 
#define MAXLINE 1024 

// Driver code 
int main(int argc, char *argv[])
{ 
    int sockfd; 
    char svrIp[18];
    char fileName[132];
    int fileSize;
    struct stat st;
    char *msgBuf;
    struct sockaddr_in	 servaddr; 
    FILE *fptr;

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    if (argc > 1)
    {
        strcpy(svrIp, argv[1]);
    }
    else
    {
        strcpy(svrIp, "127.0.0.1");     // local loopback
    }
    servaddr.sin_addr.s_addr = inet_addr(svrIp);

    int n, len; 
    // send files to server until name length is 0
    while(1)
    {
        printf("Enter file name : ");
        scanf("%s" , fileName);
        if (strlen(fileName) == 0)
        {
            break;
        }
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
