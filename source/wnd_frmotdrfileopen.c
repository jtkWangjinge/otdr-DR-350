/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrfileopen.c
* 摘    要：  实现主窗体frmotdrfileopen的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/26
*
*******************************************************************************/

#include "wnd_frmotdrfileopen.h"

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

#include "app_sorviewer.h"
#include "app_global.h"
#include "app_frmotdr.h"
#include "app_sorviewer.h"
#include "app_inno.h"

#include "wnd_frmdialog.h"
#include "wnd_frmime.h"

#include "guimyglobal.h"
#include "guiphoto.h"

#include "wnd_global.h"
#include "wnd_frmsavefile.h"
#include "wnd_frmotdrsave.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmmain.h"
#include "wnd_frmfilebrowse1.h"
#include "wnd_frmfilebrowse.h"
#include "wnd_frmotdr.h"
#include "wnd_frmoperatesor.h"


//全局变量声明
extern POTDR_TOP_SETTINGS pOtdrTopSettings;
static const char *__g_pOperateSorTopPath = NULL;			// 顶层目录路径
static char* __g_pOperateSDCurrPath = NULL;			        // SD卡当前所在路径
static char __g_pOperateFirstPath[512];                     // 进入的路径
static FILETYPE __g_fileType = SOR;
static unsigned int _g_enFileOperation = FILE_OPEN_MODE;	// 文件操作类型
extern MULTISELECT *pEnvQueue;
extern CURR_WINDOW_TYPE enCurWindow;						//当前的窗口
/**************************************************************
* 		定义FrmOtdrFileOpen中内部使用的数据结构
**************************************************************/

/***************************************************************
* 			窗体FrmOtdrFileOpen中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmOtdrFileOpen = NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pOtdrFileOpenBG = NULL;

/*********************右侧控件定义************************/
static WNDMENU1 *pOtdrFileOpenMenu = NULL;
//各项非选中背景资源
static char *pOtdrFileOpenUnpressBmp[2] =
{
    BmpFileDirectory "bg_otdrfileopen_open.bmp",
    BmpFileDirectory "bg_otdrfileopen_exit.bmp"
};

//各项选中背景资源
static char *pOtdrFileOpenSelectBmp[2] =
{
    BmpFileDirectory "bg_otdrfileopen_openSelect.bmp",
    BmpFileDirectory "bg_otdrfileopen_exitSelect.bmp"
};
/**********************桌面控件定义***********************/
//桌面控件

static GUIPICTURE *pOtdrFileOpenBtnTop = NULL;

static GUIPICTURE *pOtdrFileOpenBgIcon = NULL;
static GUIPICTURE *pOtdrFileOpenBtnBack = NULL;
static GUIPICTURE *pOtdrFileOpenBtnTopOpen = NULL;
static GUIPICTURE *pOtdrFileOpenBtnTopSave = NULL;

//标题
static GUILABEL *pOtdrFileOpenLblTitle = NULL;
static GUILABEL *pOtdrFileOpenLblTopOpen = NULL;
static GUILABEL *pOtdrFileOpenLblTopSave = NULL;
//桌面文本
static GUICHAR *pOtdrFileOpenStrTitle = NULL;
static GUICHAR *pOtdrFileOpenStrTopOpen = NULL;
static GUICHAR *pOtdrFileOpenStrTopSave = NULL;

/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int OtdrFileOpenTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int OtdrFileOpenTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**************************************************************
* 				控件函数声明
**************************************************************/
//右侧确认按钮
// static int	OtdrFileOpenBtnConfirm_Fun(void);
//右侧取消按钮
static int  OtdrFileOpenBtnCancel_Fun(void);
//返回按钮
static int  OtdrFileOpenBtnBack_Down(void *pInArg, int iInLen,
								void *pOutArg, int iOutLen);
static int  OtdrFileOpenBtnBack_Up(void *pInArg, int iInLen,
								void *pOutArg, int iOutLen);

