/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmfilebrowse.h
* 摘    要：  声明文件浏览器中的公用操作函数
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

#include "app_frmfilebrowse.h"

/***************************************
* 为实现GUIMESSAGE而需要引用的其他头文件
***************************************/
#include "app_frmotdr.h"
#include "app_middle.h"
#include "app_global.h"
#include "app_screenshots.h"

#include "guimyglobal.h"

#include "wnd_global.h"
#include "wnd_frmfilebrowse.h"

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h> 

/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
static struct dirent **__g_Namelist_Folder = NULL;
static struct dirent **__g_Namelist_NotFolder = NULL;
static struct dirent **__g_Namelist = NULL;					// 整个目录下文件的结构体数组的地址
static struct dirent *__g_NamelistPage[MAXPAGEFILENUM];		// 存放每页文件结构的指针数组
static unsigned short __g_usOTDRFrmBuff[FRMBUFFSIZE];		// 保存OTDR界面图像缓存

static int __g_iFolderNum = 0;
static int __g_iNotFolderNum = 0;
static int __g_iTotalNum = 0;

static int __g_iCurFilePos	= 0;							// 即将要显示的项目在当前目录结构数组中所处的位置
static int __g_iPageFileNum = 0;							// 当前目录下每页能实际显示的项目数

static FILESORT __g_enCurSortMethod = TIMEDESC;				// 当前的排序方法
static FILETYPE __g_enCurFileType = ANY;					// 当前系统过滤的文件类型

static unsigned char __g_ucEnterOtdrFlg = 0;				// 已经进入过OTDR界面的标志，用于屏幕截图

/********************
* GUI中的当前事件对象
********************/
static MULTISELECT *pCurrQueue = NULL;

//extern MULTISELECT *pPictureQueue;							// 存贮文件浏览器子目录下的图片名的队列


//标记是否否从MAIN窗口跳转到此窗口，用于判断MENU按键是否可用
extern int iFromFrmMainFlag;
extern unsigned int g_ucActionIndex;						// 表示初始化文件浏览器时的初始文件操作类型
extern int iLabelSaveFileTypeNum;							// 保存文件类型
//extern FileNameType GcurFileNameType;        				//当前文件命名规则

//sem_t SemSavePicture;										// 用于同步图片保存的线程
extern MULTISELECT *pEnvQueue;

#define ERROR_NO_SPACE          28
/***
  * 功能：
        根据指定的信息直接创建消息对象
  * 参数：
        1.int iQueueLimit:      需要创建的消息对象的消息队列的最大长度
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
MULTISELECT* CreateQueue(int iQueueLimit)
{
    //错误标志、返回值定义
    int iErr = 0;
    MULTISELECT *pMsgObj = NULL;

    if (iErr == 0)
    {
        //尝试分配内存
        pMsgObj = (MULTISELECT *) GuiMemAlloc(sizeof(MULTISELECT));
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //创建消息对象的消息队列
        pMsgObj->pMsgQueue = CreateList(iQueueLimit);
        if (NULL == pMsgObj->pMsgQueue)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //初始化互斥锁
        if (InitMutex(&(pMsgObj->Mutex), NULL))
        {
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
	    case -3:
	        DestroyList(pMsgObj->pMsgQueue);
	    case -2:
	        GuiMemFree(pMsgObj);
	    case -1:
	        pMsgObj = NULL;
	    default:
	        break;
    }

    return pMsgObj;
}


/***
  * 功能：
        删除消息对象
  * 参数：
        1.GUIMESSAGE **ppMsgObj:    指针的指针，指向需要销毁的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyQueue(MULTISELECT **ppMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断ppMsgObj是否为有效指针
        if (NULL == ppMsgObj)
        {
            iErr = -1; 
        }
    }

    if (iErr == 0)
    {
        //判断ppMsgObj所指向的是否为有效指针
        if (NULL == *ppMsgObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //销毁消息对象的消息队列
        if (DestroyList((*ppMsgObj)->pMsgQueue))
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //销毁消息对象，并将指针置空
        GuiMemFree(*ppMsgObj);
        *ppMsgObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        得到当前消息对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
MULTISELECT* GetCurrQueue(void)
{
    return pCurrQueue;
}


/***
  * 功能：
        设置为当前消息对象
  * 参数：
        1.GUIMESSAGE *pEventObj:    被设置为当前消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrQueue(MULTISELECT *pMsgObj)
{
    //判断pEventObj是否有效
    if (NULL == pMsgObj)
    {
        return -1;
    }

    pCurrQueue = pMsgObj;

    return 0;
}

/***
  * 功能：
        从指定消息对象中清除特定类型的消息项
  * 参数：
        1.int iMsgType:         需要清除的消息项类型
        2.GUIMESSAGE *pMsgObj:  指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
    　　必须在持有锁（消息对象中的互斥锁）的前提下才能调用，否则会导致竞态
***/
int ClearSpecialQueue(const char *pFileName, MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
	char *pTmp = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (iErr == 0)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断消息队列是否有效指针和消息队列是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //遍历消息队列找到类型匹配的消息项
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);

            if (!strcmp(pTmp, pFileName))
            {
                //删除操作将修改列表项结构，因此需先移动pItem
                pItem = pItem->pPrev;
                //找到类型匹配的消息项后从消息队列中删除
                if (ListDel(pItem->pNext, pMsgObj->pMsgQueue))
                {
                    iErr = -3;
                    break;
                }

                //释放资源
                GuiMemFree(pTmp);
                GuiMemFree(pPkt);

                //list_next_each依赖iPos进行循环控制，故删除操作后需对iPos修正
                iPos--;
                //判断是否已遍历完毕
                if (iPos == -1)
                {
                    break;
                }
            }
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return iErr;
}


/***
  * 功能：
        从指定消息对象中检查特定类型的消息项
  * 参数：
        1.int iMsgType:         需要检查的消息项类型
        2.GUIMESSAGE *pMsgObj:  指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int CheckSpecialQueue(const char *pFileName, MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
	char *pTmp = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (iErr == 0)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断消息队列是否有效指针
        if ((NULL == pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //遍历消息队列找到类型匹配的消息项
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);

			/* 链表中存在该项 */
            if (!strcmp(pTmp, pFileName))
            {
				break;
            }
        }
		
		//如果没有找到类型匹配的消息项
        if (iPos == -1)
        {
            iErr = -3;
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return iErr;
}

/***
  * 功能：
        从指定消息对象中读取消息项
  * 参数：
        1.GUIMESSAGE *pMsgObj:  指定的消息对象
  * 返回：
        成功返回消息项指针
        失败返回空指针
  * 备注：
***/
char* ReadQueue(MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;
	char *pReturn = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (iErr == 0)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断消息队列是否有效指针
        if ((NULL == pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
		//通过pEeventObj的GUILIST_ITEM成员获得pPkt的地址
        pPkt = container_of(pMsgObj->pMsgQueue->pHead, GUIOBJ, List);
        pReturn = (char *) (pPkt->pObj);
        //从事件队列中删除头列表项
        if (ListDel(pMsgObj->pMsgQueue->pHead, pMsgObj->pMsgQueue))
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //释放资源
        GuiMemFree(pPkt);
    }

    //错误处理
    switch (iErr)
    {
	    case -3:
	        pReturn = NULL;
	    case -2:
	    case -1:
	    default:
	        break;
    }	

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return pReturn;
}


/***
  * 功能：
        从指定消息对象中，根据给定的消息项，返回该消息项的上一个消息项
  * 参数：
        1.const char *pCurFileName	:		给定的消息项
        2.char *pLastFileName		:		存贮给定的消息项的上一个消息项
        3.GUIMESSAGE *pMsgObj		:  		指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int LastSpecialQueue(const char *pCurFileName, char *pLastFileName, MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
	char *pTmp = NULL;
	unsigned char ucFlg = 0;	
	unsigned char ucFlag = 0;	

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (iErr == 0)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj ||
			NULL == pCurFileName ||
			NULL == pLastFileName)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断消息队列是否有效指针
        if ((NULL == pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //遍历消息队列找到类型匹配的消息项
        list_prev_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);

			if(ucFlg)
			{
				strcpy(pLastFileName, pTmp);
				ucFlag = 1;
				break;
			}

			/* 链表中存在该项 */
            if (!strcmp(pTmp, pCurFileName))
            {
				ucFlg = 1;
            }
        }
		
		//如果没有找到类型匹配的消息项
        if (iPos == -1)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
    	if(ucFlag)
		{
			iErr = 0;
    	}
		else
		{
			iErr = -1;
		}
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return iErr;
}


