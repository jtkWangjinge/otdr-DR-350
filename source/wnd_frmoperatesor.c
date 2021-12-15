/*******************************************************************************
* Copyright(c)2015，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmoperatesor.c
* 摘    要：  声明窗体wnd_frmsavedefdir的窗体处理线程及相关操作函数
*			  该窗体提供保存、读取USB设备中的SOR文件
*
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015.07.22
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmoperatesor.h"


/*******************************************************************************
**				为实现窗体wnd_frmoperatesor而需要引用的其他头文件 			  **
*******************************************************************************/
#include "app_frmotdr.h"

#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmdialog.h"
#include "guimyglobal.h"
#include "app_frmfilebrowse.h"
#include "wnd_frmfilebrowse1.h"
#include "wnd_frmfip.h"
#include "wnd_frmmenubak.h"
#include "wnd_stack.h"
#include "wnd_frmotdr.h"
#include "wnd_frmsola.h"
#include "wnd_messagebox.h"
#include "app_filesor.h"
#include "app_sorviewer.h"
#include "guiphoto.h"
#include "app_solafile.h"
#include "wnd_frmfactoryfirmware.h"
#include "wnd_frmlanguageconfig.h"

#include "wnd_frmsolasave.h"
#include "wnd_frmotdrsave.h"
/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
static WNDMENU1 *pOperateSorMenu = NULL;					// menu控件
static const char *__g_pOperateSorTopPath = NULL;			// 顶层目录路径
static char* __g_pOperateSDCurrPath = NULL;			        // SD卡当前所在路径
static char __g_pOperateFirstPath[512];                     // 进入的路径
static FILETYPE __g_fileType = SOR;
static unsigned int _g_enFileOperation = FILE_OPEN_MODE;	// 文件操作类型
static CALLLBACKWINDOW _g_OperateSorcallBackFunc = NULL;	// 重绘前一个窗体的回调函数
extern POTDR_TOP_SETTINGS pOtdrTopSettings;					// 当前OTDR的工作设置
extern MULTISELECT *pEnvQueue;
/*******************************************************************************
**						窗体frmOperateSor中的控件定义部分					  **
*******************************************************************************/
static GUIWINDOW *pFrmOperateSor = NULL;

/**********************************背景控件定义********************************/
static GUIPICTURE *pOperateSorBg = NULL;
/*********************************标题栏控件定义*******************************/
static GUIPICTURE *pOtdrFileOpenBtnTop = NULL;

static GUIPICTURE *pOtdrFileOpenBtnTop1 = NULL;
static GUIPICTURE *pOtdrFileOpenBtnTop2 = NULL;
static GUICHAR *pOperateSorStrTitle1 = NULL;
static GUILABEL *pOperateSorLblTitle1 = NULL;
static GUICHAR *pOperateSorStrTitle2 = NULL;
static GUILABEL *pOperateSorLblTitle2 = NULL;