//顶部保存按钮声明
static int  OtdrFileOpenBtnTopSave_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  OtdrFileOpenBtnTopSave_Up(void *pInArg, int iInLen,
								void *pOutArg, int iOutLen);

/**************************************************************
* 				功能函数声明
**************************************************************/
//右侧menu按键回调函数
static void OtdrFileOpenMenuCallBack(int iOption);
//刷新右侧菜单栏
// static void RefreshRightMenu(int flag);
//使能右侧菜单栏
// static void SetRightMenuEnable(int num);
//退出当前界面
static void ExitCurrentWnd(void);
//打开上次访问的路径(回到sd卡路径)
static void OpenLastPath(void);
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
static void FileOpenReCreateWindow(GUIWINDOW **pWnd)
{	
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmOtdrFileOpenInit, FrmOtdrFileOpenExit, 
                    FrmOtdrFileOpenPaint, FrmOtdrFileOpenLoop, 
			        FrmOtdrFileOpenPause, FrmOtdrFileOpenResume,
                    NULL);
}




/***
  * 功能：
        		Confirm 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
//static int	OtdrFileOpenBtnConfirm_Fun(void)
//{
//	//错误标志、返回值定义
//	int iReturn = 0;
//	char pFileName[512] = {0};
//    char buff[512];
//    getcwd(buff, 512);
//
//	//strcpy(pFileName,pTopPath_Otdr);
//	//strcat(pFileName,"/");
//	//strcat(pFileName,pFileName_Otdr[iSelectItem_Otdr]);
//	GetSelectFileName(pFileName);
//	
//	if(strlen(pFileName) < 6)
//	{
//		printf("----filename---null---\n");
//		return 0;
//	}
//	//iReturn = ReadSor(0,pFileName);
//    //ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmOtdrFileOpen);
//	iReturn = SorViewer(pFileName, pFrmOtdrFileOpen, NULL);
//	strcpy(__g_pOperateFirstPath, buff);
//	printf("read sor file ret : %d\n",iReturn);
//	if(iReturn == 0)
//	{
//	}
//	else if(iReturn == -1)
//	{
//		DialogInit(120, 90, TransString("Warning"),
//			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
//			0, 0, FileOpenReCreateWindow, NULL, NULL);
//	}
//	else if(iReturn == -3 || iReturn == InnoBadFormat)
//	{
//		DialogInit(120, 90, TransString("Warning"),
//			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
//			0, 0, FileOpenReCreateWindow, NULL, NULL);
//	}
//	else if(iReturn == -11)
//	{
//		DialogInit(120, 90, TransString("Warning"),
//			GetCurrLanguageText(DIALOG_RDSOR_FILENAME_ERR),
//			0, 0, FileOpenReCreateWindow, NULL, NULL);
//	}
//	else
//	{
//		DialogInit(120, 90, TransString("Warning"),
//			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
//			0, 0, FileOpenReCreateWindow, NULL, NULL);
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
static int  OtdrFileOpenBtnCancel_Fun(void)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmOtdrInit, FrmOtdrExit, 
		FrmOtdrPaint, FrmOtdrLoop,
		FrmOtdrPause, FrmOtdrResume,
		NULL);           
	SendWndMsg_WindowExit(pFrmOtdrFileOpen);  
	SendSysMsg_ThreadCreate(pWnd); 
	return iReturn;
}

//返回按钮响应函数
int OtdrFileOpenBtnBack_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	return 0;
}

int OtdrFileOpenBtnBack_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	//BackCall();
	char cPathBuff[MAXPATHNAME] = { 0 };
	/* 判断当前是否在顶层目录下 */
	getcwd(cPathBuff, MAXPATHNAME);
	strcpy(__g_pOperateSDCurrPath, cPathBuff);
	ExitCurrentWnd();

	return 0;
}



