/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfipfileopen.c
* 摘    要：  实现主窗体frmfipfileopen的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/26
*
*******************************************************************************/

#include "wnd_frmfipfileopen.h"

/**************************************************************
* 	为实现窗体FrmOtdrFileOpen而需要引用的其他头文件
**************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>


#include "app_global.h"
#include "app_frmfip.h"

#include "guimyglobal.h"
#include "guiphoto.h"

#include "wnd_frmfip.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmmain.h"
#include "wnd_frmfilebrowse1.h"
#include "wnd_frmfilebrowse.h"
#include "wnd_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmime.h"

extern OpticalFiberEndSur OptFiberEndSurCheck;
extern int iCheckErrFlag;
extern FIPCALLLBACK FipCallBackFunc;
extern CURR_WINDOW_TYPE enCurWindow;

/**************************************************************
* 		定义FrmOtdrFileOpen中内部使用的数据结构
**************************************************************/
static char cLastFileName_Fip[512]=filefibergdmpath;	 //上一次所在的路径名
/***************************************************************
* 			窗体FrmOtdrFileOpen中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmFipFileOpen = NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pFipFileOpenBG = NULL;
static GUIPICTURE *pFipFileOpenBgIcon = NULL;

/*********************右侧控件定义************************/
static WNDMENU1 *pFipFileOpenMenu = NULL;
//各项非选中背景资源
static char *pFipFileOpenUnpressBmp[2] =
{
    BmpFileDirectory "bg_fipfileopen_open.bmp",
    BmpFileDirectory "bg_fipfileopen_exit.bmp"
};

//各项选中背景资源
static char *pFipFileOpenSelectBmp[2] =
{
    BmpFileDirectory "bg_fipfileopen_openSelect.bmp",
    BmpFileDirectory "bg_fipfileopen_exitSelect.bmp"
};
/**********************桌面控件定义***********************/
//桌面控件

static GUIPICTURE *pFipFileOpenBtnTop = NULL;
static GUIPICTURE *pFipFileOpenBtnBack = NULL;
//static GUIPICTURE *pFipFileOpenBtnTopOpen = NULL;

static GUILABEL *pFipFileOpenLblTopOpen = NULL;
//桌面文本
static GUICHAR *pFipFileOpenStrTopOpen = NULL;

//返回按钮响应
static int FipFileBtnBack_Down(void *pInArg, int iInLen,
					void *pOutArg, int iOutLen);
static int FipFileBtnBack_Up(void *pInArg, int iInLen,
					void *pOutArg, int iOutLen);


/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int FipFileOpenTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int FipFileOpenTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**************************************************************
* 				控件函数声明
**************************************************************/
//右侧确认按钮
// static int	FipFileOpenBtnConfirm_Fun(void);
//右侧取消按钮
static int  FipFileOpenBtnCancel_Fun(void);
/**************************************************************
* 				功能函数声明
**************************************************************/
//右侧menu按键回调函数
static void FipFileOpenMenuCallBack(int iOption);
/***
  * 功能：
		用于对话框重新还原窗体时的回调函数
  * 参数：
		1.GUIWINDOW **pWnd	:	指向窗体的指针
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static void FileOpenReCreateWindow(GUIWINDOW **pWnd);
/**************************************************************
* 				控件函数定义
**************************************************************/

