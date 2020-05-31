#include "fileSender.h"
#include "cmdLineOpts.h"
#include "threading.h"
#include <pthread.h>
#include <sys/types.h>
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

static int killAckRdr;
// timespec is a structure holding an interval broken down into seconds and nanoseconds.
static struct timespec ackTimeout;
static struct thread_info_t ackThreadInfo;
const int ACK_TIMEOUT_SEC = 1;

static char msgBuf[MAX_UDP_MSG_BYTES];
static uint32_t nBytesRem;

void *ackReaderThread(void *data);

void InitFileSender()
{
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

    // create and start the ACK reader thread
    pthread_cond_init(&ackThreadInfo.condition, NULL);
    pthread_mutex_init(&ackThreadInfo.mutex, NULL);
    const int lock_rv = pthread_mutex_lock(&ackThreadInfo.mutex);
    if (lock_rv)
    {
        error_pthread_mutex_lock(lock_rv);
        exit(EXIT_FAILURE); 
    } 
    const int create_rv = pthread_create(&ackThreadInfo.id, NULL, 
            &ackReaderThread, (void *) &ackThreadInfo);
    if (create_rv)
    {
        error_pthread_create(create_rv);
        exit(EXIT_FAILURE); 
    }
    const int unlock_rv = pthread_mutex_unlock(&ackThreadInfo.mutex);
    if (unlock_rv)
    {
        error_pthread_mutex_unlock(unlock_rv);
        exit(EXIT_FAILURE); 
    }
}

int SendFile(char fileBuf[], int nFileBytes)
{
    struct FileXfrHeader_t *udpHdr;
    udpHdr = malloc(FILE_XFR_HDR_SIZE);
    uint32_t fileBufOffs = 0;
    //  each msg contains 4 byte msg len + 4 byte total size +
    //                    4 byte msg seq + 4 byte tot msgs in file
    int nMsgsData = (nFileBytes - 1) / MAX_UDP_MSG_BYTES + 1;
    int nOverhead = 12 * nMsgsData;
    // recalc nMsgsTot including msg overhead
    nBytesRem = nMsgsData * MAX_UDP_MSG_BYTES + nOverhead;
    udpHdr->nMsgsTot = (nBytesRem - 1) / MAX_UDP_MSG_BYTES + 1;
    udpHdr->msgSeq = 0;
    udpHdr->totalSize = nFileBytes;
    uint32_t nDataBytes;

    while (nBytesRem > 0)
    {
        if (nBytesRem > MAX_UDP_MSG_BYTES)
        {
            udpHdr->msgSize = MAX_UDP_MSG_BYTES;
        }
        else
        {
           udpHdr->msgSize = nBytesRem;
        }
        nDataBytes = udpHdr->msgSize - FILE_XFR_HDR_SIZE;
        int msgOk = 0;
        //  tbd start retry loop
        size_t bufSize = 10;
        // char *nsendstr = (char*)malloc(bufSize);
        // printf("Press Enter to send another message");
        // size_t len = getline(&nsendstr, &bufSize, stdin);
        //free(nsendstr);
        int nRetries = 0;
        while (!msgOk)
        {
            memcpy(msgBuf, udpHdr, FILE_XFR_HDR_SIZE);
            memcpy(&msgBuf[FILE_XFR_HDR_SIZE], &fileBuf[fileBufOffs], nDataBytes);

            // mutex_unlock() to enable reading of ACK
            const int unlock_rv = pthread_mutex_unlock(&ackThreadInfo.mutex);
            if (unlock_rv)
            {
                error_pthread_mutex_unlock(unlock_rv);
            }
            // The clock_gettime system call has higher precision than its successor the gettimeofday().
            // It has the ability to request specific clocks using the clock id.
            // It fills in a timespec structure with the seconds and nanosecond count of the time since the Epoch (00:00 1 January, 1970 UTC).
            // CLOCK_REALTIME argument represents a system-wide real-time clock. This clock is supported by all implementations and returns the number of seconds and nanoseconds since the Epoch.
            const int gettime_rv = clock_gettime(CLOCK_REALTIME, &ackTimeout);
            if (gettime_rv)
            {
                error_clock_gettime(gettime_rv);
            }
            ackTimeout.tv_sec += ACK_TIMEOUT_SEC;
            ackTimeout.tv_nsec = 0;
            //  wait for reader thread to unlock mutex or timeout
            sendto(sockfd, (const char *)msgBuf, udpHdr->msgSize, 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
                sizeof(servaddr));
            const int timed_wait_rv =
                    pthread_cond_timedwait(&(ackThreadInfo.condition),
                                &ackThreadInfo.mutex, &ackTimeout);   
            if (timed_wait_rv)
            {
                error_pthread_cond_timedwait(timed_wait_rv);
                exit(EXIT_FAILURE); 
            }
            // const int join_rv = pthread_join(ackThreadInfo.id, NULL);
            // if (join_rv)
            // {
            //     error_pthread_join(join_rv);
            // }
            msgOk = 1;
        }       // end retry loop
        fileBufOffs += nDataBytes;
        nBytesRem -= udpHdr->msgSize;
        udpHdr->msgSeq++;
    }       // end while (nBytesRem > 0)  
}