/***
  * 功能：
        		OpenFile 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  OtdrFileOpenBtnTopSave_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	CreateSaveFileDialog(FileOpenReCreateWindow);

	return iReturn;
}


/***
  * 功能：
        		OpenFile 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  OtdrFileOpenBtnTopSave_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	

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
int FrmOtdrFileOpenInit(void *pWndObj)
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
    unsigned int StrOtdrFileOpenRightMenu1[]={
		FIP_LBL_OPEN,
		OTDR_LBL_EXIT,
	};
    //打开路径设置为默认保存路径
	//strcpy(cLastFileName_Otdr, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
	//得到当前窗体对象
	pFrmOtdrFileOpen = (GUIWINDOW *) pWndObj;
    SetDisplayFileTypeAndSelectAll(0);
	__SetCurFileType(__g_fileType);
	FrmFileBrowseInit1(0, 82, MAXPAGEFILENUM-1, FileOpenReCreateWindow, pWndObj);
	SetFileSelectMode(FILE_SINGLE_MODE);
	SetFileOperationType(_g_enFileOperation);
	SetWidgetUpdateCallBack(NULL);	
	OtdrFileOpenTextRes_Init(NULL, 0, NULL, 0);

    pOtdrFileOpenBG = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_openFile.bmp");

    pOtdrFileOpenBtnTop = CreatePhoto("bg_filebrowse_title");
	pOtdrFileOpenBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_openFile_icon.bmp");
	pOtdrFileOpenLblTitle = CreateLabel(36, 12, 100, 16, pOtdrFileOpenStrTitle);

	pOtdrFileOpenBtnBack = CreatePicture(0, 20, 35, 21, BmpFileDirectory"btn_file_back.bmp");
    pOtdrFileOpenBtnTopOpen = CreatePicture( 3, 47, 255, 35, BmpFileDirectory"btn_file_press.bmp");
    pOtdrFileOpenBtnTopSave = CreatePicture(3 + 259, 47, 255, 35, BmpFileDirectory"btn_file_unpress.bmp");
	
	pOtdrFileOpenLblTopOpen = CreateLabel(104, 56, 100, 24, pOtdrFileOpenStrTopOpen);
	pOtdrFileOpenLblTopSave = CreateLabel(363, 56, 100, 24, pOtdrFileOpenStrTopSave);
	// SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrFileOpenLblTopOpen);
	// SetLabelAlign(GUILABEL_ALIGN_CENTER,pOtdrFileOpenLblTopSave);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pOtdrFileOpenLblTopSave);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrFileOpenLblTopOpen);

	pOtdrFileOpenMenu = CreateWndMenu1(2, sizeof(StrOtdrFileOpenRightMenu1),
							StrOtdrFileOpenRightMenu1, 0xffff, 0, 1, 41, OtdrFileOpenMenuCallBack);
	//设置菜单栏背景
	int i;
	for (i = 0; i < 2; i++)
	{
		SetWndMenuItemBg(i, pOtdrFileOpenUnpressBmp[i], pOtdrFileOpenMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pOtdrFileOpenSelectBmp[i], pOtdrFileOpenMenu, MENU_SELECT);
	}

	SetFirstOpenFolder(__g_pOperateFirstPath);
	
	//右侧按键区标签
	//注册窗体控件,只对那些需要接收事件的控件进行
	//即如果该控件没有或者不需要响应输入事件，可
	//以无需注册
	//注册窗体控件必须在注册消息处理函数之前进行
	//*********************************************************/
	//注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmOtdrFileOpen, pFrmOtdrFileOpen);
	//注册桌面上的控件

	//桌面顶部控件注册
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUIPICTURE), pOtdrFileOpenBtnTopSave, pFrmOtdrFileOpen);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUIPICTURE), pOtdrFileOpenBtnBack, pFrmOtdrFileOpen);

	//右边按钮栏注册
	AddWndMenuToComp1(pOtdrFileOpenMenu, pFrmOtdrFileOpen);
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();
	//桌面按钮注册
	//注册桌面上控件的消息处理
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrFileOpenBtnBack,
					OtdrFileOpenBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrFileOpenBtnBack,
					OtdrFileOpenBtnBack_Up, NULL, 0, pMsg);

	//桌面顶部按钮注册
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrFileOpenBtnTopSave, 
	               OtdrFileOpenBtnTopSave_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrFileOpenBtnTopSave, 
	               OtdrFileOpenBtnTopSave_Down, NULL, 0, pMsg);
	//注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pOtdrFileOpenMenu, pFrmOtdrFileOpen);
    
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
int FrmOtdrFileOpenExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pFrmOtdrFileOpen = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmOtdrFileOpen);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pOtdrFileOpenBG);
	DestroyPicture(&pOtdrFileOpenBgIcon);
	DestroyPicture(&pOtdrFileOpenBtnBack);

	DestroyPicture(&pOtdrFileOpenBtnTop);
	
	DestroyLabel(&pOtdrFileOpenLblTitle);
	DestroyLabel(&pOtdrFileOpenLblTopOpen);
	DestroyLabel(&pOtdrFileOpenLblTopSave);

	DestroyPicture(&pOtdrFileOpenBtnTopOpen);
	DestroyPicture(&pOtdrFileOpenBtnTopSave);
	//按钮标签销毁
	DestroyWndMenu1(&pOtdrFileOpenMenu);
	
	FrmFileBrowseExit1(pWndObj);	
	
	//文本内容销毁
	OtdrFileOpenTextRes_Exit(NULL, 0, NULL, 0);
	//记录当前的窗口
	enCurWindow = ENUM_OTHER_WIN;

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
int FrmOtdrFileOpenPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	//得到当前窗体对象
	pFrmOtdrFileOpen = (GUIWINDOW *) pWndObj;

	//显示状态栏、桌面、信息栏控件
	DisplayPicture(pOtdrFileOpenBG);
	//按钮标签显示
	DisplayWndMenu1(pOtdrFileOpenMenu);
	//刷新菜单栏
	int i;
	for (i = 0; i < 2; ++i)
	{
		SetWndMenuItemEnble(i, 1, pOtdrFileOpenMenu);
	}
	//顶部控件显示
