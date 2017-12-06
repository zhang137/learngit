#ifndef FILETRANSFER_H_INCLUDED
#define FILETRANSFER_H_INCLUDED

#include "types.h"
#include "slidingwindow.h"
#include <netinet/in.h>

#include <string>

using std::string;
#define BUFSIZE MTU


class FileOperation
{
public:
    FileOperation():m_file(0),m_uliBreakpoint(0),sWindow(new SlidingWindow),
                    m_threadCond(PTHREAD_COND_INITIALIZER),m_mutex(PTHREAD_MUTEX_INITIALIZER){ }
    ~FileOperation() { delete sWindow; fclose(m_file); }

    int  fileOpenToSend(char *filePath,u_int64_t breakpoint);
    int  fileOpenToRecv(char *filePath,u_int64_t breakpoint);
    int  readFile(char **msg,int size);
    int  writeFile(char *msg,int size);
    void setClientPort(u_int16_t port);
    void setFileNo(u_int32_t fileNO);
    int  setClientConnect();
    int  packagingMsg(char *unpackMsg,int size,char **packedMsg);
    int  sendMsg(std::string msg);
    int  setSocketfd(int sockfd);
    std::string getMsg(int sockfd,struct sockaddr_in *remote);
public:
    int getFtell();
    int getFileSize();
    int getFileNo();
public:
    SlidingWindow *sWindow;
    pthread_cond_t   m_threadCond;
    pthread_mutex_t  m_mutex;
private:
    u_int64_t m_uliBreakpoint;
    u_int32_t m_fileno;
    u_int16_t m_sByteNum;
    u_int16_t m_port;
    int   sockfd;
    FILE *m_file;
    char recvbuf[BUFSIZE];
    char sendbuf[BUFSIZE];
    struct sockaddr_in remote;
};




#endif // FILETRANSFER_H_INCLUDED
