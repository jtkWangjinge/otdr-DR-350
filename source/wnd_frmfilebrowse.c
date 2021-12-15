/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfilebrowse.c
* 摘    要：  声明窗体wnd_frmfilebrowse(文件浏览)的窗体处理线程及相关操作函数
*			  该窗体提供了文件浏览功能。

* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-9-3

*******************************************************************************/
#include "wnd_frmfilebrowse.h"

/*******************************************************************************
**					为实现窗体frmfilebrowse而需要引用的其他头文件 			  **
*******************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

#include "app_global.h"
#include "app_frmotdr.h"
#include "app_frmsendfile.h"
#include "app_frmfilebrowse.h"

#include "guimyglobal.h"
#include "guiphoto.h"

#include "wnd_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmdialogsendfile.h"
#include "wnd_frmime.h"
#include "wnd_stack.h"
#include "wnd_frmfilebrowse1.h"
#include "wnd_droppicker.h"
#include "wnd_frmumountdevice.h"
#include "wnd_frmmenubak.h"

/*******************************************************************************
**								  宏定义				 					  **
*******************************************************************************/

enum _oprations_index
{
    FILE_OPEN_FILE_INDEX     = 0,			//打开
	FILE_FILE_TYPE_INDEX     = 1,			//文件类型
	FILE_MKDIR_INDEX         = 2,			//新建文件夹
    FILE_RENAME_INDEX        = 3,			//重命名
	FILE_DELETE_INDEX        = 4,			//删除
	FILE_SELECT_ALL_INDEX    = 5,			//全选
	FILE_COPY_INDEX          = 6,			//复制
	FILE_PASTE_INDEX		 = 7,			//粘贴
};
#define MAX_FUNCTION_NUM        6
/*******************************************************************************
**								  全局变量				 					  **
*******************************************************************************/
extern MULTISELECT *pEnvQueue;                              //多选队列

/*******************************************************************************
**								静态变量				 					  **
*******************************************************************************/
static const char*  pTopPath = MntUpDirectory;				        	// 顶层目录路径
static char         pCurrentPath[MAXPATHNAME] = "";		                // 当前绝对路径的缓存
static char         pLastPath[MAXPATHNAME]= MntUpDirectory;          	// 上一次所在的路径名
static char         pLastFileName[MAXPATHNAME] = "";	                // 用于重命名时保存将要重命名的文件名
static int          iFileOprationMode = FILE_NORMAL_MODE;               // 是否出于复制模式
// static int          iFileBrowseKeyFlag = 0;				                // 防止同时响应两个以上按键的标志
extern CURR_WINDOW_TYPE enCurWindow;
/*******************************************************************************
**						窗体frmfilebrowse中的控件定义部分					  **
*******************************************************************************/
static GUIWINDOW *pFrmFileBrowse = NULL;

static GUIPICTURE *pFileBrowseLeftBg = NULL;

static GUICHAR *pFileBrowseStrTitle = NULL;      		
static GUILABEL *pFileBrowseLblTitle = NULL; 		    

//右侧菜单栏功能按键
static GUICHAR *pFileBrowseStrFunction[MAX_FUNCTION_NUM] = {NULL};
static GUILABEL *pFileBrowseLblFunction[MAX_FUNCTION_NUM] = {NULL};
static GUIPICTURE *pFileBrowseBtnFunction[MAX_FUNCTION_NUM] = {NULL};

static WNDMENU1 *pFileBrowseMenu = NULL; //菜单栏

static GUIPICTURE *pFileBrowseBgIcon = NULL;		//文件管理器图标
static GUIPICTURE *pFileBrowseBtnHome = NULL;		//home按钮图形框

static int iSelectedFunctionIndex = 0;				//当前选中的功能项的下标
static int iFunctionEnabled[MAX_FUNCTION_NUM] = {0};//统计各个功能按钮的使能
static int iFunctionEnterFlag[MAX_FUNCTION_NUM] = { 0 };//统计各个功能确认的标志位
static unsigned int strFileBrowseMenu[MENU_MAX] = {
	FILE_LBL_OPEN,
	FILE_LBL_FILE_TYPE,
	FILE_LBL_MKDIR,
	FILE_LBL_RENAME,
	FILE_LBL_DELETE,
	FILE_LBL_SELECT_ALL, //全选
};