/*******************************************************************************
**	    窗体frmOperateSor中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/
static int OperateSorTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int OperateSorTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

static int  OperateSorBtnTop1_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int  OperateSorBtnTop1_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
static int FrmOperateSorInit(void *pWndObj);
static int FrmOperateSorExit(void *pWndObj);
static int FrmOperateSorPaint(void *pWndObj);
static int FrmOperateSorLoop(void *pWndObj);
static int FrmOperateSorPause(void *pWndObj);
static int FrmOperateSorResume(void *pWndObj);


/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static void ReCreateWindow(GUIWINDOW **pWnd);
static void OperateSorMenuCall(int iOp);
static int OpenSor(void);
static int SaveSor(void);
static int OpenSola();
static int OpenFip();
static int OpenFactoryFirmware();
//打开任意文件
static void OpenAnyFile(void);
//退出当前界面
static void ExitCurrentWnd(void);
//刷新右侧菜单栏
static void RefreshRightMenu(int flag);
//设置右侧菜单栏的使能
static void SetRightMenuEnable(int num);
//打开上次访问的路径
static void OpenLastPath(void);

/***
  * 功能：
		窗体frmOperateSor的初始化函数，建立窗体控件、注册消息处理
  * 参数：
		1.void *pWndObj:    指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOperateSorInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	unsigned int strOpenMenu[] = {
		1
	};
	unsigned int strOpenMenu1[] = {
		1	
	};
	unsigned int strSaveMenu[] = {
		1
	};

	GUIMESSAGE *pMsg = NULL;

	//得到当前窗体对象
	pFrmOperateSor = (GUIWINDOW *)pWndObj;

	/* 文件浏览控件初始化 */
	SetDisplayFileTypeAndSelectAll(0);
	FrmFileBrowseInit1(0, 40, MAXPAGEFILENUM, ReCreateWindow, pWndObj);
	SetFileSelectMode(FILE_SINGLE_MODE);
	SetWidgetUpdateCallBack(NULL);
	SetFileOperationType(_g_enFileOperation);
	SetCurFileType(__g_fileType);

	//初始化文本资源
	//如果GUI存在多国语言，在此处获得对应语言的文本资源
	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	/****************************************************************/
	OperateSorTextRes_Init(NULL, 0, NULL, 0);

	/* 建立桌面背景 */
	pOperateSorBg = CreatePhoto("bg_filebrowse");
	pOtdrFileOpenBtnTop = CreatePhoto("bg_filebrowse_title");

	/*********************************标题栏控件定义*******************************/
	pOperateSorLblTitle1 = CreateLabel(0, 24, 100, 24,
		pOperateSorStrTitle1);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOperateSorLblTitle1);

	pOperateSorLblTitle2 = CreateLabel(101, 24, 100, 24,
		pOperateSorStrTitle2);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOperateSorLblTitle2);

	if (__g_fileType == SOLA || __g_fileType == SORANDCUR)
	{
		pOtdrFileOpenBtnTop1 = CreatePhoto("otdr_top1");
		pOtdrFileOpenBtnTop2 = CreatePhoto("otdr_top2f");
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOperateSorLblTitle1);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOperateSorLblTitle2);
	}
	else
	{
		pOtdrFileOpenBtnTop1 = CreatePhoto("otdr_top1f");
		pOtdrFileOpenBtnTop2 = CreatePhoto("otdr_top2");
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOperateSorLblTitle1);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOperateSorLblTitle2);
	}

	//侧边栏menu
	if (_g_enFileOperation == FILE_OPEN_MODE)
	{
		if (strcmp(__g_pOperateFirstPath, MntUsbDirectory) == 0)
		{
			pOperateSorMenu = CreateWndMenu1(2, sizeof(strOpenMenu1), strOpenMenu1,
				(UINT16)(MENU_BACK | ~MENU_HOME),
				-1, 0, 40, OperateSorMenuCall);
		}
		else
		{
			pOperateSorMenu = CreateWndMenu1(3, sizeof(strOpenMenu), strOpenMenu,
				(UINT16)(MENU_BACK | ~MENU_HOME),
				-1, 0, 40, OperateSorMenuCall);
		}
		SetFirstOpenFolder(__g_pOperateFirstPath);
	}
	else if (_g_enFileOperation == FILE_SAVE_MODE)
	{
		pOperateSorMenu = CreateWndMenu1(2, sizeof(strSaveMenu), strSaveMenu,
			(UINT16)(MENU_BACK | ~MENU_HOME),
			-1, 0, 40, OperateSorMenuCall);
	}

	//注册窗体(因为所有的按键事件都统一由窗体进行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmOperateSor,
		pFrmOperateSor);

	//添加消息到控件队列
	AddWndMenuToComp1(pOperateSorMenu, pFrmOperateSor);

	if (__g_fileType == SOLA || __g_fileType == SORANDCUR)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrFileOpenBtnTop1,
			pFrmOperateSor);
	}

	//注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
   //即此处的操作应当在注册窗体控件的基础上进行
   //注册消息处理函数必须在注册窗体控件之后进行
   //***************************************************************/
	pMsg = GetCurrMessage();
	//注册菜单控件的消息处理函数
	LoginWndMenuToMsg1(pOperateSorMenu, pFrmOperateSor);

	if (__g_fileType == SOLA || __g_fileType == SORANDCUR)
	{
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrFileOpenBtnTop1,
			OperateSorBtnTop1_Up, NULL, 0, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrFileOpenBtnTop1,
			OperateSorBtnTop1_Down, NULL, 0, pMsg);
	}

	return iReturn;
}


