/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmupdate.c
* 摘    要：  实现主窗体FrmUpdate的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020/10/17
*******************************************************************************/
#include "wnd_frmupdate.h"
/**********************************************************************************
**						为实现窗体frmupdate而需要引用的其他头文件 				 **
***********************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "app_frmupdate.h"

#include "guibase.h"

#include "wnd_global.h"

/**********************************************************************************
**								  宏定义				 						 **
***********************************************************************************/
#define UPDATEWIN_X		120
#define UPDATEWIN_Y		90

/**********************************************************************************
**							窗体FrmUpdate中的控件定义部分						 **
***********************************************************************************/
static GUIWINDOW *pFrmUpdate = NULL;
/**********************************背景控件定义************************************/
static GUIPICTURE *pUpdateBtnBg  = NULL;       
/****************************软件升级内的控件定义**********************************/
//软件升级内的文本定义
static GUICHAR *pUpdateStrUpdateTitle   = NULL;	  
static GUICHAR *pUpdateStrInfo   = NULL;	  			//显示软件升级过程中的各种提示信息
static GUICHAR *pUpdateStrWarn 	 = NULL;	  			//显示警告信息的文本
static GUICHAR *pUpdateStrOk = NULL;	  				//显示确定
static GUICHAR *pUpdateStrCancel = NULL;	  			//显示取消


//软件升级内的Label定义
static GUILABEL *pUpdateLblInfo = NULL;
static GUILABEL *pUpdateLblWarn = NULL;
static GUILABEL *pUpdateLblOk = NULL;
static GUILABEL *pUpdateLblCancel = NULL;

//软件升级内的图片定义
static GUIPICTURE *pUpdateBgProgress  = NULL;       
static GUIPICTURE *pUpdateBtnOk = NULL; 
static GUIPICTURE *pUpdateBtnCancel  = NULL;
//字体
static GUIFONT* pGlobalFontBlack = NULL;
static GUIFONT* pGlobalFontYellow = NULL;
static GUIFONT* pGlobalFontRed = NULL;
/**********************************************************************************
**	    	窗体FrmUpdate中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
//初始化文本资源
static int UpdateTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int UpdateTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**********************************************************************************
**			    	窗体FrmUpdate中的控件事件处理函数定义部分					 **
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int UpdateWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int UpdateWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);


/************************软件升级内的按键事件处理函数******************************/
static int UpdateBtnUpdate_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int UpdateBtnCancel_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int UpdateBtnUpdate_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int UpdateBtnCancel_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);


/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/
static int Update(void);
static int ShowUpdateInfo(GUICHAR *pUpdateInfo);
static int RealUpdate(int iDeviceType);
static int Cuetomsystem(char *cmdstring);	

typedef struct _update_file_info
{
    unsigned int offset;
    unsigned int size;
}UPDATE_FILE_INFO;

#define UPDATEPATH			"/update"
#define MACH_TYPE          "O10"
#define MACH_LOWEST_VER    "1.0"
#define UPDATE_PARSE_FILE    7

