#include "retransmissionthread.h"
#include "sysglobalparam.h"

void *retransThreadFunc(void *arg)
{
    pthread_detach(pthread_self());
    RetransQueue *queue = (RetransQueue*)arg;

    while(!queue->retrans_finish)
    {
        pthread_mutex_lock(&g_mutex);
        while(queue->getCapacity() == 0)
        {
            printf("cond wait start!\n");
            pthread_cond_wait(&thread_cond,&g_mutex);
            printf("cond wait stop!\n");
        }
        pthread_mutex_unlock(&g_mutex);
        printf("retrans sequence thread\n");
        sleep(2);

    }
    printf("retransmission stop!\n");
    pthread_exit(NULL);
}

void RetransQueue::pop(int *po)
{
    pthread_mutex_lock(&mutex);
    if(isEmpty())
    {
        pthread_mutex_unlock(&mutex);
        return;
    }
    *po = capacity[rear];
    rear = (rear + 1) % (PACKETNUM+1);

    pthread_mutex_unlock(&mutex);
}

//int RetransQueue::popOne(int x)
//{
//    pthread_mutex_lock(&mutex);
//    int i = 0;
//    if(isEmpty())
//    {
//         pthread_mutex_unlock(&mutex);
//         return -1;
//    }
//    if(capacity[rear] == x)
//    {
//        rear = (rear + 1) % (PACKETNUM+1);
//        return 0;
//    }
//    if(front > rear)
//    {
//        i = rear;
//        for(i = rear+1; i != front && capacity[i] != x; i= (i+1) %(PACKETNUM +1)) ;
//        if(i == front) return -1;
//
//    }
//    else
//    {
//
//    }
//
//    pthread_mutex_unlock(&mutex);
//    return 0;
//}


void RetransQueue::push(int index)
{
    pthread_mutex_lock(&mutex);
    if(isFull())
    {
        pthread_mutex_unlock(&mutex);
        return;
    }
    capacity[front] = index;
    front = (front + 1) % (PACKETNUM+1);
    pthread_mutex_unlock(&mutex);
}

inline bool RetransQueue::isEmpty() const
{
    return (rear == front);
}

inline bool RetransQueue::isFull() const
{
    return ((front+1) == rear);
}

int RetransQueue::getCapacity() const
{
     return front>= rear ? front - rear : front+1+PACKETNUM-rear;
}

int RetransQueue::getTop() const
{
    return capacity[rear];
}


void RetransQueue::setRetransFinish(bool condition)
{
    this->retrans_finish = condition;
}

void RetransQueue::setRetransAddr(struct sockaddr_in *remote)
{
    memcpy(&retransAddr,remote,sizeof(retransAddr));
}