/***
  * 功能：
        从指定消息对象中，根据给定的消息项，返回该消息项的下一个消息项
  * 参数：
        1.const char *pCurFileName	:		给定的消息项
        2.char *pLastFileName		:		存贮给定的消息项的下一个消息项
        3.GUIMESSAGE *pMsgObj		:  		指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int NextSpecialQueue(const char *pCurFileName, char *pLastFileName, MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
	char *pTmp = NULL;
	unsigned char ucFlg = 0;	
	unsigned char ucFlag = 0;	

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (iErr == 0)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj ||
			NULL == pCurFileName ||
			NULL == pLastFileName)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断消息队列是否有效指针
        if ((NULL == pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //遍历消息队列找到类型匹配的消息项
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);

			if(ucFlg)
			{
				strcpy(pLastFileName, pTmp);
				ucFlag = 1;
				break;
			}

			/* 链表中存在该项 */
            if (!strcmp(pTmp, pCurFileName))
            {
				ucFlg = 1;
            }
        }
		
		//如果没有找到类型匹配的消息项
        if (iPos == -1)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
    	if(ucFlag)
		{
			iErr = 0;
    	}
		else
		{
			iErr = -1;
		}
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return iErr;
}


/***
  * 功能：
        写入消息项到指定消息对象的队列尾部
  * 参数：
        1.GUIMESSAGE_ITEM *pMsgItem:    需要被写入消息队列中的消息项
        2.GUIMESSAGE *pMsgObj:          指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
    　　必须在持有锁（消息对象中的互斥锁）的前提下才能调用，否则会导致竞态
***/
int WriteQueue(const char *pFileName, MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;
	char *pTmp;

    //加锁
    MutexLock(&(pMsgObj->Mutex));	

    if (iErr == 0)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pFileName || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgQueue是否为有效指针和消息队列是否已满
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListFull(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //申请内存
        pPkt = (GUIOBJ *) GuiMemAlloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -3;
        }
    }

	if (iErr == 0)
    {
    	pTmp = (char *)GuiMemAlloc(strlen(pFileName)+1);
		if(NULL == pTmp)
		{
			iErr = -4;
		}
	}

    if (iErr == 0)
    {
    	strcpy(pTmp, pFileName);
        //添加消息项到消息队列
        pPkt->pObj = pTmp;
        if (ListAdd(&(pPkt->List), pMsgObj->pMsgQueue))
        {
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -4:
        GuiMemFree(pPkt);
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return iErr;
}


/***
  * 功能：
        清空消息队列
  * 参数：
        1.GUIMESSAGE *pMsgObj:      需要被清空消息队列的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        必须在持有锁（消息对象中的互斥锁）的前提下才能调用，否则会导致竞态
***/
int ClearQueue(MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;
	char *pTmp = NULL;

	//加锁
    MutexLock(&(pMsgObj->Mutex));

    if (iErr == 0)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断事件队列pMsgQueue是否为有效指针
        if (NULL == pMsgObj->pMsgQueue)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //遍历删除消息队列项
        while (pMsgObj->pMsgQueue->iCount)
        {
            pPkt= container_of(pMsgObj->pMsgQueue->pHead, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);

            if (ListDel(pMsgObj->pMsgQueue->pHead, pMsgObj->pMsgQueue))
            {
                iErr = -3;
                break;
            }

            //释放资源
            GuiMemFree(pTmp);
            GuiMemFree(pPkt);
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));	

    return iErr;
}


int ShowQueue(MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
	char *pTmp = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);
        }
    }

    return iErr;
}


/***
  * 功能：
        多选状态下删除文件
  * 参数：
        1.GUIMESSAGE *pMsgObj:      当前多选队列对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DeleteMultiSelect(MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
	char *pTmp = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
	char cCmdBuff[512];

	//加锁
    MutexLock(&(pMsgObj->Mutex));	

    if (iErr == 0)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);
			memset(cCmdBuff, 0, sizeof(cCmdBuff));
			snprintf(cCmdBuff, sizeof(cCmdBuff), "rm -rf -- \"%s\"", pTmp);
			mysystem(cCmdBuff);
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));		

    return iErr;
}

/***
  * 功能：
        检查是否存在同名文件
  * 参数：
        1.GUIMESSAGE *pMsgObj:      当前多选队列对象
  * 返回：
        存在同名文件返回0
        不存在同名文件返回非0
  * 备注：
***/
int CheckSameName(MULTISELECT *pMsgObj, const char* dir)
{
    //错误标志、返回值定义
    int iErr = 0;
	char *pTmp = NULL;
	char *pTmp1 = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;

	//加锁
    MutexLock(&(pMsgObj->Mutex));	

    if (iErr == 0)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);
			/* 由于存贮在链表中的是全路径 */
			pTmp1 = strrchr(pTmp, '/');
			if(NULL != pTmp1)
			{
			    char path[MAXPATHNAME];
                strcpy(path, dir);
                strcat(path, pTmp1);
                if (access(path, F_OK) != -1)
                {
                    break;
                }
			}	
        }

		/* 不存在同名文件 */
        if (iPos == -1)
        {
            iErr = -3;
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));		

    return iErr;
}

/***
  * 功能：
        多选状态下复制文件
  * 参数：
        1.GUIMESSAGE *pMsgObj:      当前多选队列对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int CopyMultiSelect(MULTISELECT *pMsgObj, const char *pDestDirectory)
{
    //错误标志、返回值定义
    int iErr = 0;
	char *pTmp = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
	char cCmdBuff[512];

	//加锁
    MutexLock(&(pMsgObj->Mutex));	

    if (iErr == 0)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);
			memset(cCmdBuff, 0, sizeof(cCmdBuff));
			snprintf(cCmdBuff, sizeof(cCmdBuff), "cp -Rf -- \"%s\" \"%s\"", pTmp, pDestDirectory);
			mysystem(cCmdBuff);
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));		

    return iErr;
}


/*static void ReCreateWindow(GUIWINDOW **pWnd)
{

    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmOtdrInit, FrmOtdrExit, 
                        FrmOtdrPaint, FrmOtdrLoop, 
				        FrmOtdrPause, FrmOtdrResume,
                        NULL);          //pWnd由调度线程释放
}*/


/***
  * 功能：
        将OTDR界面的帧缓冲保存下来，以供后面截图使用
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SaveFrmBuff(void)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GUIFBMAP *pFbmap = NULL;	//帧缓冲对象
	UINT16 *pFrmBuf = NULL;   	//指向帧缓存入口的指针

	if(iReturn == 0)
    {    
        /* 得到当前帧缓冲映射对象并判断是否为有效指针 */
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iReturn = -2;
        }
    }

	if(iReturn == 0)
    {   
    	/* 当前帧缓冲入口地址 */
    	pFrmBuf = pFbmap->pMapBuff;
		
        /* 复制出当前帧缓冲中的内容 */
        if (NULL != __g_usOTDRFrmBuff)
        {
            memcpy(__g_usOTDRFrmBuff, pFrmBuf, FRMBUFFSIZE);
			__g_ucEnterOtdrFlg = 1;
        }
	}

	return iReturn;
}


/***
  * 功能：
        获得OTDR界面保存的帧缓冲
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
unsigned short *GetOTDRFrmBuff(void)
{
	return __g_usOTDRFrmBuff;
}


/***
  * 功能：
        获得进入过OTDR界面的标志
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		如果用户没有进入过OTDR界面，而此时进入文件浏览器进行截屏操作，
  		则为无效操作，此时提示用户需先进入OTDR界面
***/
unsigned char GetEnterOtdrFlg(void)
{
	return __g_ucEnterOtdrFlg;
}