/***
  * 功能：
        窗体FrmUpdate的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUpdateInit(void *pWndObj)
{
	int iReturn = 0;

	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmUpdate = (GUIWINDOW *) pWndObj;

   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    UpdateTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体背景图片 */
	pUpdateBtnBg = CreatePicture(UPDATEWIN_X, UPDATEWIN_Y, 400, 300, BmpFileDirectory"bg_update.bmp");

	//软件升级内的图片定义
	pUpdateBgProgress = CreatePicture(UPDATEWIN_X + 10, UPDATEWIN_Y+20, WINDOW_WIDTH, 20, NULL);	

	#ifdef FIRST_WRITE
	pUpdateBtnOk = CreatePicture(UPDATEWIN_X, UPDATEWIN_Y + 259, 200, 41,
										BmpFileDirectory"btn_dialog_press.bmp");
	pUpdateBtnCancel = CreatePicture(UPDATEWIN_X + 200, UPDATEWIN_Y + 259, 200, 41,
									 BmpFileDirectory "btn_dialog_unpress.bmp");

	pUpdateLblOk = CreateLabel(UPDATEWIN_X + 84, UPDATEWIN_Y + 271, 100, 16,
								  		    pUpdateStrOk);	
	pUpdateLblCancel = CreateLabel(UPDATEWIN_X + 285, UPDATEWIN_Y + 271, 100, 16,
								  		    NULL);	
	// SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblOk);	
	// SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblCancel);	

	SetPictureEnable(0, pUpdateBtnCancel);
	SetLabelEnable(0, pUpdateLblCancel);

	#else
	pUpdateBtnOk = CreatePicture(UPDATEWIN_X+ 124, UPDATEWIN_Y+ 95, 55, 18,
                                     	BmpFileDirectory"btn_dialog_press.bmp");	
	pUpdateBtnCancel = CreatePicture(UPDATEWIN_X+ 46, UPDATEWIN_Y+95, 55, 18,
                                     	BmpFileDirectory"btn_dialog_unpress.bmp");

	pUpdateLblOk = CreateLabel(UPDATEWIN_X+124, UPDATEWIN_Y+95, 55, 18, 
								  		    pUpdateStrOk);	
	pUpdateLblCancel = CreateLabel(UPDATEWIN_X+46, UPDATEWIN_Y+95, 55, 18, 
								  		    pUpdateStrCancel);	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblOk);	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblCancel);	
	#endif
	//创建字体
	pGlobalFontBlack = CreateFont(FNTFILE_STD, 16, 16, 0x00000000, 0xFFFFFFFF);
	pGlobalFontYellow = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);
	pGlobalFontRed = CreateFont(FNTFILE_STD, 16, 16, 0x00E81853, 0xFFFFFFFF);

	SetLabelFont(pGlobalFontBlack, pUpdateLblOk);
	SetLabelFont(pGlobalFontBlack, pUpdateLblCancel);
	//软件升级内的Label定义
	pUpdateLblInfo = CreateLabel(UPDATEWIN_X+81, UPDATEWIN_Y+104, 200, 20, pUpdateStrInfo);	
	pUpdateLblWarn = CreateLabel(UPDATEWIN_X+81, UPDATEWIN_Y+150, 200, 16, pUpdateStrWarn);	

	SetLabelFont(pGlobalFontBlack, pUpdateLblInfo);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblInfo);
	SetLabelFont(pGlobalFontYellow, pUpdateLblWarn);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblWarn);


	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmUpdate, 
                  pFrmUpdate);
	
    //注册软件升级内的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pUpdateBtnOk, 
                  pFrmUpdate);	
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pUpdateBtnCancel, 
                  pFrmUpdate);

	 //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmUpdate, 
                    UpdateWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmUpdate, 
                    UpdateWndKey_Up, NULL, 0, pMsg);
	
    //注册软件升级内的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pUpdateBtnOk, 
                    UpdateBtnUpdate_Down, NULL, 0, pMsg);	
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pUpdateBtnCancel, 
                    UpdateBtnCancel_Down, NULL, 0, pMsg);	

	LoginMessageReg(GUIMESSAGE_TCH_UP, pUpdateBtnOk, 
                    UpdateBtnUpdate_Up, NULL, 0, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pUpdateBtnCancel, 
                    UpdateBtnCancel_Up, NULL, 0, pMsg); 

    return iReturn;
}


/***
  * 功能：
        窗体FrmUpdate的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUpdateExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmUpdate = (GUIWINDOW *) pWndObj;
	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmUpdate);

    //销毁窗体控件
    //***************************************************************/
    //销毁桌面背景图片
    DestroyPicture(&pUpdateBtnBg);

	//销毁软件升级内的图片
    DestroyPicture(&pUpdateBgProgress);	
	DestroyPicture(&pUpdateBtnOk);	
	DestroyPicture(&pUpdateBtnCancel);	
	
	//销毁软件升级内的Label
	DestroyLabel(&pUpdateLblInfo);	
	DestroyLabel(&pUpdateLblWarn);
	DestroyLabel(&pUpdateLblOk);
	DestroyLabel(&pUpdateLblCancel);
	
	//释放文本资源
    //***************************************************************/
    UpdateTextRes_Exit(NULL, 0, NULL, 0);
	DestroyFont(&pGlobalFontBlack);
	DestroyFont(&pGlobalFontYellow);
	DestroyFont(&pGlobalFontRed);

	return iReturn;
}