/***
  * 功能：
		用于对话框重新还原窗体时的回调函数
  * 参数：
		1.GUIWINDOW **pWnd	:	指向窗体的指针
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
//static void FileOpenReCreateWindow1(GUIWINDOW **pWnd)
//{	
//    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
//                    FrmFipInit, FrmFipExit, 
//                    FrmFipPaint, FrmFipLoop, 
//			        FrmFipPause, FrmFipResume,
//                    NULL);
//}


/***
  * 功能：
        		Confirm 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
//static int	FipFileOpenBtnConfirm_Fun(void)
//{
//	//错误标志、返回值定义
//	int iReturn = 0;
//	char pFileName[512] = {0};
//	char cPathBuff[MAXPATHNAME];
//	
//	GetSelectFileName(pFileName);
//	sprintf(cPathBuff,"%s/%s",cLastFileName_Fip,pFileName);
//	printf("%s\n",cPathBuff);
//	if(ReadGdmFile(cPathBuff,&OptFiberEndSurCheck.ResultInfo))
//	{
//		printf("read dgm file failed!!!\n");
//		iReturn = -1;
//	}
//	else
//	{
//		OptFiberEndSurCheck.isScreen = 1;
//		strcpy(OptFiberEndSurCheck.cName,pFileName);
//	}
//	
//	return iReturn;
//}


/***
  * 功能：
        		Cancel 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  FipFileOpenBtnCancel_Fun(void)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmFipInit, FrmFipExit, 
		FrmFipPaint, FrmFipLoop, 
		FrmFipPause, FrmFipResume,
		NULL);           
	SendWndMsg_WindowExit(pFrmFipFileOpen);  
	SendSysMsg_ThreadCreate(pWnd); 
	return iReturn;
}


/***
  * 功能：
        		窗体FrmOtdrFileOpen 的初始化函数，建立窗体控件、
        		注册消息处理
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmFipFileOpenInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	//初始化文本资源
	//如果GUI存在多国语言，在此处获得对应语言的文
	//本资源
	//初始化文本资源必须在建立文本CreateText()或建立标
	//签CreateLabel()之前
	/**********************************************************/
	unsigned int StrOtdrFileOpenRightMenu[]={
		//OTDR_OPEN_FROM_USB,
		FIP_LBL_OPEN,
		OTDR_LBL_EXIT,
	};

	
	//得到当前窗体对象
	pFrmFipFileOpen = (GUIWINDOW *) pWndObj;
    SetDisplayFileTypeAndSelectAll(0);
	__SetCurFileType(BMP);
	FrmFileBrowseInit1(0, 40, MAXPAGEFILENUM, FileOpenReCreateWindow, pWndObj);
	SetFileSelectMode(FILE_SINGLE_MODE);
	SetFirstOpenFolder(cLastFileName_Fip);
	SetFileOperationType(FILE_OPEN_MODE);
	SetWidgetUpdateCallBack(NULL);
	FipFileOpenTextRes_Init(NULL, 0, NULL, 0);
	
	//建立桌面上的控件
	pFipFileOpenBG = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_filebrowse.bmp");
	pFipFileOpenBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_openFile_icon.bmp");
	//建立顶部按钮的控件
    pFipFileOpenBtnTop = CreatePhoto("bg_filebrowse_title");
	pFipFileOpenBtnBack = CreatePicture(0, 20, 35, 21, BmpFileDirectory"btn_file_back.bmp");
	pFipFileOpenLblTopOpen = CreateLabel(36, 12, 100, 16, pFipFileOpenStrTopOpen);
    //设置当前界面
    enCurWindow = ENUM_FAULT_WIN;
    pFipFileOpenMenu = CreateWndMenu1(2, sizeof(StrOtdrFileOpenRightMenu),StrOtdrFileOpenRightMenu,
							0xffff, 0, 1, 41, FipFileOpenMenuCallBack);
	//设置菜单栏背景
	int i;
	for (i = 0; i < 2; i++)
	{
		SetWndMenuItemBg(i, pFipFileOpenUnpressBmp[i], pFipFileOpenMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pFipFileOpenSelectBmp[i], pFipFileOpenMenu, MENU_SELECT);
	}

	//右侧按键区标签
	//注册窗体控件,只对那些需要接收事件的控件进行
	//即如果该控件没有或者不需要响应输入事件，可
	//以无需注册
	//注册窗体控件必须在注册消息处理函数之前进行
	//*********************************************************/
	//注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFipFileOpen, pFrmFipFileOpen);

	//桌面顶部控件注册
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipFileOpenBtnBack, pFrmFipFileOpen);
	//右边按钮栏注册
	AddWndMenuToComp1(pFipFileOpenMenu, pFrmFipFileOpen);
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();
	//桌面按钮注册
	    //注册桌面上控件的消息处理
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipFileOpenBtnBack,
						FipFileBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipFileOpenBtnBack,
						FipFileBtnBack_Up, NULL, 0, pMsg);

	
	//注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pFipFileOpenMenu, pFrmFipFileOpen);
    
	return iReturn;
}
/***
  * 功能：
        		窗体FrmOtdrFileOpen 的退出函数，释放所有资源
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmFipFileOpenExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pFrmFipFileOpen = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmFipFileOpen);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pFipFileOpenBG);
	DestroyPicture(&pFipFileOpenBtnTop);
	DestroyPicture(&pFipFileOpenBgIcon);
	DestroyPicture(&pFipFileOpenBtnBack);

	DestroyLabel(&pFipFileOpenLblTopOpen);

	//按钮标签销毁
	DestroyWndMenu1(&pFipFileOpenMenu);
	
	FrmFileBrowseExit1(pWndObj);	
	
	//文本内容销毁
	FipFileOpenTextRes_Exit(NULL, 0, NULL, 0);

	return iReturn;
}


/***
  * 功能：
        		窗体FrmFileOpen 的绘制函数，绘制整个窗体
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmFipFileOpenPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	//得到当前窗体对象
	pFrmFipFileOpen = (GUIWINDOW *) pWndObj;
	//显示状态栏、桌面、信息栏控件
	DisplayPicture(pFipFileOpenBG);
	// DisplayPicture(pFipFileOpenBgIcon);
	// DisplayPicture(pFipFileOpenBtnBack);
	//顶部控件显示
// 	DisplayPicture(pFipFileOpenBtnTop);

	DisplayLabel(pFipFileOpenLblTopOpen);

	//按钮标签显示
	DisplayWndMenu1(pFipFileOpenMenu);
	//刷新菜单栏
	int i;
	for (i = 0; i < 2; ++i)
	{
		SetWndMenuItemEnble(i, 1, pFipFileOpenMenu);
	}
	//显示文件界面
	SetCurrentFileTypeLabel("BMP");
	FrmFileBrowsePaint1(pWndObj);
	SetFileTypeEnabled(0);
	// enCurWindow = ENUM_FAULT_WIN; //设置当前窗体

	RefreshScreen(__FILE__, __func__, __LINE__);

		
	return iReturn;
}

/***
  * 功能：
        	窗体FrmOtdrFileOpen 的循环函数，进行窗体循环
  * 参数：
       	 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmFipFileOpenLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
		
	return iReturn;
}


/***
  * 功能：
        窗体FrmOtdrFileOpen 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFipFileOpenPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体FrmOtdrFileOpen 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmFipFileOpenResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//返回按钮响应函数
int FipFileBtnBack_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	return 0;
}

int FipFileBtnBack_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	FipFileOpenBtnCancel_Fun();

	return 0;
}

/***
  * 功能：
		     初始化文本资源
  * 参数：
		        无
  * 返回：
		        成功返回零，失败返回非零值
  * 备注：
***/
static int FipFileOpenTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	pFipFileOpenStrTopOpen = GetCurrLanguageText(OTDR_LBL_OPEN_FILE); 
	//初始化状态栏上的文本
	//...
	//初始化桌面上的文本
	//...
	//初始化桌面按钮的文本
	//...
	return iReturn;
}


