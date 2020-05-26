#include "fileSender.h"
#include "cmdLineOpts.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <errno.h>

#define MAX_UDP_MSG_BYTES 8972

static int sockfd;
static char svrIp[18];
struct sockaddr_in servaddr; 

static char msgBuf[MAX_UDP_MSG_BYTES];
static uint32_t nMsgsTot;
static uint32_t nBytesRem;
static uint32_t msgSeq;

int SetServerAddr()
{
    int ret = 1;        // success
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    GetSvrIp(svrIp);
    servaddr.sin_addr.s_addr = inet_addr(svrIp);
    servaddr.sin_port = htons(GetSvrPort());
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET,
                            SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error setting UDP read timeout\n");
        ret = 0;        // failure
    }
    return ret;
}

int SendFile(char fileBuf[], int nFileBytes)
{
    int ret = 1;
    uint32_t fileBufOffs = 0;
    //  each msg contains 4 byte msg len + 
    //                    4 byte msg seq + 4 byte tot msgs in file
    int nMsgsData = (nFileBytes - 1) / MAX_UDP_MSG_BYTES + 1;
    int nOverhead = 12 * nMsgsData;
    // recalc nMsgsTot including msg overhead
    nBytesRem = nMsgsTot * 12 + nFileBytes;
    nMsgsTot = (nBytesRem - 1) / MAX_UDP_MSG_BYTES + 1;
    msgSeq = 0;
    uint32_t nDataBytes;

    while (nBytesRem > 0)
    {
        int nToSend;
        if (nBytesRem > MAX_UDP_MSG_BYTES)
        {
            nToSend = MAX_UDP_MSG_BYTES;
        }
        else
        {
           nToSend = nBytesRem;
        }
        nDataBytes = nToSend - 12;
        int msgOk = 0;
        //  tbd start retry loop
        while (!msgOk)
        {
            memcpy(msgBuf, &nToSend, 4);
            memcpy(&msgBuf[4], &msgSeq, 4);
            memcpy(&msgBuf[8],&nMsgsTot, 4);
            memcpy(&msgBuf[12], &fileBuf[fileBufOffs], nDataBytes);
            sendto(sockfd, (const char *)msgBuf, nToSend, 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
                sizeof(servaddr)); 
            //  we expect an ACK msg from server
            struct sockaddr *ackAddr = malloc(sizeof(struct sockaddr));
            int ackbuf;
            int ackLen;
            int nRcv = recvfrom(sockfd, (char *)&ackbuf, sizeof(ackbuf), 
                        MSG_WAITALL, ackAddr, &ackLen); 
            free(ackAddr);  // add bpoint here for debug
            if (nRcv < 0)
            {
                ret = 0;
                if (errno == EAGAIN)
                {
                    printf("Timeout waiting for ACK\n");
                }
                printf("Error receiving ACK, errno= %d\n", errno);
                break;
            }
            msgOk = 1;
        }       // end retry loop
        fileBufOffs += nDataBytes;
        nBytesRem -= nToSend;
    }       // end while (nBytesRem > 0)

  
}

void CloseUdp()
{
    close(sockfd);
}