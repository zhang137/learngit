#include <cstring>
#include <unistd.h>
#include "message.h"
#include "slidingwindow.h"


int SlidingWindow::saveDatagram(char *msg,int index)
{
    if(msg == NULL || index < 0 || index >= PACKETNUM )
        return -1;
    char *temp = m_pstrWindow + MTU * index;
    if(temp < &m_pstrWindow[WINSIZE])
    {
        memcpy(temp,msg,MTU);
        this->m_iIndex += index;
    }
    temp = NULL;
    if(!(this->m_iIndex ^ DATA_READY))
        return 1;
    return 0;

}

int SlidingWindow::getDatagram(char **msg,int index)
{
    if(msg == NULL || *msg == NULL) return -1;
    char *temp = m_pstrWindow + MTU * index;
    memcpy(*msg,temp,BUFSIZE);
    temp = NULL;
    return 0;
}

int SlidingWindow::delDatagram(int index)
{
   if(index < 0 || index >= PACKETNUM )
        return -1;
   char *temp = m_pstrWindow + MTU * index;
   memset(temp,0,MTU);
   temp = NULL;
   this->m_iIndex -= index;
   return 0;
}

bool SlidingWindow::isFull()
{
    return (!(this->m_iIndex ^ DATA_READY));
}

bool SlidingWindow::isEmpty()
{
    return (this->m_iIndex == 0);
}



int SlidingWindow::unpackMsg(char *packedMsg)
{

}