/***
  * 功能：
        窗体FrmUpdate的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUpdatePaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmUpdate = (GUIWINDOW *) pWndObj;

    //显示桌面背景图片
    DisplayPicture(pUpdateBtnBg);
	//显示软件升级内的图片
    DisplayPicture(pUpdateBgProgress);
	DisplayLabel(pUpdateLblWarn);
	//开机直接执行升级程序
	Update();
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
        窗体FrmUpdate的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUpdateLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    //SendWndMsg_LoopDisable(pWndObj);   

    return iReturn;
}


/***
  * 功能：
        窗体FrmUpdate的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUpdatePause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体FrmUpdate的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUpdateResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    	窗体FrmUpdate中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/

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
static int UpdateTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//系统升级内的文本
	pUpdateStrUpdateTitle = TransString("Update");
	pUpdateStrInfo = TransString("Press update button...");
	pUpdateStrWarn = TransString("Please ensure battery value is over 50%");
	pUpdateStrOk = TransString("Update");
	pUpdateStrCancel = TransString("Cancel");
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
static int UpdateTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//释放系统升级内的文本
	free(pUpdateStrUpdateTitle);
	free(pUpdateStrInfo);
	free(pUpdateStrWarn);
	free(pUpdateStrOk);
	free(pUpdateStrCancel);

    return iReturn;
}


/**********************************************************************************
**			    			窗体的按键事件处理函数				 				 **
***********************************************************************************/

/***
  * 功能：
        窗体的按键按下事件处理函数
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	
	switch (uiValue)
    {
		case KEYCODE_BACK:
			UpdateBtnCancel_Down(NULL, 0, NULL, 0);
			break;
		case KEYCODE_HOME:
			break;
		default:
			break;
    }
	
    return iReturn;
}
/***
  * 功能：
        窗体的按键弹起事件处理函数
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	
	switch (uiValue)
    {
		case KEYCODE_BACK:
			UpdateBtnCancel_Up(NULL, 0, NULL, 0);
			break;
		case KEYCODE_HOME:
		#ifdef SAVE_SCREEN
			ScreenShot();		
		#endif	
			break;
		default:
			break;
    }
	
    return iReturn;
}                         
/**********************************************************************************
**			    			软件升级内的按键事件处理函数					     **
***********************************************************************************/

/***
  * 功能：
        升级按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateBtnUpdate_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	//TouchChange("btn_dialog_press.bmp", pUpdateBtnOk, 
	//			NULL, pUpdateLblOk, 1);
	////刷新屏幕缓存
	//RefreshScreen(__FILE__, __func__, __LINE__);	

    return iReturn;

}


/***
  * 功能：
        取消按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateBtnCancel_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	//TouchChange("btn_dialog_press.bmp", pUpdateBtnCancel, 
	//			NULL, pUpdateLblCancel, 1);
	////刷新屏幕缓存
	//RefreshScreen(__FILE__, __func__, __LINE__);	

    return iReturn;

}


/***
  * 功能：
        升级按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateBtnUpdate_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iErr = 0;

	int iReturn = 0;

	if(iErr == 0)
	{
		//TouchChange("btn_dialog_unpress.bmp", pUpdateBtnOk, 
		//			NULL, pUpdateLblOk, 1);
		////刷新屏幕缓存
		//RefreshScreen(__FILE__, __func__, __LINE__);

		/* 在开始升级文件前，确定按钮不可见，右侧区域按钮不可用，防止用户误操作 */	

		/* 确定和取消按钮消失 */
		SetPictureEnable(0, pUpdateBtnOk);
		SetPictureEnable(0, pUpdateBtnCancel);
		
		//显示桌面背景图片
		DisplayPicture(pUpdateBtnBg);
        DisplayLabel(pUpdateLblWarn);
		//刷新屏幕缓存
		//RefreshScreen(__FILE__, __func__, __LINE__);		
	}

	if(iErr == 0)
	{
		iReturn = Update();
		/* 加载SD卡或加载U盘或解压升级文件失败 */
		if(iReturn != 0)
		{
			/* 确定和取消按钮可见 */
			SetPictureEnable(1, pUpdateBtnOk);
			#ifdef FIRST_WRITE
			SetPictureEnable(0, pUpdateBtnCancel);
			#else
			SetPictureEnable(1, pUpdateBtnCancel);
			#endif
			SetLabelEnable(1, pUpdateLblOk);
			SetLabelEnable(1, pUpdateLblCancel);
			//显示软件升级按钮
			DisplayPicture(pUpdateBtnOk);
			DisplayPicture(pUpdateBtnCancel);
			DisplayLabel(pUpdateLblOk);	
			DisplayLabel(pUpdateLblCancel);	
			//刷新屏幕缓存
// 			RefreshScreen(__FILE__, __func__, __LINE__);
		}		
	}

    return iErr;
}


