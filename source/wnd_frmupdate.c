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
#include "guiphoto.h"

#include "app_global.h"
#include "app_frmupdate.h"
#include "app_parameter.h"
#include "app_frmbatteryset.h"

#include "wnd_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_messagebox.h"
#include "wnd_frmsystemsetting.h"

/**********************************************************************************
**								 变量定义				 						 **
***********************************************************************************/
static unsigned char _g_ucStartUpdateFlg = 0;		// 开始闪烁显示升级提示信息的标志

/**********************************************************************************
**								  宏定义				 						 **
***********************************************************************************/
#define UPDATEWIN_X		120
#define UPDATEWIN_Y		90

//临时升级文件路径
#define FPGA_FILE 	"/mnt/usb/f216fpga_XIN_LX16.bin"
#define APP_FILE 	"/mnt/usb/wndproc"
#define VERSION_FILE "/mnt/usb/version.bin"
#define FONT_FILE 	"/mnt/usb/font/language.db"
#define BITMAP_FILE	"/mnt/usb/bitmap/skin1/*"

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
static GUILABEL *pUpdateLblUpdateTitle = NULL;
static GUILABEL *pUpdateLblInfo = NULL;
static GUILABEL *pUpdateLblWarn = NULL;
static GUILABEL *pUpdateLblOk = NULL;
static GUILABEL *pUpdateLblCancel = NULL;


//软件升级内的图片定义
static GUIPICTURE *pUpdateBgProgress  = NULL;       
static GUIPICTURE *pUpdateBtnWarn  = NULL;  
static GUIPICTURE *pUpdateBtnOk = NULL; 
static GUIPICTURE *pUpdateBtnCancel  = NULL;

static GUIMUTEX GStatusMutex;
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
static void ReCreateWindow(GUIWINDOW **pWnd);
static int ShowUpdateInfo(GUICHAR *pUpdateInfo);
static int RealUpdate(int iDeviceType);
static void UpdateLowVersionOk(void);
static void UpdateLowVersionCancel(void);
static int Cuetomsystem(char *cmdstring);	
static void *InfoThread(void *pThreadArg);

typedef struct _update_file_info
{
    unsigned int offset;
    unsigned int size;
}UPDATE_FILE_INFO;

#define UPDATEPATH			"/update"
#define MACH_TYPE          "O10"
#define MACH_LOWEST_VER    "1.0"




#define UPDATE_PARSE_FILE    7
// static char* gUpdateCompPath[UPDATE_PARSE_FILE] = 
// {
//     UPDATEPATH"/firmware.bin",
//     UPDATEPATH"/update_wndproc",
//     UPDATEPATH"/firmware.md5",
//     UPDATEPATH"/Userfs.img",
//     UPDATEPATH"/version.bin",
//     UPDATEPATH"/Rootfs.img",
//     UPDATEPATH"/Recovery.img",
// };

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

	GUITHREAD thdInfoFb = -1;
	static unsigned char ucFirstInitFlg = 1;
    InitMutex(&GStatusMutex, NULL);
	/* 创建动态刷新升级提示信息的线程 */
	if(ucFirstInitFlg)
	{
		ucFirstInitFlg = 0;
		ThreadCreate(&thdInfoFb, NULL, InfoThread, NULL);			
	}	

	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmUpdate = (GUIWINDOW *) pWndObj;

   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    UpdateTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体背景图片 */
	//pUpdateBtnBg = CreatePicture(UPDATEWIN_X, UPDATEWIN_Y, 500, 340, 
    //                                 	BmpFileDirectory"bg_update.bmp");
	pUpdateBtnBg = CreatePicture(UPDATEWIN_X, UPDATEWIN_Y, 400, 300, BmpFileDirectory"bg_update.bmp");//CreatePhoto("bg_update");
	//软件升级内的图片定义
	pUpdateBgProgress = CreatePicture(UPDATEWIN_X + 10, UPDATEWIN_Y+20, WINDOW_WIDTH, 20, NULL);
	pUpdateBtnWarn = CreatePicture(UPDATEWIN_X + 170, UPDATEWIN_Y + 75, 60, 50,
								   BmpFileDirectory "bg_dialog_warring.bmp");

