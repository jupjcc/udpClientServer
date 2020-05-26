// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
 
#define PORT	 8080 
#define MAX_UDP_PKT_LEN 8972

// Driver code 
int main() { 
	int sockfd; 
	char rcvBuf[MAX_UDP_PKT_LEN]; 
	char *hello = "Hello from server"; 
	struct sockaddr_in servaddr, cliaddr; 
	
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
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
    printf("\n\nUdpRcv - Linux UDP server file receiver v20200526\n\n");
	printf("Server waiting for udp input at port %d\n", PORT);
	uint32_t msgSize;
	uint32_t msgSeq;
	uint32_t nMsgsTot;
	uint32_t len, nRcv; 
	char ackBuf[4];
	len = sizeof(cliaddr); //len is returned valued 

	nRcv = recvfrom(sockfd, (char *)rcvBuf, MAX_UDP_PKT_LEN,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len);
	memcpy(&msgSize, rcvBuf, 4);
	memcpy(&msgSeq, &rcvBuf[4], 4);
	memcpy(&nMsgsTot, &rcvBuf[8], 4);
	if (nRcv != msgSize)
	{
		printf("Warning: socket received %d bytes but msgSize in data is %d\n",
			nRcv, msgSize);
	}
	printf("Received %d bytes, ACKing msg # %d\n", nRcv, msgSeq);
	//  send ACK msg
	sendto(sockfd, (const char *)&msgSeq, 4, 
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
			len); 
	
	return 0; 
} 