/***
  * 功能：
		窗体frmOperateSor的退出函数，释放所有资源
  * 参数：
		1.void *pWndObj:    指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOperateSorExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;

	//得到当前窗体对象
	pFrmOperateSor = (GUIWINDOW *)pWndObj;

	//清空消息队列中的消息注册项
	//***************************************************************/
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);

	//从当前窗体中注销窗体控件
	//***************************************************************/
	ClearWindowComp(pFrmOperateSor);

	/* 销毁文件浏览器控件 */
	FrmFileBrowseExit1(pWndObj);

	/* 销毁右侧菜单栏 */
	DestroyWndMenu1(&pOperateSorMenu);

	/*******************************销毁桌面背景控件*******************************/
	DestroyPicture(&pOperateSorBg);
	DestroyPicture(&pOtdrFileOpenBtnTop);
	DestroyPicture(&pOtdrFileOpenBtnTop1);
	DestroyPicture(&pOtdrFileOpenBtnTop2);

	/*********************************销毁标题栏控件*******************************/
	DestroyLabel(&pOperateSorLblTitle1);
	DestroyLabel(&pOperateSorLblTitle2);

	/* 释放文本 */
	OperateSorTextRes_Exit(NULL, 0, NULL, 0);

	return iReturn;
}

/***
  * 功能：
		窗体frmOperateSor的绘制函数，绘制整个窗体
  * 参数：
		1.void *pWndObj:    指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOperateSorPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pFrmOperateSor = (GUIWINDOW *)pWndObj;

	//显示状态栏、桌面、信息栏
	//显示状态栏上的控件

	//显示桌面背景
	DisplayPicture(pOperateSorBg);
	DisplayPicture(pOtdrFileOpenBtnTop);
	DisplayPicture(pOtdrFileOpenBtnTop1);
	DisplayPicture(pOtdrFileOpenBtnTop2);
	/* 显示标题栏 */
	DisplayLabel(pOperateSorLblTitle1);
	DisplayLabel(pOperateSorLblTitle2);

	DisplayWndMenu1(pOperateSorMenu);

	FrmFileBrowsePaint1(pWndObj);

	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
		窗体frmOperateSor的循环函数，进行窗体循环
  * 参数：
		1.void *pWndObj:    指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOperateSorLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	static int currPathIsUSBorSDCARD = 0;   //1 SDCARD 2 USB

	char currPath[512];
	memset(currPath, 0, 512);
	getcwd(currPath, 512);

	if (access(MntUsbDirectory, F_OK) != 0)
	{
		if (currPathIsUSBorSDCARD == 2)
		{
			OpenLastPath();
		}
	}

	if (access(MntSDcardDataDirectory, F_OK) != 0)
	{

		if (currPathIsUSBorSDCARD == 1)
		{
			currPathIsUSBorSDCARD = 0;
			ExitCurrentWnd();
			SendWndMsg_LoopDisable(pFrmOperateSor);
		}
	}

	if (!strncmp(currPath, MntUsbDirectory, strlen(MntUsbDirectory)))
	{
		currPathIsUSBorSDCARD = 2;
	}
	else if (!strncmp(currPath, MntSDcardDataDirectory, strlen(MntSDcardDataDirectory)))
	{
		currPathIsUSBorSDCARD = 1;
	}

	MsecSleep(10);

	return iReturn;
}