/***
  * 功能：
		     释放文本资源
  * 参数：
		        无
  * 返回：
		        成功返回零，失败返回非零值
  * 备注：
***/
static int FipFileOpenTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//释放状态栏上的文本
	//...
	//释放按钮区的文本
	//...
	GuiMemFree(pFipFileOpenStrTopOpen); 
	
	//...
	//...

	return iReturn;
}


/***
  * 功能：
		      侧边菜单控件回调函数
  * 参数：
		        无
  * 返回：
		        int iOption: 点按的按钮ID
  * 备注：
***/
static void FipFileOpenMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	char cPathBuff[MAXPATHNAME];
		
	switch (iOption)
	{
	case 0://确认
		OpenAtIndex(GetCurrFileIndex());
	 	break;
	case 1://取消
		FipFileOpenBtnCancel_Fun();
	 	break;
	case 2:
	 	break;
	case BACK_DOWN:
	case HOME_DOWN:
		/* 判断当前是否在顶层目录下 */
		getcwd(cPathBuff, MAXPATHNAME);
		if ((strcmp(cLastFileName_Fip, cPathBuff) == 0) ||
			(strcmp(cPathBuff, MntUpDirectory) == 0))
		{
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
								FrmFipInit, FrmFipExit,
								FrmFipPaint, FrmFipLoop,
								FrmFipPause, FrmFipResume,
								NULL);
			SendWndMsg_WindowExit(pFrmFipFileOpen);
			SendSysMsg_ThreadCreate(pWnd);
		}
		else
		{
			OpenFileAndFolder("..");
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
		break;
	default:
		break;
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
static void FileOpenReCreateWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmFipFileOpenInit, FrmFipFileOpenExit,
		FrmFipFileOpenPaint, FrmFipFileOpenLoop,
		FrmFipFileOpenPause, FrmFipFileOpenResume,
		NULL);
}