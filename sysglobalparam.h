#ifndef SYSGLOBALPARAM_H_INCLUDED
#define SYSGLOBALPARAM_H_INCLUDED

#include "types.h"
enum {
    //SEND_BUFSIZE =  256 * 1024,
    //RECV_BUFSIZE =  256 * 1024,
    PACKETNUM = 8,
    DATA_READY = 36,
    MTU = 175//1430
};

enum {

    CONTINUE_SEND_FILE_REQ = 0x15,//文件断点续传请求
    CONTINUE_SEND_FILE_RESP = 0x65,//文件断点续传回应
    CONTINUE_RECEIVE_FILE_REQ = 0x66,//文件断点续传请求
    CONTINUE_RECEIVE_FILE_RESP = 0x67,//文件断点续传回应
    READY_TO_CONTINUE_SEND_FILE_REQ = 0x68,//文件断点续传请求准备好
    FILE_FINISH_REQ = 0x16,//文件接收完请求
    DENY_SEND_FILE_REQ = 0x18,//拒绝发送文件请求
    FILE_INTERRUPT_REQ = 0x19,//文件收发已中断
    FILE_REQ = 0x05,//文件发送请求
    FILE_RESP = 0x75,//文件发送响应
    ACCEPT_FILE_REQ = 0x06,//同意接收文件
    DENY_FILE_REQ = 0x76,//拒绝接收文件请求
    FILE_PART_REQ = 0x07,//文件片段请求
    FILE_PART_RESP = 0x77,//文件片段响应
    FILE_PART_RSEND_cREQ = 0x78,//重传文件片段请求
    FILE_PART_RSEND_cRESP = 0x79//重传文件片段响应

};

static u_int32_t  sequence_id = 0;
static u_int32_t  message_id = 0;
static u_int32_t  file_no = 0;

static u_int16_t   server_port = 32767;


#define WINSIZE MTU*PACKETNUM




#endif // SYSGLOBALPARAM_H_INCLUDED
