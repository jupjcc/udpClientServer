#include "threading.h"
#include <stdio.h>
#include <errno.h>

void error_pthread_mutex_unlock(const int unlock_rv)
{
    fprintf(stderr, "Failed to unlock mutex.\n");
    switch (unlock_rv)
    {
        case EINVAL:
            fprintf(stderr, "The value specified by mutex does not refer to an initialized mutex object.\n");
            break;
        case EAGAIN:
            fprintf(stderr, "The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.\n");
            break;
        case EPERM:
            fprintf(stderr, "The current thread does not own the mutex.\n");
            break;
        default:
            break;
    }
}
 
void error_pthread_mutex_lock(const int lock_rv)
{
    fprintf(stderr, "Failed to lock mutex.\n");
    switch (lock_rv)
    {
        case EINVAL:
            fprintf(stderr, "The value specified by mutex does not refer to an initialized mutex object or the mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than the mutex's current priority ceiling.\n");
            break;
        case EAGAIN:
            fprintf(stderr, "The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.\n");
            break;
        case EDEADLK:
            fprintf(stderr, "A deadlock condition was detected or the current thread already owns the mutex.\n");
            break;
        default:
            break;
    }
}
 
void error_pthread_cond_signal(const int signal_rv)
{
    fprintf(stderr, "Could not signal.\n");
    if (signal_rv == EINVAL)
    {
        fprintf(stderr, "The value cond does not refer to an initialised condition variable.\n");
    }
}
 
void error_pthread_setcanceltype(const int setcanceltype_rv)
{
    fprintf(stderr, "Could not change cancelability type of thread.\n");
    if (setcanceltype_rv == EINVAL)
    {
        fprintf(stderr, "Invalid value for type.\n");
    }
}
 
void error_pthread_create(const int create_rv)
{
    fprintf(stderr, "Could not create thread.\n");
    switch (create_rv)
    {
        case EAGAIN:
            fprintf(stderr, "Insufficient resources to create another thread or a system-imposed limit on the number of threads was encountered.\n");
            break;
        case EINVAL:
            fprintf(stderr, "Invalid settings in attr.\n");
            break;
        case EPERM:
            fprintf(stderr, "No permission to set the scheduling policy and parameters specified in attr.\n");
            break;
        default:
            break;
    }
}
 
void error_pthread_cond_wait(const int wait_rv)
{
    fprintf(stderr, "Conditional wait, failed.\n");
    switch (wait_rv)
    {
        case ETIMEDOUT:
            fprintf(stderr, "The time specified by abstime to pthread_cond_wait() has passed.\n");
            break;
        case EINVAL:
            fprintf(stderr, "The value specified by abstime, cond or mutex is invalid.\n");
            break;
        case EPERM:
            fprintf(stderr, "The mutex was not owned by the current thread at the time of the call.\n");
            break;
        default:
            break;
    }
}
 
void error_pthread_join(const int join_rv)
{
 
    fprintf(stderr, "Could not join thread.\n");
    switch (join_rv)
    {
        case EINVAL:
            fprintf(stderr, "The implementation has detected that the value specified by thread does not refer to a joinable thread.\n");
            break;
        case ESRCH:
            fprintf(stderr, "No thread could be found corresponding to that specified by the given thread ID.\n");
            break;
        case EDEADLK:
            fprintf(stderr, "A deadlock was detected or the value of thread specifies the calling thread.\n");
            break;
        default:
            break;
    }
}
void error_clock_gettime(const int gettime_rv)
{
    fprintf(stderr, "Could not get time from clock.\n");
    switch (gettime_rv)
    {
        case EFAULT:
            fprintf(stderr, "tp points outside the accessible address space.\n");
            break;
        case EINVAL:
            fprintf(stderr, "The clk_id specified is not supported on this system.\n");
            break;
        case EPERM:
            fprintf(stderr, "clock_settime() does not have permission to set the clock indicated.\n");
            break;
        default:
            break;
    }
}
 