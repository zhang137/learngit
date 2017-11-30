#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "filetransfer.h"
#include "message.h"

#define SA struct sockaddr

int FileOperation::fileOpenToSend(char *filePath,u_int64_t breakpoint)
{
    if(filePath == NULL || breakpoint < 0) return -1;

    m_file = fopen(filePath,"rb");
    if(m_file == NULL ||  fseek(m_file,breakpoint,SEEK_SET) < 0)
        return -1;
    return 0;
}

int FileOperation::fileOpenToRecv(char *filePath,u_int64_t breakpoint)
{
    if(filePath == NULL || breakpoint < 0)
        return -1;

    m_file = fopen(filePath,"ab+");
    if(m_file == NULL ||  fseek(m_file,breakpoint,SEEK_SET) < 0)
        return -1;
    return 0;
}

int FileOperation::readFile(char **msg,int size)
{
    int rsize = 0;
    rsize = fread(*msg,(size_t)1,size,m_file);

    if(rsize == 0 && feof(m_file)) return 0;
    if(ferror(m_file)) return -1;

    m_uliBreakpoint = ftell(m_file);

    return rsize;
}

int FileOperation::writeFile(char *msg,int size)
{
    int rsize = 0;
    rsize = fwrite(msg,(size_t)1,size,m_file);

    if(ferror(m_file)) return -1;
    m_uliBreakpoint = ftell(m_file);

    return rsize;
}

void FileOperation::setClientPort(u_int16_t port)
{
    m_port = port;
}

int FileOperation::setClientConnect()
{
    remote.sin_addr.s_addr = htonl(INADDR_ANY);   //对端IP
    remote.sin_port = htons(m_port);  ///
    remote.sin_family = AF_INET;

    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq socket set error");
        return -1;
    }
    bool flags = 1;
    int SEND_BUFSIZE = 250250;
    int RECV_BUFSIZE = 250250;
    if(setsockopt(sockfd,SOL_SOCKET ,SO_REUSEADDR,(char *) & flags,sizeof(bool)) < 0
        || setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,(char *) & SEND_BUFSIZE,sizeof(int)) < 0
        || setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(char *) & RECV_BUFSIZE,sizeof(int)) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq socket config error");
        close(sockfd);
        return -1;
    }

    int flag = 0;
    if( ioctl(sockfd,FIONBIO,1) < 0 || (flag = fcntl(sockfd,F_GETFD,0)) < 0
        || fcntl(sockfd,F_SETFD,flag | O_NONBLOCK) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq socket set unblock error");
        close(sockfd);
        return -1;
    }

    if(connect(sockfd,(SA *)&remote,sizeof(remote)) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq socket set unblock error");
        close(sockfd);
        return -1;
    }


    return sockfd;
}

std::string FileOperation::getMsg(int sockfd,struct sockaddr_in *remote)
{
    socklen_t len = sizeof(*remote);
    memset(recvbuf,0,BUFSIZE);
    while(recvfrom(sockfd,recvbuf,BUFSIZE,0,(SA*)remote,&len) < 0)
    {
        if(errno == EINTR)
            continue;
        if(errno != EWOULDBLOCK || errno != EAGAIN)
            return std::string();
    }
    return std::string(recvbuf);
}

int FileOperation::sendMsg(std::string msg)
{
    memcpy(sendbuf,msg.c_str(),msg.size());
    while( sendto(sockfd,sendbuf,BUFSIZE,0,(SA*)&remote,sizeof(remote)) < 0)
    {
        if(errno == EINTR ){
            continue;
        }
        else if (errno != EAGAIN || errno != EWOULDBLOCK) {
            //logAtelier("ERROR: UDPServerPlugin::sendMsg errno:%d error:%s.",errno,strerr(errno));
            return -1;
        }
    }
    memset(sendbuf,0,BUFSIZE);
    return 0;
}

int FileOperation::getFtell()
{
    return m_uliBreakpoint;
}

int FileOperation::getFileSize()
{
    int size  = 0;
    if(m_file != NULL)
    {
        fseek(m_file,SEEK_END,0);
        size = ftell(m_file);
        fseek(m_file,SEEK_SET,m_uliBreakpoint);
    }
    return size;
}

void FileOperation::setFileNo(u_int32_t fileNO)
{
    m_fileno = fileNO;
}

int FileOperation::getFileNo()
{
    return m_fileno;
}

int  FileOperation::setSocketfd(int sockfd)
{
    this->sockfd = sockfd;
}


//
//int FileOperation::packagingMsg(char *unpackMsg,int size,int sendId,char **packedMsg)
//{
//
//}
//