/***
  * 功能：
        取消按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateBtnCancel_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iErr = 0;

// 	GUIWINDOW *pWnd = NULL;
// 
// 	TouchChange("btn_dialog_unpress.bmp", pUpdateBtnCancel, 
// 				NULL, pUpdateLblCancel, 1);
// 	RefreshScreen(__FILE__, __func__, __LINE__);
// 
// 	pWnd = CreateWindow(0, 0, 320, 240, 
// 						FrmSystemSettingInit, FrmSystemSettingExit,
// 						FrmSystemSettingPaint, FrmSystemSettingLoop,
// 						FrmSystemSettingPause, FrmSystemSettingResume,
// 						NULL);			
// 	SendWndMsg_WindowExit(pFrmUpdate);		
// 	SendSysMsg_ThreadCreate(pWnd);	

    return iErr;
}


/**********************************************************************************
**			    			窗体内的其他函数定义					 		     **
***********************************************************************************/
/***
  * 功能：
		创建升级服务线程
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void *ServerThread(void *pThreadArg)
{
	char *cmdstring = pThreadArg;

	/* 创建升级服务进程，此函数一般不会返回
	   返回说明子进程意外终止，则说明升级失败*/
	int iRet = Cuetomsystem(cmdstring);
	if(iRet == 0)
		return NULL;	
	MsgSendSelf(ERR_WRBOOTSTRAP, FAIL);	

    return NULL;   
}

