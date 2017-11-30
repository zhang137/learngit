#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include "types.h"

using std::string;

#pragma pack(1)

//消息头部
typedef struct tgMSGHEADER
{
    u_int32_t     SEQUENCE_ID;              //消息流水号,顺序累加,步长为1,循环使用（一对请求和应答消息的流水号必须相同）
    u_int32_t     MESSAGE_ID;               //消息ID
    u_int16_t     TOTAL_LENGTH;             //消息总长度(含消息头及消息体)
    u_int16_t     BLOCK_INDEX;              //拆包的下标，0开始
    u_int16_t     BLOCK_TOTAL;              //总共拆包的块数
    u_int8_t      COMMAND_ID;               //命令或响应类型
    u_int8_t      VERSION;                  //版本号。高位4bit表示主版本号,低位4bit表示次版本号
}MESSHEADER;

//文件请求
typedef struct tgREQUEST_FILE
{
    u_int8_t    FILE_ABSTRUCT[16];        //文件摘要
    u_int32_t   FILE_NO;                  //文件编号
    u_int8_t    FILE_TYPE;                //文件类型
    char        FILE_NAME[256];           //文件名称
    tgREQUEST_FILE()
    { memset(FILE_NAME,0,256);}
    //int8_t             FILE_NAME[100];
}REQUEST_FILE;

//文件回应
typedef struct tgRESPONSES_FILE
{
    u_int32_t   FILE_NO;                  //文件编号
    char        FILE_NAME[256];           //文件名称
    tgRESPONSES_FILE()
    { memset(FILE_NAME,0,256);}
    //int8_t             FILE_NAME[100];
}RESPONSES_FILE;


//拒绝接受文件
typedef struct tgREFUSE_FILE
{
    char         FILE_NAME[256];           //文件名称
    tgREFUSE_FILE()
    { memset(FILE_NAME,0,256);}
}REFUSE_FILE;

//同意接收文件
typedef struct tgREVEIVE_FILE
{
    u_int32_t    FILE_NO;                  //文件名称
    u_int16_t    PORT;                     //客户端接收端口
    char         FILE_NAME[256];           //文件名称
    tgREVEIVE_FILE()
    { memset(FILE_NAME,0,256);}
}RECEIVE_FILE;

//文件片段请求
typedef struct tgFILE_PART_REG
{
    u_int32_t     FILE_NO;                  //文件编号
    u_int32_t     REG_FRAGMENT_NUM;         //文件片段编号 可以请求多个
}FILE_PART_REG;

//文件片段响应
typedef struct tgFILE_PART_RESQ
{
    u_int32_t     FILE_NO;                  //文件编号
    u_int32_t     REG_FRAGMENT_NUM;         //文件片段编号
    //后跟数据 长短由头部长度 减去 4+2 字节确定
}FILE_PART_RESQ;

//重传片段请求
typedef struct tgFILE_PART_RSEND_REQ
{
    u_int32_t     FILE_NO;                  //文件编号
    u_int32_t     REG_FRAGMENT_NUM;         //文件片段编号
    //后跟数据 长短由头部长度 减去 4+2 字节确定
}FILE_PART_RSEND_REQ;

//重传片段响应
typedef struct tgFILE_PART_RSEND_RESP
{
    u_int32_t     FILE_NO;                  //文件编号
    u_int32_t     REG_FRAGMENT_NUM;         //文件片段编号
    //后跟数据 长短由头部长度 减去 4+2 字节确定
}FILE_PART_RSEND_RESP;



#pragma pack()

#endif // MESSAGE_H_INCLUDED