//各项非选中背景资源
static char *pFileBrowseUnpressBmp[MAX_FUNCTION_NUM] =
{
    BmpFileDirectory "bg_filebrowse_file.bmp",
    BmpFileDirectory "bg_filebrowse_fileStyle.bmp",
    BmpFileDirectory "bg_filebrowse_create.bmp",
    BmpFileDirectory "bg_filebrowse_rename.bmp",
    BmpFileDirectory "bg_filebrowse_delete.bmp",
    BmpFileDirectory "bg_filebrowse_all.bmp"
};

//各项选中背景资源
static char *pFileBrowseSelectBmp[MAX_FUNCTION_NUM] =
{
    BmpFileDirectory "bg_filebrowse_fileSelect.bmp",
    BmpFileDirectory "bg_filebrowse_fileStyleSelect.bmp",
    BmpFileDirectory "bg_filebrowse_createSelect.bmp",
    BmpFileDirectory "bg_filebrowse_renameSelect.bmp",
    BmpFileDirectory "bg_filebrowse_deleteSelect.bmp",
    BmpFileDirectory "bg_filebrowse_allSelect.bmp"
};

//各项选中背景资源
static char *pFileBrowseDisableBmp[MAX_FUNCTION_NUM] =
{
    BmpFileDirectory "bg_filebrowse_fileDisable.bmp",
    BmpFileDirectory "bg_filebrowse_fileStyleDisable.bmp",
    BmpFileDirectory "bg_filebrowse_createDisable.bmp",
    BmpFileDirectory "bg_filebrowse_renameDisable.bmp",
    BmpFileDirectory "bg_filebrowse_deleteDisable.bmp",
    BmpFileDirectory "bg_filebrowse_allDisable.bmp"
};

/*******************************************************************************
**	    窗体frmfilebrowse中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/
static int FileBrowseTextRes_Init(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);
static int FileBrowseTextRes_Exit(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);

/*******************************************************************************
**			    窗体frmfilebrowse中的控件事件处理函数定义部分				  **
*******************************************************************************/

/***************************窗体的按键事件处理函数*****************************/
// static int FileBrowseWndKey_Down(void *pInArg, int iInLen, 
//                               void *pOutArg, int iOutLen);
// static int FileBrowseWndKey_Up(void *pInArg, int iInLen, 
//                             void *pOutArg, int iOutLen);

/**************************菜单栏控件的事件处理函数****************************/
static int FileBrowseBtnFunction_Down(void *pInArg,  int iInLen, 
                               void *pOutArg, int iOutLen);
static int FileBrowseBtnFunction_Up(void *pInArg,  int iInLen, 
                             void *pOutArg, int iOutLen);
/***********************帮助和退出按钮的事件处理函数***************************/
static int FileBrowseBtnHome_Down(void *pInArg,  int iInLen, 
                                 void *pOutArg, int iOutLen);
static int FileBrowseBtnHome_Up(void *pInArg,  int iInLen, 
                               void *pOutArg, int iOutLen);

/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static void WidgetUpdate(void);
static void ReCreateWindow(GUIWINDOW **pWnd);
static void FileBrowseRename();
//获取各个按钮的使能情况
// static void GetFunctionEnable(void);
static void FileBrowseMenuCallBack(int iOption);