/***
  * 功能：
		实现对popen函数的二次封装
  * 参数：
		1.char *pCmd:	需要执行的命令行
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int mysystem(char *pCmd)
{
	int iReturn = 0;
	SendMsg(pCmd);
	iReturn = RecMsg();
	return iReturn;
}

//挂载升级所需要的分区
void MountUpdateMtdBlock(void)
{
    if(-1 == access(UPDATEPATH, F_OK))
	{
		mysystem("mkdir -m 777 "UPDATEPATH);
	}

    #ifdef FIRST_WRITE
    mysystem("flash_eraseall /dev/mtd7");
    #endif
    mysystem("umount "UPDATEPATH);
    mysystem("mount -t yaffs2 /dev/mtdblock7 /update");
    mysystem("rm -rf "UPDATEPATH"/*");

    return;
}

//解压升级包
// static int UpdatePktParse(char* pPktPath)
// {
// 	int iErr = 0;
// 
// 	char cCmdBuff[TEMP_ARRAY_SIZE] = {0};
// 	char cCmdMd5Buff[TEMP_ARRAY_SIZE] = {0};
// 
// 	if (strstr(pPktPath, "usb"))
// 	{
// 		sprintf(cCmdMd5Buff, "cp %s /update/ -rf", MntUsbDirectory"/update_wndproc");
// 	}
// 	else if (strstr(pPktPath, "sdcard"))
// 	{
// 		sprintf(cCmdMd5Buff, "cp %s /update/ -rf", MntSDcardDataDirectory"/update_wndproc");
// 	}
// 
// 	if (!iErr)
// 	{
// 		if (!mysystem(cCmdMd5Buff))
// 		{
// 			sprintf(cCmdBuff, "cp %s /update/ -rf", pPktPath);
// 			iErr = mysystem(cCmdBuff);
// 		}
// 	}
// 	return iErr;
// #if 0
//     char tmpBuf[1024] = {0};
//     UPDATE_FILE_INFO tmpFileHeader[UPDATE_PARSE_FILE];
//     FILE* fdSrc = NULL;
//     FILE* tmpFd = NULL;
//     int i = 0;
//     int tmpLen = 0;
//     int ret = -1;
//     
//     fdSrc = fopen(pPktPath, "rb");
//     if (NULL == fdSrc)
//     {
//         return -1;
//     }
// 
//     memset(tmpFileHeader, 0, sizeof(tmpFileHeader));
//     fread(tmpFileHeader, 1, sizeof(tmpFileHeader), fdSrc);
//     ret = 0;
//     for(i = 0;i < UPDATE_PARSE_FILE; i++)
//     {
//         if(0 == tmpFileHeader[i].size)
//         {
//             continue;
//         }
//         tmpFd = fopen(gUpdateCompPath[i], "wb");
//         if(NULL == tmpFd)
//         {
//             ret = -2-i;
//             break;
//         }
//         fseek(fdSrc, tmpFileHeader[i].offset, SEEK_SET);
//         while(tmpFileHeader[i].size != 0)
//         {
//             if(tmpFileHeader[i].size>=1024)
//             {
//                 tmpLen = 1024;
//             }
//             else
//             {
//                 tmpLen = tmpFileHeader[i].size;
//             }
//             fread(tmpBuf, 1, tmpLen,fdSrc);
//             fwrite(tmpBuf, 1, tmpLen, tmpFd);
//             tmpFileHeader[i].size -= tmpLen;
//         }
//         fflush(tmpFd);
//         fclose(tmpFd);
//     }
//     fclose(fdSrc);
//     return ret;
// #endif
// }

/***
  * 功能：
		显示进度条
  * 参数：
		1.unsigned int uiPlaceX	:       进度条水平放置位置，以左上角为基点
		2.unsigned int uiPlaceY	:       进度条垂直放置位置，以左上角为基点
		3.unsigned int uiWidth	:      	进度条水平宽度
		4.unsigned int uiHeight	:     	进度条垂直高度
		5、int iProgress		:		当前进度，0-100
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int ShowProgressBar(unsigned int uiPlaceX, unsigned int uiPlaceY,
	unsigned int uiWidth, unsigned int uiHeight,
	int iProgress)
{
	int iErr = 0;

	GUICHAR *pProgressStr = NULL;
	GUILABEL *pProgressLbl = NULL;

	GUIPEN *pPen;
	unsigned int uiColorBack;
	unsigned int uiCurWidth = 0;

	char cTmpBuf[50];

	if (iErr == 0)
	{
		//判断uiWidth和uiHeight是否有效
		if (uiWidth < 1 || uiHeight < 1)
		{
			iErr = -1;
		}
	}

	if (iErr == 0)
	{
		pPen = GetCurrPen();
		uiColorBack = pPen->uiPenColor;

		/* 根据当前的进度，计算绘制的进度条的宽度 */
		uiCurWidth = (iProgress*uiWidth) / 100;

		/* 画进度条背景 */
		pPen->uiPenColor = 0xF4F4F4;
		DrawBlock(uiPlaceX, uiPlaceY, uiPlaceX + uiWidth, uiPlaceY + uiHeight);

		/* 画当前进度 */
		pPen->uiPenColor = 0xEBD300;
		DrawBlock(uiPlaceX, uiPlaceY, uiPlaceX + uiCurWidth, uiPlaceY + uiHeight);
	}

	if (iErr == 0)
	{

		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%d%%", iProgress);
		pProgressStr = TransString(cTmpBuf);
		if (uiWidth - uiCurWidth > 32)
		{
			pProgressLbl = CreateLabel(uiPlaceX + uiCurWidth, uiPlaceY - 10, 50, 24,
				pProgressStr);
		}
		else
		{
			pProgressLbl = CreateLabel(uiPlaceX + uiWidth - 32, uiPlaceY - 10, 50, 24,
				pProgressStr);
		}

		SetLabelFont(pGlobalFontBlack, pProgressLbl);

		/* 画背景 */
		pPen->uiPenColor = 0xF4F4F4;
		DrawBlock(uiPlaceX, uiPlaceY - 10, uiWidth + uiPlaceX, uiHeight + uiPlaceY - 10);

		DisplayLabel(pProgressLbl);
 		RefreshScreen(__FILE__, __func__, __LINE__);
		free(pProgressStr);
		DestroyLabel(&pProgressLbl);
		pPen->uiPenColor = uiColorBack;
	}

	return iErr;
}

