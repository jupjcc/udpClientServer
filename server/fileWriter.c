#include "udpMsgDef.h"
#include "fileWriter.h"
#include "threading.h"
#include "cmdLineOpts.h"
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

//  collect file data and write the file when complete
static char *udpFillBuf0 = 0;
static char *udpFillBuf1 = 0;
static int bufSel = 0;
static uint32_t fbOffs = 0;
static uint32_t fileSize;

static struct thread_info_t fwThreadInfo;
static void *fileWriterThread(void *data);

void InitFileWriter()
{
    // create and start the fw thread
    pthread_cond_init(&fwThreadInfo.condition, NULL);
    pthread_mutex_init(&fwThreadInfo.mutex, NULL);
    const int lock_rv = pthread_mutex_lock(&fwThreadInfo.mutex);
    if (lock_rv)
    {
        error_pthread_mutex_lock(lock_rv);
        exit(EXIT_FAILURE); 
    } 
    const int create_rv = pthread_create(&fwThreadInfo.id, NULL, 
            &fileWriterThread, (void *) &fwThreadInfo);
    if (create_rv)
    {
        error_pthread_create(create_rv);
        exit(EXIT_FAILURE); 
    }
    const int unlock_rv = pthread_mutex_unlock(&fwThreadInfo.mutex);
    if (unlock_rv)
    {
        error_pthread_mutex_unlock(unlock_rv);
        exit(EXIT_FAILURE); 
    }
    udpFillBuf0 = malloc(GetBufSize());
    udpFillBuf1 = malloc(GetBufSize());
}

void AddToFwBuf(char rcvBuf[])
{
    char **udpFillBuf = (bufSel==0)? (char**)&udpFillBuf0 : (char**)&udpFillBuf1;
    struct FileXfrHeader_t *udpHdr = (struct FileXfrHeader_t*)rcvBuf;
    // printf("udpFillBuf addr is 0x%12" PRIx64 "\n", (uint64_t)udpFillBuf);
    uint32_t msgDataSize = *(uint32_t*)rcvBuf - FILE_XFR_HDR_SIZE;
    if (udpHdr->pktSeq == 0)
    {
        fbOffs = 0;
        // 20200531: jcc allocating file buffer to correct file size for each
        //           file was unreliable/buggy and complicated, + overhead of
        //           freeing/reallocating added cpu overhead.
        //           So decided to statically allocate and provide a command
        //           line parameter for buffer size in case unexpectedly large
        //           files need to be transferred
    }
    // // for loop for debugging
    // int i;
    // for (i=0; i<msgDataSize; i++)
    // {
    //     (*udpFillBuf)[i] = rcvBuf[i+12];
    // }
    memcpy(*udpFillBuf+fbOffs, &rcvBuf[FILE_XFR_HDR_SIZE], msgDataSize);
    fbOffs += msgDataSize;
    // printf("pktSeq = %d, nPktsTot = %d, fbOffs = %d, fileSize= %d\n",
    //     udpHdr->pktSeq, udpHdr->nPktsTot, fbOffs, udpHdr->totalSize);
    if (udpHdr->pktSeq >= (udpHdr->nPktsTot - 1))
    {
        fwThreadInfo.fileBuf = *udpFillBuf;
        fwThreadInfo.nBytes = udpHdr->nBytesMsg;
        //  signal file writer thread that data is ready
        const int signal_rv = pthread_cond_signal(&fwThreadInfo.condition);
        if (signal_rv)
        {
            error_pthread_cond_signal(signal_rv);
        }
        bufSel = bufSel? 0: 1;
    }
}

// Thread that writes to output file.
void *fileWriterThread(void *data)
{
    // We know that the input data pointer is pointing to a thread_info_t so we are casting it to the right type.
    struct thread_info_t *thread_info = (struct thread_info_t *) data;

    // We block this thread trying to lock the mutex, this way we will make sure that the parent thread had enough time to call pthread_cond_timedwait().
    // When the parent thread calls pthread_cond_timedwait() it will unlock the mutex and this thread will be able to proceed.
    const int lock_rv = pthread_mutex_lock(&(thread_info->mutex));
    if (lock_rv)
    {
        error_pthread_mutex_lock(lock_rv);
        printf("File writer error getting lock\n");
        exit(EXIT_FAILURE);
    }
    // int oldtype;
    // // The pthread_setcanceltype() sets the cancelability type of the calling thread to the value given in type.
    // // The previous cancelability type of the thread is returned in the buffer pointed to by oldtype.
    // // The argument PTHREAD_CANCEL_ASYNCHRONOUS means that the thread can be canceled at any time.
    // const int setcanceltype_rv = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    // if (setcanceltype_rv)
    // {
    //     error_pthread_setcanceltype(setcanceltype_rv);
    // }
    while (1)
    {
        //  we'll be signaled when a file buffer is full
        const int wait_rv =
                    pthread_cond_wait(&fwThreadInfo.condition,
                                &fwThreadInfo.mutex);   
        if (wait_rv)
        {
            error_pthread_cond_wait(wait_rv);
            printf("File writer error waiting for file buffer full\n");
            exit(EXIT_FAILURE);
        }
        //  tbd create file name based on date/time and write the buffer to it
        time_t rawtime;
        char fileNameBuf[255];
        char fileName[255];
        time (&rawtime);
        struct tm *info;
        info = localtime(&rawtime);
        strftime(fileNameBuf, sizeof(fileName), "%Y%m%d_%H%M%S", info);
        sprintf(fileName, "/home/jupjcc/udpcode/data/rcv/%s.bin", fileNameBuf);

        //   write the file
        FILE *fptr;
        if ((fptr = fopen(fileName, "wb")) == NULL)
        {
            printf("Error (errno=%d) creating file:\n %s\n", errno, fileName);
        }
        fwrite(thread_info->fileBuf, thread_info->nBytes,1, fptr);
        fclose(fptr);
        printf("File created:\n%s\n", fileName);
        const int unlock_rv = pthread_mutex_unlock(&fwThreadInfo.mutex);
        if (unlock_rv)
        {
            error_pthread_mutex_unlock(unlock_rv);
        }
    }
    return NULL;
}
