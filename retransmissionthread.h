#ifndef RETRANSMISSIONTHREAD_H_INCLUDED
#define RETRANSMISSIONTHREAD_H_INCLUDED
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include "message.h"

#define SA struct sockaddr

static pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

class RetransQueue
{
public:
    RetransQueue():mutex(PTHREAD_MUTEX_INITIALIZER),front(0),rear(0),retrans_finish(false)
    {
        memset(&retransAddr,0,sizeof(retransAddr));
    }
    ~RetransQueue()  { pthread_mutex_destroy(&mutex); }

    void pop(int *po);
    //int  popOne(int x);
    void push(int index);
    void setRetransFinish(bool condition);
    void setRetransAddr(struct sockaddr_in *remote);
    void setSendTimeoutInterval(int interval[], int size);
    int  getCapacity()const;
    int  getTop() const;
    inline bool isEmpty() const;
    inline bool isFull()const;
public:

    bool retrans_finish;
private:
    pthread_mutex_t mutex;
    struct sockaddr_in retransAddr;
    int front;
    int rear;
    int capacity[9];
};


void *retransThreadFunc(void *arg);


#endif // RETRANSMISSIONTHREAD_H_INCLUDED