/***
  * 功能：
		执行解压升级功能
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		升级包1:	包含待升级的各个部分文件
***/
static int Update(void)
{
	int iErr = 0;
	int iReturn = 0;
	char cCmdBuff[512];						// md5校验命令
	char cMd5Path[512];						// 存贮md5文件的路径
	char cUpdateProgramPath[512];			// 存放真正升级应用程序的路径
	char cLogPath[512];						// 存放升级LOG信息的路径
	int iDeviceType = 0;
	GUITHREAD thdServerFb = -1;	

	//获得rootfs分区剩余空间大小
	/* 查找md5文件的路径 */
	if(iErr == 0)
	{
		/* 显示尝试加载U盘或SD卡 */
		ShowUpdateInfo(TransString("Try to load u disk or sdcard..."));
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 2);

		/* 延时一段时间，等待U盘挂载 */
		MsecSleep(3000);
		/* 显示查找升级文件 */
		ShowUpdateInfo(TransString("Searching upgrade file..."));
		iReturn = GetUpdatePath(cMd5Path);
		switch(iReturn)
		{
			case -2:
				/* 提示插入SD卡或U盘 */
				ShowUpdateInfo(TransString("Please insert sdcard or u disk"));
				iErr = -1;
				break;
			case -3:
				/* 显示放入升级文件到SD卡或U盘 */
				ShowUpdateInfo(TransString("Please input upgrade file into sdcard or u disk!"));
				iErr = -1;
				break;
			case -4:
				/* 提示磁盘空间不足，请用修复软件修复 */
				ShowUpdateInfo(TransString("The remain space is not enough to upgrade..."));
				iErr = -1;
				break;
			default :
				break;
		}
	}

	/* 获取升级包1、升级应用程序的路径 */
	if(iErr == 0)
	{
		/* 显示正在解压升级文件 */
		ShowUpdateInfo(TransString("Unzipping upgrade file..."));
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 8);

		if(strstr(cMd5Path, "usb"))
		{
			iDeviceType = PAR_USB;
			sprintf(cLogPath, MntUsbDirectory"/log_wndproc.txt");
			sprintf(cUpdateProgramPath, MntUsbDirectory"/update_wndproc");
			sprintf(cCmdBuff, "cd /mnt/usb; md5sum -c -s %s", USBPATH);
		}
		else if(strstr(cMd5Path, "sdcard"))
		{
            iDeviceType = PAR_SD;
			sprintf(cLogPath, SDCardDirectory"/log_wndproc.txt");
			sprintf(cUpdateProgramPath, SDCardDirectory"/update_wndproc");
			sprintf(cCmdBuff, "cd /mnt/sdcard; md5sum -c -s %s", SDPATH);
		}
	}

	/* 输出信息重定向到文件当中 */
	if(iErr == 0)
	{
		int fd = 0;
		fd = open(cLogPath, O_RDWR | O_CREAT | O_TRUNC, 666);
		dup2(fd, 1);	
		dup2(fd, 2);
		mysystem("date");
	}	

	/* 校验升级包1、升级应用程序的md5值 */
	if(iErr == 0)
	{
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 20);
		iReturn = mysystem(cCmdBuff);
		/* md5校验失败 */
		if(iReturn != 0)
		{
			/* 显示升级包损坏 */
			ShowUpdateInfo(TransString("The upgrade file is damage!"));
		
			iErr = -4;				
		}
	}	
	
	/* 启动真正的升级程序进行升级 */
	if(iErr == 0)
	{
 		/* 创建升级服务线程 */
		ThreadCreate(&thdServerFb, ServerThread, cUpdateProgramPath);

		RealUpdate(iDeviceType);
	}
    
	return iErr;
}


