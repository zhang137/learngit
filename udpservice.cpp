#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <time.h>
#include "udpservice.h"

#define SA struct sockaddr

//namespace atelier_plugin
//{

//AtelierPlugin* CreateUDPService()
//{
//	return new UDPService();
//}

void receive_file_thread(void *arg)
{
    int clientfd = *(int *)arg;

}

void *sendFileThread(void *arg)
{
    pthread_detach(pthread_self());

    FileOperation *fOption = (FileOperation *)arg;
    //RetransQueue *rQueue= new RetransQueue;
    int sendNum = 0,sendCount = 0,fSize = fOption->getFileSize();
    fOption->setClientConnect();
    char *filebuf = new char[FILEBUF];
    memset(filebuf,0,FILEBUF);
    char *sendbuf = new char[BUFSIZE];
    memset(sendbuf,0,BUFSIZE);


    MESSHEADER head;
    head.COMMAND_ID  = FILE_PART_REQ;
    head.SEQUENCE_ID = sequence_id++;
    head.TOTAL_LENGTH = BUFSIZE;
    head.VERSION = 0;
    if(fSize % FILEBUF == 0)
        head.BLOCK_TOTAL = fSize / FILEBUF;
    else
        head.BLOCK_TOTAL = fSize / FILEBUF + 1;
    head.VERSION = 0;

    FILE_PART_REG file_part_reg;
//    pthread_t thread;
//    pthread_create(&thread,NULL,retransThreadFunc,(void*)rQueue);

    while(fOption->getFtell() < fSize)
    {
        while(sendNum < 8)
        {
            if(fOption->readFile(&filebuf,FILEBUF) <= 0)
            {
                break;
            }
            head.MESSAGE_ID = message_id++;
            head.BLOCK_INDEX = sendCount++;
            file_part_reg.REG_FRAGMENT_NUM = sendNum;
            file_part_reg.FILE_NO = fOption->getFileNo();

            memcpy(sendbuf,&head,sizeof(head));
            memcpy(sendbuf + sizeof(head),&file_part_reg,sizeof(file_part_reg));
            memcpy(sendbuf + sizeof(head) + sizeof(file_part_reg),filebuf,FILEBUF);

            fOption->sWindow->saveDatagram(sendbuf,sendNum);
            fOption->sendMsg(sendbuf);
            sendNum++;
        }
        pthread_mutex_lock(&fOption->m_mutex);
        while(!fOption->sWindow->isEmpty())
        {
            pthread_cond_wait(&fOption->m_threadCond,&fOption->m_mutex);
        }
        pthread_mutex_unlock(&fOption->m_mutex);
    }


}

void *recvFileThread(void *arg)
{
    pthread_detach(pthread_self());

    FileOperation *fOption = (FileOperation *)arg;
    //RetransQueue *rQueue= new RetransQueue;
    int sendNum = 0,sendCount = 0,fSize = fOption->getFileSize();
    fOption->setClientConnect();
    char *filebuf = new char[FILEBUF];
    memset(filebuf,0,FILEBUF);
    char *sendbuf = new char[BUFSIZE];
    memset(sendbuf,0,BUFSIZE);


    MESSHEADER head;
    head.COMMAND_ID  = FILE_PART_RESP;
    head.SEQUENCE_ID = sequence_id++;
    head.TOTAL_LENGTH = BUFSIZE;
    if(fSize % FILEBUF == 0)
        head.BLOCK_TOTAL = fSize / FILEBUF;
    else
        head.BLOCK_TOTAL = fSize / FILEBUF + 1;
    head.VERSION = 0;

    FILE_PART_REG file_part_reg;
//    pthread_t thread;
//    pthread_create(&thread,NULL,retransThreadFunc,(void*)rQueue);

    while(fOption->getFtell() < fSize)
    {
        while(sendNum < 8)
        {
            if(fOption->readFile(&filebuf,FILEBUF) <= 0)
            {
                break;
            }
            head.MESSAGE_ID = message_id++;
            file_part_reg.REG_FRAGMENT_NUM = sendNum;
            file_part_reg.FILE_NO = fOption->getFileNo();

            memcpy(sendbuf,&head,sizeof(head));
            memcpy(sendbuf + sizeof(head),&file_part_reg,sizeof(file_part_reg));
            memcpy(sendbuf + sizeof(head) + sizeof(file_part_reg),filebuf,FILEBUF);

            fOption->sWindow->saveDatagram(sendbuf,sendNum);
            fOption->sendMsg(sendbuf);
            sendNum++;
        }
        pthread_mutex_lock(&fOption->m_mutex);
        while(!fOption->sWindow->isEmpty())
        {
            pthread_cond_wait(&fOption->m_threadCond,&fOption->m_mutex);
        }
        pthread_mutex_unlock(&fOption->m_mutex);
    }


}




