/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmquicksave.c
* 摘    要：  声明主窗体frmquicksave 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.0
* 作    者：  	     
* 完成日期：  2015-3-2
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/

#include "wnd_frmquicksave.h"

/**************************************************************
* 	为实现窗体frmquicksave而需要引用的其他头文件
**************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmime.h"
#include "guimyglobal.h"

#include "wnd_frmotdrsave.h"
#include "wnd_frmsola.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmmain.h"
#include "wnd_frmdialog.h"
#include "app_frmotdr.h"
#include "wnd_frmfilebrowse.h"
#include "wnd_frmfilebrowse1.h"
#include "app_frmfilebrowse.h"
#include "guiphoto.h"

/**************************************************************
* 		定义frmquicksave中内部使用的数据结构
**************************************************************/
unsigned int StrQuickSaveRightMenu[] = {	
1
};
static char cQuickSaveName[512] = "Fiber0000.SOR";
static char TmpQuickSaveName[512];
static CALLLBACKWINDOW _g_QuickSavecallBackFunc = NULL;		// 重绘前一个窗体的回调函数
static const char *__g_pQuickSaveTopPath = NULL;			// 顶层目录路径
static char *__g_pQuickSaveLastPath = NULL;                 // 还原路径
static char __g_cFirstPath_Quick[512];						// 首次打开快速保存时应该进入的路径
static FILETYPE __g_fileType = SOR;
extern POTDR_TOP_SETTINGS pOtdrTopSettings;


/***************************************************************
* 			窗体FrmOtdrFileOpen中的窗体控件
****************************************************************/
static GUIWINDOW *pFrmQuickSave = NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pQuickSaveBG = NULL;
//static GUIPICTURE *pQuickSaveBGTableTitle = NULL;
static GUIPICTURE *pQuickSaveBtnTop = NULL;
static GUIPICTURE *pQuickSaveBtnInput = NULL;

/*********************右侧控件定义************************/
static WNDMENU1 *pQuickSaveMenu = NULL;

/**********************桌面控件定义***********************/
//快速存储标题标签和文本
static GUILABEL *pQuickSaveLblLabel = NULL;
static GUILABEL *pQuickSaveLblInput = NULL;

static GUICHAR *pQuickSaveStrLabel = NULL;
static GUICHAR *pQuickSaveStrInput = NULL;


/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int QuickSaveTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int QuickSaveTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/**************************************************************
* 				控件函数声明
**************************************************************/
//右侧确认按钮
static int	QuickSaveBtnConfirm_Fun(void);
//右侧进入按钮
static int  QuickSaveBtnCancel_Fun(void);

static void QuickSaveMenuCallBack(int iOption);

static int QuickSaveBackTo(void);
/**************************************************************
* 				控件函数定义
**************************************************************/
/***
  * 功能：
		重新绘制窗体
  * 参数：
		无
  * 返回：
  		无
  * 备注：
***/
static void ReCreateQuickSaveWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmQuickSaveInit, FrmQuickSaveExit, 
		FrmQuickSavePaint, FrmQuickSaveLoop, 
		FrmQuickSavePause, FrmQuickSaveResume,
		NULL);
}


/***
  * 功能：
		回调函数
  * 参数：
		无
  * 返回：
  		无
  * 备注：
***/
/*static void QuikSaveCurveOKReCall(void)
{
	char buff[512];
	getcwd(buff, sizeof(buff));
	strcat(buff, "/");
	strcat(buff, TmpQuickSaveName);

	WriteSor(0, buff);
}*/

#if 0
/***
  * 功能：
		输入完毕回调函数
  * 参数：
		无
  * 返回：
  		无
  * 备注：
***/
void QuickSaveInputOK(void)
{
	char cTmpInputBuff[512];
	int iReturn = 0;

	GetIMEInputBuff(cTmpInputBuff);
	CODER_LOG(CoderGu, "back from IME cQuickSaveName = %s\n", cQuickSaveName);


	/* 判断将要创建的目录名是否合法 */
	iReturn = RegularSeach(cTmpInputBuff);
	if(iReturn != 0)
	{
		CreateIMEDialog(TransString("DIALOG_FILE_INPUT_INCORRECT"));
	}
	else
	{
		strcpy(cQuickSaveName, cTmpInputBuff);
	}
}
#endif
/* 判断文件名是否为空 */
static int IsFileNameEmpty(char* fileName)
{
    int iErr = 0;
    if((strcmp(fileName, "") == 0) && (__g_fileType == SOR))
	{
		DialogInit(120, 90, TransString("Warning"),
    		TransString("DIALOG_INFO_SOR_NAME_WARN"),
    		0, 0, ReCreateQuickSaveWindow, NULL, NULL);
		iErr = -1;
	}
    return iErr;
}

