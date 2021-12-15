/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmdialog.h
* 摘    要：  声明窗体wnd_frmdialog(对话框)的窗体处理线程及相关操作函数。该窗体
*             提供了对话框功能。
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmdialogsendfile.h"


/**********************************************************************************
**						为实现窗体frmdialog而需要引用的其他头文件				 **
***********************************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "app_frmsendfile.h"
#include "wnd_frmdialog.h"
#include "appwifi.h"

/**********************************************************************************
**								变量定义				 						 **
***********************************************************************************/
static int iDialogSendfileWinX = 0; 							// 窗体左上角X
static int iDialogSendfileWinY = 0;								// 窗体左上角Y
static int iDialogSendfileKeyFlag = 0;
CALLLBACKWINDOW DialogsendfilecallBackFunc = NULL;				// 重绘前一个窗体的回调函数
CALLLBACKHANDLE CancelsendfilecallBackFunc = NULL;				// dialog窗体中按下cancel按钮后的回调函数



/**********************************************************************************
**							窗体frmdialog中的控件定义部分						 **
***********************************************************************************/
static GUIWINDOW *pFrmDialogSendfile = NULL;					//窗体控件


/**********************************背景控件定义************************************/ 
static GUIPICTURE *pDialogSendfileBg = NULL;					//对话框的背景图片


/******************************窗体标题栏控件定义**********************************/


/******************************对话框的控件定义************************************/
static GUICHAR *pDialogSendfileStrTitle 	= NULL;				//桌面上对话框的标题文本     			
static GUICHAR *pDialogSendfileStrCancle 	= NULL;				//桌面上对话框的取消键的文本
static GUICHAR *pDialogSendfileStrIP		= NULL;				//显示IP的文本

static GUICHAR *pDialogSendfileStrProgressTitle	= NULL;			//总进度标签文本
static GUICHAR *pDialogSendfileStrProgress	= NULL;				//总进度文本
static GUICHAR *pDialogSendfileStrSending	= NULL;				//正在传输文本

static GUILABEL *pDialogSendfileLblTitle    = NULL;     		//对话框标题Lbl控件
static GUILABEL *pDialogSendfileLblCancle   = NULL;     		//对话框取消按钮的Lbl控件
static GUILABEL *pDialogSendfileLblIP		= NULL;				//显示IP的控件

static GUILABEL *pDialogSendfileLblProgressTitle	= NULL;		//总进度标签文本
static GUILABEL *pDialogSendfileLblProgress	= NULL;				//总进度文本
static GUILABEL *pDialogSendfileLblSending	= NULL;				//正在传输文本


static GUIPICTURE *pDialogSendfileBtnCancle = NULL;				//对话框的取消按钮图片


/********************************右侧菜单控件定义**********************************/
 

/**********************************************************************************
**	    	窗体frmdialog中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
static int DialogSendfileTextRes_Init(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);

static int DialogSendfileTextRes_Exit(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);


/**********************************************************************************
**			    	窗体frmdialog中的控件事件处理函数定义部分					 **
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int DialogSendfileWndKey_Down(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
static int DialogSendfileWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/************************软件升级内的按键事件处理函数******************************/

static int DialogSendfileBtnCancle_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int DialogSendfileBtnCancle_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);


/**************************菜单栏控件的事件处理函数********************************/


/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/
static int	DialogSendfileExit(void);

/***
  * 功能：
        拼接gui文本
  * 参数：
        拼接前的文本
  * 返回：
        拼接后的文本
  * 备注：
  
***/

static GUICHAR* Joint(GUICHAR *str1,GUICHAR *str2)
{
	int i=0,j=0;
	
	GUICHAR *Return = NULL;
		
	if(NULL == str1 && NULL == str2)
	{
		return NULL;
	}

	if(NULL == str1)
	{
		return str2;
	}

	if(NULL == str2)
	{
		return str1;
	}

	while(str1[i] != 0)i++;
	while(str2[j] != 0)j++;

	Return = (GUICHAR *)malloc(2 * (i + j + 1));

	memcpy(Return,str1,2*i);
	memcpy(&Return[i],str2,2*(j+1));

	free(str1);
	free(str2);
	return Return;
}