/***
  * 功能：
        设置需要截屏的OTDR窗体对象
  * 参数：
        1.unsigned char ucWndtype:   设置需要截屏的窗体
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int g_ucWndtype = 0;
int SetSaveOtdrWin(int ucWndtype)
{
	int iErr = 0;

	g_ucWndtype = ucWndtype;
	
	return iErr;
}


/***
  * 功能：
        在文件浏览器中保存图片的线程
  * 参数：
        1.void *pThreadArg:    线程参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
void *SavePictureThreadFunc(void *pThreadArg)
{
	int iErr = 0;
	
	//char cBmpFileName[512] = {0};
	//char cJpgFileName[512] = {0};
	GUIFBMAP *pFbmap = NULL;	//帧缓冲对象
	UINT16 *pFrmBuf = NULL;   	//指向帧缓存入口的指针	

    //临时变量定义
    GUIPEN *pPen;
	unsigned int uiColorBack;

	static GUILABEL *pScreenShotsLblInfo = NULL;	
	static GUICHAR  *pScreenShotsStrInfo = NULL; 

	//pScreenShotsStrInfo = GetTextResource(DIALOG_INFO_SAVE_SCREESHORTS, lCurrentLanguage);
	pScreenShotsLblInfo = CreateLabel(10, 5, 500, 24, pScreenShotsStrInfo);
	
	while(1)
	{
		/* 获取信号量，开始进行截屏 */
		//sem_wait(&SemSavePicture);

		/* 显示截屏界面 */
		if(iErr == 0)
	    {    
	        /* 得到当前帧缓冲映射对象并判断是否为有效指针 */
	        pFbmap = GetCurrFbmap();
	        if (NULL == pFbmap)
	        {
	            iErr = -2;
	        }
	    }

		if(iErr == 0)
	    {   
	    	/* 当前帧缓冲入口地址 */
	    	pFrmBuf = pFbmap->pMapBuff;
			
	        /* 复制出当前帧缓冲中的内容 */
	        if (NULL != pFrmBuf)
	        {
	            memcpy(pFrmBuf, __g_usOTDRFrmBuff, FRMBUFFSIZE);
	        }
		}		

		/* 显示正在保存屏幕截图...... */
		pPen = GetCurrPen();
		uiColorBack = pPen->uiPenColor;
		pPen->uiPenColor = 0x636466; 
		DrawBlock(0, 0, 799, 27);
		pPen->uiPenColor = uiColorBack;		
		//pScreenShotsStrInfo = GetTextResource(DIALOG_INFO_SAVE_SCREESHORTS, lCurrentLanguage);
		SetLabelText(pScreenShotsStrInfo, pScreenShotsLblInfo);
		DisplayLabel(pScreenShotsLblInfo);
		RefreshScreen(__FILE__, __func__, __LINE__);
		
		/* 获取bmp文件名 */
		//GetNewFileName(cBmpFileName, getcwd(NULL, 0), OTDRWND, BMPFILE, FULLNAME);

		/* 获取jpg文件名 */	
		//GetNewFileName(cJpgFileName, getcwd(NULL, 0), OTDRWND, JPGFILE, FULLNAME);

		switch(iLabelSaveFileTypeNum)
		{
			case 1:
				/* 保存图片 */
				//iErr = ScreenShots(0, 797, 0, 479, cBmpFileName, NULL);
				break;
			case 2:
				/* 保存图片 */
				//iErr = ScreenShots(0, 797, 0, 479, cBmpFileName, cJpgFileName);
				break;
			default:
				break;
		}
		
		/* 返回文件浏览器界面 */
	    //FileBrowseInit(g_ucActionIndex, 1, 1, GetCurrWindow(), ReCreateWindow, 0);		
	}
}


/***
  * 功能：
        创建实现在文件浏览器中保存图片的线程
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int CreateSavePicture(void)
{
	int iReturn = 0;

	pthread_t thdMessage = -1;
    pthread_attr_t attr;

	//初始化信号量
	//sem_init(&SemSavePicture, 0, 0);

	/* 
	 * 此线程为可分离的 
	 */
    if ( (iReturn = pthread_attr_init(&attr)) != 0)
	{
		 return iReturn;
	}
    iReturn = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (iReturn == 0)
	{
		iReturn = pthread_create(&thdMessage, &attr, SavePictureThreadFunc, NULL);
		if(iReturn != 0)
		{
			LOG(LOG_ERROR, "can't create thread\n");
			pthread_attr_destroy(&attr);
		}
	}

    return 0;	
}


/***
  * 功能：
        将文件的文件名升序排序
  * 参数：
        1.const void *a:      第一个元素
        1.const void *b:      第二个元素
  * 返回：
        返回文件名比较结果
  * 备注：
  		不区分文件名大小写
***/
static int FileNameASCE(const void *a, const void *b)
{
	if(a && b)
	{
		struct dirent **dira = (struct dirent **)a;
		struct dirent **dirb = (struct dirent **)b;
		
		return strcasecmp((*dira)->d_name, (*dirb)->d_name);		
	}
	else
	{
		return 0;
	}
}


/***
  * 功能：
        将文件的文件名降序排序
  * 参数：
        1.const void *a:      第一个元素
        1.const void *b:      第二个元素
  * 返回：
        返回文件名比较结果
  * 备注：
  		不区分文件名大小写
***/
static int FileNameDESC(const void *a, const void *b)
{
	if(a && b)
	{
		struct dirent **dira = (struct dirent **)a;
		struct dirent **dirb = (struct dirent **)b;
		
		return strcasecmp((*dirb)->d_name, (*dira)->d_name);		
	}
	else
	{
		return 0;
	}
}


/***
  * 功能：
        将文件内容修改时间按升序排列
  * 参数：
        1.const void *a:      第一个元素
        1.const void *b:      第二个元素
  * 返回：
        返回文件内容修改时间比较结果
  * 备注：
***/
static int FileChangedTimeASCE(const void *a, const void *b)
{
	if(a && b)
	{
		struct dirent **dira = (struct dirent **)a;
		struct dirent **dirb = (struct dirent **)b;

		struct stat statbufa;
		struct stat statbufb;
		
		lstat((*dira)->d_name, &statbufa);
		lstat((*dirb)->d_name, &statbufb);

		return (statbufa.st_mtime - statbufb.st_mtime);		
	}
	else
	{
		return 0;
	}
}

/***
  * 功能：
        将文件内容修改时间按降序排列
  * 参数：
        1.const void *a:      第一个元素
        1.const void *b:      第二个元素
  * 返回：
        返回文件内容修改时间比较结果
  * 备注：
***/
static int FileChangedTimeDESC(const void *a, const void *b)
{
	if(a && b)
	{
		struct dirent **dira = (struct dirent **)a;
		struct dirent **dirb = (struct dirent **)b;

		struct stat statbufa;
		struct stat statbufb;
		
		lstat((*dira)->d_name, &statbufa);
		lstat((*dirb)->d_name, &statbufb);

		return (statbufb.st_mtime - statbufa.st_mtime);		
	}
	else
	{
		return 0;
	}
}


/***
  * 功能：
        合并文件和目录结构到一个目录结构数组中
  * 参数：
        1.FILESORT enSort:	将要设置的排序方法
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int MergeFileAndFolder(FILESORT enSort)
{
	int iErr = 0;
	int i = 0;

	switch(enSort)
	{
		/* 升序排列，目录靠前 */
		case NAMEASEC:
		case TIMEASEC:
			if(NULL != __g_Namelist)
			{
				if(NULL != __g_Namelist_Folder)
				{
					for(i=0; i<__g_iFolderNum; i++)
					{
						__g_Namelist[i] = __g_Namelist_Folder[i];
					}
				}
				else
				{
					i = 0;
				}
			
				if(NULL != __g_Namelist_NotFolder)
				{
					for(; i<__g_iFolderNum+__g_iNotFolderNum; i++)
					{
						__g_Namelist[i] = __g_Namelist_NotFolder[i-__g_iFolderNum];
					}
				}
			}	

			break;
			
		/* 降序排列，非目录文件靠前 */	
		case NAMEDESC:
		case TIMEDESC:
			if(NULL != __g_Namelist)
			{
				if(NULL != __g_Namelist_NotFolder)
				{
					for(i=0; i<__g_iNotFolderNum; i++)
					{
						__g_Namelist[i] = __g_Namelist_NotFolder[i];
					}
				}
				else
				{
					i = 0;
				}
			
				if(NULL != __g_Namelist_Folder)
				{
					for(; i<__g_iFolderNum+__g_iNotFolderNum; i++)
					{
						__g_Namelist[i] = __g_Namelist_Folder[i-__g_iNotFolderNum];
					}
				}
			}			
			break;	
		default:
			break;				
	}	

	return iErr;
}

