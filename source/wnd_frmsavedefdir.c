/*******************************************************************************
* Copyright(c)2015，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmsavedefdir.c
* 摘    要：  声明窗体wnd_frmsavedefdir的窗体处理线程及相关操作函数
*			  该窗体提供了选择默认存储路径的功能。
*             
*
* 当前版本：  v1.0.1
* 作    者：  
* 完成日期：  2015.05.27
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：
*******************************************************************************/

#include "wnd_frmsavedefdir.h"


/*******************************************************************************
**					为实现窗体frmsavedefdir而需要引用的其他头文件 			  **
*******************************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmime.h"
#include "guimyglobal.h"
#include "app_frmfilebrowse.h"
#include "wnd_frmfilebrowse1.h"
#include "wnd_frmmenubak.h"
#include "wnd_stack.h"
#include "guiphoto.h"

/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
int iSaveDefDirKeyFlag = 0;								// 防止同时响应两个以上按键的标志
static WNDMENU1 *pSaveDefDirMenu = NULL;				// menu控件
#define FUNCTION_MENU_NUM							3	// 菜单项
//各项非选中背景资源
static char *pSaveDefDirUnpressBmp[FUNCTION_MENU_NUM] =
{
    BmpFileDirectory "bg_savedefdir_open.bmp",
    BmpFileDirectory "bg_savedefdir_mkdir.bmp",
	BmpFileDirectory "bg_savedefdir_confirm.bmp"
};

//各项选中背景资源
static char *pSaveDefDirSelectBmp[FUNCTION_MENU_NUM] =
{
    BmpFileDirectory "bg_savedefdir_openSelect.bmp",
    BmpFileDirectory "bg_savedefdir_mkdirSelect.bmp",
	BmpFileDirectory "bg_savedefdir_confirmSelect.bmp"
};
static char g_cLastFilePath[MAXPATHNAME] = MntUpDirectory;// 上一次所在的路径名
static const char *__g_pSaveDefDirTopPath = NULL;		// 顶层目录路径
static char *g_pUserPath = NULL;						// 指向存贮用户选择的默认的路径的指针
extern CURR_WINDOW_TYPE enCurWindow;					//当前的窗口
/*******************************************************************************
**						窗体frmsavedefdir中的控件定义部分					  **
*******************************************************************************/
static GUIWINDOW *pFrmSaveDefDir = NULL;

/**********************************背景控件定义********************************/ 
static GUIPICTURE *pSaveDefDirBg = NULL;
//static GUIPICTURE *pSaveDefDirBgTableTitle = NULL;

/*********************************标题栏控件定义*******************************/
static GUICHAR *pSaveDefDirStrTitle = NULL;      		
static GUILABEL *pSaveDefDirLblTitle = NULL;       


/*******************************************************************************
**	    窗体frmsavedefdir中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/
static int SaveDefDirTextRes_Init(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);
static int SaveDefDirTextRes_Exit(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);
/*******************************************************************************
**			    窗体frmsavedefdir中的控件事件处理函数定义部分				  **
*******************************************************************************/

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
static int FrmSaveDefDirInit(void *pWndObj);
static int FrmSaveDefDirExit(void *pWndObj);
static int FrmSaveDefDirPaint(void *pWndObj);
static int FrmSaveDefDirLoop(void *pWndObj);
static int FrmSaveDefDirPause(void *pWndObj);
static int FrmSaveDefDirResume(void *pWndObj);


/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static void ReCreateWindow(GUIWINDOW **pWnd);
static void SaveDefDirMenuCall(int iOp);