/***
  * 功能：
        窗体frmfilebrowse的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowseInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;		 	
	
    //临时变量定义
    GUIMESSAGE *pMsg = NULL; 

    //得到当前窗体对象
    pFrmFileBrowse = (GUIWINDOW *) pWndObj;

	/* 文件浏览控件初始化 */
    SetDisplayFileTypeAndSelectAll(1);
	__SetCurFileType(ANY);
	isInSmallFileBrowse = 1;
	FrmFileBrowseInit1(0, 40, MAXPAGEFILENUM, ReCreateWindow, pWndObj);
	SetFileSelectMode(FILE_MULTIPLE_MODE);
    RepaintWnd(FrmFileBrowsePaint);
	SetFirstOpenFolder(pLastPath);
	SetWidgetUpdateCallBack(WidgetUpdate);
	SetFileOperationType(iFileOprationMode);
    ClearQueue(pEnvQueue);
    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    FileBrowseTextRes_Init(NULL, 0, NULL, 0);
	iSelectedFunctionIndex = 0;
	iFunctionEnabled[0] = 1;
	iFunctionEnterFlag[0] = 1;
    enCurWindow = ENUM_FILE_WIN;//设置当前界面
    //建立窗体控件
    /****************************************************************/

    //建立桌面上的控件
	pFileBrowseLeftBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_filebrowse.bmp");

	/*********************************标题栏控件定义*******************************/
	pFileBrowseLblTitle = CreateLabel(40, 12, 100, 16, pFileBrowseStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFileBrowseLblTitle);
	
	/********************************右侧菜单控件定义******************************/
	 //建立按钮区的按钮显示标签
    //右侧菜单栏
    int i;
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        pFileBrowseBtnFunction[i] = CreatePicture(241, 43 + i * 25,
                                    72, 24, BmpFileDirectory"bg_menu_item_disable.bmp");
		pFileBrowseLblFunction[i] = CreateLabel(241, 43 + i * 25+4,
									72, 16, pFileBrowseStrFunction[i]);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pFileBrowseLblFunction[i]);
    }
	/***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
   	int itemOffset = GetItemWordOffset();
	//自定以设置每一项文字的偏移量
   	SetItemWordOffset(30);
	pFileBrowseMenu = CreateWndMenu1(MAX_FUNCTION_NUM, sizeof(strFileBrowseMenu), strFileBrowseMenu, 0xff00,
									 0, 1, 41, FileBrowseMenuCallBack);
	SetItemWordOffset(itemOffset);
	//文件管理的图标
	pFileBrowseBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_filebrowse_icon.bmp");

    //建立信息栏上的控件
    pFileBrowseBtnHome = CreatePicture(0, 20, 36, 21, BmpFileDirectory"bg_filebrowse_menu_home_unpress.bmp");

	//加载选中与非选中的资源
	for(i = 0; i < MAX_FUNCTION_NUM; ++i)
	{
		SetWndMenuItemBg(i, pFileBrowseUnpressBmp[i], pFileBrowseMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pFileBrowseSelectBmp[i], pFileBrowseMenu, MENU_SELECT);
		SetWndMenuItemBg(i, pFileBrowseDisableBmp[i], pFileBrowseMenu, MENU_DISABLE);
	}

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFileBrowse, 
                  pFrmFileBrowse);    
	
    //注册按钮区的控件
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileBrowseBtnFunction[i], 
                  pFrmFileBrowse);
    }
    //注册信息栏上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileBrowseBtnHome, 
                  pFrmFileBrowse);
	/***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/
	AddWndMenuToComp1(pFileBrowseMenu, pFrmFileBrowse);
	//注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();

    //注册窗体的按键消息处理
    // LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmFileBrowse, 
    //                 FileBrowseWndKey_Down, NULL, 0, pMsg);
    // LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmFileBrowse, 
    //                 FileBrowseWndKey_Up, NULL, 0, pMsg);
    
    //注册信息栏上控件的消息处理
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileBrowseBtnFunction[i], 
                    FileBrowseBtnFunction_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileBrowseBtnFunction[i], 
                    FileBrowseBtnFunction_Up, NULL, i, pMsg);
    }

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileBrowseBtnHome, 
                    FileBrowseBtnHome_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFileBrowseBtnHome, 
                    FileBrowseBtnHome_Up, NULL, 0, pMsg);
	/***************************************************************************
    *                      注册右侧菜单区控件的消息处理
    ***************************************************************************/
	LoginWndMenuToMsg1(pFileBrowseMenu, pFrmFileBrowse);

	return iReturn;
}

/***
  * 功能：
        窗体frmfilebrowse的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowseExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmFileBrowse = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmFileBrowse);

    //销毁窗体控件
    //***************************************************************/
	FrmFileBrowseExit1(pWndObj);	
	
    //销毁桌面上的控件
    DestroyPicture(&pFileBrowseLeftBg);

	/* 销毁标题栏的控件 */
	DestroyLabel(&pFileBrowseLblTitle);
	    
    int i;
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        DestroyPicture(&pFileBrowseBtnFunction[i]);
        DestroyLabel(&pFileBrowseLblFunction[i]);
    }
    //销毁信息栏上的控件
	DestroyPicture(&pFileBrowseBgIcon);
    DestroyPicture(&pFileBrowseBtnHome);
	/***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFileBrowseMenu);
	//释放文本资源
    //***************************************************************/
    FileBrowseTextRes_Exit(NULL, 0, NULL, 0);
	
    return iReturn;
} 