/***
  * 功能：
        对已经打开的目录里的文件进行排序
  * 参数：
        1.FILESORT enSort:	将要设置的排序方法
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SortDirectoryFiles(struct dirent** dirents, int count, FILESORT enSort)
{
	if (dirents == NULL)
		return -1;
	
	switch(enSort)
	{
		/* 文件名升序排列 */
		case NAMEASEC:
			qsort(dirents, count, sizeof(struct dirent *), FileNameASCE);
			break;
			
		/* 文件名降序排列 */	
		case NAMEDESC:
			qsort(dirents, count, sizeof(struct dirent *), FileNameDESC);
			break;	

		/* 文件内容修改时间升序排列 */	
		case TIMEASEC:
			qsort(dirents, count, sizeof(struct dirent *), FileChangedTimeASCE);
			break;	

		/* 文件内容修改时间降序排列 */	
		case TIMEDESC:
			qsort(dirents, count, sizeof(struct dirent *), FileChangedTimeDESC);
			break;	
		default:
			break;				
	}
	return 0;
}

/***
  * 功能：
        设置当前的文件排序方法
  * 参数：
        1.FILESORT enSort:			将要设置的排序方法
		2.unsigned int uiMaxFileNum:当前页最大显示文件数量
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetFileSortMethod(FILESORT enSort, unsigned int uiMaxFileNum)
{
	int iErr = 0;
	int i = 0;

	if(iErr == 0)
	{
#if 0
		switch(enSort)
		{
			/* 文件名升序排列 */
			case NAMEASEC:
				qsort(__g_Namelist_Folder, __g_iFolderNum, 
						sizeof(struct dirent *), FileNameASCE);
				qsort(__g_Namelist_NotFolder, __g_iNotFolderNum, 
						sizeof(struct dirent *), FileNameASCE);
				break;
				
			/* 文件名降序排列 */	
			case NAMEDESC:
				qsort(__g_Namelist_Folder, __g_iFolderNum, 
						sizeof(struct dirent *), FileNameDESC);
				qsort(__g_Namelist_NotFolder, __g_iNotFolderNum, 
						sizeof(struct dirent *), FileNameDESC);				
				break;	

			/* 文件内容修改时间升序排列 */	
			case TIMEASEC:
				qsort(__g_Namelist_Folder, __g_iFolderNum, 
						sizeof(struct dirent *), FileChangedTimeASCE);
				qsort(__g_Namelist_NotFolder, __g_iNotFolderNum, 
						sizeof(struct dirent *), FileChangedTimeASCE);				
				break;	

			/* 文件内容修改时间降序排列 */	
			case TIMEDESC:
				qsort(__g_Namelist_Folder, __g_iFolderNum, 
					sizeof(struct dirent *), FileChangedTimeDESC);
				qsort(__g_Namelist_NotFolder, __g_iNotFolderNum, 
					sizeof(struct dirent *), FileChangedTimeDESC);				
				break;	
			default:
				break;				
		}
#endif
		SortDirectoryFiles(__g_Namelist_Folder, __g_iFolderNum, enSort);
		SortDirectoryFiles(__g_Namelist_NotFolder, __g_iNotFolderNum, enSort);
		
		MergeFileAndFolder(enSort);

		/* 获取每页能实际显示的项目数 */
		if (__g_iTotalNum <= uiMaxFileNum)
		{
			__g_iPageFileNum = __g_iTotalNum;
		}
		else
		{
			__g_iPageFileNum = uiMaxFileNum;
		}

		/* 更新每页文件结构的指针数组 */
		for(i=0; i<__g_iPageFileNum; i++)
		{	
			__g_NamelistPage[i] = __g_Namelist[i];
		}		
		for (; i < uiMaxFileNum; i++)
		{
			__g_NamelistPage[i] = NULL;
		}
		/* 更新文件位置 */
		__g_iCurFilePos = __g_iPageFileNum;		
	}

	return iErr;
}


/***
  * 功能：
        设置当前的文件排序方法
  * 参数：
        1.FILESORT enSort:			将要设置的排序方法
		2.unsigned int uiMaxFileNum:当前页最大显示文件个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurSortMethod(FILESORT enSort, unsigned int uiMaxFileNum)
{
	int iErr = 0;

	__g_enCurSortMethod = enSort;
	SetFileSortMethod(__g_enCurSortMethod, uiMaxFileNum);

	return iErr;
}


/***
  * 功能：
        获取当前的文件排序方法
  * 参数：
        无
  * 返回：
        返回当前的文件排序方法
  * 备注：
***/
FILESORT GetCurSortMethod(void)
{
	return __g_enCurSortMethod;
}


/***
  * 功能：
        设置当前的文件过滤类型
  * 参数：
        1.FILETYPE enFilter:	将要设置的文件过滤类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurFileType(FILETYPE enFilter)
{
	int iErr = 0;

	__g_enCurFileType = enFilter;
	
	return iErr;
}


/***
  * 功能：
        获取当前的文件过滤类型
  * 参数：
        无
  * 返回：
        返回当前的文件过滤类型
  * 备注：
***/
FILETYPE GetCurFileType(void)
{
	return __g_enCurFileType;
}


/***
  * 功能：
        返回当前的文件数目
  * 参数：
        无
  * 返回：
        返回当前的文件数目
  * 备注：
***/
int GetCurFileNum(void)
{
	return __g_iTotalNum;
}


/***
  * 功能：
        返回当前目录下所有文件的目录结构体数组
  * 参数：
        无
  * 返回：
        返回当前的文件目录结构体数组
  * 备注：
***/
struct dirent ** GetCurFileNamelist(void)
{
	return __g_Namelist;
}


/***
  * 功能：
        返回存放每页文件结构的指针数组
  * 参数：
        无
  * 返回：
        返回存放每页文件结构的指针数组
  * 备注：
***/
struct dirent ** GetCurPageFileNamelist(void)
{
	return __g_NamelistPage;
}


/***
  * 功能：
        返回当前目录下每页能实际显示的项目数
  * 参数：
        无
  * 返回：
        返回当前目录下每页能实际显示的项目数
  * 备注：
***/
int GetActulPageFileNum(void)
{
	return __g_iPageFileNum;
}


/***
  * 功能：
        过滤目录
  * 参数：
        1、const struct dirent *dir:	目录结构
  * 返回：
        目录文件返回1
        非目录文件返回0
  * 备注：
***/
static int FilterFileTypeFolder(const struct dirent *dir)
{
	int iErr = 0;
	struct stat statbuf;

	if(NULL == dir)
	{
		return 0;
	}

	if(strcmp(".",dir->d_name) == 0)	  					
	{
		return 0;
	}
	if(strcmp("..",dir->d_name) == 0)	  					
	{
		return 0;
	}	

	iErr = lstat(dir->d_name, &statbuf);	

	if(iErr == 0)
	{
		if( S_ISDIR(statbuf.st_mode) )	//判断是目录
		{
			return 1;
		}
		else
		{
			return 0;
		}	
	}
	else
	{
		return 0;
	}
}