static void RefreshDialog(void)
{
	DisplayPicture(pDialogSendfileBg);
	DisplayPicture(pDialogSendfileBtnCancle);
	
    DisplayLabel(pDialogSendfileLblTitle);
    DisplayLabel(pDialogSendfileLblIP);
	DisplayLabel(pDialogSendfileLblCancle);
	
	DisplayLabel(pDialogSendfileLblProgressTitle);
	DisplayLabel(pDialogSendfileLblProgress);
	DisplayLabel(pDialogSendfileLblSending);
	
	RefreshScreen(__FILE__, __func__, __LINE__); 
}

int sendFileCallback(long long size)
{
	char PromptInfo[128];
	SELECT_FILE_INFO_SINGLE File;
	
	sprintf(PromptInfo,"%d/%d",getsendCount(),getfileCount());
	free(pDialogSendfileStrProgress);
	pDialogSendfileStrProgress = TransString(PromptInfo);
	SetLabelText(pDialogSendfileStrProgress, pDialogSendfileLblProgress);
		
	File = getsendfile();	
	
	sprintf(PromptInfo,"%s: %lld%%",File.pfilename,size*100/File.size);
	free(pDialogSendfileStrSending);
	pDialogSendfileStrSending = TransString(PromptInfo);
	SetLabelText(pDialogSendfileStrSending, pDialogSendfileLblSending);

	RefreshDialog();

	return 0;
}

int completeCallback(long successCount)
{
	GUICHAR *dialogText;
	char sendcount[8];
	char filecount[8];

	sprintf(sendcount,"%ld ",successCount);
	sprintf(filecount,"%ld ",getfileCount()-successCount);
	
	dialogText = Joint(TransString("DIALOG_SENDFILE_SUCCESS"),TransString(sendcount));
	dialogText = Joint(dialogText,TransString("DIALOG_SENDFILE_FAIL"));
	dialogText = Joint(dialogText,TransString(filecount));
	
	DialogInit(120, 90, TransString("DIALOG_SENDFILE_COMPLETE"),
		dialogText,
		0, 0, DialogsendfilecallBackFunc, NULL, NULL);	
	return 0;
}

int netDisconnectCallback(long para)
{	
	DialogInit(120, 90, TransString("DIALOG_SENDFILE_SENDTO"),
		TransString("DIALOG_SENDFILE_CONNECTINTERRUPT"),
		0, 0, DialogsendfilecallBackFunc, NULL, NULL);	
	return 0;
}




/***
  * 功能：
        窗体frmdialog的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogSendfileInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmDialogSendfile = (GUIWINDOW *) pWndObj;

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    DialogSendfileTextRes_Init(NULL, 0, NULL, 0);
	
    //建立窗体控件
    /****************************************************************/
    //建立桌面上的控件
    pDialogSendfileBg = CreatePicture(iDialogSendfileWinX, iDialogSendfileWinY, 442, 286,
                                 BmpFileDirectory"bg_dialogsendfile.bmp");


	pDialogSendfileLblTitle  = CreateLabel(iDialogSendfileWinX, iDialogSendfileWinY+7, 
							  	   442, 24, pDialogSendfileStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogSendfileLblTitle);

	pDialogSendfileLblIP= CreateLabel(iDialogSendfileWinX, iDialogSendfileWinY+50, 
							  	   442, 24, pDialogSendfileStrIP);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogSendfileLblIP);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogSendfileLblIP);

	
	pDialogSendfileLblProgressTitle = CreateLabel(iDialogSendfileWinX, iDialogSendfileWinY+90, 
							  	   442, 24, pDialogSendfileStrProgressTitle);
	
	pDialogSendfileLblProgress = CreateLabel(iDialogSendfileWinX, iDialogSendfileWinY+130, 
							  	   442, 24, pDialogSendfileStrProgress);

	pDialogSendfileLblSending = CreateLabel(iDialogSendfileWinX, iDialogSendfileWinY+170, 
							  	   442, 48, pDialogSendfileStrSending);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogSendfileLblProgressTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogSendfileLblProgress);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogSendfileLblSending);
	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogSendfileLblProgressTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogSendfileLblProgress);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogSendfileLblSending);
	/* 显示取消按钮 */

	pDialogSendfileBtnCancle = CreatePicture(iDialogSendfileWinX + 322, iDialogSendfileWinY + 235,  
									 110, 40, BmpFileDirectory"btn_dialog_unpress.bmp");

	pDialogSendfileLblCancle = CreateLabel(iDialogSendfileWinX + 322, iDialogSendfileWinY + 248, 
							  	   	110, 24,     pDialogSendfileStrCancle);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogSendfileLblCancle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogSendfileLblCancle);

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件
    //...

    //设置菜单按钮区的标题控件的字体，橘黄色

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmDialogSendfile, 
                  pFrmDialogSendfile);    
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDialogSendfileBtnCancle, 
                  pFrmDialogSendfile);

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();

    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmDialogSendfile, 
                    DialogSendfileWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmDialogSendfile, 
                    DialogSendfileWndKey_Up, NULL, 0, pMsg);
	
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDialogSendfileBtnCancle, 
                    DialogSendfileBtnCancle_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pDialogSendfileBtnCancle, 
                    DialogSendfileBtnCancle_Up, NULL, 0, pMsg);

    return iReturn;
}