/***
  * 功能：
		窗体frmOperateSor的挂起函数，进行窗体挂起前预处理
  * 参数：
		1.void *pWndObj:    指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOperateSorPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
		窗体frmOperateSor的恢复函数，进行窗体恢复前预处理
  * 参数：
		1.void *pWndObj:    指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOperateSorResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/*******************************************************************************
**	    窗体frmOperateSor中的初始化文本资源、 释放文本资源函数定义部分		  **
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
static int OperateSorTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	if (_g_enFileOperation == FILE_OPEN_MODE)
	{
		if (__g_fileType == SOLA || __g_fileType == SORANDCUR)
		{
			pOperateSorStrTitle1 = TransString("OTDR_SAVE_LBL_SAVEFILE");
			pOperateSorStrTitle2 = TransString("OTDR_SAVE_LBL_OPENFILE");
		}
		else
		{
			pOperateSorStrTitle1 = TransString("OTDR_SAVE_LBL_OPENFILE");
			pOperateSorStrTitle2 = TransString(" ");
		}
	}
	else if (_g_enFileOperation == FILE_SAVE_MODE)
	{
		pOperateSorStrTitle1 = TransString("OTDR_SAVE_LBL_SAVEFILE");
		pOperateSorStrTitle2 = TransString(" ");
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
static int OperateSorTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pOperateSorStrTitle1);
	GuiMemFree(pOperateSorStrTitle2);

	return iReturn;
}

static void BackCall(void)
{
	char cPathBuff[MAXPATHNAME] = {0};
	getcwd(cPathBuff, MAXPATHNAME);
	if ((strcmp(__g_pOperateSorTopPath, cPathBuff) == 0) ||
		(strcmp(cPathBuff, MntSDcardDataDirectory) == 0))
	{
		strcpy(__g_pOperateSDCurrPath, cPathBuff);
		ExitCurrentWnd();
		return;
	}

	if (strcmp(cPathBuff, MntUsbDirectory) == 0)
	{
		OpenLastPath();
	}
	else
	{
		OpenFileAndFolder("..");
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
}

static void OpenFileOrOpenFromUsb(void)
{
	char cPathBuff[MAXPATHNAME] = {};
	/* 判断当前是否在U盘目录下 */
	getcwd(cPathBuff, MAXPATHNAME);
	if (strncmp(cPathBuff, MntUsbDirectory, strlen(MntUsbDirectory)) == 0)//打开
	{
		OpenAnyFile();
	}
	else//从U盘打开
	{
		getcwd(__g_pOperateSDCurrPath, MAXPATHNAME);
		if (access(MntUsbDirectory, F_OK) == 0)
		{
			OpenFileAndFolder(MntUsbDirectory);
			RefreshRightMenu(0);
		}
		else
		{
			getcwd(__g_pOperateFirstPath, MAXPATHNAME);
			DialogInit(120, 90, TransString("Warning"),
				TransString("DIALOG_NO_USB"),
				0, 0, ReCreateWindow, NULL, NULL);
		}
	}
}

static void OpenFileOrExit(void)
{
	char cPathBuff[MAXPATHNAME] = {0};
	/* 判断当前是否在U盘目录下 */
	getcwd(cPathBuff, MAXPATHNAME);
	if (strncmp(cPathBuff, MntUsbDirectory, strlen(MntUsbDirectory)) == 0)//退出
	{
		OpenLastPath();
	}
	else//打开
	{
		OpenAnyFile();
	}
}