/***
  * 功能：
        根据设置的过滤类型过滤文件
  * 参数：
        1、const struct dirent *dir:	目录结构
  * 返回：
        满足过滤类型返回1
        不满足过滤类型返回0
  * 备注：
***/
static int FilterFileTypeNotFolder(const struct dirent *dir)
{
	struct stat statbuf;
	char *pExtension = NULL;
	char cExtension[10];
	int iErr = 0;	

	if (NULL == dir)
	{
		return 0;
	}	

	iErr = lstat(dir->d_name, &statbuf);
	if(iErr == 0)
	{
		if (S_ISDIR(statbuf.st_mode) ||			//判断是目录
		    (__g_enCurFileType == NONE) )		//只需要显示目录	
		{
			return 0;
		}
		else
		{
			switch(__g_enCurFileType)
			{
			case SOR:
				strcpy(cExtension, "SOR");
				break;
			case BMP:
				strcpy(cExtension, "BMP");
				break;	
			case JPG:
				strcpy(cExtension, "JPG");
				break;	
			case SOLA:
				strcpy(cExtension, "SOLA");
				break;	
			case HTML:
				strcpy(cExtension, "HTML");
				break;	
			case TXT:
				strcpy(cExtension, "TXT");
				break;	
			case GDM:
				strcpy(cExtension, "GDM");
				break;
			case PDF:
				strcpy(cExtension, "PDF");
				break;
				break;						
            case BIN:
                strcpy(cExtension, "BIN");
                break;
            case CUR:
                strcpy(cExtension, "CUR");
                break;
            case SORANDCUR:
                strcpy(cExtension, "SORCUR");
                break;
			case ANY:
				strcpy(cExtension, "ALL");
				break;	
			default :
				break;
			}
			
			if (strcasecmp(cExtension, "ALL") == 0)
			{
				return 1;
			}
			else
			{
				pExtension = strrchr(dir->d_name, '.');
				if ((NULL != pExtension) && 
                    (pExtension != dir->d_name))
				{
					pExtension++;
					if(strcasecmp(cExtension, "SORCUR") == 0)
					{
                        return (strcasecmp(pExtension, "SOR") == 0 || strcasecmp(pExtension, "CUR") == 0);
					}
					else
					{
					    return (strcasecmp(cExtension, pExtension) == 0);
					}
         		}
				/* 是隐藏文件或没有扩展名 */
				else
				{
					return 0;
				}
			}
		}		
	}	
	else
	{
		return 0;
	}
}


/***
  * 功能：
        获取文件的扩展名
  * 参数：
        1、const char *pFileName:	文件名
  * 返回：
		成功返回扩展名的枚举
  * 备注：
***/
FILETYPE GetFileExtension(const char *pFileName)
{
	int iErr = 0;
	char *pTmp = NULL;
	int i = 0;
	char cExtension[10];
	FILETYPE enExtension = OTHER;
	if(iErr == 0)
	{
		if(NULL == pFileName)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		/* 是隐藏文件*/
		if(pFileName[0] == '.')   //如果是隐藏文件则直接返回
		{
			iErr = -2;
		}
	}


	if(iErr == 0)
	{
		pTmp = strrchr(pFileName, '.');   
		/* 是没有扩展名 */
		if(pTmp == 0)   //如果是没有扩展名，则直接返回
		{
			iErr = -3;
		}
	}

	if(iErr == 0)
	{
		pTmp++;

		for(i=0; pTmp[i]!='\0'; i++)
		{
			cExtension[i] = pTmp[i];			
		}
		cExtension[i] = '\0';
	}

	if(iErr == 0)
	{
		/* 不区分大小写比较扩展名 */
		if(strcasecmp(cExtension, "SOR") == 0)
		{
			enExtension = (FILETYPE)SOR;
		}
		else if(strcasecmp(cExtension, "BMP") == 0)
		{
			enExtension = (FILETYPE)BMP;
		}
		else if(strcasecmp(cExtension, "JPG") == 0)
		{
			enExtension = (FILETYPE)JPG;
		}
		else if(strcasecmp(cExtension, "SOLA") == 0)
		{
			enExtension = (FILETYPE)SOLA;
		}		
		else if(strcasecmp(cExtension, "HTML") == 0)
		{
			enExtension = (FILETYPE)HTML;
		}		
		else if(strcasecmp(cExtension, "TXT") == 0)
		{
			enExtension = (FILETYPE)TXT;
		}	
		else if(strcasecmp(cExtension, "GDM") == 0)
		{
			enExtension = (FILETYPE)GDM;
		}		
		else if(strcasecmp(cExtension, "PDF") == 0)
		{
			enExtension = (FILETYPE)PDF;
		}		
		else if(strcasecmp(cExtension, "INNO") == 0)//在显示类型和打开时都用到该函数，为了类型不显示并且能打开保留
		{
		    #ifndef FORC
			enExtension = (FILETYPE)INNO;
            #endif
		}
        else if(strcasecmp(cExtension, "CUR") == 0)
		{
			enExtension = (FILETYPE)CUR;
		}	
		else if(strcasecmp(cExtension, "BIN") == 0)
		{
			enExtension = (FILETYPE)BIN;
		}		
		else
		{
			enExtension = (FILETYPE)OTHER;
		}
	}

	switch(iErr)
	{
	    case -3:
		case -2:
		case -1:
			enExtension = (FILETYPE)NONE;
			break;
		default :
			break;
	}

	return enExtension;
}


/***
  * 功能：
        遍历指定路径的目录下的文件
  * 参数：
        1.const char *pFilePath:      需要遍历的路径
		2.unsigned int uiMaxFileNum:  当前页最大显示文件个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ScanDirectory(const char *pFilePath, unsigned int uiMaxFileNum)
{
	int iErr = 0;
	unsigned long ulSize = 0;
	
	if(iErr == 0)
	{
		if(NULL == pFilePath)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		/* 将欲打开的目录设为当前的工作目录
		 * 若没有切换工作目录，则调用lstat函数会失败
		 */
		if( chdir(pFilePath) )
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		/* 释放上一次打开目录申请的内存 */
		GuiMemFree(__g_Namelist_Folder);
		GuiMemFree(__g_Namelist_NotFolder);
		GuiMemFree(__g_Namelist);

		/* 遍历该目录，将该目录下的文件夹存于__g_Namelist_Folder中 */
		__g_iFolderNum = scandir(pFilePath, &__g_Namelist_Folder, FilterFileTypeFolder, 0);
		if(__g_iFolderNum < 0)
		{
			__g_iFolderNum = 0;
			__g_Namelist_Folder = NULL;
		}
		/* 遍历该目录，将该目录下的非目录文件存于__g_Namelist_NotFolder中 */
		__g_iNotFolderNum = scandir(pFilePath, &__g_Namelist_NotFolder, FilterFileTypeNotFolder, 0);
		if(__g_iNotFolderNum < 0)
		{
			__g_iNotFolderNum = 0;
			__g_Namelist_NotFolder = NULL;
		}

		/* 申请指针数组，存放该目录下所有文件的目录结构 */
		__g_iTotalNum = __g_iFolderNum + __g_iNotFolderNum;
		
		ulSize = __g_iTotalNum*sizeof(struct dirent *);
		__g_Namelist = (struct dirent **)GuiMemAlloc(ulSize);
		SetFileSortMethod(__g_enCurSortMethod, uiMaxFileNum);
	}	

	return iErr;
}

/***
  * 功能：
        向前移动一个条目
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int LastItem(int *pSelectItem)
{
	int iErr = 0;
	int i = 0;

	/* 获取目录结构数组地址 */
	if(iErr == 0)
	{
		if(NULL == __g_Namelist)
		{
			iErr = -1;
		}
	}	

	if(iErr ==0)
	{
		(*pSelectItem)--;
		
		if( (*pSelectItem) < 0 )
		{
			*pSelectItem = 0;
			/* 可以向上移动 */
			if(__g_iCurFilePos > __g_iPageFileNum)
			{
				/* 更新每页文件结构的指针数组 */
				for(i=0; i<__g_iPageFileNum; i++)
				{
					__g_NamelistPage[i] = __g_Namelist[i-1+__g_iCurFilePos-__g_iPageFileNum];
				}	
				for(; i<MAXPAGEFILENUM; i++)
				{
					__g_NamelistPage[i] = NULL;
				}

				/* 更新文件位置 */
				__g_iCurFilePos--;
			}
			/* 当前选中的条目已经移动到头部 */
			else
			{
				__g_iCurFilePos = __g_iPageFileNum;
				iErr = -3;
			}			
		}
	}

	return iErr;	
}


/***
  * 功能：
        向后移动一个条目
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int NextItem(int *pSelectItem)
{
	int iErr = 0;
	int i = 0;

	/* 获取目录结构数组地址 */
	if(iErr == 0)
	{
		if(NULL == __g_Namelist)
		{
			iErr = -1;
		}
	}	

	if(iErr ==0)
	{
		(*pSelectItem)++;
		/* 超过当前显示页 */
		if(*pSelectItem >= __g_iPageFileNum)
		{
			if(__g_iCurFilePos < __g_iTotalNum)
			{
				*pSelectItem = __g_iPageFileNum-1;
				/* 更新每页文件结构的指针数组 */
				for(i=0; i<__g_iPageFileNum; i++)
				{
					__g_NamelistPage[i] = __g_Namelist[i+1+__g_iCurFilePos-__g_iPageFileNum];
				}		
				for(; i<MAXPAGEFILENUM; i++)
				{
					__g_NamelistPage[i] = NULL;
				}
				
				/* 更新文件位置 */
				__g_iCurFilePos++;
			}
			/* 当前选中的条目已经移动到尾部 */
			else
			{
				*pSelectItem = __g_iPageFileNum-1;
				__g_iCurFilePos = __g_iTotalNum;
				iErr = -2;
			}
		}    	
	}

	return iErr;	
}


