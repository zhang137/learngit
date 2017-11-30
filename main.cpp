#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "fileinfolist.h"
#include "filetransfer.h"
#include "slidingwindow.h"
#include "message.h"
//#include "retransmissionthread.h"


using namespace std;

//pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
//pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;


int getRandPort()
{
    srand(time(NULL));
    return (rand() % (2 << 14)) + (2 << 13);
}


//void *retransthread_function(void *arg)
//{
//    pthread_detach(pthread_self());
//    RetransQueue *queue = (RetransQueue*)arg;
//
//    while(!queue->retrans_finish)
//    {
//        pthread_mutex_lock(&g_mutex);
//        while(queue->getCapacity() == 0)
//        {
//            printf("cond wait start!\n");
//            pthread_cond_wait(&thread_cond,&g_mutex);
//            printf("cond wait stop!\n");
//        }
//        pthread_mutex_unlock(&g_mutex);
//        printf("retrans sequence thread\n");
//        sleep(2);
//
//    }
//    printf("retransmission stop!\n");
//    pthread_exit(NULL);
//}

int main()
{

    FILE_PART_RESQ test;
    test.FILE_NO = 1;
    test.REG_FRAGMENT_NUM = 2;

    FILE_PART_RSEND_REQ *test1 = (FILE_PART_RSEND_REQ *)&test;
    cout<<test1->FILE_NO<<" " <<test1->REG_FRAGMENT_NUM<<endl;

    FileOperation file,file2;
    file.fileOpenToSend("/home/usr/projects/slidingWindowPro/filetransfer.h",0);
    file2.fileOpenToRecv("/home/usr/projects/slidingWindowPro/filetransfer1.h",0);
    printf("%ld\n",file.getFileSize());

    char *buf = new char[BUFSIZE];
    while(file.readFile(&buf,BUFSIZE) > 0)
    {
        file2.writeFile(buf,BUFSIZE);
        printf("%s",buf);
        memset(buf,0,BUFSIZE);
    }
    delete buf;
    printf("%ld",file.getFtell());
    struct list_head *head = new struct list_head;
    struct tgFILEINFONODE node1,node2,node3,node4,node5;

    INIT_LIST_HEAD(head);

    cout<<getRandPort()<<endl;
    //head->front = head->next = head;
    //LIST_INIT(&head);
    //LIST_HEAD_INIT(&file_head);
    //LIST_INIT(node1.FILELIST);
    node1.FILE_NO = 1;
    node1.FILE_NAME = "apple";

    //LIST_INIT(node2.FILELIST);
    node2.FILE_NO = 2;
    node2.FILE_NAME = "blue";

    //LIST_INIT(node3.FILELIST);
    node3.FILE_NO = 3;
    node3.FILE_NAME = "color";

    //LIST_INIT(node4.FILELIST);
    node4.FILE_NO = 4;
    node4.FILE_NAME = "red";

    list_add_tail((&node1.list),head);
    list_add_tail(&(node2.list),head);
    list_add_tail(&(node3.list),head);
    list_add_tail(&(node4.list),head);

    struct list_head *entry = new  struct list_head;
    struct list_head *entry1 = new  struct list_head;//= head.next->next->next;
    //list_entry( entry,(struct tgFILEINFONODE),list);
    //((struct tgFILEINFONODE*)((char *)(entry) - (unsigned long)(&(((struct tgFILEINFONODE*)0)->list))))
   // cout<<head->next->next
   //struct list_head *temp
    char filename[256] = "color";
    struct list_head *entry2 = new  struct list_head;
    FILEINFONODE *curNode = NULL;
    list_for_each(entry2,head)
    {
        FILEINFONODE *curNode = list_entry(entry2,FILEINFONODE,list);
        int size = strlen(filename);
        string curFileName = curNode->FILE_NAME;
        if(size == curFileName.size() &&
            (strncmp(filename,curFileName.c_str(),curFileName.size()) == 0))
        {
            cout<< "File:"<<curNode->FILE_NO<<endl;
            break;
        }

//        curNode = list_entry(entry2,FILEINFONODE,list);
//        if(strncmp("color132",curNode->FILE_NAME.c_str(),curNode->FILE_NAME.size()) == 0)
//        {
//            cout<< "File:"<<curNode->FILE_NO<<endl;
//            break;
//        }
    }


   list_for_each_safe(entry,entry1,head)
    {
        //struct list_head *temp = entry;
        //head = *head.next;
        cout<<" FILE_NO:" <<(list_entry(entry,struct tgFILEINFONODE ,list)->FILE_NAME) <<endl;
        list_del(entry);
    }
    //cout<<sizeof(i)<<" "<< sizeof(FILE_PART_REG)<<endl;

    //cout<<" FILE_NO:" <<(list_entry(entry,struct tgFILEINFONODE ,list)->FILE_NAME) <<endl;
//    FILELIST_DELETE(head,3,list)
//    SlidingWindow w;
//    char *temp = new char[MTU];
//    memset(temp,1,MTU);
//    w.saveDatagram(temp,3);
//    char *msg = new char[WINSIZE];
//    memcpy(msg,"dsadasdsadada",WINSIZE);
//    w.getDatagram(&msg);
//    cout << "Hello world!\n" << msg<< sizeof(size_t)<<"  "<< sizeof(long long)<< endl;
//
//    RetransQueue *r = new RetransQueue;
//    r->setRetransFinish(false);
//    r->push(2);
//    r->push(5);
//    r->push(7);
//    int num = r->getCapacity();
//    cout<<"queue capacity:"<< num<<endl;
//
//    pthread_t thread;
//    pthread_create(&thread,NULL,retransthread_function,(void*)r);
//    sleep(5);
//    int x;
//    r->pop(&x);
//    r->pop(&x);
//    r->pop(&x);
//    num = r->getCapacity();
//
//    cout<<"queue capacity:"<< num<<endl;
//    sleep(1);
//    pthread_cond_signal(&thread_cond);
//
//    sleep(5);
//
//    r->push(2);
//    pthread_cond_signal(&thread_cond);
//    num = r->getCapacity();
//    cout<<"queue capacity:"<< num<<endl;
//    sleep(4);
//    r->setRetransFinish(true);
//
//    sleep(5);
//    printf("main thread stop!\n");
//    pthread_join(&thread,NULL);
//    delete r;
//    r = NULL;
    return 0;
}