/***
  * 功能：
        窗体frmfilebrowse的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowsePaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i;
	
    //显示桌面背景
	DisplayPicture(pFileBrowseLeftBg);
    
	/* 显示标题栏的控件 */	
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        SetPictureEnable(1, pFileBrowseBtnFunction[i]);
    }
	
    /* 显示帮助和返回按钮 */
    // DisplayPicture(pFileBrowseBtnHome);
	// DisplayPicture(pFileBrowseBgIcon);
	/* 显示右侧的按钮图片 */
	PaintMenu(iFileOprationMode);
	DisplayLabel(pFileBrowseLblTitle);
	FrmFileBrowsePaint1(pWndObj);
	SetPowerEnable(1, 1);

	SyncCurrFbmap();

    return iReturn;
}


/***
  * 功能：
        窗体frmfilebrowse的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowseLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	PaintMenu(iFileOprationMode); 	
    //检测是否挂载SD卡或U盘
	iReturn = DetectionSD();
	if(iReturn)
	{	
		OpenFileAndFolder(pTopPath);
	}
    SyncCurrFbmap();
    //睡眠20ms 适度释放CPU
    MsecSleep(20);
	
    return iReturn;
}


/***
  * 功能：
        窗体frmfilebrowse的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowsePause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmfilebrowse的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowseResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}

//获取各个按钮的使能情况
// static void GetFunctionEnable(void)
// {
// 	int i;
// 	for (i = 0; i < MAX_FUNCTION_NUM; ++i)
// 	{
// 		iFunctionEnabled[i] = GetPictureEnable(pFileBrowseBtnFunction[i]);
// 	}
// }

//设置打开按钮的状态
static void SetOpenButtonStatus(int count)
{
	if (count > 1)
		SetWndMenuItemEnble(FILE_OPEN_FILE_INDEX, 0, pFileBrowseMenu);
	else
		SetWndMenuItemEnble(FILE_OPEN_FILE_INDEX, 1, pFileBrowseMenu);
}

//设置全选按钮的状态
static void SetSelectAllButtonStatus(char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_SELECT_ALL_INDEX, 0, pFileBrowseMenu);
	else
		SetWndMenuItemEnble(FILE_SELECT_ALL_INDEX, 1, pFileBrowseMenu);
}

//设置文件类型按钮的状态
static void SetFileTypeButtonStatus(char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_FILE_TYPE_INDEX, 0, pFileBrowseMenu);
	else
		SetWndMenuItemEnble(FILE_FILE_TYPE_INDEX, 1, pFileBrowseMenu);
}

//设置新建文件夹按钮的状态
static void SetMkdirButtonStatus(char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_MKDIR_INDEX, 0, pFileBrowseMenu);
	else
		SetWndMenuItemEnble(FILE_MKDIR_INDEX, 1, pFileBrowseMenu);
}

//设置重命名按钮的状态
static void SetRenameButtonStatus(int count, char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_RENAME_INDEX, 0, pFileBrowseMenu);
	else
	{
		if (count < 1 || count > 1)
			SetWndMenuItemEnble(FILE_RENAME_INDEX, 0, pFileBrowseMenu);
		else
			SetWndMenuItemEnble(FILE_RENAME_INDEX, 1, pFileBrowseMenu);
	}
}

//设置复制按钮的状态
static void SetCopyButtonStatus(int count, char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_COPY_INDEX, 0, pFileBrowseMenu);
	else
	{
		if (count < 1)
			SetWndMenuItemEnble(FILE_COPY_INDEX, 0, pFileBrowseMenu);
		else
			SetWndMenuItemEnble(FILE_COPY_INDEX, 1, pFileBrowseMenu);
	}
}

//设置粘贴按钮的状态
static void SetPasteButtonStatus(int mode, char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_PASTE_INDEX, 0, pFileBrowseMenu);
	else
	{
		if (mode == FILE_COPY_MODE)
			SetWndMenuItemEnble(FILE_PASTE_INDEX, 1, pFileBrowseMenu);
		else
			SetWndMenuItemEnble(FILE_PASTE_INDEX, 0, pFileBrowseMenu);
	}
}

//设置删除按钮的状态
static void SetDeleteButtonStatus(int count, char *currentPath)
{
	if (!strcmp(currentPath, pTopPath)) //当前位置处于顶层目录
		SetWndMenuItemEnble(FILE_DELETE_INDEX, 0, pFileBrowseMenu);
	else
	{
		if (count < 1)
			SetWndMenuItemEnble(FILE_DELETE_INDEX, 0, pFileBrowseMenu);
		else
			SetWndMenuItemEnble(FILE_DELETE_INDEX, 1, pFileBrowseMenu);
	}
}

//绘制菜单栏
void PaintMenu(int mode)
{
	//多选列表选项的个数
	int count = pEnvQueue->pMsgQueue->iCount;
	char currentPath[512] = {0};
	memset(currentPath, 0, 512);
	getcwd(currentPath, 512);

	//刷新按钮背景
	RefreshMenuBackground(pFileBrowseMenu);

	int i;
	//使能所有菜单项
	for (i = 0; i < MENU_MAX; ++i)
	{
		SetWndMenuItemEnble(i, 1, pFileBrowseMenu);
	}

	SetOpenButtonStatus(count);
	SetSelectAllButtonStatus(currentPath);
	SetFileTypeButtonStatus(currentPath);
	SetMkdirButtonStatus(currentPath);
	SetRenameButtonStatus(count, currentPath);
	SetCopyButtonStatus(count, currentPath);
	SetPasteButtonStatus(mode, currentPath);
	SetDeleteButtonStatus(count, currentPath);
}

static int FileBrowseTextRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	pFileBrowseStrTitle = GetCurrLanguageText(MAIN_LBL_FILE);
    int i;
    int strFileBrowseMenu[] = {
		FILE_LBL_OPEN,
		FILE_LBL_SELECT_ALL,//全选
		FILE_LBL_MKDIR,
		FILE_LBL_RENAME,
		FILE_LBL_COPY,
		FILE_LBL_PASTE,
		FILE_LBL_DELETE,
    };
        
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        pFileBrowseStrFunction[i] = GetCurrLanguageText(strFileBrowseMenu[i]);
    }

    return iReturn;
}

static int FileBrowseTextRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	GuiMemFree(pFileBrowseStrTitle);

    int i;
    for(i = 0; i < MAX_FUNCTION_NUM; ++i)
    {
        GuiMemFree(pFileBrowseStrFunction[i]);
    }
    
    return iReturn;
}


/*******************************************************************************
**			    			窗体的按键事件处理函数				 		 	  **
*******************************************************************************/
/***
  * 功能：
		窗体物理按键down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
// static int FileBrowseWndKey_Down(void *pInArg, int iInLen, 
//                              void *pOutArg, int iOutLen)
// {
//     //错误标志、返回值定义
//     int iReturn = 0;
//     //临时变量定义
//     unsigned int uiValue;

//     uiValue = (unsigned int)pInArg;
// 	if(iFileBrowseKeyFlag == 0)
// 	{
// 		iFileBrowseKeyFlag = 1;
//         //FileBrowseKeyDown(uiValue);
//     	switch (uiValue)
//         {
// 		case KEYCODE_ESC:
// 			//FileBrowseBtnHome_Down(NULL, 0, NULL, 0);
// 			break;
// 		default:
// 			break;
//         }
// 	}
    
//     return iReturn;
// }

/***
  * 功能：
		窗体物理按键up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
// static int FileBrowseWndKey_Up(void *pInArg, int iInLen, 
//                            void *pOutArg, int iOutLen)
// {
//     //错误标志、返回值定义
//     int iReturn = 0;
//     //临时变量定义
//     unsigned int uiValue;

//     uiValue = (unsigned int)pInArg;
// 	//退出按键是公用的
// 	if (uiValue == KEYCODE_ESC)
// 	{
// 		FileBrowseBtnHome_Up(NULL, 0, NULL, 0);
// 	}

// 	if (isInSmallFileBrowse)
// 	{
// 		//中间部分的按键处理
// 		KeyBoardCallBack1(uiValue);
// 	}
// 	else
// 	{
// 		switch (uiValue)
// 		{
// 		case KEYCODE_UP:
// 			if (iSelectedFunctionIndex != 0)
// 			{
// 				iSelectedFunctionIndex--;
// 			}
// 			break;
// 		case KEYCODE_DOWN:
// 			if (iSelectedFunctionIndex != (MAX_FUNCTION_NUM - 1))
// 			{
// 				iSelectedFunctionIndex++;
// 			}
// 			break;
// 		case KEYCODE_LEFT:
// 			isInSmallFileBrowse = 1;
// 			break;
// 		case KEYCODE_ENTER:
// 			iFunctionEnterFlag[iSelectedFunctionIndex] = 1;
// 			FileBrowseBtnFunction_Down(NULL, 0, NULL, iSelectedFunctionIndex);
// 			break;
// 		default:
// 			break;
// 		}
// 	}

// 	return iReturn;
// }


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
                         FrmFileBrowseInit, FrmFileBrowseExit, 
                         FrmFileBrowsePaint, FrmFileBrowseLoop, 
			             FrmFileBrowsePause, FrmFileBrowseResume,
                         NULL);
}


/*******************************************************************************
**							菜单栏控件的事件处理函数						  **
*******************************************************************************/
/*
 * 确认删除多个文件时的回调函数
 */