/***
  * 功能：
        向后翻页
  * 参数：
        1、int *pSelectItem:			当前选中项
		2、unsigned int uiMaxFileNum:	每页最大显示个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int PageDown1(int *pSelectItem, unsigned int uiMaxFileNum)
{
	int iErr = 0;
	int i = 0;
	int iTmpPos = 0;	
	/* 当前目录下的所有文件能够显示在一页上，不需要翻页 */
	if(iErr == 0)
	{
		if (__g_iTotalNum <= uiMaxFileNum)
		{
			iErr = -1;
		}
	}	

	/* 获取目录结构数组地址 */
	if(iErr == 0)
	{
		if(NULL == __g_Namelist)
		{
			iErr = -2;
		}
	}	

	/* 该目录下的文件要多页显示 */
	if(iErr ==0)
	{
		/* 当前选中的条目不是在一页的最后一个位置 */
		if ((*pSelectItem) != (uiMaxFileNum - 1))
		{
			*pSelectItem = uiMaxFileNum - 1;
		}
		/* 当前选中的条目在一页的最后一个位置 */
		else
		{
			//回到当前页的第一个位置
			*pSelectItem = 0;
			iTmpPos = __g_iCurFilePos + uiMaxFileNum;
			/* 不是最后一页 */
			if(iTmpPos < __g_iTotalNum)
			{
				iTmpPos = __g_iCurFilePos;
				__g_iCurFilePos += uiMaxFileNum;
				__g_iPageFileNum = uiMaxFileNum;
			}
			/* 是最后一页 */
			else
			{
				__g_iPageFileNum = __g_iTotalNum - __g_iCurFilePos;
				iTmpPos = __g_iCurFilePos;
// 				iTmpPos = __g_iTotalNum - MAXPAGEFILENUM;
				__g_iCurFilePos = __g_iTotalNum;
			}

			/* 更新每页文件结构的指针数组 */
			for(i=0; i<__g_iPageFileNum; i++)
			{
				__g_NamelistPage[i] = __g_Namelist[iTmpPos+i];
			}	
			for (; i < uiMaxFileNum; i++)
			{
				__g_NamelistPage[i] = NULL;
			}
		}
	}

	return iErr;
}


/***
  * 功能：
        向前翻页
  * 参数：
        1、int *pSelectItem:			当前选中项
		2、unsigned int uiMaxFileNum:	每页最大显示个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int PageUp1(int *pSelectItem, unsigned int uiMaxFileNum)
{
	int iErr = 0;
	int i = 0;
	int iTmpPos = 0;

	/* 当前目录下的所有文件能够显示在一页上，不需要翻页 */
	if(iErr == 0)
	{
		if (__g_iTotalNum <= uiMaxFileNum)
		{
			iErr = -1;
		}
	}	

	/* 获取目录结构数组地址 */
	if(iErr == 0)
	{
		if(NULL == __g_Namelist)
		{
			iErr = -2;
		}
	}	

	/* 该目录下的文件要多页显示 */
	if(iErr ==0)
	{
		/* 当前选中的条目不是在一页的开始位置 */
		if((*pSelectItem) != 0)
		{
			*pSelectItem = 0;
		}
		else
		{
			iTmpPos = __g_iCurFilePos - uiMaxFileNum;
			*pSelectItem = uiMaxFileNum - 1;
			/* 回到第一页 */
			if(iTmpPos <= 0)
			{
				iTmpPos = 0;
				__g_iCurFilePos = uiMaxFileNum;
			}
			/* 不是第一页 */
			else
			{
				__g_iPageFileNum = uiMaxFileNum;
				iTmpPos = __g_iCurFilePos - 2 * uiMaxFileNum;
				/* 回到第一页 */
				if(iTmpPos <= 0)
				{
					iTmpPos = 0;
					__g_iCurFilePos = uiMaxFileNum;
				}
				/* 不是第一页 */
				else
				{
					if (__g_iCurFilePos < 3 * uiMaxFileNum)
					{
						iTmpPos = __g_iCurFilePos - (uiMaxFileNum + 1);
					}
					else
					{
						iTmpPos = __g_iCurFilePos - 2 * uiMaxFileNum;
					}
					__g_iCurFilePos -= uiMaxFileNum;
				}
			}
			/* 更新每页文件结构的指针数组 */
			for(i=0; i<__g_iPageFileNum; i++)
			{
				__g_NamelistPage[i] = __g_Namelist[iTmpPos+i];
			}	
			for (; i < uiMaxFileNum; i++)
			{
				__g_NamelistPage[i] = NULL;
			}
		}					
	}

	return iErr;
}


/***
  * 功能：
        进入图片浏览器之前，遍历当前目录下的图片名字，并存入链表中
  * 参数：
        无
  * 返回：
        无
  * 备注：
  		根据当前选择的文件类型，若是bmp，则只读入bmp图片，
  		若是jpg，则只读入jpg图片，若是*类型，则读入bmp和jpg图片
***/
void FillPictureQueue(void)
{
	int i = 0;
	char cPathBuff[512];
	char cTmpPathBuff[512];
	FILETYPE enExtension;

	for(i=0; i<__g_iNotFolderNum; i++)
	{ 
		switch(__g_enCurFileType)
		{
			case BMP:
			case JPG:
				getcwd(cPathBuff, 512);
				sprintf(cTmpPathBuff, "%s/%s", cPathBuff, __g_Namelist_NotFolder[i]->d_name);
				//WriteQueue(cTmpPathBuff, pPictureQueue);
				break;
			case SOR:	
			case OTHER:
			case NONE:
			case ANY:
				enExtension = GetFileExtension(__g_Namelist_NotFolder[i]->d_name);
				if( (enExtension==BMP) || (enExtension==JPG) )
				{
					getcwd(cPathBuff, 512);
					sprintf(cTmpPathBuff, "%s/%s", cPathBuff, __g_Namelist_NotFolder[i]->d_name);
					//WriteQueue(cTmpPathBuff, pPictureQueue);
				}
				break;
			default :						
				break;					
		}
		
	}
}


/***
  * 功能：
        检测是否挂载SD卡或U盘
  * 参数：
        无
  * 返回：
        0:	不需要重新打开/mnt目录				
        -1:	需要重新打开/mnt目录				
  * 备注：
  		此函数一般放在loop当中，循环检测SD或USB是否插拔；
  		对于插入事件，如果在/mnt目录下，则要重新打开/mnt目录
  		如果不在/mnt目录下，则不需要重新打开/mnt目录；
  		对于拔出事件，则要重新打开/mnt目录
***/
int DetectionSD(void)
{
	int iErr = 0;

	static unsigned char ucSDSta = 0;			//0: SD卡不存在；1: SD卡存在 
	static unsigned char ucLastSDSta = 0;		//上一次SD卡的状态
	static unsigned char ucUSBSta = 0;			//0: USB不存在；1: USB存在 
	static unsigned char ucLastUSBSta = 0;		//上一次USB的状态
	char *pPwdPath = NULL;

	/* SD卡已经挂载了，因为此函数被重复调用，可能有两种情况 
	 * 1、SD卡首次插入，如果此时在/mnt目录下，则需要重新打开/mnt目录；
	   2、SD卡已经插入，不论在什么目录，则不需要重新打开/mnt目录
	 */
	if( access("/tmp/sdcard", F_OK) == 0)
	{
		ucSDSta = 1;
		/* 获得当前目录 */	
		pPwdPath = getcwd(NULL, 0);
		if(NULL != pPwdPath)
		{
			/* 此时在/mnt目录下 */
			if(strcmp(pPwdPath, MntUpDirectory) == 0) 
			{
				/* 和上次SD卡的状态比较，不相同，重新打开/mnt目录 */
				if(ucSDSta != ucLastSDSta)
				{
					ucLastSDSta = ucSDSta;
					iErr = -1;
				}
			}
			GuiMemFree(pPwdPath);
		}
	}
	/* SD卡没有挂载 */
	else		
	{
		ucSDSta = 0;
		/* 和上次SD卡的状态不同 */
		if(ucSDSta != ucLastSDSta)	
		{
			ucLastSDSta = ucSDSta;
			iErr = -1;
		}
	}

	/* USB卡已经挂载了，因为此函数被重复调用，可能有两种情况 
	 * 1、USB首次插入，如果此时在/mnt目录下，则需要重新打开/mnt目录；
	   2、USB已经插入，不论在什么目录，则不需要重新打开/mnt目录
	 */
	if( access("/tmp/usb", F_OK) == 0) 			
	{
		ucUSBSta = 1;
		/* 获得当前目录 */	
		pPwdPath = getcwd(NULL, 0);
		if(NULL != pPwdPath)
		{
			/* 此时在/mnt目录下 */
			if(strcmp(pPwdPath, MntUpDirectory) == 0)		
			{
				/* 和上次USB的状态不同 */
				if(ucUSBSta != ucLastUSBSta)		
				{
					ucLastUSBSta = ucUSBSta;
					iErr = -1;
				}
			}	
			GuiMemFree(pPwdPath);
		}
	}
	else										
	{
		ucUSBSta = 0;
		/* 和上次USB的状态不同 */
		if(ucUSBSta != ucLastUSBSta)			
		{
			ucLastUSBSta = ucUSBSta;
			iErr = -1;
		}
	}	

	return iErr;
}


