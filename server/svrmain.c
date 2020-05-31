// Server side implementation of UDP client-server model 
#include "threading.h"
#include "fileWriter.h"
#include "udpMsgDef.h"
#include "cmdLineOpts.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
 
static char *fileRcvBuf;

// Driver code 
int main(int argc, char *argv[])
{
	static struct thread_info_t fwThreadInfo;	// file writer thread

	int sockfd; 
	char rcvBuf[MAX_UDP_PKT_LEN];
	struct FileXfrHeader_t *udpHdr = (struct FileXfrHeader_t *)rcvBuf;
	struct sockaddr_in servaddr, cliaddr;


    printf("\n\nUdpRcv - Linux UDP server file receiver v20200531\n\n");
    if (argc < 2)
    {
		ShowCmdLineHelp();
        return 0;
    }
    ReadCmdLine(argc, argv);        // cmdLineOpts
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(GetSvrPort()); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	InitFileWriter();
	printf("Server waiting for udp input at port %d\n", GetSvrPort());
	uint32_t cliAddrLen, nRcv; 
	char ackBuf[4];
	cliAddrLen = sizeof(cliaddr); //cliAddrLen is returned value

	while (1)
	{
		nRcv = recvfrom(sockfd, (char *)rcvBuf, MAX_UDP_PKT_LEN,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&cliAddrLen);
		if (nRcv < 0)
		{
			printf("Error receiving packet, errno= %d\n", errno);
		}
		// char addrstr[32];
        // inet_ntop(AF_INET, &(((struct sockaddr_in *)&cliaddr)->sin_addr),
        //             addrstr, 32);
		// printf("incoming msg from %s\n", addrstr);
		// nRcv = recv(sockfd, (char *)rcvBuf, MAX_UDP_PKT_LEN,
		// 		MSG_WAITALL);
		// printf("nRcv=%d", nRcv);
		if (nRcv != udpHdr->pktSize)
		{
			printf("Warning: socket received %d bytes but pktSize in data is %d\n",
				nRcv, udpHdr->pktSize);
		}
		else
		{
			//  send ACK packet
			ssize_t nSent = sendto(sockfd, (const char *)&udpHdr->pktSeq, 4, 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, cliAddrLen);
			// printf("Received %d bytes, sent ACK of pkt # %d\n", nRcv, pktSeq);
			if (nSent < 0)
			{
				printf("Error sending ack, errno=%d\n", errno);
			}
			// Add to file writer buffer. If file is complete then use separate
			//  thread to write it to disk.
			AddToFwBuf(rcvBuf);
		}
		
	}	
	return 0; 
} 