/* 判断是否有波形文件 */
static int IsExitCurveFile(void)
{
    int iErr = 0;
	if((GetCurveNum(pOtdrTopSettings->pDisplayInfo) <= 0)&& (__g_fileType == SOR))
	{
		DialogInit(120, 90, TransString("DIALOG_ERROR"),
					TransString("DIALOG_NO_CURVE"),
					0, 0, ReCreateQuickSaveWindow, NULL, NULL);
		iErr = -1;
	}
    return iErr;
}

/* 判断用户输入的文件名是否含有.sor扩展名，
	   没有的话自动添加扩展名，自动将扩展名转换为大写*/
static void SetFileExtension(char* initExtension, char* newExtension)
{
    char *pExtension = NULL;
	pExtension = strrchr(TmpQuickSaveName, '.');
	/* 找到扩展名 */
	if( NULL != pExtension)
	{
		/* 扩展名是SOR文件 */
		if( strcasecmp(initExtension, pExtension) == 0 )
		{
			strcpy(pExtension, newExtension);
		}
		/* 扩展名不是SOR文件 */
		else
		{
			strcat(TmpQuickSaveName, newExtension);
		}
	}
	/* 没有扩展名 */
	else
	{
		strcat(TmpQuickSaveName, newExtension);
	}
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
static int	QuickSaveBtnConfirm_Fun(void)
{
	//错误标志、返回值定义
	int iErr = 0;
	int iReturn = 0;
	char buff[512];

	/* 判断是否有波形文件 */
	if(iErr == 0)
	{
		strcpy(TmpQuickSaveName, cQuickSaveName);
		getcwd(__g_cFirstPath_Quick, sizeof(__g_cFirstPath_Quick));
        iErr = IsFileNameEmpty(cQuickSaveName) ? -1 : IsExitCurveFile();
	}

	/* 判断文件名是否为空 */
	if(iErr == 0)
	{
        iErr = IsFileNameEmpty(TmpQuickSaveName);
	}

	/* 判断用户输入的文件名是否含有.sor扩展名，
	   没有的话自动添加扩展名，自动将扩展名转换为大写*/
	if(iErr == 0)
	{
	    if(__g_fileType == SOR)
        {
            SetFileExtension(".sor", ".SOR");
        }
        else
        {
            SetFileExtension(".sola", ".SOLA");
        }
	}

	/* 判断是否存在同名文件 */
	if(iErr == 0)
	{
        if (!CanWriteFile(TmpQuickSaveName))
        {
            iErr = -3;
        }
	}

	if(iErr == 0)
	{
		getcwd(buff, sizeof(buff));
		strcat(buff, "/");
		strcat(buff, TmpQuickSaveName);
		CODER_LOG(CoderGu, "current name %s\n", buff);
        iReturn = (__g_fileType == SOR) ? WriteSor(0, buff) : WriteSola(buff, 0);
		if(iReturn == 0)
		{
			DialogInit(120, 90, TransString("DIALOG_NOTE"),
				GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
				0, 0, ReCreateQuickSaveWindow, NULL, NULL);
		}
		else if(iReturn == -2)
		{
			DialogInit(120, 90, TransString("Warning"),
				GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
				0, 0, ReCreateQuickSaveWindow, NULL, NULL);
		}
		else
		{
			DialogInit(120, 90, TransString("Warning"),
				GetCurrLanguageText(DIALOG_SOR_SYSTEM_ERR),
				0, 0, ReCreateQuickSaveWindow, NULL, NULL);
		}
	}

	return iErr;
}


/***
  * 功能：
        		Cancel 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  QuickSaveBtnCancel_Fun(void)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIWINDOW *pWnd = NULL;

	GuiMemFree(pQuickSaveStrLabel);

    (*_g_QuickSavecallBackFunc)(&pWnd);
    SendWndMsg_WindowExit(pFrmQuickSave);     
    SendSysMsg_ThreadCreate(pWnd);           

	return iReturn;
}
//取消点击标题栏重命名文件名的功能
#if 0
/***
  * 功能：
        		Input 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  QuickSaveBtnInput_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	
	return iReturn;
}


/***
  * 功能：
        		Input 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  QuickSaveBtnInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	char buff[512] = {0};
	int iLength = 0;
	printf("QuickSaveBtnInput_Up cQuickSaveName is %s\n", cQuickSaveName);
	if(strcmp(cQuickSaveName, "") != 0)
	{
		sprintf(buff, "%s", cQuickSaveName);
		if((strstr(cQuickSaveName, ".SOR") != NULL) || (strstr(cQuickSaveName, ".SOLA") != NULL))
		{
			iLength = strlen(buff);
			buff[iLength - 4] = '\0';
		}
	}

	IMEInit(buff, 22, 0, ReCreateQuickSaveWindow, QuickSaveInputOK, NULL);
	return iReturn;
}
#endif

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
int FrmQuickSaveInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;

	pFrmQuickSave = (GUIWINDOW *) pWndObj;
	SetDisplayFileTypeAndSelectAll(0);
	FrmFileBrowseInit1(0, 40, MAXPAGEFILENUM, ReCreateQuickSaveWindow, pFrmQuickSave);
	SetFirstOpenFolder(__g_cFirstPath_Quick);
	SetFileOperationType(FILE_SAVE_MODE);
	SetCurFileType(__g_fileType);
	SetWidgetUpdateCallBack(NULL);
	
	QuickSaveTextRes_Init(NULL, 0, NULL, 0);
    pQuickSaveBG = CreatePhoto("bg_filebrowse");
    //pQuickSaveBGTableTitle = CreatePhoto("otdr_top1f");
    pQuickSaveBtnTop = CreatePhoto("bg_filebrowse_title");
	pQuickSaveMenu = CreateWndMenu1(2, sizeof(StrQuickSaveRightMenu),
								StrQuickSaveRightMenu, (UINT16)(MENU_BACK | ~MENU_HOME),
								-1, 0, 40, QuickSaveMenuCallBack);
	
	pQuickSaveLblLabel = CreateLabel(0, 24, 100, 24, pQuickSaveStrLabel);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pQuickSaveLblLabel);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pQuickSaveLblLabel);
	
	/*建立快速存储的自定义命名按钮*/
	//pQuickSaveBtnInput = CreatePicture(0, 40, 274, 22,
	//						BmpFileDirectory"btn_quicksave_input.bmp");
	pQuickSaveBtnInput = CreatePhoto("btn_quicksave_input");
	pQuickSaveLblInput = CreateLabel(3, 47, 250, 24, pQuickSaveStrInput);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pQuickSaveLblInput);
	
	
	//注册桌面上的控件
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmQuickSave, 
	              pFrmQuickSave);
	//AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pQuickSaveBtnInput, 
	//	pFrmQuickSave);
	//右边按钮栏注册
	AddWndMenuToComp1(pQuickSaveMenu, pFrmQuickSave);

	pMsg = GetCurrMessage();
	
	//注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pQuickSaveMenu, pFrmQuickSave);
    
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
int FrmQuickSaveExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pFrmQuickSave = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmQuickSave);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pQuickSaveBG);
    //DestroyPicture(&pQuickSaveBGTableTitle);
	DestroyPicture(&pQuickSaveBtnInput);
	DestroyPicture(&pQuickSaveBtnTop);
	
	//按钮标签销毁
	DestroyWndMenu1(&pQuickSaveMenu);
	FrmFileBrowseExit1(pWndObj);	
	//文本内容销毁
	QuickSaveTextRes_Exit(NULL, 0, NULL, 0);
    chdir(__g_pQuickSaveLastPath);//切换回到上次路径
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
int FrmQuickSavePaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	//得到当前窗体对象
	pFrmQuickSave = (GUIWINDOW *) pWndObj;

	//显示状态栏、桌面、信息栏控件
	
	DisplayPicture(pQuickSaveBG);
	DisplayPicture(pQuickSaveBtnTop);
    //DisplayPicture(pQuickSaveBGTableTitle);
	DisplayPicture(pQuickSaveBtnInput);
	DisplayLabel(pQuickSaveLblLabel);
	DisplayLabel(pQuickSaveLblInput);
	FrmFileBrowsePaint1(pWndObj);
	//按钮标签显示
	DisplayWndMenu1(pQuickSaveMenu);
	
	//显示文件界面
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}