static void MultiDeleteOkHandle(void)
{
	ShowDialogExtraInfo(GetCurrLanguageText(FILE_LBL_DELETE_FILE));
	DeleteMultiSelect(pEnvQueue);
	ClearQueue(pEnvQueue);
}
 /***
  * 功能：
		确认覆盖文件时调用的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void CopyOkHandle(void)
{
	ShowDialogExtraInfo(GetCurrLanguageText(FILE_LBL_FILE_ALREADY_EXIST));
	PasteFromClipboard(pCurrentPath, FILE_COPY_MODE);
	ClearQueue(pEnvQueue);
}

#if 0
/***
  * 功能：
		确认覆盖文件时调用的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void SendToUsbOkHandle(void)
{
	ShowDialogExtraInfo(GetCurrLanguageText(DIALOG_SENDING));
	CopyMultiSelect(pEnvQueue, MntUsbDirectory);
	ClearQueue(pEnvQueue);
}
#endif

/***
  * 功能：
		执行重命名的回调函数
  * 参数：
  		无
  * 返回：
		无
  * 备注：
***/
static void RenameOk(void)
{
	int iReturn = 0;
	int iErr = 0;
	char cTmpInputBuff[512];

	GetIMEInputBuff(cTmpInputBuff);

	iReturn = RegularSeach(cTmpInputBuff);

	if( iReturn==0 )
	{
		char *pTmp = strrchr(pLastFileName, '/');
		//如果文件名未修改，不做任何操作
		if(pTmp)
		{
            if(!strcmp(++pTmp, cTmpInputBuff))
                return;
		}
		else
		{
            if(!strcmp(pLastFileName, cTmpInputBuff))
                return;
		}

		/*
		 * 判断将要重命名后的文件是否是同名
		 */
		if( access(cTmpInputBuff, F_OK) == 0) //含有同名文件
		{
			/* 弹出对话框提示有同名文件存在 */
			CreateIMEDialog(GetCurrLanguageText(FILE_LBL_SAME_FILE));
		}
		/* 不存在同名目录 */
		else
		{
			iErr = rename(pLastFileName, cTmpInputBuff);
			if(iErr != 0)
			{
				CreateIMEDialog(GetCurrLanguageText(FILE_LBL_RENAME_FAILED));
			}
		}
	}
	else
	{
		CreateIMEDialog(GetCurrLanguageText(DIALOG_LBL_FILENAME_INVAILD));
	}
}