/***
  * 功能：
        窗体frmsavedefdir的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveDefDirInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	unsigned int strMenu[FUNCTION_MENU_NUM] = {
		FILE_LBL_OPEN,
		FILE_LBL_MKDIR,
		OTDR_DEFAULT_FOLDER_LBL_CONFIRM
	};	

    //得到当前窗体对象
    pFrmSaveDefDir = (GUIWINDOW *) pWndObj;

	/* 文件浏览控件初始化 */
    SetDisplayFileTypeAndSelectAll(0);
	isInSmallFileBrowse = 0;
	FrmFileBrowseInit1(0, 40, MAXPAGEFILENUM, ReCreateWindow, pWndObj);
	SetFirstOpenFolder(g_cLastFilePath);
	SetWidgetUpdateCallBack(NULL);
	SetFileOperationType(FILE_SAVE_MODE);
	SetCurFileType(NONE);

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    SaveDefDirTextRes_Init(NULL, 0, NULL, 0);

    /* 建立桌面背景 */
    pSaveDefDirBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_filebrowse.bmp");
	/*********************************标题栏控件定义*******************************/
	pSaveDefDirLblTitle = CreateLabel(36, 12, 100, 16,
									  pSaveDefDirStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSaveDefDirLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSaveDefDirLblTitle);	

	//侧边栏menu
	pSaveDefDirMenu = CreateWndMenu1(3, sizeof(strMenu), strMenu, 
										(UINT16)(MENU_BACK | ~MENU_HOME), 
										0, 1, 41, SaveDefDirMenuCall);
	//设置菜单栏背景
	int i;
	for (i = 0; i < FUNCTION_MENU_NUM; i++)
	{
		SetWndMenuItemBg(i, pSaveDefDirUnpressBmp[i], pSaveDefDirMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pSaveDefDirSelectBmp[i], pSaveDefDirMenu, MENU_SELECT);
	}

	//添加消息到控件队列
	AddWndMenuToComp1(pSaveDefDirMenu, pFrmSaveDefDir);

	//注册菜单控件的消息处理函数
	LoginWndMenuToMsg1(pSaveDefDirMenu, pFrmSaveDefDir);

    return iReturn;
}


/***
  * 功能：
        窗体frmsavedefdir的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveDefDirExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmSaveDefDir = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSaveDefDir);

	/* 销毁文件浏览器控件 */
	FrmFileBrowseExit1(pWndObj);	

	/* 销毁右侧菜单栏 */
	DestroyWndMenu1(&pSaveDefDirMenu);

	/*******************************销毁桌面背景控件*******************************/
    DestroyPicture(&pSaveDefDirBg);
    //DestroyPicture(&pSaveDefDirBgTableTitle);
	/*********************************销毁标题栏控件*******************************/
	DestroyLabel(&pSaveDefDirLblTitle);

	/* 释放文本 */
	SaveDefDirTextRes_Exit(NULL, 0, NULL, 0);
	
    return iReturn;
} 

/***
  * 功能：
        窗体frmsavedefdir的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveDefDirPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmSaveDefDir = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    //显示状态栏上的控件
	
    //显示桌面背景
	DisplayPicture(pSaveDefDirBg);
    //DisplayPicture(pSaveDefDirBgTableTitle);
	/* 显示标题栏 */
	DisplayLabel(pSaveDefDirLblTitle);

	DisplayWndMenu1(pSaveDefDirMenu);
	//刷新菜单栏
	int i;
	for (i = 0; i < FUNCTION_MENU_NUM; ++i)
	{
		SetWndMenuItemEnble(i, 1, pSaveDefDirMenu);
	}

	FrmFileBrowsePaint1(pWndObj);	
	SetFileTypeEnabled(0);
	enCurWindow = ENUM_OPEN_OTDR_WIN;//设置当前界面
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
        窗体frmsavedefdir的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveDefDirLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);
	 	
    return iReturn;
}


