/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmupdate.h
* 摘    要：  声明系统升级相关函数
*            
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "app_frmupdate.h"
#include "wnd_global.h"

/*******************************************************************************
**					为实现应用层frmupdate而需要引用的其他头文件 			  **
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>  

/**********************************************************************************
**								变量定义				 						 **
***********************************************************************************/
static int __g_msgid = -1;  
static struct msg_st __g_Message;


/***
  * 功能：
        获取升级文件路径
  * 参数：
        1、char *pUpdatePath:	存贮升级文件路径
  * 返回：
        成功返回零，失败返回非零值
        -2:	未插入SD卡或U盘
        -3:	未放入升级文件
  * 备注：
  		优先加载U盘中的应用程序升级文件
***/
int GetUpdatePath(char *pUpdatePath)
{
	int iErr = 0;
	unsigned char ucUdiskFlg =0;
	unsigned char ucSDcardFlg =0;
	char *pUpDateFilePath = NULL;
    
	if(iErr == 0)
	{
		if(NULL == pUpdatePath)
		{
			iErr = -1;
		}
	}

	/*
	 *	判断是否插入U盘或SD卡，并置位相应的标记
	 *	若没有插入任何有效的设备，提示用户插入 
	 */
	if(iErr == 0)
	{
		/* 挂载U盘失败 */
		if ( access(USB, F_OK) == -1 )				
	    {
	    	ucUdiskFlg = 0;
			/* 显示加载U盘或SD卡 */
			if ( access(SD, F_OK) == -1 )	
			{	
		        iErr = -2;		 
			}
			/* 挂载SD卡成功 */
			else	
			{
				ucSDcardFlg = 1;
			}
	    }
		/* 挂载U盘成功 */
		else		
		{
			ucUdiskFlg = 1;
			if ( access(SD, F_OK) == -1 )	
			{	
				ucSDcardFlg = 0;
			}
			/* 挂载SD卡成功 */
			else	
			{
				ucSDcardFlg = 1;
			}
		}		
	}

	/* 
	 *	优先加载U盘中的应用程序升级文件
	 *  判断U盘或SD卡中是否有相应的应用程序升级文件
	 */
	if(iErr == 0)
	{
		/* U盘挂载成功 */
		if(ucUdiskFlg)	
		{
			/* U盘中不存在升级文件 */
			if( access(USBPATH, F_OK) == -1) 
			{
				/* SD卡挂载成功 */
				if(ucSDcardFlg)	
				{
					/* SD卡中也不存在升级文件 */
					if( access(SDPATH, F_OK) == -1) 
					{	
				        iErr = -3;	
					}		
					/* SD卡中存在升级文件 */
					else	
					{
						pUpDateFilePath = SDPATH;
					}
				}
			}
			/* U盘中存在升级文件 */
			else	
			{
				pUpDateFilePath = USBPATH;
			}
		}
		/* U盘没有挂载成功 */
		else	
		{
			/* SD卡挂载成功 */
			if(ucSDcardFlg)	
			{
				/* SD卡中不存在升级文件 */
				if( access(SDPATH, F_OK) == -1) 
				{
					iErr = -3;	
				}	
				/* SD卡中存在升级文件 */
				else	
				{
					pUpDateFilePath = SDPATH;
				}
			}
		}		
	}

	if(iErr == 0)
	{
		strcpy(pUpdatePath, pUpDateFilePath);
	}

	return iErr;
}


/***
  * 功能：
		初始化消息队列
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int InitMsgQueue(void)
{
	int iErr = 0;

	//建立消息队列	
	__g_msgid = msgget((key_t)1235, 0666 | IPC_CREAT);	
	if(__g_msgid == -1)  
	{  
		iErr = -1;	
	}		

	return iErr;
}


/***
  * 功能：
		向消息队列发送消息
  * 参数：
		1、int iCmdType		:	命令码
		2、int iParameter	:	附加参数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int MsgSend(int iCmdType, int iParameter)
{
	int iErr = 0;

	struct msg Msg;
	
	Msg.iCmdType = iCmdType;
	Msg.iParameter = iParameter;
	memcpy(__g_Message.message, &Msg, sizeof(Msg));
	__g_Message.msg_type = TO_SERVER; 
	
	if(msgsnd(__g_msgid, (void*)&__g_Message, MESSAGE_NUM, 0) == -1)  
	{  
		iErr = -1; 
	} 

	return iErr;
}


/***
  * 功能：
		向自己的消息队列发送消息
  * 参数：
		1、int iCmdType		:	命令码
		2、int iParameter	:	附加参数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int MsgSendSelf(int iCmdType, int iParameter)
{
	int iErr = 0;

	struct msg Msg;
	
	Msg.iCmdType = iCmdType;
	Msg.iParameter = iParameter;
	memcpy(__g_Message.message, &Msg, sizeof(Msg));
	__g_Message.msg_type = FROM_SERVER; 
	
	if(msgsnd(__g_msgid, (void*)&__g_Message, MESSAGE_NUM, 0) == -1)  
	{  
		iErr = -1; 
	} 

	return iErr;
}


/***
  * 功能：
		从消息队列中获取消息
  * 参数：
		1、struct msg *pMsg:	消息缓冲区	
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int GetMsg(struct msg *pMsg)
{
	int iErr = 0;	

	if(msgrcv(__g_msgid, (void*)&__g_Message, MESSAGE_NUM, FROM_SERVER, 0) == -1)  
	{  
		iErr = -1; 
	} 
	else
	{
		memcpy(pMsg, __g_Message.message, sizeof(struct msg));
	}

	return iErr;
}

/***
  * 功能：
		获得磁盘指定分区剩余空间大小
  * 参数：
		1、const char *path:磁盘路径	
  * 返回：
		返回磁盘剩余空间大小(字节)
  * 备注：
***/
unsigned long long getDiskFreeSize(const char *path)
{
    unsigned long long freeSize = 0;
    unsigned long long blockSize = 0;
    struct statfs diskInfo;  
    if(!path)
    {
        return 0;
    }
    if(access(path, F_OK))
    {
        return 0;
    }

    statfs(path,&diskInfo);  

    blockSize = diskInfo.f_bsize;               // 每个block里面包含的字节数  
    freeSize = blockSize*diskInfo.f_bfree;

    return freeSize;
}