/***
  * 功能：
		重命名按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static void FileBrowseRename()
{
    //临时变量定义
	char *pTmp = NULL;
	char *pTmp1 = NULL; 

	/* 保存此时的路径，已便关闭键盘重新打开目录时还是回到此路径 */
	getcwd(pLastPath, MAXPATHNAME);	
	SetFirstOpenFolder(pLastPath);

	pTmp = ReadQueue(pEnvQueue);
	if (NULL != pTmp)
	{
		int iLen = 0;
		unsigned short *pUnicode = NULL;
	
		strcpy(pLastFileName, pTmp);
		pTmp1 = strrchr(pTmp, '/');

		if(NULL != pTmp1)
		{
			pTmp1++;
			pUnicode = GetUtf8ToUcs2(pTmp1);
			iLen = StringStrlen(pUnicode);	
		}

		/* 文件名过长 */
		if(iLen > 90)
		{
			DialogInit(120, 90, TransString("Warning"),
						GetCurrLanguageText(FILE_LBL_LONG_FILE_NAME),
						0, 0, ReCreateWindow, NULL, NULL);					
		}
		else
		{
			if(NULL == pTmp1)
			{
				IMEInit(pTmp, 90, 0, ReCreateWindow, RenameOk, NULL);
			}
			else
			{
				IMEInit(pTmp1, 15, 0, ReCreateWindow, RenameOk, NULL);
			}			
		}		
		
		GuiMemFree(pTmp);
	}	
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