/***
  * 功能：
        	窗体FrmQuickSave 的循环函数，进行窗体循环
  * 参数：
       	 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmQuickSaveLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	/* 检测是否挂载U盘 */
	if( strncmp(__g_pQuickSaveTopPath, MntUsbDirectory, strlen(MntUsbDirectory)) == 0 )	
	{
		iReturn = CheckMountUSB();
		if(iReturn)
		{	
			GUIWINDOW *pWnd = NULL;
			
			(*_g_QuickSavecallBackFunc)(&pWnd);
			SendWndMsg_WindowExit(pFrmQuickSave);	  
			SendSysMsg_ThreadCreate(pWnd); 
		}			
	}
	
	return iReturn;
}


/***
  * 功能：
        窗体FrmQuickSave 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmQuickSavePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体FrmQuickSave 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmQuickSaveResume(void *pWndObj)
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
static int QuickSaveTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pQuickSaveStrInput = TransString(cQuickSaveName); 

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
static int QuickSaveTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//GuiMemFree(pQuickSaveStrLabel);
	GuiMemFree(pQuickSaveStrInput); 

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
static void QuickSaveMenuCallBack(int iOption)
{
	
	switch (iOption)
	{
	case 0://确认
		QuickSaveBtnConfirm_Fun();
	 	break;
	case 1://取消
		QuickSaveBtnCancel_Fun();
	 	break;
	case BACK_DOWN:
		QuickSaveBackTo();
		break;
	case HOME_DOWN:
	 	break;
		
	default:
		break;
	}
}

//设置传入快速储存界面时的文件名字
void SetQuickSaveName(char * arg)
{
	memcpy(cQuickSaveName, arg, 512);
}

static int QuickSaveBackTo(void)
{
	int iErr = 0;
	char cPathBuff[512];
	GUIWINDOW *pWnd = NULL;
	
	if(iErr == 0)
	{
		/* 判断当前是否在顶层目录下 */
		getcwd(cPathBuff, 512);
		if( (strcmp(__g_pQuickSaveTopPath, cPathBuff) == 0) ||
			(strcmp(cPathBuff, MntUpDirectory) == 0) )
		{
			(*_g_QuickSavecallBackFunc)(&pWnd);
			SendWndMsg_WindowExit(pFrmQuickSave);	  
			SendSysMsg_ThreadCreate(pWnd);	

			iErr = -1;
		}		
	}
	
	if(iErr == 0)
	{
		OpenFileAndFolder("..");
		RefreshScreen(__FILE__, __func__, __LINE__);
	}

	return iErr;
}