/***
   * 功能：
		 执行解压升级功能
   * 参数：
		 无
   * 返回：
		 成功返回零，失败返回非零值
   * 备注：
		 升级包1:	 包含升级包2、升级应用程序、md5校验文件
		 升级包2:	 包含待升级的各个部分文件
 ***/
 static int RealUpdate(int iDeviceType)
 {
 	int iErr = 0;
	
	struct msg Msg;
	int iExitFlg = 0;

	InitMsgQueue();

	/* 开始进行升级命令 */
	MsgSend(CMD_UPDATE, iDeviceType);

	while(!iExitFlg)
	{
		if(GetMsg(&Msg))  
		{  
			continue; 
		}
		
		switch(Msg.iCmdType)
		{
			/* 升级失败类错误 */
			case ERR_MKDIR:
			case ERR_MOUNT_ROOTFS:
			{
		    	unsigned long long rootfsFreeSize = getDiskFreeSize("/");
                printf("free size : %lld\n", rootfsFreeSize);
                if(rootfsFreeSize < 5*1024*1024)
                {
                    /* 提示磁盘空间不足，请用修复软件修复 */
					/*DialogInit(47, 77, TransString("Warning"),
								GetCurrLanguageText(SYSTEM_LBL_INSUFFICIENT_DISK_SPACE),
								1, 2, ReCreateWindow, NULL, NULL);*/
					printf("-----%s-----%d---CmdType = %d-the remain disk space is not enough......\n",
						__func__, __LINE__, Msg.iCmdType);
    				iExitFlg = 1;
    				break;
                }
            }
			case ERR_MOUNT_USERFS:
			case ERR_MOUNT_CACHE:
			case ERR_WRBOOTSTRAP:
			case ERR_WRLOGO:
			case ERR_WRRECOVERY:
			case ERR_WRFPGA:
			case ERR_WRKERNEL:
			case ERR_WRROOTFS:
			case ERR_WRUSERFS:	
				/*DialogInit(47, 77, TransString("Warning"),
							TransString("update failed!"),
							1, 2, ReCreateWindow, UpdateLowVersionOk, UpdateLowVersionCancel);*/	
				printf("-----%s-----%d---CmdType = %d-update failed......\n", __func__, __LINE__, Msg.iCmdType);
				iExitFlg = 1;	
				break;
			/* 解压失败错误 */
			case ERR_EXTRACT:
				/*DialogInit(47, 77, TransString("Warning"),
							GetCurrLanguageText(SYSTEM_LBL_UNZIP_FAILED),
							1, 2, ReCreateWindow, NULL, NULL);*/
				printf("-----%s-----%d---CmdType = %d-unzip failed......\n", __func__, __LINE__, Msg.iCmdType);
				iExitFlg = 1;				
				break;
			/* 机器型号不匹配 */	
			case ERR_SOFT_NOT_MATCH:
				/*DialogInit(47, 77, TransString("Warning"),
							TransString("The machine type is not match!"),
							1, 2, ReCreateWindow, NULL, NULL);*/
				printf("-----%s-----%d---CmdType = %d-The machine type is not match!......\n", __func__, __LINE__, Msg.iCmdType);
				iExitFlg = 1;				
				break;
			/* 待升级应用程序低于当前版本 */	
			case CMD_LOWFLG:
				/*DialogInit(47, 77, TransString("Warning"),
							TransString("update to low version."),
							1, 1, ReCreateWindow, UpdateLowVersionOk, UpdateLowVersionCancel);*/	
				printf("-----%s-----%d---CmdType = %d-update to low version.......\n", __func__, __LINE__, Msg.iCmdType);
				iExitFlg = 1;	
				/* 升级Userfs*/
	            MsgSend(CMD_WRUSERFS, 0);
				break;
			/* 显示"升级成功..."信息 */
			case CMD_WRSUCESS:
				#ifdef TEST_WRITE
				mysystem("reboot");
				#endif
				ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 100);
				/*DialogInit(47, 77, TransString("Warning"),
							GetCurrLanguageText(SYSTEM_LBL_UPDATE_SUCCESS),
							1, 2, ReCreateWindow, NULL, NULL);*/
				ShowUpdateInfo(TransString("update success"));
				printf("-----%s-----%d---CmdType = %d-update success.......\n", __func__, __LINE__, Msg.iCmdType);
				iExitFlg = 1;					
				break;	
			/* 显示"正在写入扇区..."信息 */	
			case CMD_WRSECTOR:	
				ShowUpdateInfo(TransString("Writing sector..."));
				break;					
			/* 显示进度条 */	
			case CMD_SHOWPROGRESS:
				ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, Msg.iParameter);
				break;	
			default:
				break;
		}

		MsecSleep(50);
	}

	return iErr;
 }