/***
  * 功能：
		用于对话框重新还原窗体时的回调函数
  * 参数：
		1.GUIWINDOW **pWnd	:	指向窗体的指针
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static void ReCreateWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmOperateSorInit, FrmOperateSorExit,
		FrmOperateSorPaint, FrmOperateSorLoop,
		FrmOperateSorPause, FrmOperateSorResume,
		NULL);
}

/***
  * 功能：
		菜单控件回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void OperateSorMenuCall(int iOp)
{
	switch (iOp)
	{
	case 0:
		OpenFileOrOpenFromUsb();
		break;
	case 1:
		OpenFileOrExit();
		break;
	case 2://退出
		getcwd(__g_pOperateSDCurrPath, MAXPATHNAME);
		ExitCurrentWnd();
		break;
	case BACK_DOWN:
		BackCall();
		break;
	default:
		break;
	}
}

static int  OperateSorBtnTop1_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;

	if (__g_fileType == SORANDCUR)
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmOtdrSaveInit, FrmOtdrSaveExit,
			FrmOtdrSavePaint, FrmOtdrSaveLoop,
			FrmOtdrSavePause, FrmOtdrSaveResume,
			NULL);
	}
	else if (__g_fileType == SOLA)
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmSOLASaveInit, FrmSOLASaveExit,
			FrmSOLASavePaint, FrmSOLASaveLoop,
			FrmSOLASavePause, FrmSOLASaveResume,
			NULL);
	}

	SendWndMsg_WindowExit(pFrmOperateSor);
	SendSysMsg_ThreadCreate(pWnd);

	return iReturn;
}

static int  OperateSorBtnTop1_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	int iReturn = 0;

	return iReturn;
}

//打开任意文件
static void OpenAnyFile(void)
{
	if (_g_enFileOperation == FILE_OPEN_MODE)
	{
		if (__g_fileType == SOR)
		{
			OpenSor();
		}
		else if (__g_fileType == SOLA)
		{
			OpenSola();
		}
		else if (__g_fileType == GDM)
		{
			OpenFip();
		}
		else if (__g_fileType == BIN)
		{
			OpenFactoryFirmware();
		}
	}
	else if (_g_enFileOperation == FILE_SAVE_MODE)
	{
		if (__g_fileType == SOR)
		{
			SaveSor();
		}
		else if (__g_fileType == SOLA)
			;
	}
}
//退出当前界面
static void ExitCurrentWnd(void)
{
	GUIWINDOW *pWnd = NULL;
	(*_g_OperateSorcallBackFunc)(&pWnd);
	SendWndMsg_WindowExit(pFrmOperateSor);
	SendSysMsg_ThreadCreate(pWnd);
}
//刷新右侧菜单栏
static void RefreshRightMenu(int flag)
{
	if (flag)
	{
		unsigned int strOpenMenu[] = {
			1
		};
		RefreshMenu1(pOperateSorMenu, -1, 6, (UINT16)(MENU_BACK | ~MENU_HOME),
			sizeof(strOpenMenu), strOpenMenu, 1);
		SetRightMenuEnable(3);
	}
	else
	{
		unsigned int strOpenMenu[] = {
			1
		};

		RefreshMenu1(pOperateSorMenu, -1, 6, (UINT16)(MENU_BACK | ~MENU_HOME),
			sizeof(strOpenMenu), strOpenMenu, 1);
		SetRightMenuEnable(2);
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
}
//使能右侧菜单栏
static void SetRightMenuEnable(int num)
{
	int i = 0;
	for (i = 0; i < 6; ++i)
	{
		if (i < num)
		{
			SetWndMenuItemEnble(i, 1, pOperateSorMenu);
		}
		else
		{
			SetWndMenuItemEnble(i, 0, pOperateSorMenu);
		}
	}
}

//打开上次访问的路径(回到sd卡路径)
static void OpenLastPath(void)
{
	if (strcmp(__g_pOperateSorTopPath, MntUsbDirectory) == 0)
	{
		ExitCurrentWnd();
		return;
	}

	if (access(__g_pOperateSDCurrPath, F_OK) != 0)
	{
		if (access(MntSDcardDataDirectory, F_OK) != 0)
		{
			ExitCurrentWnd();
			return;
		}
		strcpy(__g_pOperateSDCurrPath, MntSDcardDataDirectory);
	}
	chdir(__g_pOperateSDCurrPath);
	OpenFileAndFolder(__g_pOperateSDCurrPath);
	RefreshRightMenu(1);
}

/***
  * 功能：
		创建用户自定义路径窗体
  * 参数：
		1、FILEOPERATION enFileOperation:	文件操作类型
		2、const char *pTopPath			:	允许用户返回的顶层目录
  * 返回：
		无
  * 备注：
***/
int FileDialog(unsigned int enFileOperation, const char *pTopPath, char *pCurrPath, CALLLBACKWINDOW func, FILETYPE fileType)
{
	int iErr = 0;
	GUIWINDOW *pWnd = NULL;

	if (iErr == 0)
	{
		if ((NULL == pTopPath) ||
			(NULL == pCurrPath) ||
			(NULL == func))
		{
			iErr = -1;
		}
	}

	LOG(LOG_INFO, "current path is %s\n", pCurrPath);

	if (iErr == 0)
	{
		iErr = GetFirstPath(pCurrPath, __g_pOperateFirstPath);
		if (iErr == -1)
		{
			DialogInit(120, 90, TransString("Warning"),
				GetCurrLanguageText(FILE_LBL_NO_DISK),
				0, 0, func, NULL, NULL);
		}
	}

	if (iErr >= 0)
	{
		__g_pOperateSorTopPath = pTopPath;
		__g_pOperateSDCurrPath = pCurrPath;
		_g_enFileOperation = enFileOperation;
		_g_OperateSorcallBackFunc = func;
		__g_fileType = fileType;
		//清空选择项
		ClearQueue(pEnvQueue);
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmOperateSorInit, FrmOperateSorExit,
			FrmOperateSorPaint, FrmOperateSorLoop,
			FrmOperateSorPause, FrmOperateSorResume,
			NULL);
		SendWndMsg_WindowExit(GetCurrWindow());
		SendSysMsg_ThreadCreate(pWnd);
	}

	return iErr;
}