/***
  * 功能：
		创建快速保存窗体
  * 参数：
  		1、const char *pTopPath	:	允许用户返回的顶层目录
  		2、CALLLBACKWINDOW func	:	重绘前一个窗体的回调函数
  		3、GUICHAR *pTitleStr	:	窗体名字
  * 返回：
		无
  * 备注：
***/
int CreateQuickSaveWin(const char *pTopPath, char* pLastPath, CALLLBACKWINDOW func, GUICHAR *pTitleStr, FILETYPE filetype)
{
	int iErr = 0;  
	GUIWINDOW *pWnd = NULL; 	

	if(iErr == 0)
	{
		if( (NULL == pTopPath) ||
			(NULL == func))
		{
			iErr = -1;
		}
	}	

	/* 判断是否挂载了USB */
	if(iErr == 0)
	{
		if( strncmp(pTopPath, MntUsbDirectory, strlen(MntUsbDirectory)) == 0 )
		{
			if(access("/tmp/usb", F_OK) != 0) 
			{
				DialogInit(120, 90, TransString("Warning"),
							TransString("DIALOG_NO_USB"),
							0, 0, func, NULL, NULL);  
				iErr = -2;
			}			
		}
	}	

	if(iErr == 0)
	{
		__g_pQuickSaveTopPath = pTopPath;
        __g_pQuickSaveLastPath = pLastPath;
		strcpy(__g_cFirstPath_Quick, __g_pQuickSaveTopPath);
		_g_QuickSavecallBackFunc = func;
		pQuickSaveStrLabel = pTitleStr;
        __g_fileType = filetype;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmQuickSaveInit, FrmQuickSaveExit, 
							FrmQuickSavePaint, FrmQuickSaveLoop, 
							FrmQuickSavePause, FrmQuickSaveResume,
							NULL);			
		SendWndMsg_WindowExit(GetCurrWindow());  
		SendSysMsg_ThreadCreate(pWnd);			
	}

	return iErr;
}