UDPService::UDPService()
{
    m_iMaxFdNum = 100;
    m_pEvents = new epoll_event[m_iMaxFdNum];
    INIT_LIST_HEAD(sendHead);
    INIT_LIST_HEAD(recvHead);
    memset(recvmsg,0,BUFSIZE);
    memset(sendmsg,0,BUFSIZE);
}

UDPService::~UDPService()
{
    struct list_head *entry = new  struct list_head;
    struct list_head *entry1 = new  struct list_head;
     list_for_each_safe(entry,entry1,recvHead)
    {
        list_del(entry);
    }

    list_for_each_safe(entry,entry1,sendHead)
    {
        list_del(entry);
    }

    delete [] m_pEvents;
}

//void UDPService::Initialize()
//{
//
//}

void UDPService::Close()
{
	close(m_sockfd);
	close(m_epollfd);
}
//void UDPService::Execute(std::string action, std::string callback_id,std::string args)
//{
//    logAtelier("HelloPlugin::Execute !!!  action: %s",action.c_str());
//	if(args.empty())  logAtelier("CONFIG: UDPServerPlugin parameters set empty!");
//
//    if(action == "Start")
//    {
//        initServer(args);
//		logAtelier("INFO: UDPServerPlugin::Execute Start PostMsgToJs message on port:%s.", args.c_str());
//
//		for(;;)
//		{
//            handleClientInfo();
//		}
//    }
//	else if(action == "Stop")
//	{
//		Close();
//		PostMsgToJs(callback_id, true, 1, "Service End!", false);
//	}
//}

void  UDPService::initServer(std::string &port)
{
    struct sockaddr_in local;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(port.c_str()));
    if(((m_sockfd = initSocket()) < 0) || (setUnBlocked() < 0 )
           || ( setSockoption() < 0) || (bind(m_sockfd,(SA *)&local,sizeof(local)) < 0))
    {
        //logAtelier("CONFIG: UDPServerPlugin::initServer socket config failed");
        return;
    }
     initEpoll();
    //logAtelier("INFO: UDPServerPlugin::Execute Start PostMsgToJs message on port:%s.", args.c_str());
}

