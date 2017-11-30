#ifndef UDPSERVICE_H_INCLUDED
#define UDPSERVICE_H_INCLUDED

#include <string>
#include "sysglobalparam.h"
#include "filetransfer.h"
#include "fileinfolist.h"
#include "message.h"
#include "slidingwindow.h"
#include "retransmissionthread.h"
//#include <atelier_plugin/atelier_plugin_common.h>
//#include <atelier_plugin/atelier_plugin.h>
#include <netinet/in.h>
#define BUFSIZE MTU

static  int FILEBUF = BUFSIZE - 24;
//namespace atelier_plugin
//{
//#ifdef __cplusplus
//extern "C" {
//#endif
//__attribute__ ((visibility("default"))) AtelierPlugin* CreateUDPServer();
//#ifdef __cplusplus
//}
//#endif

class FileOperation;

class UDPService //: public AtelierPlugin
{

public:
    UDPService();
    virtual ~UDPService();

    //virtual void Initialize();
    //virtual void Execute(std::string action, std::string callback_id,
    //std::string args);
	void Close();

	int  setUnBlocked();
	int  setSockoption();
	int  initSocket();
	int  initEpoll();
    void initServer(std::string &port);
	int  setEpollctl(int epollfd, int option,int sockfd,uint32_t events);
	//命令相关
    int  acceptClientReq(int port);
    void handleClientInfo();
	int  sendFileReq(char *fileInfo);
    int  continueToSendFileReq();
	int  parseMsg( struct sockaddr_in *remote,std::string &cmd);
	//文件发送相关
	int  startSendFile(MESSHEADER *head,struct sockaddr_in *remote);
	int  startRecvFile(MESSHEADER *pHead);
	void getFileInformation(MESSHEADER *cmd, struct sockaddr_in *remote);
	void acknowledgeSegment(MESSHEADER *cmd);
	void rsendFileSegment(MESSHEADER *cmd);
	void ackRSendFileSegment(MESSHEADER *cmd);

    //
	std::string getMsg(int sockfd,struct sockaddr_in *remote);
    int sendMsg(int sockfd,struct sockaddr_in *remote,std::string msg);

    //
    int getRandPort();
private:
    int  m_sockfd;
    int  m_epollfd;
    int  m_iMaxFdNum;
    struct epoll_event *m_pEvents;
    struct list_head *recvHead;
    struct list_head *sendHead;
    char recvmsg[BUFSIZE];
    char sendmsg[BUFSIZE];
};


void *sendThread(void *arg);

//}



#endif // UDPSERVICE_H_INCLUDED