#ifdef FIRST_WRITE
	pUpdateBtnOk = CreatePicture(UPDATEWIN_X + 124, UPDATEWIN_Y + 95, 55, 18,
										BmpFileDirectory"btn_dialog_press.bmp");
	pUpdateBtnCancel = CreatePicture(UPDATEWIN_X + 46, UPDATEWIN_Y + 95, 55, 18,
                                     	NULL);

	pUpdateLblOk = CreateLabel(UPDATEWIN_X + 124, UPDATEWIN_Y + 95, 55, 18,
								  		    pUpdateStrOk);	
	pUpdateLblCancel = CreateLabel(UPDATEWIN_X + 46, UPDATEWIN_Y + 95, 55, 18,
								  		    NULL);	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblOk);	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblCancel);	

	SetPictureEnable(0, pUpdateBtnCancel);
	SetLabelEnable(0, pUpdateLblCancel);

	#else
	pUpdateBtnOk = CreatePicture(UPDATEWIN_X, UPDATEWIN_Y+ 259, 200, 41,
                                     	BmpFileDirectory"btn_dialog_press.bmp");	
	pUpdateBtnCancel = CreatePicture(UPDATEWIN_X+ 200, UPDATEWIN_Y+259, 200, 41,
                                     	BmpFileDirectory"btn_dialog_unpress.bmp");

	pUpdateLblOk = CreateLabel(UPDATEWIN_X + 84, UPDATEWIN_Y + 271, 100, 16,
							   pUpdateStrOk);
	pUpdateLblCancel = CreateLabel(UPDATEWIN_X + 285, UPDATEWIN_Y + 271, 100, 16,
								   pUpdateStrCancel);
	// SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblOk);	
	// SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblCancel);	
	#endif
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pUpdateLblOk);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pUpdateLblCancel);
	//软件升级内的Label定义
	pUpdateLblUpdateTitle = CreateLabel(UPDATEWIN_X+8, UPDATEWIN_Y+9, 100, 16, 
								  		    pUpdateStrUpdateTitle);
	pUpdateLblInfo = CreateLabel(UPDATEWIN_X+81, UPDATEWIN_Y+104, 200, 20, pUpdateStrInfo);	
	pUpdateLblWarn = CreateLabel(UPDATEWIN_X+81, UPDATEWIN_Y+150, 200, 16, pUpdateStrWarn);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pUpdateLblInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pUpdateLblWarn);
	//SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblUpdateTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pUpdateLblInfo);
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

	_g_ucStartUpdateFlg = 0;

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
    DestroyPicture(&pUpdateBtnWarn);
	DestroyPicture(&pUpdateBtnOk);	
	DestroyPicture(&pUpdateBtnCancel);	
	
	//销毁软件升级内的Label
	DestroyLabel(&pUpdateLblUpdateTitle);
	DestroyLabel(&pUpdateLblInfo);	
	DestroyLabel(&pUpdateLblWarn);
	DestroyLabel(&pUpdateLblOk);
	DestroyLabel(&pUpdateLblCancel);
	
	//释放文本资源
    //***************************************************************/
    UpdateTextRes_Exit(NULL, 0, NULL, 0);
	MutexDestroy(&GStatusMutex);
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

	DispTransparent(80, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    //显示桌面背景图片
    DisplayPicture(pUpdateBtnBg);

	//显示软件升级内的图片
    DisplayPicture(pUpdateBgProgress);
    DisplayPicture(pUpdateBtnWarn);
	DisplayPicture(pUpdateBtnOk);
	DisplayPicture(pUpdateBtnCancel);	

	//显示软件升级内的Label
	DisplayLabel(pUpdateLblUpdateTitle);
	// DisplayLabel(pUpdateLblInfo);	
	DisplayLabel(pUpdateLblWarn);
	DisplayLabel(pUpdateLblOk);
	DisplayLabel(pUpdateLblCancel);

	SetPowerEnable(0, 1);
	//刷新屏幕缓存
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
	// if((CheckPowerOn(BATTERY) == 1) && (CheckBatteryValueOverHalf() == 1))
	if (1)
	{
		if(!pUpdateBtnOk->Visible.iEnable)
		{
			SetPictureEnable(1, pUpdateBtnOk);
			TouchChange("btn_dialog_unpress.bmp", pUpdateBtnOk, pUpdateStrOk, pUpdateLblOk, 1);
			ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_PRESS_UPDATE));
		}
	}
	else
	{
		if(pUpdateBtnOk->Visible.iEnable)
		{
			TouchChange("btn_dialog_unpress.bmp", pUpdateBtnOk, pUpdateStrOk, pUpdateLblOk, 3);
			SetPictureEnable(0, pUpdateBtnOk);
			ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_INPUT_BATTERY));
		}
	}
    MsecSleep(10);
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
	pUpdateStrUpdateTitle = GetCurrLanguageText(SYSTEM_LBL_UPDATE);
	pUpdateStrInfo = GetCurrLanguageText(SYSTEM_LBL_PRESS_UPDATE);
	pUpdateStrWarn = GetCurrLanguageText(SYSTEM_LBL_POWER_OVER_50);
	pUpdateStrOk = GetCurrLanguageText(SYSTEM_LBL_UPDATE);
	pUpdateStrCancel = GetCurrLanguageText(SYSTEM_LBL_CANCEL);
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
	GuiMemFree(pUpdateStrUpdateTitle);
	GuiMemFree(pUpdateStrInfo);
	GuiMemFree(pUpdateStrWarn);
	GuiMemFree(pUpdateStrOk);	
	GuiMemFree(pUpdateStrCancel);	

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
		case KEYCODE_ESC:
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
	case KEYCODE_ENTER:
		UpdateBtnUpdate_Down(NULL, 0, NULL, 0);
		UpdateBtnUpdate_Up(NULL, 0, NULL, 0);
		break;
	case KEYCODE_ESC:
		UpdateBtnCancel_Down(NULL, 0, NULL, 0);
		UpdateBtnCancel_Up(NULL, 0, NULL, 0);
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

	TouchChange("btn_dialog_press.bmp", pUpdateBtnOk, 
				NULL, pUpdateLblOk, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

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

	TouchChange("btn_dialog_press.bmp", pUpdateBtnCancel, 
				NULL, pUpdateLblCancel, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

    return iReturn;

}

#ifdef TEST_UPDATE
//临时测试升级，仅升级fpga，wndproc
static int TestUpgrade(void)
{
	int iErr = 0;
	//获得rootfs分区剩余空间大小
	/* 查找md5文件的路径 */
	if (iErr == 0)
	{
		/* 显示尝试加载U盘或SD卡 */
		ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_LOAD_DISK));
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 2);

		/* 延时一段时间，等待U盘挂载 */
		MsecSleep(3000);

		/* 显示查找升级文件 */
		ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_SEARCH_UPDATE_FILE));
		/* 挂载U盘成功 */
		if (access(USB, F_OK) == -1)
		{
			iErr = -1;
		}
	}

	/* 获取升级包1、升级应用程序的路径 */
	if (iErr == 0)
	{
		/* 显示正在解压升级文件 */
		ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_UNZIPPING_UPDATE_FILE));
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 8);
	}

	if (iErr == 0)
	{
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 20);
		char cCmdBuff[128] = {0};
		memset(cCmdBuff, 0, 128);
		//升级fpga
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 40);
		ShowUpdateInfo(TransString("write fpga..."));
		sprintf(cCmdBuff, "cp "FPGA_FILE" "MntSDcardDataDirectory" -rf");
		mysystem(cCmdBuff);
		//升级应用程序
		//1、bitmap
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 60);
		ShowUpdateInfo(TransString("write bitmap..."));
		memset(cCmdBuff, 0, 128);
		sprintf(cCmdBuff, "cp "BITMAP_FILE" "BmpFileDirectory" -rf");
		mysystem(cCmdBuff);
		//2、font
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 80);
		ShowUpdateInfo(TransString("write font..."));
		memset(cCmdBuff, 0, 128);
		sprintf(cCmdBuff, "cp "FONT_FILE" "FontFileDirectory" -rf");
		mysystem(cCmdBuff);
		//3、wndproc
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 90);
		ShowUpdateInfo(TransString("write wndproc..."));
		memset(cCmdBuff, 0, 128);
		sprintf(cCmdBuff, "cp "APP_FILE" "MntSDcardDataDirectory" -rf");
		mysystem(cCmdBuff);
		memset(cCmdBuff, 0, 128);
		sprintf(cCmdBuff, "cp "VERSION_FILE" "MntSDcardDataDirectory" -rf");
		mysystem(cCmdBuff);
		mysystem("sync");
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 100);
		DialogInit(120, 90, GetCurrLanguageText(SYSTEM_LBL_UPDATE),
				   GetCurrLanguageText(SYSTEM_LBL_UPDATE_SUCCESS),
				   1, 2, ReCreateWindow, NULL, NULL);
	}

	return iErr;
}
#endif
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
		TouchChange("btn_dialog_unpress.bmp", pUpdateBtnOk, 
					NULL, pUpdateLblOk, 1);
		//刷新屏幕缓存
		// RefreshScreen(__FILE__, __func__, __LINE__);

		/* 在开始升级文件前，确定按钮不可见，右侧区域按钮不可用，防止用户误操作 */	

		/* 确定和取消按钮消失 */
		SetPictureEnable(0, pUpdateBtnOk);
		SetPictureEnable(0, pUpdateBtnCancel);
		
		//显示桌面背景图片
		DisplayPicture(pUpdateBtnBg);
		DisplayLabel(pUpdateLblUpdateTitle);
		//刷新屏幕缓存
		RefreshScreen(__FILE__, __func__, __LINE__);		
	}

	if(iErr == 0)
	{
		//开始启动升级
		_g_ucStartUpdateFlg = 1;

		SetPowerManageEnable(0);
#ifndef TEST_UPDATE
		iReturn = Update();
#else
		iReturn = TestUpgrade();
#endif
		SetPowerManageEnable(1);	
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
			RefreshScreen(__FILE__, __func__, __LINE__);
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

	GUIWINDOW *pWnd = NULL;

	TouchChange("btn_dialog_unpress.bmp", pUpdateBtnCancel, 
				NULL, pUpdateLblCancel, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSystemSettingInit, FrmSystemSettingExit,
						FrmSystemSettingPaint, FrmSystemSettingLoop,
						FrmSystemSettingPause, FrmSystemSettingResume,
						NULL);			
	SendWndMsg_WindowExit(pFrmUpdate);		
	SendSysMsg_ThreadCreate(pWnd);	

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
		创建动态刷新升级提示信息的线程
  * 参数：
		无
  * 返回：
		无
  * 备注：
  		实现提示信息闪烁效果，增加用户体验，防止升级过程中误认为死机
***/
static void *InfoThread(void *pThreadArg)
{
	GUIPEN *pPen;
	unsigned int uiColorBack;	

	while(1)
	{
		MsecSleep(50);
		if(_g_ucStartUpdateFlg)
		{	
			MsecSleep(1000);
			if(_g_ucStartUpdateFlg == 0)
			{
				continue;
			}
            MutexLock(&GStatusMutex);
			pPen = GetCurrPen();
			uiColorBack = pPen->uiPenColor;
			/* 画背景 */
			SetLabelFont(getGlobalFnt(EN_FONT_RED), pUpdateLblInfo);
			DisplayLabel(pUpdateLblInfo);
			pPen->uiPenColor = uiColorBack;	
			RefreshScreen(__FILE__, __func__, __LINE__);
			MutexUnlock(&GStatusMutex);
			MsecSleep(1000);
			if(_g_ucStartUpdateFlg == 0)
			{
				continue;
			}
            MutexLock(&GStatusMutex);
			pPen = GetCurrPen();
			uiColorBack = pPen->uiPenColor;
			/* 画背景 */
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pUpdateLblInfo);
			DisplayLabel(pUpdateLblInfo);	
			pPen->uiPenColor = uiColorBack;	
			RefreshScreen(__FILE__, __func__, __LINE__);
            MutexUnlock(&GStatusMutex);
		}	
	}
    return NULL;   
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
		ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_LOAD_DISK));
		/* 显示进度 */
		ShowProgressBar(UPDATEWIN_X + 50, UPDATEWIN_Y + 175, 300, 12, 2);

		/* 延时一段时间，等待U盘挂载 */
		MsecSleep(3000);
		/* 显示查找升级文件 */
		ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_SEARCH_UPDATE_FILE));
		iReturn = GetUpdatePath(cMd5Path);
		switch(iReturn)
		{
			case -2:
				/* 提示插入SD卡或U盘 */
				ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_INSERT_DISK));
				iErr = -1;
				break;
			case -3:
				/* 显示放入升级文件到SD卡或U盘 */
				ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_INPUT_UPDATE_FILE));
				iErr = -1;
				break;
			case -4:
				/* 提示磁盘空间不足，请用修复软件修复 */
				ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_INSUFFICIENT_DISK_SPACE));
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
		ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_UNZIPPING_UPDATE_FILE));
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
			sprintf(cLogPath, MntSDcardDataDirectory"/log_wndproc.txt");
			sprintf(cUpdateProgramPath, MntSDcardDataDirectory"/update_wndproc");
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
			ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_DAMAGED_UPDATING_FILE));
		
			iErr = -4;				
		}
	}	
	
	/* 启动真正的升级程序进行升级 */
	if(iErr == 0)
	{
 		/* 创建升级服务线程 */
 		ThreadCreate(&thdServerFb, NULL, ServerThread, cUpdateProgramPath);	
 
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
				LOG(LOG_INFO, "free size : %lld\n", rootfsFreeSize);
				if(rootfsFreeSize < 5*1024*1024)
                {
                    /* 提示磁盘空间不足，请用修复软件修复 */
    				DialogInit(120, 90, TransString("Warning"),
    							GetCurrLanguageText(SYSTEM_LBL_INSUFFICIENT_DISK_SPACE),
    							0, 2, ReCreateWindow, NULL, NULL);
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
				DialogInit(120, 90, TransString("Warning"),
							TransString("update failed!"),
							0, 2, ReCreateWindow, UpdateLowVersionOk, UpdateLowVersionCancel);	
				iExitFlg = 1;	
				break;
			/* 解压失败错误 */
			case ERR_EXTRACT:
				DialogInit(120, 90, TransString("Warning"),
							GetCurrLanguageText(SYSTEM_LBL_UNZIP_FAILED),
							0, 2, ReCreateWindow, NULL, NULL);	
				iExitFlg = 1;				
				break;
			/* 机器型号不匹配 */	
			case ERR_SOFT_NOT_MATCH:
				DialogInit(120, 90, TransString("Warning"),
							TransString("The machine type is not match!"),
							0, 2, ReCreateWindow, NULL, NULL);
				iExitFlg = 1;				
				break;
			/* 待升级应用程序低于当前版本 */	
			case CMD_LOWFLG:
				DialogInit(58, 100, TransString("Warning"),
							TransString("update to low version."),
							0, 1, ReCreateWindow, UpdateLowVersionOk, UpdateLowVersionCancel);	
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
				DialogInit(120, 90, GetCurrLanguageText(SYSTEM_LBL_UPDATE),
						   GetCurrLanguageText(SYSTEM_LBL_UPDATE_SUCCESS),
						   0, 2, ReCreateWindow, NULL, NULL);
				iExitFlg = 1;					
				break;	
			/* 显示"正在写入扇区..."信息 */	
			case CMD_WRSECTOR:	
				ShowUpdateInfo(GetCurrLanguageText(SYSTEM_LBL_WRITING_SECTOR));
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
		 升级到低版本
   * 参数：
		 无
   * 返回：
		 无
   * 备注：
 ***/
static void UpdateLowVersionOk(void)
{
	struct msg Msg;

	ShowDialogExtraInfo(GetCurrLanguageText(SYSTEM_LBL_WRITING_SECTOR));

	/* 升级Userfs */
	MsgSend(CMD_WRUSERFS, 0);
	while(1)
	{
		if(GetMsg(&Msg))  
		{  
			continue; 
		} 
	
		switch(Msg.iCmdType)
		{
			/* 显示"升级成功..."信息 */
			case CMD_WRSUCESS:		
				ShowDialogExtraInfo(GetCurrLanguageText(SYSTEM_LBL_UPDATE_SUCCESS));
				break;					
			default:
				break;
		}

		MsecSleep(50);
	}	
}

/***
* 功能：
	 用户选择不升级到低版本
* 参数：
	 无
* 返回：
	 无
* 备注：
***/
static void UpdateLowVersionCancel(void)
{
	struct msg Msg;

	ShowDialogExtraInfo(GetCurrLanguageText(SYSTEM_LBL_WRITING_SECTOR));

	/* 保存参数 */
	MsgSend(CMD_SAVEPARAMETER, 0);

	while(1)
	{
		MsecSleep(50);
	
		if(GetMsg(&Msg))  
		{  
			continue; 
		} 
	
		switch(Msg.iCmdType)
		{
			/* 显示"升级成功..."信息 */
			case CMD_WRSUCESS:		
				//ShowDialogExtraInfo(GetCurrLanguageText(UPDATE_UPDATE_SUCESS));
				return;
				break;					
			default:
				break;
		}
	}	
}


/***
  * 功能：
        用于对话框重新还原窗体时的回调函数
  * 参数：
        1.GUIWINDOW **pWnd:		指向窗体对象的指针的指针
  * 返回：
        无
  * 备注：
***/
static void ReCreateWindow(GUIWINDOW **pWnd)
{	
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmUpdateInit, FrmUpdateExit, 
                    FrmUpdatePaint, FrmUpdateLoop, 
			        FrmUpdatePause, FrmUpdateResume,
                    NULL);
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
        MutexLock(&GStatusMutex);
		/* 画背景 */
		pPen->uiPenColor = 0xF4F4F4;
		DrawBlock(UPDATEWIN_X + 51, UPDATEWIN_Y + 104, UPDATEWIN_X + 51 + 300, UPDATEWIN_Y + 104 + 60);
		
		pUpdateStrInfo = pUpdateInfo;
		SetLabelText(pUpdateStrInfo, pUpdateLblInfo);
		DisplayLabel(pUpdateLblInfo);	
		//刷新屏幕缓存
		RefreshScreen(__FILE__, __func__, __LINE__);

		pPen->uiPenColor = uiColorBack;	
        MutexUnlock(&GStatusMutex);
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