/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmsendfile.h
* 摘    要：  实现文件传输功能的初始化和函数声明
*
* 取代版本：
* 原 作 者：
* 完成日期：
*
* 
*******************************************************************************/

#ifndef _APP_FRMSENDFILE_H_
#define _APP_FRMSENDFILE_H_


typedef int (*SENDFILECALLBACK)(long );
typedef int (*SENDFILECALLBACK1)(long long);

typedef struct _select_file_info_single
{
	char pfilepath[512];	//文件路径
	char pfilename[512];	//文件名
	long size;						//文件大小
	char type;						//文件类型，0->文件，1->文件夹，2->其他
}__attribute__((packed))SELECT_FILE_INFO_SINGLE;

typedef struct _send_info
{
	int sendFileSwitch;					//文件发送开关，1为允许发送，0为终止发送
	int connectToPc;					//是否已与PC连接，1为已连接
	int sendCount;						//已发送文件数
	int fileCount;						//发送文件总数
	char fileReady;						//文件是否准备好
	SELECT_FILE_INFO_SINGLE *pfileList;	//文件列表
	SENDFILECALLBACK1 callbackfunc;		//发送文件过程的回调函数,传入的参数为当前传送文件已传送的大小
	SENDFILECALLBACK callbackcomplete;	//发送完成后回调函数,传入的参数为发送成功个数
	SENDFILECALLBACK callbackdisconnect;//发送过程网络中断回调函数
} __attribute__((packed))SEND_INFO;


/*初始化文件发送功能*/
int Sendfile_Init(void);

/***
  * 功能：
        设置需发送的文件信息
  * 参数：
        1:char **path:文件路径
        2:int count:文件个数
  * 返回：
        成功返回0，失败返回-1
  * 备注：
  		
***/
int setFileInfo(char **path, int count);



/*得到文件发送开关状态*/
int getsendFileSwitch(void);

/*设置文件发送开关状态*/
void setsendFileSwitch(int sendswitch);


/*得到是否已连接到PC*/
int getconnectToPC(void);

/*得到已发送文件数*/
int getsendCount(void);

/*获得发送文件总数*/
int getfileCount(void);


/*设置回调函数(调用时传入的参数为当前正在发送文件已发送的大小)*/
void setcallback(SENDFILECALLBACK1 callback);

/*设置发送完成后的回调函数(调用时传入的参数为成功传送个数)*/
void setcompletecallback(SENDFILECALLBACK callback);

/*设置发送完成后的回调函数(调用时传入的参数为成功传送个数)*/
void setdisconnectcallback(SENDFILECALLBACK callback);

/*获得正在传送文件信息*/
SELECT_FILE_INFO_SINGLE getsendfile(void);

/*取消文件发送*/
int CancelSendFile(void);

/*退出文件发送功能*/
int Sendfile_Quit(void);

#endif