// 	DisplayPicture(pOtdrFileOpenBtnTop);
	
	// DisplayPicture(pOtdrFileOpenBgIcon);
	// DisplayPicture(pOtdrFileOpenBtnBack);
	DisplayPicture(pOtdrFileOpenBtnTopOpen);
	DisplayPicture(pOtdrFileOpenBtnTopSave);

	DisplayLabel(pOtdrFileOpenLblTitle);
	DisplayLabel(pOtdrFileOpenLblTopOpen);
	DisplayLabel(pOtdrFileOpenLblTopSave);

	//显示文件界面
	SetCurrentFileTypeLabel("SOR");
	FrmFileBrowsePaint1(pWndObj);
	SetFileTypeEnabled(0);
	
	enCurWindow = ENUM_OPEN_OTDR_WIN;	//设置当前窗体

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
int FrmOtdrFileOpenLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	static int currPathIsUSBorSDCARD = 0;   //1 SDCARD 2 USB
    
	char currPath[512];
	memset(currPath, 0, 512);
	getcwd(currPath, 512);
	
	if(access(MntUsbDirectory, F_OK) != 0)
	{
        if(currPathIsUSBorSDCARD == 2)
        {
            OpenLastPath();
        }
	}
	
	if(access(MntSDcardDataDirectory, F_OK) != 0)
	{
	    
        if(currPathIsUSBorSDCARD == 1)
        {
            currPathIsUSBorSDCARD = 0;
            ExitCurrentWnd();
            SendWndMsg_LoopDisable(pFrmOtdrFileOpen);
        }
	}
	
	if(!strncmp(currPath, MntUsbDirectory, strlen(MntUsbDirectory)))
	{	
        currPathIsUSBorSDCARD = 2;
	}	
	else if(!strncmp(currPath, MntSDcardDataDirectory, strlen(MntSDcardDataDirectory)))
	{
        currPathIsUSBorSDCARD = 1;
	}

	MsecSleep(10);
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
int FrmOtdrFileOpenPause(void *pWndObj)
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
int FrmOtdrFileOpenResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
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
static int OtdrFileOpenTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pOtdrFileOpenStrTitle = GetCurrLanguageText(OTDR_LBL_OPEN_FILE);
	pOtdrFileOpenStrTopOpen = GetCurrLanguageText(OTDR_LBL_OPEN_FILE);
	pOtdrFileOpenStrTopSave = GetCurrLanguageText(OTDR_LBL_SAVE_FILE);

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
static int OtdrFileOpenTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//释放状态栏上的文本
	//...
	//释放按钮区的文本
	//...
	GuiMemFree(pOtdrFileOpenStrTitle);
	GuiMemFree(pOtdrFileOpenStrTopOpen); 
	GuiMemFree(pOtdrFileOpenStrTopSave); 
	
	//...
	//...

	return iReturn;
}