/***
  * 功能：
		打开SOR文件
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int OpenSor(void)
{
	int iErr = 0;

	char cFileName[512] = { 0 };

	GetSelectFileName(cFileName);
	iErr = SorViewer(cFileName, pFrmOperateSor, NULL);

	return iErr;
}
/***
  * 功能：
		打开Sola文件
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int OpenSola()
{
	int iErr = 0;
	char cFileName[512] = { 0 };
	char buff[512];
	getcwd(buff, 512);

	GetSelectFileName(cFileName);
	if (strlen(cFileName) == 0)
		return -1;
	ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmOperateSor);
	iErr = SolaViewer(cFileName, pFrmOperateSor, NULL);
	if (iErr == SolaOk)
	{
		LOG(LOG_INFO, "Load SolaFile ok: %s\n", cFileName);
	}
	else if (iErr == SolaIsDirectory)
	{
		strcpy(__g_pOperateFirstPath, buff);
		LOG(LOG_INFO, "OpenSola current path is %s\n", __g_pOperateFirstPath);
		DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
			0, 0, ReCreateWindow, NULL, NULL);
	}
	else if (iErr == SolaBadVersion)
	{
		//版本不匹配
		strcpy(__g_pOperateFirstPath, buff);
		DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
			0, 0, ReCreateWindow, NULL, NULL);
	}
	else if (iErr == SolaUnknownError)
	{
		//未知错误
	}
	return iErr;
}
/***
  * 功能：
		打开gdm文件
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int OpenFip()
{
	//错误标志、返回值定义
	int iReturn = 0;
	char pFileName[512] = { 0 };
	char buff[512];
	getcwd(buff, 512);

	GetSelectFileName(pFileName);
	if (strlen(pFileName) == 0)
		return -1;
	LOG(LOG_INFO, "OpenFip pFileName %s\n", pFileName);
	ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmOperateSor);
	iReturn = FiberViewer(pFileName, pFrmOperateSor, NULL);
	if (iReturn)
	{
		strcpy(__g_pOperateFirstPath, buff);
		DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
			0, 0, ReCreateWindow, NULL, NULL);
	}
	return iReturn;
}
/***
  * 功能：
		保存SOR文件
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int SaveSor(void)
{
	//错误标志、返回值定义
	int iReturn = 0;
	char buff[512];
	PTASK_CONTEXTS pTask_Context = NULL;

	pTask_Context = pOtdrTopSettings->pTask_Context;
	getcwd(buff, sizeof(buff));

	//strcat(buff, "/");
	//strcat(buff, cQuickSaveName);
	CODER_LOG(CoderGu, "current name %s\n", buff);

	if (pTask_Context->iTaskNum == 0)
	{
		DialogInit(120, 90, TransString("DIALOG_ERROR"),
			TransString("DIALOG_NO_CURVE"),
			0, 0, ReCreateWindow, NULL, NULL);
		return iReturn;
	}
	else
	{
		struct stat st;
		if (stat(buff, &st) == 0) //find a exist name
		{
			DialogInit(120, 90, TransString("DIALOG_NOTE"),
				GetCurrLanguageText(FILE_LBL_FILE_ALREADY_EXIST),
				0, 1, ReCreateWindow, NULL, NULL);
		}
		else
		{
			if (strcmp(buff, "") != 0)
			{
				if (strstr(buff, ".SOR") == NULL)
				{
					strcat(buff, ".SOR");
				}
				CODER_LOG(CoderGu, "current cQuickSaveName %s\n", buff);
				int iRet = WriteSor(0, buff);
				LOG(LOG_INFO, "----iRet = %d---\n", iRet);

				if (iRet == 0)
				{
					DialogInit(120, 90, TransString("DIALOG_NOTE"),
						GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
						0, 0, ReCreateWindow, NULL, NULL);
				}
				else if (iReturn == -2)
				{
					DialogInit(120, 90, TransString("Warning"),
						GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
						0, 0, ReCreateWindow, NULL, NULL);
				}
				else
				{
					DialogInit(120, 90, TransString("Warning"),
						GetCurrLanguageText(DIALOG_SOR_SYSTEM_ERR),
						0, 0, ReCreateWindow, NULL, NULL);
				}
			}
		}
	}

	return iReturn;
}

static int iLanguageConfigSerialNoFlag = 0;         //加载语言配置文件时，是否需要检测序列号

void setLanguageConfigSerialNoFlag(int flag)
{
	iLanguageConfigSerialNoFlag = flag;
}

//设置初始操作路径
int setOperateFirstPath(const char *pPath)
{
	int iErr = 0;

	if (iErr == 0)
	{
		if (NULL == pPath)
		{
			iErr = -1;
		}
	}

	if (iErr == 0)
	{
		strcpy(__g_pOperateFirstPath, pPath);
	}

	return iErr;
}

int getLanguageConfigSerialNoFlag(void)
{
	return iLanguageConfigSerialNoFlag;
}

/***
  * 功能：
		打开工厂配置文件
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int OpenFactoryFirmware()
{
	//错误标志、返回值定义
	int iReturn = 0;
	char pFileName[512] = { 0 };
	char buff[512];
	getcwd(buff, 512);

	GetSelectFileName(pFileName);
	if (strlen(pFileName) == 0)
		return -1;

	if (getLanguageConfigSerialNoFlag())
	{
		iReturn = LanguageConfigViewer(pFileName, pFrmOperateSor, NULL);
	}
	else
	{
		iReturn = FactoryFirmwareViewer(pFileName, pFrmOperateSor, NULL);
	}

	switch (iReturn)
	{
	case OtdrConfigOk:
		break;
	case OtdrConfigBadChecksum:
		DialogInit(120, 90, TransString("Warning"),
			TransString("FACTORY_IMPORT_PROFILE_BADCHECKESUM"),
			0, 0, ReCreateWindow, NULL, NULL);
		break;
	case OtdrConfigBadFormat:
		DialogInit(120, 90, TransString("Warning"),
			TransString("FACTORY_IMPORT_PROFILE_FORMAT_ERROR"),
			0, 0, ReCreateWindow, NULL, NULL);
		break;
	case OtdrConfigAccessFailed:
		DialogInit(120, 90, TransString("Warning"),
			TransString("FACTORY_IMPORT_PROFILE_OPENFAIL"),
			0, 0, ReCreateWindow, NULL, NULL);
		break;
	case OtdrConfigSerialNoError:
		DialogInit(120, 90, TransString("Warning"),
			TransString("SYSTEM_SET_LANGUAGE_CONFIG_SERIALNO_ERROR"),
			0, 0, ReCreateWindow, NULL, NULL);
		break;
	default:
		DialogInit(120, 90, TransString("Warning"),
			TransString("FACTORY_IMPORT_PROFILE_UNKNOWN"),
			0, 0, ReCreateWindow, NULL, NULL);
		break;
	}

	return iReturn;
}