/***
  * 功能：
        检测SD卡是否挂载成功
  * 参数：
        无
  * 返回：
        0	挂载成功			
        -1	挂载失败			
  * 备注：
  		通过分析执行mount命令的结果来判断是否挂载
***/
int CheckMountSD(void)
{
#if 1
	char cArgu[512] = {0};
	FILE *mnt_tab = NULL;

	mnt_tab = fopen("/proc/mounts", "r");
	if (mnt_tab)
	{
		while(!feof(mnt_tab))
		{
			memset(cArgu, '\0', sizeof(cArgu) );
			if (NULL != fgets(cArgu, sizeof(cArgu) - 1, mnt_tab))
			{	
				if (strstr(cArgu, "/mnt/sdcard"))
				{
					fclose(mnt_tab); 
					return 0;
				}
			}
		}
		fclose(mnt_tab);
		return -1;
	}
	else
	{
		return -1;
	}
#else
	FILE *stream; 
	char cArgu[512];			// 命令返回结果

	/* 将命令执行的结果进行重定向，以便分析执行结果 */
	stream = popen("mount", "r");

	/* 分析执行的结果 */
	while(!feof(stream))
	{
		memset(cArgu, '\0', sizeof(cArgu) );
		/* 读取一行参数 */
		if(fgets(cArgu, sizeof(cArgu)-1, stream) != NULL)
		{	
			if(strstr(cArgu, "/mnt/sdcard"))
			{
				pclose(stream); 
				return 0;
			}
		}
	}
	pclose(stream); 

	return -1;		
#endif
}


/***
  * 功能：
        检测USB是否挂载成功
  * 参数：
        无
  * 返回：
        0	挂载成功			
        -1	挂载失败			
  * 备注：
  		通过分析执行mount命令的结果来判断是否挂载
***/
int CheckMountUSB(void)
{
#if 1
	char cArgu[512] = {0};
	FILE *mnt_tab = NULL;

	mnt_tab = fopen("/proc/mounts", "r");
	if (mnt_tab)
	{
		while(!feof(mnt_tab))
		{
			memset(cArgu, '\0', sizeof(cArgu) );
			if (NULL != fgets(cArgu, sizeof(cArgu) - 1, mnt_tab))
			{	
				if (strstr(cArgu, MntUsbDirectory))
				{
					fclose(mnt_tab); 
					return 0;
				}
			}
		}
		fclose(mnt_tab);
		return -1;
	}
	else
	{
		return -1;
	}
#else
	FILE *stream; 
	char cArgu[512];			// 命令返回结果

	/* 将命令执行的结果进行重定向，以便分析执行结果 */
	stream = popen("mount", "r");

	/* 分析执行的结果 */
	while(!feof(stream))
	{
		memset(cArgu, '\0', sizeof(cArgu) );
		/* 读取一行参数 */
		if(fgets(cArgu, sizeof(cArgu)-1, stream) != NULL)
		{	
			if(strstr(cArgu, "/mnt/usb"))
			{
				pclose(stream); 
				return 0;
			}
		}
	}
	pclose(stream); 

	return -1;	
#endif
}

/***
  * 功能：
        检测存储器的占用信息
  * 参数：
        1.char *pDiskPath   :		存储器的挂载路径
        2.int iFormat	    :		存储信息的格式(0:B/B, 1:KB/KB, 2:MB/MB, 3:GB/GB, 默认是MB/MB)
  * 返回：
        成功返回存储信息字符串，失败返回NULL			
  * 备注：
  		
***/
char* GetDiskInformation(char* DiskPath, int iFormat)
{
    struct statfs diskInfo;  
    statfs(DiskPath, &diskInfo);
    unsigned long long blocksize = diskInfo.f_bsize;                    //每个block里包含的字节数  
    unsigned long long totalsize = blocksize * diskInfo.f_blocks;       //总的字节数，f_blocks为block的数目
    //unsigned long long freeDisk = diskInfo.f_bfree * blocksize;       //剩余空间的大小  
    unsigned long long availableDisk = diskInfo.f_bavail * blocksize;   //可用空间大小

    char* cDiskInfo = GuiMemAlloc(30);
    if(cDiskInfo == NULL)
    {
        return NULL;
    }
    switch(iFormat)
    {
        case 0:
            sprintf(cDiskInfo, "%lluB/%lluB", availableDisk, totalsize);
            break;
        case 1:
            sprintf(cDiskInfo, "%lluKB/%lluKB", availableDisk>>10, totalsize>>10);
            break;
        case 2:
            sprintf(cDiskInfo, "%.2fMB/%.2fMB", (float)(availableDisk>>10)/1024, (float)(totalsize>>10)/1024);
            break;
        case 3:
            sprintf(cDiskInfo, "%.2fGB/%.2fGB", (float)(availableDisk>>20)/1024, (float)(totalsize>>20)/1024);
            break;
        default:
            sprintf(cDiskInfo, "%.2fMB/%.2fMB", (float)(availableDisk>>10)/1024, (float)(totalsize>>10)/1024);
            break;
    }
    return cDiskInfo;
}

/***
  * 功能：
        获取CPU相关信息
  * 参数：
        无
  * 返回：
        失败为NULL,成功获得CPU信息
***/
CPUINFO* GetCPUInformation(void)
{
    CPUINFO* pCpuInfo = GuiMemAlloc(sizeof(CPUINFO));
    if(pCpuInfo == NULL)
    {
        return NULL;
    }
    pCpuInfo->num = sysconf (_SC_NPROCESSORS_CONF);
    pCpuInfo->size = sysconf (_SC_PAGESIZE);
    pCpuInfo->pageNum = sysconf (_SC_PHYS_PAGES);
    pCpuInfo->avaliablePageNum = sysconf (_SC_AVPHYS_PAGES);

    return pCpuInfo;
}

/***
  * 功能：
        判断内存是否不足
  * 参数：
        1.int fd        :   文件描述符
        2.char* buf     :   将要写入文件的内容
        3.int size      :   写入文件内容的大小
  * 返回：
        -1:代表内存不足，-2:代表写入的文件内容为空, 0:代表内存足够
***/
int CheckRemainSpace(int fd, char* buf, int size)
{
    int iRet = 0;
    if(buf == NULL)
    {
        return -2;
    }
    if(size != write(fd, buf, size))
    {
        if(errno == ERROR_NO_SPACE)
        {
            iRet = -1;
        }
    }
    return iRet;
}
/***
  * 功能：
        每次进入文件浏览器时检测SD卡或U盘是否挂载成功
  * 参数：
        无
  * 返回：
        0:	挂载成功				
        -1:	挂载失败		
  * 备注：
  		有时SD卡或U盘拔出，却没有删除/mnt目录下的文件夹，所以每次打开目录时，
  		通过分析执行mount命令的结果来判断是否挂载
***/	
int FirstCheckMount(void)
{
	int iErr = 0;
	int iReturn = 0;
	
	if( access("/tmp/sdcard", F_OK) == 0) 		
	{
		iReturn = CheckMountSD();
		if(iReturn < 0)
		{
			mysystem("rm -rf /tmp/sdcard");

			iErr = -1;
		}
	}

	if( access("/tmp/usb", F_OK) == 0) 		
	{
		iReturn = CheckMountUSB();
		if(iReturn < 0)
		{
			mysystem("rm -rf /tmp/usb");

			iErr = -1;
		}
	}

	return iErr;
}