//打开上次访问的路径(回到sd卡路径)
static void OpenLastPath(void)
{
    if(access(__g_pOperateSDCurrPath, F_OK) != 0)
    {
        if(access(MntSDcardDataDirectory, F_OK) != 0)
        {
            ExitCurrentWnd();
            return;
        }
        strcpy(__g_pOperateSDCurrPath, MntSDcardDataDirectory);
    }
    chdir(__g_pOperateSDCurrPath);
    OpenFileAndFolder(__g_pOperateSDCurrPath);
//     RefreshRightMenu(1);
}

//static void OpenFileOrOpenFromUsb(void)
//{
//    char cPathBuff[MAXPATHNAME] = {};
//    /* 判断当前是否在U盘目录下 */
//	getcwd(cPathBuff, MAXPATHNAME);
//	if( strncmp(cPathBuff, MntUsbDirectory, strlen(MntUsbDirectory)) == 0)//打开
//    {
//        OtdrFileOpenBtnConfirm_Fun();                
//    }
//    else//从U盘打开
//    {
//        getcwd(__g_pOperateSDCurrPath, MAXPATHNAME);
//        if(access(MntUsbDirectory, F_OK) == 0)
//        {
//            OpenFileAndFolder(MntUsbDirectory);
////             RefreshRightMenu(0);
//        }
//		else
//        {            
//            getcwd(__g_pOperateFirstPath, MAXPATHNAME);
//            DialogInit(120, 90, TransString("Warning"),
//				TransString("DIALOG_NO_USB"),
//				0, 0, FileOpenReCreateWindow, NULL, NULL);
//        }      
//    }
//}