/***
  * 功能：
        窗体frmdialog的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogSendfileExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	DBG_ENTER();
    //得到当前窗体对象
	
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmDialogSendfile);
	
	setcallback(NULL);
	setcompletecallback(NULL);
	setdisconnectcallback(NULL);

    //销毁窗体控件
    //***************************************************************/	
    //销毁桌面上的控件
	DestroyPicture(&pDialogSendfileBg);
	DestroyPicture(&pDialogSendfileBtnCancle);
	
	DestroyLabel(&pDialogSendfileLblTitle);
	DestroyLabel(&pDialogSendfileLblCancle);
	DestroyLabel(&pDialogSendfileLblIP);

	DestroyLabel(&pDialogSendfileLblProgressTitle);
	DestroyLabel(&pDialogSendfileLblProgress);
	DestroyLabel(&pDialogSendfileLblSending);

    //释放文本资源
    //***************************************************************/
    DialogSendfileTextRes_Exit(NULL, 0, NULL, 0);
	
    //***************************************************************/
    //释放按钮区的字体
    //...
	DBG_EXIT(iReturn);
    return iReturn;
}


/***
  * 功能：
        窗体frmdialog的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogSendfilePaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    //得到当前窗体对象
    pFrmDialogSendfile = (GUIWINDOW *) pWndObj;
	setcallback(sendFileCallback);
	setcompletecallback(completeCallback);
	setdisconnectcallback(netDisconnectCallback);
	/* 绘制无效区域窗体 */
	DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    //显示桌面上的控件
	DisplayPicture(pDialogSendfileBg);
	DisplayPicture(pDialogSendfileBtnCancle);
	
    DisplayLabel(pDialogSendfileLblTitle);
	DisplayLabel(pDialogSendfileLblCancle);
	DisplayLabel(pDialogSendfileLblIP);
	
	DisplayLabel(pDialogSendfileLblProgressTitle);
	DisplayLabel(pDialogSendfileLblProgress);
	DisplayLabel(pDialogSendfileLblSending);

	SetPowerEnable(0, 0);

	
	
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__); 


	

    return iReturn;
}

/***
  * 功能：
        窗体frmdialog的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/



int FrmDialogSendfileLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SendWndMsg_LoopDisable(pWndObj);

		
    return iReturn;
	
}


/***
  * 功能：
        窗体frmdialog的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogSendfilePause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmdialog的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogSendfileResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/**********************************************************************************
**	    	窗体frmdialog中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/

/***
  * 功能：
        初始化文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileTextRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	char hostIP[128] = {0};
	char progress[16];
  	char IP[32];
    //初始化桌面上的文本
	pDialogSendfileStrCancle = TransString("DIALOG_CANCEL");
	pDialogSendfileStrTitle  = TransString("DIALOG_SENDFILE_SENDTO");
	
	strcpy(hostIP,getIP());
	sprintf(IP,"IP:%s",hostIP);
	pDialogSendfileStrIP	 = TransString(IP);
	
	pDialogSendfileStrProgressTitle= TransString("DIALOG_SENDFILE_PROGRESS");
	
	sprintf(progress,"0/%d",getfileCount());
	pDialogSendfileStrProgress= TransString(progress);

	if(getconnectToPC())
	{	
		pDialogSendfileStrSending= TransString("DIALOG_SENDFILE_PREPARE");
	}
	else
	{
		pDialogSendfileStrSending= TransString("DIALOG_SENDFILE_PLEASECONNECT");
	}

    return iReturn;
}


/***
  * 功能：
        释放文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileTextRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//对话框标题的文本
	GuiMemFree(pDialogSendfileStrTitle);
	
    //对话框上的文本
	GuiMemFree(pDialogSendfileStrCancle);
	GuiMemFree(pDialogSendfileStrIP);

	GuiMemFree(pDialogSendfileStrProgressTitle);
	GuiMemFree(pDialogSendfileStrProgress);
	GuiMemFree(pDialogSendfileStrSending);

    return iReturn;
}


/**********************************************************************************
**			    			窗体的按键事件处理函数				 				 **
***********************************************************************************/

