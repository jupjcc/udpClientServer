#ifndef THREADING_H
#define THREADING_H

#include <stdint.h>
#include <pthread.h>

    struct thread_info_t
    {
        // Used to identify a thread.
        pthread_t id;
        // A condition is a synchronization device that allows threads to suspend execution and relinquish the processors until some predicate on shared data is satisfied.
        // The basic operations on conditions are: signal the condition (when the predicate becomes true), and wait for the condition, suspending the thread execution until another thread signals the condition.
        pthread_cond_t condition;
        // A mutex is a MUTual EXclusion device, and is useful for protecting shared data structures from concurrent modifications, and implementing critical sections and monitors.
        // A mutex has two possible states: unlocked (not owned by any thread), and locked (owned by one thread).
        // A mutex can never be owned by two different threads simultaneously.
        // A thread attempting to lock a mutex that is already locked by another thread is suspended until the owning thread unlocks the mutex first.
        pthread_mutex_t mutex;
        char *fileBuf;
        uint32_t nBytes;
    };

    void error_pthread_mutex_unlock(const int unlock_rv);
    void error_pthread_mutex_lock(const int lock_rv);
    void error_pthread_cond_signal(const int signal_rv);
    void error_pthread_setcanceltype(const int setcanceltype_rv);
    void error_pthread_create(const int create_rv);
    void error_pthread_cond_wait(const int wait_rv);
    void error_pthread_join(const int join_rv);
    void error_clock_gettime(const int gettime_rv);

#endif 