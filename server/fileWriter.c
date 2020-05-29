#include "fileWriter.h"
#include "threading.h"
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
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
}

void AddToFwBuf(char rcvBuf[])
{
    char **udpFillBuf = (bufSel==0)? (char**)&udpFillBuf0 : (char**)&udpFillBuf1;
    printf("udpFillBuf addr is 0x%12" PRIx64 "\n", (uint64_t)udpFillBuf);
    uint32_t msgDataSize = *(uint32_t*)rcvBuf - 12;
    uint32_t msgSeq = *(uint32_t*)&rcvBuf[4];
    uint32_t nMsgsTot = *(uint32_t*)&rcvBuf[8];
    if (msgSeq == 0)
    {
        if (*udpFillBuf != 0)
        {
            free(udpFillBuf);
        }
        fbOffs = 0;
        //  allocate write buffer for completed file
        fileSize = msgDataSize + 
                    ((nMsgsTot - 1) * (MAX_UDP_PKT_LEN - 12));
        *udpFillBuf = malloc(fileSize);
    }
    // // for loop for debugging
    // int i;
    // for (i=0; i<msgDataSize; i++)
    // {
    //     (*udpFillBuf)[i] = rcvBuf[i+12];
    // }
    memcpy(*udpFillBuf+fbOffs, &rcvBuf[12], msgDataSize);
    fbOffs += msgDataSize;
    if (fbOffs >= fileSize)
    {
        //  signal file writer thread that data is ready
        const int wait_rv =
                    pthread_cond_wait(&fwThreadInfo.condition,
                                &fwThreadInfo.mutex);   
        if (wait_rv)
        {
            error_pthread_cond_wait(wait_rv);
        }
    }
}

    // void writeFile(char *buf, uint32_t nBytes)
    // {
    //     // create file named by date and time
    //     time_t rawtime;
    //     char buffer [255];
    //     time (&rawtime);

    //     sprintf(buffer,"%s",ctime(&rawtime) );
    //     printf("%s",buffer);
    //     fopen(buffer,"w");
    //     FILE *fptr;
    //     int dbg=1;
    // }

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
    }
    int oldtype;
    // The pthread_setcanceltype() sets the cancelability type of the calling thread to the value given in type.
    // The previous cancelability type of the thread is returned in the buffer pointed to by oldtype.
    // The argument PTHREAD_CANCEL_ASYNCHRONOUS means that the thread can be canceled at any time.
    const int setcanceltype_rv = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    if (setcanceltype_rv)
    {
        error_pthread_setcanceltype(setcanceltype_rv);
    }

    free(thread_info->fileBuf);  // add bpoint here for debug
    return NULL;
}