int UDPService::initSocket()
{
    int fd = 0;
    if((fd = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        //logAtelier("CONFIG: UDPServerPlugin socket init failed");
        return -1;
    }
    return fd;
}

int  UDPService::initEpoll()
{
    m_epollfd = epoll_create(100);
    if(m_epollfd < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::initEpoll epoll_create error");
        return -1;
    }
    if( setEpollctl(m_epollfd,EPOLL_CTL_ADD,m_sockfd,EPOLLIN | EPOLLET) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::initEpoll epoll_ctl error");
        return -1;
    }
    return 0;
}

int UDPService::setEpollctl(int epollfd, int option,int sockfd,uint32_t events)
{
    struct epoll_event event;
    event.data.fd = sockfd;
    event.events = events;

    if(epoll_ctl(epollfd,option,sockfd,&event) < 0)
    {
        return -1;
    }
    return 0;
}


void UDPService::handleClientInfo()
{
    int nfds = epoll_wait(m_epollfd,m_pEvents,m_iMaxFdNum,2);
    if(nfds < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::handleClientInfo epoll_wait error");
        return;
    }
    for(int i = 0; i < nfds; i++)
    {

        if(m_pEvents[i].events & EPOLLIN)
        {
            struct sockaddr_in remote;
            int sockfd = m_pEvents[i].data.fd;
            std::string cmd = getMsg(sockfd,&remote);
            if(!cmd.empty())
            {
                parseMsg(&remote,cmd);
            }else
            {
                if( setEpollctl(m_epollfd,EPOLL_CTL_DEL,m_pEvents[i].data.fd,0) < 0)
                {
                    //logAtelier("CONFIG: UDPServerPlugin::handleClientInfo epoll_ctl error");
                    return ;
                }
            }
        }else if(m_pEvents[i].events & EPOLLIN)
        {

        }

    }
}

int UDPService::acceptClientReq(int port)
{

    struct sockaddr_in newClient;
    newClient.sin_addr.s_addr = htonl(INADDR_ANY);
    newClient.sin_port = htons(port);  ///
    newClient.sin_family = AF_INET;

    int clientfd = 0;
    if((clientfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq socket set error");
        return -1;
    }

    bool flags = 1;
    int SEND_BUFSIZE = 250250;
    int RECV_BUFSIZE = 250250;
    if(setsockopt(clientfd,SOL_SOCKET ,SO_REUSEADDR,(char *) & flags,sizeof(bool)) < 0
        || setsockopt(m_sockfd,SOL_SOCKET,SO_SNDBUF,(char *) & SEND_BUFSIZE,sizeof(int)) < 0
        || setsockopt(m_sockfd,SOL_SOCKET,SO_RCVBUF,(char *) & RECV_BUFSIZE,sizeof(int)) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq setsockopt error");
        close(clientfd);
        return -1;
    }

    int flag = 0;
    if( ioctl(clientfd,FIONBIO,1) < 0 || (flag = fcntl(clientfd,F_GETFD,0)) < 0
        || fcntl(clientfd,F_SETFD,flag | O_NONBLOCK) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq socket set unblock error");
        close(clientfd);
        return -1;
    }
    if(bind(clientfd,(SA*)&newClient,sizeof(newClient)) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq bind error");
        close(clientfd);
        return -1;
    }

    if( setEpollctl(m_epollfd,EPOLL_CTL_ADD,clientfd,EPOLLIN | EPOLLET) < 0)
    {
        //logAtelier("CONFIG: UDPServerPlugin::acceptClientReq epoll_ctl error");
        close(clientfd);
        return -1;
    }
    return clientfd;
}


std::string UDPService::getMsg(int sockfd,struct sockaddr_in *remote)
{
    socklen_t len = sizeof(*remote);
    memset(recvmsg,0,BUFSIZE);
    while(recvfrom(sockfd,recvmsg,BUFSIZE,0,(SA*)remote,&len) < 0)
    {
        if(errno == EINTR)
            continue;
        if(errno != EWOULDBLOCK || errno != EAGAIN)
            return std::string();
    }
    return std::string(recvmsg);
}

int UDPService::sendMsg(int sockfd,struct sockaddr_in *remote,std::string msg)
{
    memcpy(sendmsg,msg.c_str(),msg.size());
    while( sendto(sockfd,sendmsg,BUFSIZE,0,(SA*)remote,sizeof(remote)) < 0)
    {
        if(errno == EINTR ){
            continue;
        }
        else if (errno != EAGAIN || errno != EWOULDBLOCK) {
            //logAtelier("ERROR: UDPServerPlugin::sendMsg errno:%d error:%s.",errno,strerr(errno));
            return -1;
        }
    }
    memset(sendmsg,0,BUFSIZE);
    return 0;
}

int UDPService::parseMsg( struct sockaddr_in *remote, std::string &cmd)
{
    MESSHEADER *msgHead = (MESSHEADER *)(cmd.c_str());
    u_int8_t icmd = msgHead->COMMAND_ID;
    switch(icmd)
    {
    case CONTINUE_SEND_FILE_REQ://文件发送断点续传请求

    case CONTINUE_SEND_FILE_RESP://文件发送断点续传回应

    case CONTINUE_RECEIVE_FILE_REQ://文件接受断点续传请求

    case CONTINUE_RECEIVE_FILE_RESP://文件接受断点续传回应

    case READY_TO_CONTINUE_SEND_FILE_REQ://文件断点续传请求准备好

    case FILE_FINISH_REQ://文件接收完请求

    case DENY_SEND_FILE_REQ://拒绝发送文件请求

    case FILE_INTERRUPT_REQ://文件收发已中断

    case FILE_REQ://文件发送请求
         getFileInformation(msgHead,remote);
         break;
    case FILE_RESP://文件发送响应
         startRecvFile(msgHead);
         break;
    case ACCEPT_FILE_REQ://同意接收文件
         startSendFile(msgHead,remote);
         break;
    case DENY_FILE_REQ://拒绝接收文件请求

    case FILE_PART_REQ://文件片段请求
         getDatagram(msgHead);
         break;
    case FILE_PART_RESP://文件片段响应
         acknowledgeSegment(msgHead);
         break;
    case FILE_PART_RSEND_cREQ://重传文件片段请求
         rsendFileSegment(msgHead);
         break;
    case FILE_PART_RSEND_cRESP://重传文件片段响应
         ackRSendFileSegment(msgHead);
         break;
    default:
        return -1;
    }
    return 0;
}



int UDPService::sendFileReq(char *fileInfo)
{

}

int UDPService::continueToSendFileReq()
{

}

int UDPService::startSendFile(MESSHEADER *pHead,struct sockaddr_in *remote)
{
    RECEIVE_FILE *pCmdHead = (RECEIVE_FILE *)(pHead+1);
    uint16_t port = pCmdHead->PORT;
    uint32_t fileNO = pCmdHead->FILE_NO;

    RESPONSES_FILE respMsg;
    int respFileSize = strlen(pCmdHead->FILE_NAME);
    memcpy(respMsg.FILE_NAME,pCmdHead->FILE_NAME,respFileSize);
    respMsg.FILE_NO = pCmdHead->FILE_NO;

    MESSHEADER head;
    memset(&head,0,sizeof(head));
    head.COMMAND_ID = FILE_RESP;
    head.MESSAGE_ID = message_id++;
    head.SEQUENCE_ID = pHead->SEQUENCE_ID;
    head.VERSION = 0;
    uint32_t msglen = sizeof(MESSHEADER) + sizeof(respMsg);
    head.TOTAL_LENGTH = msglen;

    char *msg = new char[msglen];
    memset(msg,0,msglen);
    memcpy(msg,(char *)&head,sizeof(head));
    memcpy(msg+sizeof(head),(char *)&respMsg,sizeof(respMsg));

    sendMsg(m_sockfd,remote,msg);
    delete msg;

    char *fName = pCmdHead->FILE_NAME;

    FILEINFONODE fileinfo;
    INIT_LIST_HEAD(&fileinfo.list);
    fileinfo.FILE_NAME = pCmdHead->FILE_NAME;
    fileinfo.FILE_NO = pCmdHead->FILE_NO;
    fileinfo.FILE_IS_SEND = true;
    fileinfo.FILE_OPT = new FileOperation;
    if(fileinfo.FILE_OPT->fileOpenToSend(fName,0) < 0)
    {
        //logAtelier("ERROR: UDPServerPlugin::startSendFile open file error");
        delete (fileinfo.FILE_OPT);
        return -1;
    }
    list_add_tail(&fileinfo.list,recvHead);

    fileinfo.FILE_OPT->setFileNo(fileNO);
    fileinfo.FILE_OPT->setClientPort(port);

    pthread_t fileThread;
    if(pthread_create(&fileThread,NULL,sendFileThread,(void *)(fileinfo.FILE_OPT)) < 0)
    {

        return -1;
    }
    return 0;
}

int UDPService::startRecvFile(MESSHEADER *pHead)
{
    RESPONSES_FILE *pCmdHead = (RESPONSES_FILE *)(pHead+1);

    uint32_t fileNO = pCmdHead->FILE_NO;
    char *fName = pCmdHead->FILE_NAME;

    struct list_head *entry = new list_head;
    FILEINFONODE *curNode = NULL;
    list_for_each(entry,sendHead)
    {
        curNode = list_entry(entry,FILEINFONODE,list);
        if(pCmdHead->FILE_NO == curNode->FILE_NO)
        {
            break;
        }
    }

    if((&curNode->list) != head)
    {
        curNode->FILE_OPT = new FileOperation;
        if(fileinfo.FILE_OPT->fileOpenToRecv() < 0)
        {
            //logAtelier("ERROR: UDPServerPlugin::startRecvFile open file error");
            delete (fileinfo.FILE_OPT);
            return -1;
        }
        fileinfo.FILE_OPT->setFileNo(fileNO);
        fileinfo.FILE_OPT->setSocketfd(curNode->RECV_SOCKET);

        pthread_t fileThread;
        if(pthread_create(&fileThread,NULL,recvFileThread,(void *)(fileinfo.FILE_OPT)) < 0)
        {
            return -1;
        }
    }
    return 0;
}




void UDPService::acknowledgeSegment(MESSHEADER *cmd)
{
    FILE_PART_RESQ *ackMsg = (FILE_PART_RESQ *)(cmd + 1);
    struct list_head *entry = new list_head;
    FILEINFONODE *curNode = NULL;
    list_for_each(entry,sendHead)
    {
        curNode = list_entry(entry,FILEINFONODE,list);
        if(ackMsg->FILE_NO == curNode->FILE_NO)
        {
            break;
        }
    }
    if(&(curNode->list) != sendHead)
    {
        curNode->FILE_OPT->sWindow->delDatagram(ackMsg->REG_FRAGMENT_NUM);
        pthread_cond_signal(&curNode->FILE_OPT->m_threadCond);
    }
}

void UDPService::rsendFileSegment(MESSHEADER *cmd)
{
    FILE_PART_RSEND_REQ *rendReq = (FILE_PART_RSEND_REQ *)(cmd + 1);
    struct list_head *entry = new list_head;
    FILEINFONODE *curNode = NULL;
    list_for_each(entry,sendHead)
    {
        curNode = list_entry(entry,FILEINFONODE,list);
        if(rendReq->FILE_NO == curNode->FILE_NO)
        {
            break;
        }
    }
    if(&(curNode->list) != sendHead)
    {
        char *buf = new char[BUFSIZE];
        curNode->FILE_OPT->sWindow->getDatagram(&buf,rendReq->REG_FRAGMENT_NUM);
        MESSHEADER *head = (MESSHEADER *)buf;
        head->COMMAND_ID = FILE_PART_RSEND_cRESP;
        head->MESSAGE_ID = message_id++;
        head->SEQUENCE_ID = cmd->SEQUENCE_ID;

        curNode->FILE_OPT->sendMsg(buf);
    }
}

void UDPService::ackRSendFileSegment(MESSHEADER *cmd)
{
    FILE_PART_RSEND_RESP *rendReq = (FILE_PART_RSEND_RESP *)(cmd + 1);
    struct list_head *entry = new list_head;
    FILEINFONODE *curNode = NULL;
    list_for_each(entry,recvHead)
    {
        curNode = list_entry(entry,FILEINFONODE,list);
        if(rendReq->FILE_NO == curNode->FILE_NO)
        {
            break;
        }
    }

    //待完成



//    char *buf = new char[FILEBUF];
//    curNode->FILE_OPT->sWindow->getDatagram(&buf,rendReq->REG_FRAGMENT_NUM);
////    MESSHEADER *head = (MESSHEADER *)buf;
////    head->COMMAND_ID = FILE_PART_RSEND_cRESP;
////    head->MESSAGE_ID = message_id++;
////    head->SEQUENCE_ID = cmd->SEQUENCE_ID;
//
//    curNode->FILE_OPT->sendMsg(buf);
//    delete buf;
}



void UDPService::getFileInformation(MESSHEADER *cmd, struct sockaddr_in *remote)
{
    MESSHEADER *pMsg = cmd;
    int sign = 0;
    REQUEST_FILE *pFileMsg = (REQUEST_FILE *)(pFileMsg+1);
    char *pFName = NULL;
    memcpy(pFName,pFileMsg->FILE_NAME,strlen(pFileMsg->FILE_NAME));
    int ruqFileSize =  strlen(pFileMsg->FILE_NAME);

    struct list_head *entry = new list_head;
    FILEINFONODE *curNode = NULL;

    list_for_each(entry,recvHead)
    {
        curNode = list_entry(entry,FILEINFONODE,list);
        string curFileName = curNode->FILE_NAME;
        if(ruqFileSize == curFileName.size() &&
            (strncmp(pFName,curFileName.c_str(),curFileName.size()) == 0))
        {
            if(curNode->FILE_IS_SEND)
                sign = 1;
            break;
        }
    }

    if(sign)
    {
        REFUSE_FILE refuseMsg;
        memcpy(refuseMsg.FILE_NAME,pFileMsg->FILE_NAME,ruqFileSize);

        MESSHEADER head;
        memset(&head,0,sizeof(head));
        head.COMMAND_ID = DENY_FILE_REQ;
        head.MESSAGE_ID = message_id++;
        head.SEQUENCE_ID = pMsg->SEQUENCE_ID;
        head.VERSION = pMsg->VERSION;
        uint32_t msglen = sizeof(MESSHEADER) + sizeof(refuseMsg);
        head.TOTAL_LENGTH = msglen;

        char *msg = new char[msglen];
        memset(msg,0,msglen);
        memcpy(msg,(char *)&head,sizeof(head));
        memcpy(msg+sizeof(head),(char *)&refuseMsg,sizeof(refuseMsg));

        sendMsg(m_sockfd,remote,msg);
        delete msg;
    }
    else
    {
        int sockfd = 0,port = server_port++;
        if((sockfd = acceptClientReq(port)) < 0)
            return;
        FILEINFONODE fileinfo;
        INIT_LIST_HEAD(&fileinfo.list);
        fileinfo.FILE_NAME = pFileMsg->FILE_NAME;
        fileinfo.FILE_NO = pFileMsg->FILE_NO;
        fileinfo.FILE_IS_SEND = true;
        fileinfo.RECV_SOCKET = sockfd;
        list_add_tail(&fileinfo.list,recvHead);

        RECEIVE_FILE agreeMsg;
        agreeMsg.FILE_NO  = pFileMsg->FILE_NO;
        memcpy(agreeMsg.FILE_NAME,pFileMsg->FILE_NAME,ruqFileSize);
        agreeMsg.PORT     = port;

        MESSHEADER head;
        memset(&head,0,sizeof(head));
        head.COMMAND_ID   = FILE_RESP;
        head.MESSAGE_ID   = message_id++;
        head.SEQUENCE_ID  = pMsg->SEQUENCE_ID;
        head.VERSION      = pMsg->VERSION;
        uint32_t msglen   = sizeof(MESSHEADER) + sizeof(agreeMsg);
        head.TOTAL_LENGTH = msglen;

        char *msg = new char[msglen];
        memset(msg,0,msglen);
        memcpy(msg,(char *)&head,sizeof(head));
        memcpy(msg+sizeof(head),(char *)&agreeMsg,sizeof(agreeMsg));

        sendMsg(m_sockfd,remote,msg);
        delete msg;
    }
}



int UDPService::setUnBlocked()
{
    int flag;
    if( ioctl(m_sockfd,FIONBIO,1) < 0 || (flag = fcntl(m_sockfd,F_GETFD,0)) < 0
        || fcntl(m_sockfd,F_SETFD,flag | O_NONBLOCK) < 0)
        return -1;
    return 0;
}

int UDPService::setSockoption()
{
    int SEND_BUFSIZE = 250250;
    int RECV_BUFSIZE = 250250;
    bool flags = 1;
    if(setsockopt(m_sockfd,SOL_SOCKET ,SO_REUSEADDR,(char *) & flags,sizeof(bool)) < 0
        || setsockopt(m_sockfd,SOL_SOCKET,SO_SNDBUF,(char *) & SEND_BUFSIZE,sizeof(int)) < 0
        || setsockopt(m_sockfd,SOL_SOCKET,SO_RCVBUF,(char *) & RECV_BUFSIZE,sizeof(int)) < 0)
        //|| setsockopt(m_sockfd,IPPROTO_UDP,,(char *) & flags,sizeof(int)) < 0)
    {
        return -1;
    }
    return 0;
}


int UDPService::getRandPort()
{
    srand(time(NULL));
    return (rand() % (2 << 14)) + (2 << 13);
}



//}