/***
  * 功能：
        窗体按键的down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileWndKey_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
	
    uiValue = (unsigned int)pInArg;

	if(iDialogSendfileKeyFlag == 0)
	{
		iDialogSendfileKeyFlag = 1;
		
    	switch (uiValue)
	    {
			case KEYCODE_ESC:
				if(pDialogSendfileBtnCancle->Visible.iEnable)
				{
					DialogSendfileBtnCancle_Down(NULL, 0, NULL, 0);
				}
				break;
			case KEYCODE_HOME:
				break;
			default:
				break;
	    }
	}
    return iReturn;
}


/***
  * 功能：
        窗体按键的up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileWndKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	if(iDialogSendfileKeyFlag == 1)
	{
		iDialogSendfileKeyFlag = 0;
    	switch (uiValue)
	    {
			case KEYCODE_ESC:
				if(pDialogSendfileBtnCancle->Visible.iEnable)
				{
					DialogSendfileBtnCancle_Up(NULL, 0, NULL, 0);
				}				
				break;
			case KEYCODE_HOME:
			#ifdef SAVE_SCREEN
				ScreenShot();
			#endif
				break;
			default:
				break;
	    }
	}
	
    return iReturn;
}


/**********************************************************************************
**							对话框内的控件事件处理函数							 **
***********************************************************************************/



/***
  * 功能：
		cancle按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileBtnCancle_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	TouchChange("btn_dialog_press.bmp", pDialogSendfileBtnCancle, NULL, pDialogSendfileLblCancle, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
    return iReturn;
}


/***
  * 功能：
		cancle按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileBtnCancle_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	TouchChange("btn_dialog_unpress.bmp", pDialogSendfileBtnCancle, NULL, pDialogSendfileLblCancle, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);	
	//刷新屏幕缓存

	CancelSendFile();		//退出发送线程
	
	if(CancelsendfilecallBackFunc != NULL)
	{
		(*CancelsendfilecallBackFunc)();	
	}

	DialogSendfileExit();
	
    return iReturn;
}



/***
  * 功能：
		对话框退出
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogSendfileExit(void)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;

    (*DialogsendfilecallBackFunc)(&pWnd);
    SendWndMsg_WindowExit(pFrmDialogSendfile);      //发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}


/***
  * 功能：
		建立对话框
  * 参数：
		1.x         : 	对话框左上角的横坐标
		2.y         : 	对话框左上角的纵坐标
		4.func	   : 	还原窗体的回调函数
		5.cancelfunc: 	按下cancel按钮后执行的回调函数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
  		如果没有需要在按下cancel按钮后需要处理的回调函数，可输入NULL
***/
void DialogSendfileInit(int x, int y,CALLLBACKWINDOW func, CALLLBACKHANDLE cancelfunc)
{
    GUIWINDOW *pWnd = NULL;
	
	iDialogSendfileWinX = x;
	iDialogSendfileWinY = y;
	DialogsendfilecallBackFunc = func;
	CancelsendfilecallBackFunc = cancelfunc;

    pWnd = CreateWindow(x, y, 442, 286,
                        FrmDialogSendfileInit , FrmDialogSendfileExit, 
                        FrmDialogSendfilePaint, FrmDialogSendfileLoop, 
					    FrmDialogSendfilePause, FrmDialogSendfileResume,
                        NULL);          				//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());				//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);						//发送消息以便调用新的窗体
}