/***
  * 功能：
        显示升级过程中的提示信息
  * 参数：
        1、const char *pUpdateInfo:	提示信息
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int ShowUpdateInfo(GUICHAR *pUpdateInfo)
{
	int iErr = 0;

    GUIPEN *pPen;
	unsigned int uiColorBack;		

	if(iErr == 0)
	{
		if(NULL == pUpdateInfo)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		pPen = GetCurrPen();
		uiColorBack = pPen->uiPenColor;
//         MutexLock(&GStatusMutex);
		/* 画背景 */
		pPen->uiPenColor = 0xF4F4F4;
		DrawBlock(UPDATEWIN_X+51, UPDATEWIN_Y+104, UPDATEWIN_X+51 + 300, UPDATEWIN_Y+104 + 60);	
		
		pUpdateStrInfo = pUpdateInfo;
		SetLabelText(pUpdateStrInfo, pUpdateLblInfo);
		DisplayLabel(pUpdateLblInfo);	
		//刷新屏幕缓存
 		RefreshScreen(__FILE__, __func__, __LINE__);

		pPen->uiPenColor = uiColorBack;	
//         MutexUnlock(&GStatusMutex);
	}

	return iErr;
}


/***
* 功能：
	 执行解压升级功能
* 参数：
	 1、char *cmdstring:	需要执行的程序的字符串
* 返回：
	 成功返回零，失败返回非零值
* 备注：
***/
static int Cuetomsystem(char *cmdstring)	 
{
	pid_t				 pid;
	int				 	 status;
	struct sigaction	 ignore, saveintr, savequit;
	sigset_t			 chldmask, savemask;

	if (cmdstring == NULL)
	 	return(1); 	 /* always a command processor with UNIX */

	ignore.sa_handler = SIG_IGN;	 /* ignore SIGINT and SIGQUIT */
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	if (sigaction(SIGINT, &ignore, &saveintr) < 0)
	 	return(-1);
	if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
	 	return(-1);
	sigemptyset(&chldmask);		 /* now block SIGCHLD */
	sigaddset(&chldmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
	 	return(-1);

	if ((pid = fork()) < 0) 
	{
	 	status = -1;	 /* probably out of processes */
	} 
	else if (pid == 0) 
	{ 		 /* child */
		 /* restore previous signal actions & reset signal mask */
		 sigaction(SIGINT, &saveintr, NULL);
		 sigaction(SIGQUIT, &savequit, NULL);
		 sigprocmask(SIG_SETMASK, &savemask, NULL);

		 execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		 _exit(127);	 /* exec error */
	} 
	else 
	{						 /* parent */
	 	waitpid(pid, &status, 0);
		//if (errno != EINTR) {
		//	 status = -1; /* error other than EINTR from waitpid() */
		//	 break;
		// }
	}

	/* restore previous signal actions & reset signal mask */
	if (sigaction(SIGINT, &saveintr, NULL) < 0)
		return(-1);
	if (sigaction(SIGQUIT, &savequit, NULL) < 0)
	 	return(-1);
	if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
	 	return(-1);

	return(status);
}