/***
  * 功能：
        窗体frmsavedefdir的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveDefDirPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmsavedefdir的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveDefDirResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    窗体frmsavedefdir中的初始化文本资源、 释放文本资源函数定义部分		  **
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
static int SaveDefDirTextRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	pSaveDefDirStrTitle = GetCurrLanguageText(OTDR_DEFAULT_FOLDER_LBL_TITLE);
	
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
static int SaveDefDirTextRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	GuiMemFree(pSaveDefDirStrTitle);

    return iReturn;
}


/***
  * 功能：
		创建目录的回调函数
  * 参数：
  		无
  * 返回：
		无
  * 备注：
***/
static void MakeDirOk(void)
{
	int iReturn = 0;
	int iErr = 0;
	char cTmpInputBuff[512];

	GetIMEInputBuff(cTmpInputBuff);
	LOG(LOG_INFO, "cTmpInputBuff = %s\n", cTmpInputBuff);

	/* 判断将要创建的目录名是否合法 */
	iReturn = RegularSeach(cTmpInputBuff);
	if( iReturn==0 )
	{
		/* 判断将要创建的目录是否含有同名目录 */
		if( access(cTmpInputBuff, F_OK) == 0) //含有同名文件
		{
			/* 弹出对话框提示有同名文件存在 */
			CreateIMEDialog(GetCurrLanguageText(FILE_LBL_SAME_FILE));
		}
		/* 不存在同名目录 */
		else
		{
			iErr = mkdir(cTmpInputBuff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			if(iErr != 0)
			{
				CreateIMEDialog(GetCurrLanguageText(FILE_LBL_CREATE_FOLDER_FAILED));
			}
		}
	}
	else
	{
		CreateIMEDialog(GetCurrLanguageText(FILE_LBL_INVALID_FOLDER_NAME));
	}
}


/***
  * 功能：
		创建目录按钮up事件
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int MakeDir_Up(void)
{
    //错误标志、返回值定义
    int iErr = 0;

	if(iErr == 0)
	{
		getcwd(g_cLastFilePath, MAXPATHNAME);	
		SetFirstOpenFolder(g_cLastFilePath);
	}

	if(iErr == 0)
	{
		//在/mnt目录下 */
		if (IsUpDir())
		{
			/* 未检测到SD卡和U盘 */
			if(GetCurFileNum() <= 0)
			{
				DialogInit(120, 90, TransString("Warning"),
							GetCurrLanguageText(FILE_LBL_NO_DISK),
							0, 0, ReCreateWindow, NULL, NULL);			
			}
			/* 在/mnt目录下进行了操作 */
			else
			{
				DialogInit(120, 90, TransString("Warning"),
							GetCurrLanguageText(FILE_LBL_CREATE_FOLDER_FAILED),
							0, 0, ReCreateWindow, NULL, NULL);
			}
			return -1;
		}
		IMEInit(NULL, 23, 0, ReCreateWindow, MakeDirOk, NULL);
	}	
	
    return iErr;
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
                    FrmSaveDefDirInit, FrmSaveDefDirExit, 
                    FrmSaveDefDirPaint, FrmSaveDefDirLoop, 
			        FrmSaveDefDirPause, FrmSaveDefDirResume,
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
static void SaveDefDirMenuCall(int iOp)
{
    Stack *ps = NULL;
	Item func;	
	
	switch (iOp)
	{
	/* 打开文件夹 */
	case 0:
		OpenAtIndex(GetCurrFileIndex());
		break;
	/* 新建文件夹 */
	case 1:
		MakeDir_Up();
		break;
	/* 确定按钮 */		
	case 2:
		getcwd(g_pUserPath, MAXPATHNAME);
		ps = GetCurrWndStack();
		WndPop(ps, &func, pFrmSaveDefDir); 			
		break;
	/* 退出按钮 */
	case 3:
	case HOME_DOWN:
	case BACK_DOWN:
	{
		/* 判断当前是否在顶层目录下 */
		char cPathBuff[MAXPATHNAME];
		getcwd(cPathBuff, MAXPATHNAME);
		if (strcmp(cPathBuff, MntUpDirectory) == 0)
		{
			ps = GetCurrWndStack();
			WndPop(ps, &func, pFrmSaveDefDir);
		}
		else
		{
			OpenFileAndFolder("..");
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
		break;
	default:
		break;
	}
}


/***
  * 功能：
		创建用户自定义路径窗体
  * 参数：
  		1、const char *pCurPath:	存贮用户选择的默认路径
  		2、const char *pTopPath:	允许用户返回的顶层目录
  * 返回：
		无
  * 备注：
***/
int CreateUserDefinedPath(char *pCurPath, const char *pTopPath)
{
	int iErr = 0; 
	GUIWINDOW *pWnd = NULL; 

	if(iErr == 0)
	{
		if( (NULL == pCurPath) ||
			(NULL == pTopPath))
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		__g_pSaveDefDirTopPath = pTopPath;
		strcpy(g_cLastFilePath, pCurPath);
		g_pUserPath = pCurPath;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmSaveDefDirInit, FrmSaveDefDirExit, 
							FrmSaveDefDirPaint, FrmSaveDefDirLoop, 
							FrmSaveDefDirPause, FrmSaveDefDirResume,
							NULL);			
		SendWndMsg_WindowExit(GetCurrWindow());  
		SendSysMsg_ThreadCreate(pWnd);			
	}	

	return iErr; 
}