void CloseUdp()
{
    close(sockfd);
}

// This is the thread that will be called by pthread_create() and it will
//   be executed by the new thread.
void *ackReaderThread(void *data)
{
    // We know that the input data pointer is pointing to a thread_info_t so we are casting it to the right type.
    struct thread_info_t *thread_info = (struct thread_info_t *) data;

    // We block this thread trying to lock the mutex, this way we will make sure that the parent thread had enough time to call pthread_cond_timedwait().
    // When the parent thread calls pthread_cond_timedwait() it will unlock the mutex and this thread will be able to proceed.
    const int lock_rv = pthread_mutex_lock(&(thread_info->mutex));
    if (lock_rv)
    {
        error_pthread_mutex_lock(lock_rv);
    }
    int oldtype;
    // The pthread_setcanceltype() sets the cancelability type of the calling thread to the value given in type.
    // The previous cancelability type of the thread is returned in the buffer pointed to by oldtype.
    // The argument PTHREAD_CANCEL_ASYNCHRONOUS means that the thread can be canceled at any time.
    const int setcanceltype_rv = pthread_setcanceltype(
                        PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    if (setcanceltype_rv)
    {
        error_pthread_setcanceltype(setcanceltype_rv);
    }

    //  we expect an ACK msg from server
    //  this thread blocks on reads of that ACK and unlocks the sender
    //  thread to continue
    // struct sockaddr *ackAddr = malloc(sizeof(struct sockaddr));
    struct sockaddr *ackAddr = malloc(200);
    int ackbuf[1024];
    int ackLen;
    killAckRdr = 0;
    while (!killAckRdr)
    {
        int nRcv = recv(sockfd, (char *)&ackbuf, 1024, 0); 
        // int nRcv = recvfrom(sockfd, (char *)&ackbuf, sizeof(ackbuf), 
        //             0, ackAddr, &ackLen); 
        if (nRcv < 0)
        {
            if (errno == EAGAIN)
            {
                // printf("Timeout waiting for ACK\n");
            }
            else
            {
                printf("Error receiving ACK, errno= %d\n", errno);
                break;
            }
        }
        else
        {
            uint32_t ack = *(uint32_t*)&ackbuf;
            // The pthread_cond_signal() call unblocks at least one of the threads that are blocked on the specified condition variable cond (if any threads are blocked on cond).
            const int signal_rv = pthread_cond_signal(&(thread_info->condition));
            if (signal_rv)
            {
                error_pthread_cond_signal(signal_rv);
            }
        
            // The pthread_mutex_unlock() function shall release the mutex object referenced by mutex.
            const int unlock_rv = pthread_mutex_unlock(&(thread_info->mutex));
            if (unlock_rv)
            {
                error_pthread_mutex_unlock(unlock_rv);
            }
        }
    }       // end while !kill
    free(ackAddr);  // add bpoint here for debug
    return NULL;
}