/***
  * 功能：
        将文件类型枚举变量转换为相应的字符串
  * 参数：
        1、FILETYPE emFileType	:	文件类型枚举变量	
        2、char *pFileTypeString:	存贮转换后的文件类型字符串		
  * 返回：
		0:		成功
		非0:	失败
  * 备注：
***/
int FileTypeToFileString(FILETYPE emFileType, char *pFileTypeString)
{
	int iErr = 0;

	if(iErr == 0)
	{
		if(NULL == pFileTypeString)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
	    int count = 11;
        char* string[] = {
           "ALL", "SOR", "BMP", "JPG",
           "SOLA", "HTML", "TXT", "GDM",
           "PDF", "CUR", "BIN"
        };
        int i;
        for(i = 0; i < count; ++i)
        {
            if(emFileType == i)
            {
                strcpy(pFileTypeString, string[i]);
                break;
		    }
	    }
    }
	return iErr;
}


/***
  * 功能：
        将文件类型字符转转换为相应的文件类型枚举变量
  * 参数：
        1、FILETYPE *emFileType	:	文件类型枚举变量	
        2、char *pFileTypeString:	文件类型字符串		
  * 返回：
		0:		成功
		非0:	失败
  * 备注：
***/
int FileStringToFileType(FILETYPE *emFileType, char *pFileTypeString)
{
	int iErr = 0;

	if(iErr == 0)
	{
		if( (NULL == pFileTypeString) ||
			(NULL == emFileType))
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		/* 不区分大小写比较扩展名 */
		if(strcasecmp(pFileTypeString, "SOR") == 0)
		{
			*emFileType = SOR;
		}
		else if(strcasecmp(pFileTypeString, "BMP") == 0)
		{
			*emFileType = BMP;
		}
		else if(strcasecmp(pFileTypeString, "JPG") == 0)
		{
			*emFileType = JPG;
		}
		else if(strcasecmp(pFileTypeString, "SOLA") == 0)
		{
			*emFileType = SOLA;
		}	
		else if(strcasecmp(pFileTypeString, "HTML") == 0)
		{
			*emFileType = HTML;
		}
		else if(strcasecmp(pFileTypeString, "TXT") == 0)
		{
			*emFileType = TXT;
		}
		else if(strcasecmp(pFileTypeString, "GDM") == 0)
		{
			*emFileType = GDM;
		}	
		else if(strcasecmp(pFileTypeString, "PDF") == 0)
		{
			*emFileType = PDF;
		}	
		else if(strcasecmp(pFileTypeString, "INNO") == 0)
		{
		    #ifndef FORC
			*emFileType = INNO;
            #endif
		}
        else if(strcasecmp(pFileTypeString, "CUR") == 0)
		{
			*emFileType = CUR;
		}
		else if(strcasecmp(pFileTypeString, "BIN") == 0)
		{
			*emFileType = BIN;
		}	
		else if(strcasecmp(pFileTypeString, "ALL") == 0)
		{
			*emFileType = ANY;
		}			
		else
		{
			SetCurFileType(OTHER);
		}
	}

	return iErr;
}


/***
  * 功能：
        在给定的字符串中删除指定的字符串
  * 参数：
        1、char *pSrcString		:	源字符串
        2、char *pSpecifyString	:	待删除的字符串		
  * 返回：
		0:		成功
		非0:	失败
  * 备注：
***/
/*char *DeleteSpecifyString(char *pSrcString, char *pSpecifyString)
{
	int iErr = 0;
	char *pTmp = NULL;

	if(iErr == 0)
	{
		if( (NULL == pSrcString) ||
			(NULL == pSpecifyString))
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		pTmp = strstr();
	}	

	return NULL;
}*/



/***
  * 功能：
        全选专用函数，全部选中所选文件类型
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int FullSelectQueue(void)
{
	int i = 0;
	char cPathBuff[512];
	char cTmpPathBuff[512];

	ClearQueue(pEnvQueue);

	getcwd(cPathBuff, 512);
	for(i=0; i<__g_iTotalNum; i++)
	{ 
		sprintf(cTmpPathBuff, "%s/%s", cPathBuff, __g_Namelist[i]->d_name);
		WriteQueue(cTmpPathBuff, pEnvQueue);
	}

	return 0;
}

/***
  * 功能：
        判断是否已经全选了
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int IsFullSelect(void)
{
	//错误标志、返回值定义
    int iErr = 0;


	//加锁
    MutexLock(&(pEnvQueue->Mutex));	

    if (iErr == 0)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pEnvQueue)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pEnvQueue->pMsgQueue) || 
            ListEmpty(pEnvQueue->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        if(__g_iTotalNum != ListCount(pEnvQueue->pMsgQueue))
			iErr = -3;
    }
    //解锁
    MutexUnlock(&(pEnvQueue->Mutex));		


	return iErr;
}

/***
  * 功能：
        判断是否是顶层目录
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int IsUpDir(void)
{
	char cPathBuff[512];
	
	getcwd(cPathBuff, 512);
		
	return (0 == strcmp(cPathBuff, MntUpDirectory));
}

int isDirectory(const char *pPath)
{   
    struct stat s_buf; 
    
    if(pPath == NULL)
    {
        return 0;
    }

    /*获取文件信息，把信息放到s_buf中*/ 
    stat(pPath,&s_buf); 

    /*判断输入的文件路径是否目录，若是目录，则往下执行，分析目录下的文件*/ 
    if(S_ISDIR(s_buf.st_mode)) 
    {
        return 1;
    }

    return 0;
}

/***
  * 功能：
        判断指定路径是否存在，不存在则创建
  * 参数：
  		char *pPath 文件路径
  * 返回：
        成功返回0，失败返回-1
  * 备注：
  		
***/
int isExist(const char *pPath)
{
    if(!pPath)
    {
        return -1;
    }
    char tempPath[PATH_MAX];
    char *p;
    strcpy(tempPath, pPath);

    p = strrchr(tempPath, '/');
    *p = 0;
    if(access(tempPath, F_OK))
    {
        isExist(tempPath);
    }
    
    if(access(pPath, F_OK))
    {
        return mkdir(pPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    return 0;
}

/***
  * 功能：
        从队列中获取文件列表
  * 参数：
        1.MULTISELECT *pMsgObj:     队列指针(传入)
        1.char** selectedList:      文件列表(传入)
  * 返回：
        无
  * 备注：
  		文件列表通过参数传出，传入的文件列表选项个数大于队列中选项个数
***/
void GetFileListFromQueue(MULTISELECT *pMsgObj, char** selectedList)
{
    //错误标志、返回值定义
    int iErr = 0;
	char *pTmp = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    
    if (iErr == 0)
    {
        //判断selectedList和pMsgObj是否为有效指针
        if (NULL == pMsgObj || NULL == selectedList)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }
    
    if (iErr == 0)
    {
        int i = 0;
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pTmp = (char *) (pPkt->pObj);
            int len = strlen(pTmp)+1;
            selectedList[i] = (char*)malloc(len);
            memset(selectedList[i], 0, len);
            memcpy(selectedList[i], pTmp, len);
            i++;
        }
    }
}

/***
  * 功能：
        获取当前目录的文件个数
  * 参数:
  * 返回：
        无
  * 备注：
***/
int GetCurrentDirFileNum()
{
    return __g_iTotalNum;
}

/***
  * 功能：
		获取当前文件位置
  * 参数:
  * 返回：
		无
  * 备注：用于翻页
***/
int GetCurFilePos(void)
{
	return __g_iCurFilePos;
}