//复制文件
static void CopyFile(void)
{
    iFileOprationMode = FILE_COPY_MODE;
    SetFileOperationType(iFileOprationMode);
    //获得当前所在的目录
	getcwd(pCurrentPath, MAXPATHNAME);
	strcpy(pLastPath, pCurrentPath);	
	SetFirstOpenFolder(pLastPath);
    CopyFileToClipboard();
    FrmFileBrowsePaint(pFrmFileBrowse);
}
//粘贴文件
static int PasteFile(void)
{
    //获得当前所在的目录
	getcwd(pCurrentPath, MAXPATHNAME);
	strcpy(pLastPath, pCurrentPath);	
	SetFirstOpenFolder(pLastPath);

    //如果是在文件管理器的根目录(/mnt)下执行粘贴，直接忽略
    if (IsUpDir())
        return -1;

    //判断是否存在同名文件，如果不存在同名文件，则粘贴
    int ret = PasteFromClipboard(pCurrentPath, FILE_NORMAL_MODE);
    if(ret == -3)
    {
        DialogInit(120, 90, TransString("Warning"),
				   GetCurrLanguageText(FILE_LBL_FILE_NOT_EXIST),
				   0, 0, ReCreateWindow, NULL, NULL);
    }
    else if(ret == -4)
    {
        DialogInit(120, 90, TransString("Warning"),
				   GetCurrLanguageText(FILE_LBL_NOT_PASTE),
				   1, 0, ReCreateWindow, NULL, NULL);        
    }
    else if(ret < 0)
    {
        DialogInit(120, 90, TransString("Warning"),
				   GetCurrLanguageText(FILE_LBL_FILE_ALREADY_EXIST),
				   0, 1, ReCreateWindow, CopyOkHandle, NULL);
    }
    else
    {
        ClearQueue(pEnvQueue);
        
		WidgetUpdate();
		OpenFileAndFolder(pLastPath);
		SyncCurrFbmap();
        
		DisableInfoDialog(pFrmFileBrowse);        
    }
    return 0;
}
//删除文件
static void DeleteFile(void)
{
    getcwd(pLastPath, MAXPATHNAME);	
	SetFirstOpenFolder(pLastPath);

	DialogInit(120, 90, TransString("Warning"),
			   GetCurrLanguageText(FILE_LBL_DELETE_FILE),
		       0, 1, ReCreateWindow, MultiDeleteOkHandle, NULL);
}
#if 0
/***
  * 功能：
		发送到usb按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static void FileBrowseSendToUsb(void)
{
    getcwd(pCurrentPath, MAXPATHNAME);
    strcpy(pLastPath, pCurrentPath);
    SetFirstOpenFolder(pLastPath);

    if ((strncmp(pCurrentPath, MntUsbDirectory, strlen(MntUsbDirectory)) == 0))
	{
		ClearQueue(pEnvQueue);
		DialogInit(120, 90, TransString("Warning"),
				   GetCurrLanguageText(DIALOG_UNDER_USB),
				   0, 0, ReCreateWindow, NULL, NULL);			
		return;
	}	
    
    //预防usb未挂载导致文件管理首页创建了一个usb文件
    if(access(MntUsbDirectory, F_OK))
    {
        ClearQueue(pEnvQueue);
		DialogInit(120, 90, TransString("Warning"),
					TransString("DIALOG_NO_USB"),
					0, 0, ReCreateWindow, NULL, NULL);			
		return;
    }
	//不存在同名文件
	if (CheckSameName(pEnvQueue, MntUsbDirectory))
	{
		//ShowInfoDialog(GetCurrLanguageText(DIALOG_SENDING), pFrmFileBrowse);
		CopyMultiSelect(pEnvQueue, MntUsbDirectory);
		ClearQueue(pEnvQueue);
		FrmFileBrowsePaint(pFrmFileBrowse);
		DisableInfoDialog(pFrmFileBrowse);
	}
	//存在同名文件
	else
	{
		DialogInit(120, 90, TransString("Warning"),
				   GetCurrLanguageText(FILE_LBL_FILE_ALREADY_EXIST),
				   0, 1, ReCreateWindow, SendToUsbOkHandle, NULL);			
	}
    
	return;
}
#endif
//新建文件夹
static void CreateNewFolder(void)
{
    getcwd(pLastPath, MAXPATHNAME);	
	SetFirstOpenFolder(pLastPath);

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
						TransString("DIALOG_OPREATE_EQUI"),
						0, 0, ReCreateWindow, NULL, NULL);
		}
        return;
	}

    IMEInit(NULL, 15, 0, ReCreateWindow, MakeDirOk, NULL);
}

static int FileBrowseBtnFunction_Down(void *pInArg,  int iInLen, 
                               void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    int iTmp = iOutLen;
    UpdateFullSelect();//更新全选按钮
    switch(iTmp)
    {
        case FILE_OPEN_FILE_INDEX:
            OpenAtIndex(GetCurrFileIndex());
            break;
        case FILE_SELECT_ALL_INDEX:
            SelectAllFile();
            break;
        case FILE_MKDIR_INDEX://创建文件夹
            CreateNewFolder();
            break;
        case FILE_RENAME_INDEX://重命名
            FileBrowseRename();
            break;
        case FILE_COPY_INDEX://复制
            CopyFile();
            break;
        case FILE_PASTE_INDEX://粘贴
            PasteFile();
            break;
        case FILE_DELETE_INDEX://删除
            DeleteFile();
            break;
        default:
            break;    
    }
    return iReturn;
}
static int FileBrowseBtnFunction_Up(void *pInArg,  int iInLen, 
                             void *pOutArg, int iOutLen)
{
    return 0;
}

//右侧菜单栏操作回调函数
static void FileBrowseMenuCallBack(int iOption)
{
	switch (iOption)
	{
	case FILE_OPEN_FILE_INDEX://打开文件
		OpenAtIndex(GetCurrFileIndex());
		break;
	case FILE_FILE_TYPE_INDEX://文件类型
		SelectFileType();
		break;
	case FILE_MKDIR_INDEX: //创建文件夹
		CreateNewFolder();
		break;
	case FILE_RENAME_INDEX: //重命名
		FileBrowseRename();
		break;
	case FILE_DELETE_INDEX: //删除
		DeleteFile();
		break;
	case FILE_SELECT_ALL_INDEX://全选
		SelectAllFile();
		break;
	case FILE_COPY_INDEX: //复制
		CopyFile();
		break;
	case FILE_PASTE_INDEX: //粘贴
		PasteFile();
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		FileBrowseBtnHome_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}
/*******************************************************************************
**			    			home按钮事件定义					 		  **
*******************************************************************************/

