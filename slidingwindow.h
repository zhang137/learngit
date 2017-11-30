#ifndef SLIDINGWINDOW_H_INCLUDED
#define SLIDINGWINDOW_H_INCLUDED

#include <string.h>
#include "sysglobalparam.h"

#define BUFSIZE MTU

class SlidingWindow
{
public:
    SlidingWindow():m_pstrWindow( new char[WINSIZE]),m_iIndex(0) { memset(m_pstrWindow,0,WINSIZE);}
    ~SlidingWindow(){ delete m_pstrWindow;}
    int saveDatagram(char *msg,int index);
    int getDatagram(char **msg,int index);
    int delDatagram(int index);
    int unpackMsg(char *packedMsg);
    bool isFull();
    bool isEmpty();
private:
    char *m_pstrWindow;
    unsigned short m_iIndex;
};



#endif // SLIDINGWINDOW_H_INCLUDED