static void BackCall(void)
{
    char cPathBuff[MAXPATHNAME] = {0};
    /* 判断当前是否在顶层目录下 */
	getcwd(cPathBuff, MAXPATHNAME);
	if(strcmp(cPathBuff, MntUpDirectory) == 0)
	{	
	    strcpy(__g_pOperateSDCurrPath, cPathBuff);
		ExitCurrentWnd();
		return;
	}		
    if(strcmp(cPathBuff, MntUsbDirectory) == 0)
    {
        OpenLastPath();
    }
    else
    {
        OpenFileAndFolder("..");
	    RefreshScreen(__FILE__, __func__, __LINE__);
    }
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
static void OtdrFileOpenMenuCallBack(int iOption)
{
	switch (iOption)
	{
	case 0:
// 	    OpenFileOrExit();
		OpenAtIndex(GetCurrFileIndex());
	 	break;
	case 1://取消
	    getcwd(__g_pOperateSDCurrPath, MAXPATHNAME);
		OtdrFileOpenBtnCancel_Fun();
	 	break;
	case BACK_DOWN:
	case HOME_DOWN:
		BackCall();
	 	break;
	default:
		break;
	}
}
//退出当前界面
static void ExitCurrentWnd(void)
{
    GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                    FrmOtdrInit, FrmOtdrExit, 
	                    FrmOtdrPaint, FrmOtdrLoop, 
	            	 	FrmOtdrPause, FrmOtdrResume,
	                    NULL);           
	SendWndMsg_WindowExit(pFrmOtdrFileOpen);  
	SendSysMsg_ThreadCreate(pWnd);
}
//刷新右侧菜单栏
//static void RefreshRightMenu(int flag)
//{
//    if(flag)
//    {
//        unsigned int strOpenMenu[] = {
//            OTDR_OPEN_FROM_USB,
//	        OTDR_SAVE_LBL_OPEN,
//	        OTDR_QUICK_SAVE_QUIT,
//        };
//        RefreshMenu1(pOtdrFileOpenMenu, -1, 6, (UINT16)(MENU_BACK | ~MENU_HOME), 
//				sizeof(strOpenMenu), strOpenMenu, 1);
//        SetRightMenuEnable(3);
//    }
//    else
//    {
//        unsigned int strOpenMenu[] = {
//	        OTDR_SAVE_LBL_OPEN,
//	        OTDR_QUICK_SAVE_QUIT,
//        };
//        
//        RefreshMenu1(pOtdrFileOpenMenu, -1, 6, (UINT16)(MENU_BACK | ~MENU_HOME), 
//				sizeof(strOpenMenu), strOpenMenu, 1);
//        SetRightMenuEnable(2);
//    }
//    RefreshScreen(__FILE__, __func__, __LINE__);
//}
//使能右侧菜单栏
//static void SetRightMenuEnable(int num)
//{
//    int i = 0;
//    for(i = 0; i < 6; ++i)
//    {
//        if(i < num)
//        {
//            SetWndMenuItemEnble(i, 1, pOtdrFileOpenMenu);
//        }
//        else
//        {
//            SetWndMenuItemEnble(i, 0, pOtdrFileOpenMenu);
//        }
//    }
//}

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
int OtdrFileDialog(unsigned int enFileOperation, const char *pTopPath, char *pCurrPath, CALLLBACKWINDOW func, FILETYPE fileType)
{
	int iErr = 0; 
	GUIWINDOW *pWnd = NULL; 

	if(iErr == 0)
	{
		if( (NULL == pTopPath) ||
			(NULL == pCurrPath))
		{
			iErr = -1;
		}
	}

	LOG(LOG_INFO, "current path is %s\n", pCurrPath);

	if(iErr == 0)
    {
        iErr = GetFirstPath(pCurrPath, __g_pOperateFirstPath);
        if(iErr == -1)
        {
            DialogInit(120, 90, TransString("Warning"),
					GetCurrLanguageText(FILE_LBL_NO_DISK),
					0, 0, func, NULL, NULL);
        }
    }
    
	if(iErr >= 0)
	{
		__g_pOperateSorTopPath = pTopPath;
		__g_pOperateSDCurrPath = pCurrPath;
		_g_enFileOperation = enFileOperation;
        __g_fileType = fileType;
        //清空选择项
        ClearQueue(pEnvQueue);
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
			FrmOtdrFileOpenInit, FrmOtdrFileOpenExit, 
			FrmOtdrFileOpenPaint, FrmOtdrFileOpenLoop, 
			FrmOtdrFileOpenPause, FrmOtdrFileOpenResume,
			NULL);           
		SendWndMsg_WindowExit(GetCurrWindow());  
		SendSysMsg_ThreadCreate(pWnd);	
	}	

	return iErr; 

}

int setOtdrOperateFirstPath(const char *pPath)
{
    int iErr = 0;

	if(iErr == 0)
	{
		if(NULL == pPath)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		strcpy(__g_pOperateFirstPath, pPath);
	}

    return iErr;
}