static int IsTopPath(const char *path)
{
	return strcmp(path, pTopPath) == 0;
}

static int IsRootPath(const char *path)
{
	return strcmp(path, MntUpDirectory) == 0;
}

static int FileBrowseBtnHome_Down(void *pInArg,  int iInLen, 
								 void *pOutArg, int iOutLen)
{
	int iReturn = 0;
    
	TouchChange("bg_filebrowse_menu_home_press.bmp", pFileBrowseBtnHome, 
				NULL, NULL, 1);    
	SyncCurrFbmap();	

	return iReturn;
}

static int FileBrowseBtnHome_Up(void *pInArg, int iInLen, 
							 void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	// TouchChange("bg_filebrowse_menu_home_unpress.bmp", pFileBrowseBtnHome, 
	// 			NULL, NULL, 0); 
	// SyncCurrFbmap();

	char currPath[MAXPATHNAME];
	getcwd(currPath, MAXPATHNAME);
	if (IsTopPath(currPath) || IsRootPath(currPath))
	{
		strcpy(pLastPath, MntUpDirectory);
		ReturnMenuOK();
	}
	else
	{
		ClearQueue(pEnvQueue);
		OpenFileAndFolder("..");
		PaintMenu(iFileOprationMode);
		SyncCurrFbmap();
	}
	

	return iReturn;
}


/***
  * 功能：
		根据当前的多选状态，更新删除、粘贴、重命名按钮状态
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void WidgetUpdate(void)
{
    PaintMenu(iFileOprationMode);
}

/***
  * 功能：
        设置当前文件浏览器的操作类型
  * 参数：
        1、FILEOPERATION enFileOperation:	将要设置的文件操作类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int __SetFileOperationType(unsigned int enFileOperation)
{
	int iErr = 0;

	return iErr;
}


/***
  * 功能：
		设置文件浏览器打开的目录
  * 参数：
		1.const char *pPathFiles:	将要打开的目录
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int __SetOpenFolder(const char *pPathFiles)
{
	int iErr = 0;

	if(iErr == 0)
	{
		if(NULL == pPathFiles)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		strcpy(pLastPath, pPathFiles);
	}	
	
	return iErr;
}

int FileManager(GUIWINDOW* mainWindow)
{
	GUIWINDOW *pWnd = NULL;
    
	strcpy(pLastPath, MntUpDirectory);
    chdir(MntUpDirectory);//切换到顶层目录
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						FrmFileBrowseInit, FrmFileBrowseExit, 
						FrmFileBrowsePaint, FrmFileBrowseLoop, 
						FrmFileBrowsePause, FrmFileBrowseResume,
						NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(mainWindow);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}
