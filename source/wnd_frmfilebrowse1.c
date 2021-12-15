/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfilebrowse.c
* 摘    要：  声明窗体wnd_frmfilebrowse(文件浏览)的窗体处理线程及相关操作函数
*			  该窗体提供了文件浏览功能。        
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-9-3
*
*/
/*******************************************************************************/
#include "wnd_frmfilebrowse1.h"
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
#include "app_sorviewer.h"
#include "app_frmfilebrowse.h"
#include "app_parameter.h"

#include "guimyglobal.h"
#include "guiphoto.h"

#include "wnd_frmdialog.h"
#include "wnd_frmimageviewer.h"
#include "wnd_frmotdr.h"
#include "wnd_frmsola.h"
#include "wnd_frmfip.h"
#include "wnd_droppicker.h"
#include "wnd_frmoperatesor.h"
#include "wnd_frmotdrfileopen.h"
#include "wnd_popselector.h"
#include "wnd_frmmenubak.h"

/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
static unsigned int uiFileOprationMode = FILE_NORMAL_MODE;	// 当前的文件操作类型
static int uiCurrFileIndex = 0;								// 当前选中文件的索引
static FILE_SELECT_MODE uiSelectMode = FILE_SINGLE_MODE;    // 文件选择模式
static char *__g_pFileName[MAXPAGEFILENUM]; 				// 存贮一页当中的文件名
static char __g_cFirstFilePath[MAXPATHNAME]= MntUpDirectory;// 首次进入文件浏览器时，要打开的目录
static FILETYPE enCurrentFileType = ANY;			        // 当前系统过滤的文件类型
static unsigned char g_ucOpenFolderFlg = 0;					// 进入新的目录的标志
static FILEBROWSECALLLBACK __pFuncCallBack = NULL;			// 更新用户控件状态的回调函数
static CALLLBACKWINDOW __pFuncReWincallBack = NULL;			// 重绘前一个窗体的回调函数
static CALLLBACKWINDOW1 __pFuncRePaintWincallBack = NULL;   // 重绘前一个窗体的回调函数

extern MULTISELECT *pEnvQueue;
extern PSYSTEMSET pCurSystemSet;
extern POTDR_TOP_SETTINGS pOtdrTopSettings;					//当前OTDR的工作设置
static MULTISELECT *pClipboardQueue = NULL;                 //剪切板队列
static int iSelectedOption = 0;								//默认选中返回上级目录按钮
static int iDisplayFlag = 0;								//是否显示check按钮
static int isClickEnter = 0;								//是否点击Enter键
static int iRefreshFlag = 0;								//是否刷新选择条
static unsigned int s_uiMaxFileNum = MAXPAGEFILENUM;		//当前页面最大显示文件数量

int isInSmallFileBrowse = 1;								//是否在小的浏览器窗口(0:表示不在，1:表示在)
/*******************************************************************************
**						窗体frmfilebrowse中的控件定义部分					  **
*******************************************************************************/
static GUIWINDOW *pFrmFileBrowse = NULL;
/**********************************背景控件定义********************************/ 
static GUIPICTURE *pFileBrowseLeftBg = NULL;		
/**************************显示当前路径的控件定义******************************/
static GUICHAR *pFileBrowseStrPathTitle = NULL;
static GUICHAR *pFileBrowseStrPath = NULL;

static GUILABEL *pFileBrowseLblPathTitle = NULL;    	//显示路径名前缀
static GUILABEL *pFileBrowseLblPath = NULL;   		    //显示路径的标签

static GUIPICTURE *pFileBrowseBtnPath = NULL;   		//显示路径的背景图片
static GUIPICTURE *pFileBrowseBtnUpDir = NULL;			//返回上一级目录
static GUIPICTURE *pFileBrowseBgFileType= NULL;         //显示文件类型的背景图片
/**************************文件显示区域的控件定义******************************/
static GUICHAR *pFileBrowseStrFileNameTitle = NULL;
static GUICHAR *pFileBrowseStrFileTimeTitle = NULL;
static GUICHAR *pFileBrowseStrFileTypeTitle = NULL;
static GUICHAR *pFileBrowseStrCurFileType = NULL;

static GUICHAR *pFileBrowseStr[MAXPAGEFILENUM];
static GUICHAR *pFileBrowseStrTimeInfo[MAXPAGEFILENUM];

static GUILABEL *pFileBrowseLblFileNameTitle = NULL;  	//文件名的Title
static GUILABEL *pFileBrowseLblFileTimeTitle = NULL;  	//文件时间信息的Title
static GUILABEL *pFileBrowseLblFileTypeTitle = NULL;  	//文件类型的Title
static GUILABEL *pFileBrowseLblCurrFileType = NULL;  	 //当前选择的文件类型

static GUILABEL *pFileBrowseLbl[MAXPAGEFILENUM];    	//显示每个文件文件名的LAbel控件
static GUILABEL *pFileBrowseLblTimeInfo[MAXPAGEFILENUM];//显示每个文件时间信息的LAbel控件

static GUIPICTURE *pFileBrowseBtn[MAXPAGEFILENUM];  	//显示每个文件的背景图片
static GUIPICTURE *pFileBrowseBtnIco[MAXPAGEFILENUM];   //显示文件类型的ICO图片
static GUIPICTURE *pFileBrowseBtnCheck[MAXPAGEFILENUM]; //显示在多选模式时该文件是否选中
 
/*******************************************************************************
**	    窗体frmfilebrowse中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/
static int FileBrowseTextRes_Init1(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);
static int FileBrowseTextRes_Exit1(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);

/*******************************************************************************
**			    窗体frmfilebrowse中的控件事件处理函数定义部分				  **
*******************************************************************************/

/***************************窗体的按键事件处理函数*****************************/
/**********************文件浏览区域的事件处理函数******************************/
static int FileBrowseBtnCheck_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int FileBrowseBtnCheck_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);

static int FileBrowseBtnUp_Up(void *pInArg, int iInLen,
							   void *pOutArg, int iOutLen);
static int FileBrowseBtnDown_Up(void *pInArg, int iInLen,
								void *pOutArg, int iOutLen);

static int FileBrowseBtnUpDir_Down(void *pInArg, int iInLen,
								void *pOutArg, int iOutLen);
static int FileBrowseBtnFileType_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);
//static int FileBrowseBtnSelectAll_Down(void *pInArg, int iInLen, 
//                                 void *pOutArg, int iOutLen);

static int FileBrowseBtnUpDir_Up(void *pInArg, int iInLen,
								void *pOutArg, int iOutLen);
static int FileBrowseBtnFileType_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);
//static int FileBrowseBtnSelectAll_Up(void *pInArg, int iInLen, 
//                                 void *pOutArg, int iOutLen);

/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static void PaintMultiSelect(void);
static int UpdateMultiSelectICO(const char *pFileName, int iIndex);
static int UpdateFileRes(void);
static int OpenFolder(const char *pFilePath);
static void DisplayFile(void);
static void FileTypeDropCallBack(int iSelected);
//设置选中项并添加到选中列表中
static void SetSelectedList(void);
/***
  * 功能：
        窗体frmfilebrowse的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1、unsigned int uiPlaceX:		控件的X坐标    
        2、unsigned int uiPlaceY:		控件的Y坐标
		3、unsigned int uiMaxFileNum :	每页最大显示文件个数
        4、CALLLBACKWINDOW func :		重绘前一个窗体
        5、void *pWndObj		:		指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowseInit1(unsigned int uiPlaceX, unsigned int uiPlaceY, unsigned int uiMaxFileNum, 
						CALLLBACKWINDOW func, void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	int j = 0;			
	
    //临时变量定义
    GUIMESSAGE *pMsg = NULL; 

    //得到当前窗体对象
    pFrmFileBrowse = (GUIWINDOW *) pWndObj;
	__pFuncReWincallBack = func;
	s_uiMaxFileNum = uiMaxFileNum;

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    FileBrowseTextRes_Init1(NULL, 0, NULL, 0);
	SetCurFileType(enCurrentFileType);
    if(pClipboardQueue == NULL)
    {
        pClipboardQueue = CreateQueue(MULTISELECT_LIMIT);
    } 
    /* 建立背景图片 */
	pFileBrowseLeftBg = CreatePicture(uiPlaceX, uiPlaceY, 520, 440, BmpFileDirectory"bg_filebrowse1.bmp");
	/* 显示路径名的前缀标题的Label，显示路径: */
	pFileBrowseLblPathTitle = CreateLabel(uiPlaceX+17,  uiPlaceY+24, 80,  16, 
											      pFileBrowseStrPathTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblPathTitle);

	/* 建立显示路径信息的标签和背景图片 */
	pFileBrowseBtnPath = CreatePicture(uiPlaceX+64, uiPlaceY+14, 430, 36, BmpFileDirectory"bg_filebrowse1_pathname.bmp");
	pFileBrowseLblPath  = CreateLabel(uiPlaceX+68, uiPlaceY+24, 300, 16, pFileBrowseStrPath);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblPath);

	/* 建立显示返回上一级目录背景图片 */
	pFileBrowseBtnUpDir = CreatePicture(uiPlaceX + 210, uiPlaceY + 2, 16, 16,
		BmpFileDirectory"bg_filebrowse_updir_press.bmp");

	/* 显示文件名标题的Label */
	pFileBrowseLblFileNameTitle = CreateLabel(uiPlaceX+97, uiPlaceY+74, 100, 16, 
											      pFileBrowseStrFileNameTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblFileNameTitle);

	/* 显示日期标题的Label */
	pFileBrowseLblFileTimeTitle = CreateLabel(uiPlaceX+327, uiPlaceY+74, 100, 16, 
											      pFileBrowseStrFileTimeTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblFileTimeTitle);
                                                      
	/* 显示文件类型标题的Label */
	pFileBrowseLblFileTypeTitle = CreateLabel(uiPlaceX+10,  uiPlaceY+52, 100,  16, 
											      pFileBrowseStrFileTypeTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblFileTypeTitle);

	/* 显示当前文件类型的背景图片 */
	pFileBrowseBgFileType = CreatePicture(uiPlaceX + 35, uiPlaceY + 114, 411, 33,
										  BmpFileDirectory "bg_filebrowse1_filetype.bmp");

	/* 显示当前文件类型的Label */
    pFileBrowseLblCurrFileType= CreateLabel(uiPlaceX+100,  uiPlaceY+46+9, 80,  16, 
											      pFileBrowseStrCurFileType);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblCurrFileType);

	//文件列表按钮
    for(i = 0; i < s_uiMaxFileNum; ++i)
    {
		pFileBrowseBtn[i] = CreatePicture(uiPlaceX, uiPlaceY + 104 + 42 * i, 470, 41,
										  BmpFileDirectory"btn_filebrowse1_file_unselect.bmp");
	}
	/* 建立显示文件类型的Picture */
	for(i=0,j=uiPlaceY+114; i<s_uiMaxFileNum; i++,j+=42)
	{
		pFileBrowseBtnIco[i] = CreatePicture(uiPlaceX+35, j, 20, 20, NULL);
	}	

	/* 建立显示文件名的Label */
	for(i=0,j=uiPlaceY+116; i<s_uiMaxFileNum; i++,j+=42)
	{
		pFileBrowseLbl[i] = CreateLabel(uiPlaceX+66, j, 200, 16, 
										    pFileBrowseStr[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLbl[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pFileBrowseLbl[i]);
	}
	
	/* 建立显示文件时间信息的Label */
	for(i=0,j=uiPlaceY+116; i<s_uiMaxFileNum; i++,j+=42)
	{
		pFileBrowseLblTimeInfo[i] = CreateLabel(uiPlaceX+299, j, 110, 16, 
												    pFileBrowseStrTimeInfo[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblTimeInfo[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pFileBrowseLblTimeInfo[i]);
	}

	//建立在多选模式时该文件是否选中的图标
    for(i = 0; i < s_uiMaxFileNum; ++i)
    {
        pFileBrowseBtnCheck[i] = CreatePicture(uiPlaceX+472, uiPlaceY+114+42*i, 20, 20, NULL);
    }
    
    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileBrowseBtnUpDir,
		pFrmFileBrowse);

    //注册桌面上的控件
	for(i=0; i<s_uiMaxFileNum; i++)
	{	
	    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileBrowseBtn[i], 
	                  pFrmFileBrowse);	
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileBrowseBtnCheck[i], 
	                  pFrmFileBrowse);	
	}
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileBrowseBgFileType, 
                  pFrmFileBrowse);
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();

    //注册桌面上控件的消息处理

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileBrowseBtnUpDir,
		FileBrowseBtnUpDir_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFileBrowseBtnUpDir,
		FileBrowseBtnUpDir_Up, NULL, 0, pMsg);

	for(i=0; i<s_uiMaxFileNum; i++)
	{	
	    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileBrowseBtn[i], 
	                    FileBrowseBtnCheck_Down, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pFileBrowseBtn[i], 
	                    FileBrowseBtnCheck_Up, NULL, i, pMsg);		
	}		

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileBrowseBgFileType, 
                    FileBrowseBtnFileType_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFileBrowseBgFileType, 
                    FileBrowseBtnFileType_Up, NULL, 0, pMsg);

	//初始化或重新刷新界面时，重置多选标志位
	isClickEnter = 0;
    
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
int FrmFileBrowseExit1(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	int i;

    //得到当前窗体对象
    pFrmFileBrowse = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmFileBrowse);
    //销毁桌面上的控件
	DestroyLabel(&pFileBrowseLblPathTitle);
	DestroyLabel(&pFileBrowseLblFileNameTitle);
	DestroyLabel(&pFileBrowseLblFileTimeTitle);
    DestroyLabel(&pFileBrowseLblFileTypeTitle);
	
    DestroyPicture(&pFileBrowseLeftBg);
	
	DestroyPicture(&pFileBrowseBtnUpDir);
    DestroyPicture(&pFileBrowseBtnPath);
	DestroyLabel(&pFileBrowseLblPath);

    DestroyPicture(&pFileBrowseBgFileType);
    DestroyLabel(&pFileBrowseLblCurrFileType);
    
    for(i=0; i<s_uiMaxFileNum; i++)
	{
		DestroyLabel(&pFileBrowseLbl[i]);
        DestroyLabel(&pFileBrowseLblTimeInfo[i]);
	}
	
    for(i=0; i<s_uiMaxFileNum; i++)
	{
		DestroyPicture(&pFileBrowseBtn[i]);
        DestroyPicture(&pFileBrowseBtnIco[i]);
        DestroyPicture(&pFileBrowseBtnCheck[i]);
	}
	
    //释放文本资源
    //***************************************************************/
    FileBrowseTextRes_Exit1(NULL, 0, NULL, 0);
	uiCurrFileIndex = 0;
	iSelectedOption = 0;
	
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
int FrmFileBrowsePaint1(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    //显示桌面背景
	DisplayPicture(pFileBrowseLeftBg);
	// DisplayPicture(pFileBrowseBtnUpDir);

    // DisplayLabel(pFileBrowseLblFileTypeTitle);
    // DisplayPicture(pFileBrowseBgFileType);	
    // DisplayLabel(pFileBrowseLblCurrFileType);
	/* 显示标题栏名字、路径名标题、文件名和日期、文件大小、文件类型 */
	DisplayLabel(pFileBrowseLblPathTitle);
	DisplayLabel(pFileBrowseLblFileNameTitle);
	DisplayLabel(pFileBrowseLblFileTimeTitle);

	//显示文件内容 */
	OpenFileAndFolder(__g_cFirstFilePath);
	if (!iRefreshFlag)	
		SetPowerEnable(1, 1);

    return iReturn;
}


/*******************************************************************************
**	    窗体frmfilebrowse中的初始化文本资源、 释放文本资源函数定义部分		  **
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
static int FileBrowseTextRes_Init1(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//桌面上设置框的文本
	pFileBrowseStrPathTitle     = GetCurrLanguageText(FILE_LBL_PATH);
    
	pFileBrowseStrFileNameTitle = GetCurrLanguageText(FILE_LBL_FILE_NAME);
	pFileBrowseStrFileTimeTitle = GetCurrLanguageText(FILE_LBL_DATE);
    pFileBrowseStrFileTypeTitle = GetCurrLanguageText(FILE_LBL_FILE_TYPE);

    char* pFileType[FILETYPENUM] = {"ALL", "SOR", "BMP"};
    pFileBrowseStrCurFileType = TransString(pFileType[enCurrentFileType]);
 	
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
static int FileBrowseTextRes_Exit1(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i;
	
	//桌面上设置框的文本
	GuiMemFree(pFileBrowseStrPathTitle);
	GuiMemFree(pFileBrowseStrFileNameTitle);
	GuiMemFree(pFileBrowseStrFileTimeTitle);
    GuiMemFree(pFileBrowseStrFileTypeTitle);

    GuiMemFree(pFileBrowseStrCurFileType);
	
    //释放桌面上的文本
	for(i=0; i<s_uiMaxFileNum; i++)
	{
		GuiMemFree(__g_pFileName[i]);
	} 
	
    return iReturn;
}

/*******************************************************************************
**			    			窗体的按键事件处理函数				 		 	  **
*******************************************************************************/
/***
  * 功能：
		文件浏览器窗体按键的down事件
  * 参数：
		1.int iValue:	按键的键值
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FileBrowseKeyDown(unsigned int uiValue)
{
	int iErr = 0;

	switch (uiValue)
    {
	case KEYCODE_UP:
		LOG(LOG_INFO, "----up-----\n");
		break;
	case KEYCODE_DOWN:
		LOG(LOG_INFO, "----down----\n");
		break;
	case KEYCODE_RIGHT:
		LOG(LOG_INFO, "----right----\n");
		break;
	case KEYCODE_ENTER:
		LOG(LOG_INFO, "-----enter-----\n");
		break;
	case KEYCODE_ESC:
		FileBrowseBtnCheck_Down(NULL, 0, NULL, 0);
		break;
	default:
		break;
    }

	return iErr;
}

/***
  * 功能：
		文件浏览器窗体按键的up事件
  * 参数：
		1.int iValue:	按键的键值
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FileBrowseKeyUp(unsigned int uiValue)
{
	int iErr = 0;

	switch (uiValue)
    {
    case KEYCODE_UP:
		LOG(LOG_INFO, "----up---1--\n");
		break;
    case KEYCODE_DOWN:
		LOG(LOG_INFO, "----down--1--\n");
		break;
	case KEYCODE_RIGHT:
		LOG(LOG_INFO, "----right--1--\n");
		break;
    case KEYCODE_ENTER:
		LOG(LOG_INFO, "-----enter--1---\n");
		break;
	case KEYCODE_HOME:
		LOG(LOG_INFO, "----test up--1--\n");
		break;
	case KEYCODE_START:
		break;
	case KEYCODE_ESC:
		FileBrowseBtnCheck_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
    }

	return iErr;	
}

/*******************************************************************************
**						文件浏览区域的事件处理函数							  **
*******************************************************************************/

/***
  * 功能：
		多选按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnCheck_Down(void *pInArg,   int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    if (GetCurFileNum() <= 0)
        return -1;

    int iCurrTouchIndex = iOutLen;
    
    if (iCurrTouchIndex < GetActulPageFileNum())
    {
        uiCurrFileIndex = iCurrTouchIndex;
		if (isClickEnter)
		{
			PaintMultiSelect();
		}
    	DisplayFile();
    	SyncCurrFbmap();
    }

    return iReturn;
}

//获取当前选中文件的索引
int GetCurrFileIndex(void)
{
    return uiCurrFileIndex;
}

//设置文件类型的使能
int SetFileTypeEnabled(int iEnabled)
{
	// if (iEnabled)
	// {
	// 	SetPictureEnable(1, pFileBrowseBgFileType);
	// 	DisplayPicture(pFileBrowseBgFileType);
	// 	DisplayLabel(pFileBrowseLblCurrFileType);
	// 	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileBrowseLblCurrFileType);
	// }
	// else
	// {
	// 	DisplayPicture(pFileBrowseBgFileType);
	// 	SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFileBrowseLblCurrFileType);
	// 	DisplayLabel(pFileBrowseLblCurrFileType);
	// 	SetPictureEnable(0, pFileBrowseBgFileType);
	// }

	return 0;
}

//设置当前文件类型的文本
void SetCurrentFileTypeLabel(char * fileType)
{
	if (fileType)
	{
		pFileBrowseStrCurFileType = TransString(fileType);
		SetLabelText(pFileBrowseStrCurFileType, pFileBrowseLblCurrFileType);
	}
}
/***
  * 功能：
		选中文件up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnCheck_Up(void *pInArg,	 int iInLen, 
							     void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iErr = 0;
	
	return iErr;
}

/***
  * 功能：
		向上一个条目按钮的up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnUp_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	PageUp1(&uiCurrFileIndex, s_uiMaxFileNum);
	UpdateFileRes();
	DisplayFile();
	SyncCurrFbmap();

	return iReturn;
}


/***
  * 功能：
		向下一个条目按钮的up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnDown_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	PageDown1(&uiCurrFileIndex, s_uiMaxFileNum);
	UpdateFileRes();
	DisplayFile();
	SyncCurrFbmap();

	return iReturn;
}

#if 0
/***
  * 功能：
		按照文件名字排序按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnNameSort_Down(void *pInArg,  int iInLen, 
                                 		void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	switch(g_emCurSortMethod)
	{
	/* 文件名升序排列 */
	case NAMEASEC:
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_nameasec_press.bmp", pFileBrowseBtnNameSort);				
		break;
		
	/* 文件名降序排列 */	
	case NAMEDESC:
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_namedesc_press.bmp", pFileBrowseBtnNameSort);						
		break;	

	/* 文件内容修改时间升序排列 */	
	case TIMEASEC:
	/* 文件内容修改时间降序排列 */	
	case TIMEDESC:
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_namenone_press.bmp", pFileBrowseBtnNameSort);					
		break;	
	default:
		break;				
	}

	//DisplayPicture(pFileBrowseBtnNameSort);
	DisplayLabel(pFileBrowseLblFileNameTitle);
	SyncCurrFbmap();

    return iReturn;
}


/***
  * 功能：
		按照文件内容修改时间排序按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnTimeSort_Down(void *pInArg,  int iInLen, 
                                 		void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	switch(g_emCurSortMethod)
	{
	/* 文件名升序排列 */
	case NAMEASEC:
	/* 文件名降序排列 */	
	case NAMEDESC:
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timenone_press.bmp", pFileBrowseBtnTimeSort);						
		break;	

	/* 文件内容修改时间升序排列 */	
	case TIMEASEC:
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timeasec_press.bmp", pFileBrowseBtnTimeSort);				
		break;	
	/* 文件内容修改时间降序排列 */	
	case TIMEDESC:
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timedesc_press.bmp", pFileBrowseBtnTimeSort);							
		break;	
	default:
		break;				
	}

	//DisplayPicture(pFileBrowseBtnTimeSort);
	DisplayLabel(pFileBrowseLblFileTimeTitle);
	SyncCurrFbmap();

    return iReturn;
}


/***
  * 功能：
		按照文件名字排序按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnNameSort_Up(void *pInArg,  int iInLen, 
                                 		void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 排序按钮 */
	switch(g_emCurSortMethod)
	{
	/* 文件名升序排列 */
	case NAMEASEC:
		g_emCurSortMethod = NAMEDESC;
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_namedesc_unpress.bmp", pFileBrowseBtnNameSort);
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timenone_unpress.bmp", pFileBrowseBtnTimeSort);
		break;
		
	/* 文件名降序排列 */	
	case NAMEDESC:
		g_emCurSortMethod = NAMEASEC;
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_nameasec_unpress.bmp", pFileBrowseBtnNameSort);
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timenone_unpress.bmp", pFileBrowseBtnTimeSort);			
		break;	

	/* 文件内容修改时间升序排列 */	
	case TIMEASEC:
	/* 文件内容修改时间降序排列 */	
	case TIMEDESC:
		g_emCurSortMethod = NAMEASEC;
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_nameasec_unpress.bmp", pFileBrowseBtnNameSort);
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timenone_unpress.bmp", pFileBrowseBtnTimeSort);						
		break;	
	default:
		break;				
	}

	//DisplayPicture(pFileBrowseBtnNameSort);
	//DisplayPicture(pFileBrowseBtnTimeSort);
    if(iDisplayFlag)
    {
        DisplayPicture(pFileBrowseBtnFileType);
        DisplayPicture(pFileBrowseBtnSelectAll);
    }
    
    DisplayLabel(pFileBrowseLblFileTypeTitle);
	DisplayLabel(pFileBrowseLblFileNameTitle);
	DisplayLabel(pFileBrowseLblFileTimeTitle);	

	/* 提示 文件打开中 */
	//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);
	
	SetCurSortMethod(g_emCurSortMethod);
	uiCurrFileIndex = 0;
	UpdateFileRes();		
	DisplayFile();
	
	DisableInfoDialog(pFrmFileBrowse);
	SyncCurrFbmap();

    return iReturn;	
}


/***
  * 功能：
		按照文件内容修改时间排序按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnTimeSort_Up(void *pInArg,  int iInLen, 
                                 		void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 排序按钮 */
	switch(g_emCurSortMethod)
	{
	/* 文件名升序排列 */
	case NAMEASEC:
	/* 文件名降序排列 */	
	case NAMEDESC:
		g_emCurSortMethod = TIMEASEC;
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_namenone_unpress.bmp", pFileBrowseBtnNameSort);
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timeasec_unpress.bmp", pFileBrowseBtnTimeSort);					
		break;	

	/* 文件内容修改时间升序排列 */	
	case TIMEASEC:
		g_emCurSortMethod = TIMEDESC;
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_namenone_unpress.bmp", pFileBrowseBtnNameSort);
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timedesc_unpress.bmp", pFileBrowseBtnTimeSort);						
		break;	

	/* 文件内容修改时间降序排列 */	
	case TIMEDESC:
		g_emCurSortMethod = TIMEASEC;
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_namenone_unpress.bmp", pFileBrowseBtnNameSort);
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse_timeasec_unpress.bmp", pFileBrowseBtnTimeSort);						
		break;	
	default:
		break;				
	}		

	//DisplayPicture(pFileBrowseBtnNameSort);
	//DisplayPicture(pFileBrowseBtnTimeSort);
    if(iDisplayFlag)
    {
        DisplayPicture(pFileBrowseBtnFileType);
        DisplayPicture(pFileBrowseBtnSelectAll);
        //DisplayLabel(pFileBrowseLblFileTypeTitle);
    }
	DisplayLabel(pFileBrowseLblFileNameTitle);
	DisplayLabel(pFileBrowseLblFileTimeTitle);	
	/* 提示 文件打开中 */
	//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);

	SetCurSortMethod(g_emCurSortMethod);
	uiCurrFileIndex = 0;
	UpdateFileRes();	
	DisplayFile();

	DisableInfoDialog(pFrmFileBrowse);
	SyncCurrFbmap();

    return iReturn;	
}
#endif

/***
  * 功能：
		返回上一级目录down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnUpDir_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/***
  * 功能：
		文件类型down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnFileType_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	// TouchChange("bg_filebrowse_updir_unpress.bmp", pFileBrowseBtnUpDir,
	// 	NULL, NULL, 0);

	// RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}

/***
  * 功能：
		返回上一级目录up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FileBrowseBtnUpDir_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
// 	TouchChange("bg_filebrowse_updir_press.bmp", pFileBrowseBtnUpDir,
// 		NULL, NULL, 0);
// 	RefreshScreen(__FILE__, __func__, __LINE__);

// 	if (IsUpDir())
// 	{
// 		strcpy(__g_cFirstFilePath, MntUpDirectory);

// 		GUIWINDOW *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
// 			FrmMainInit, FrmMainExit,
// 			FrmMainPaint, FrmMainLoop,
// 			FrmMainPause, FrmMainResume,
// 			NULL);
// 		SendWndMsg_WindowExit(pFrmFileBrowse);
// 		SendSysMsg_ThreadCreate(pWnd);
// 	}
// 	else
// 	{
// 		ClearQueue(pEnvQueue);
// 		OpenFileAndFolder("..");
// // 		PaintMenu(uiFileOprationMode);
// 		SyncCurrFbmap();
// 	}

	return 0;
}

/***
  * 功能：
		选择文件类型up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FileBrowseBtnFileType_Up(void *pInArg, int iInLen, 
                                    void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int count = FILETYPENUM;
    static char *pFileType[FILETYPENUM] = {
        "ALL", "SOR", "BMP"
    };
    static int enable[FILETYPENUM] = {1, 1, 1};

    //解决下拉列表出现时插拔usb产生界面重叠问题 	
    SendWndMsg_LoopDisable(pFrmFileBrowse);

	//DispTransparent(80, 0x0, 0, 0, 245, WINDOW_HEIGHT);
	CreateDropPicker1(105, 94, count, pFileType, NULL, 
					  enable, enCurrentFileType, FileTypeDropCallBack, pFrmFileBrowse);	
    
    return iReturn;
}

//设置文件类型
void SelectFileType(void)
{
	int count = FILETYPENUM;
	static char *pFileType[FILETYPENUM] = {
		"ALL", "SOR", "BMP"};
	//解决下拉列表出现时插拔usb产生界面重叠问题
	SendWndMsg_LoopDisable(pFrmFileBrowse);
	DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	CreatePopSelector(120, count, pFileType, NULL,
					  enCurrentFileType, FileTypeDropCallBack, pFrmFileBrowse);
}

//选择所有文件/文件夹,成功返回0，失败（顶层目录）返回-1
int SelectAllFile(void)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
    if (IsUpDir())
    {
		return -1;
    }   

	//判断是否已经全部选中，如果已经全部选中则清除，如果没有则全部选中，更新显示。
	if (0 == IsFullSelect())
	{
		ClearQueue(pEnvQueue);
	}
	else
	{
		FullSelectQueue();
		ResetOpenFolderFlag();
	}
    
	UpdateFullSelect();

    return iReturn;
}

//static int FileBrowseBtnSelectAll_Down(void *pInArg, int iInLen, 
//                                 void *pOutArg, int iOutLen)
//{
//    //错误标志、返回值定义
//    int iReturn = 0;
//
//    iReturn = SelectAllFile();
//    if(iReturn)
//    {
//        printf("already top directory\n");
//    }
//
//    return iReturn;
//}
//
//static int FileBrowseBtnSelectAll_Up(void *pInArg, int iInLen, 
//                                 void *pOutArg, int iOutLen)
//{
//    //错误标志、返回值定义
//    int iReturn = 0;
//    return iReturn;
//}

/***
  * 功能：
		选择文件类型后的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void FileTypeDropCallBack(int iSelected) 
{
	enCurrentFileType = iSelected;
	SetCurFileType(enCurrentFileType);
    char* pFileType[FILETYPENUM] = {"ALL", "SOR", "BMP"};
    pFileBrowseStrCurFileType = TransString(pFileType[enCurrentFileType]);
    SetLabelText(pFileBrowseStrCurFileType, pFileBrowseLblCurrFileType);
	pOtdrTopSettings->pUser_Setting->sFileNameSetting.enFilter = enCurrentFileType;	

    char currentPath[512] = {0};
    getcwd(currentPath, 512);
    SetFirstOpenFolder(currentPath);
    ClearQueue(pEnvQueue);
	(*__pFuncRePaintWincallBack)(pFrmFileBrowse);
    SendWndMsg_LoopEnable(pFrmFileBrowse);	
}

//处理向上翻页的函数
static void HandlePageUp(void)
{
	if (isInSmallFileBrowse)
	{
		isClickEnter = 0;

		if ((iSelectedOption == 0) && (GetCurFilePos() > s_uiMaxFileNum))
		{
			//向上翻页
			FileBrowseBtnUp_Up(NULL, 0, NULL, 0);
			iSelectedOption = s_uiMaxFileNum-1;//跳转到列表末位置
		}
		else
		{
			//文件列表向上递进
			if (iSelectedOption != 0)
				iSelectedOption--;
			//刷新选中的文件
			if (iSelectedOption >= 0)
				FileBrowseBtnCheck_Down(NULL, 0, NULL, iSelectedOption % s_uiMaxFileNum);
		}
	}
}

//处理向下翻页的函数
static void HandlePageDown(void)
{
	if (isInSmallFileBrowse)
	{
		iSelectedOption++;
		isClickEnter = 0;

		if ((iSelectedOption / s_uiMaxFileNum))
		{
			//向下翻页
			FileBrowseBtnDown_Up(NULL, 0, NULL, 0);
			iSelectedOption = 0;//回到列表首位置
		}
		else
		{
			if (iSelectedOption < GetActulPageFileNum())
				//刷新选中的文件
				FileBrowseBtnCheck_Down(NULL, 0, NULL, iSelectedOption % s_uiMaxFileNum);
			else
				iSelectedOption--;
		}
	}
}

//处理enter键函数
static void HandleEnterKey(void)
{
	if (isInSmallFileBrowse)
	{
        if (GetCurFileNum() <= 0)
            return;
        isClickEnter = 1;
        if (iSelectedOption < GetActulPageFileNum())
        {
            uiCurrFileIndex = iSelectedOption;
            OpenAtIndex(iSelectedOption);
        }
    }
}

//设置选中项并添加到选中列表中
static void SetSelectedList(void)
{
    if (g_ucOpenFolderFlg)
    {
        g_ucOpenFolderFlg = 0;
        ClearQueue(pEnvQueue);
    }

    if (GetCurFileNum() <= 0)
        return;
    char currPath[MAXPATHNAME];
    getcwd(currPath, MAXPATHNAME);
    int iLen = strlen(currPath);
    iLen += strlen(__g_pFileName[uiCurrFileIndex]);
    char *pCurFileName = (char *)GuiMemAlloc(iLen + 2);
    if (NULL != pCurFileName)
    {
        sprintf(pCurFileName, "%s/%s", currPath, __g_pFileName[uiCurrFileIndex]);
        //非 sdcard && usb && .. 目录
        if (strcmp(pCurFileName, MntSDcardDataDirectory) &&
            strcmp(pCurFileName, MntUsbDirectory) &&
            strcmp("..", __g_pFileName[uiCurrFileIndex]))
        {
            //只有单选的时候才有效
            if (IsFullSelect())
            {
                ClearQueue(pEnvQueue);
                WriteQueue(pCurFileName, pEnvQueue);
            }
        }
        GuiMemFree(pCurFileName);
    }
}

//按键响应回调函数
void KeyBoardCallBack1(int iSelected)
{
	switch (iSelected)
	{
	case KEYCODE_UP:
		HandlePageUp();
		break;
	case KEYCODE_DOWN:
		HandlePageDown();
		break;
	case KEYCODE_RIGHT:
		isInSmallFileBrowse = 0;
        uiCurrFileIndex = iSelectedOption;
        //将选中项添加到选中列表中
        SetSelectedList();
        break;
	case KEYCODE_ENTER:
		HandleEnterKey();
		break;
	default:
		break;
	}
}

//设置背景选中
void RefreshFileBrowseSelectedFile(void)
{
	// iEnable
	// 	? SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_select.bmp", pFileBrowseLeftBg)
	// 	: SetPictureBitmap(BmpFileDirectory "bg_filebrowse1.bmp", pFileBrowseLeftBg);
	//重新刷新界面
	//GetCurrWindow()->fnWndPaint(GetCurrWindow());
	char currentPath[512] = {0};
	getcwd(currentPath, 512);
	SetFirstOpenFolder(currentPath);
	iRefreshFlag = 1;
	FrmFileBrowsePaint1(pFrmFileBrowse);
	iRefreshFlag = 0;
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
int __SetCurFileType(FILETYPE enFilter)
{
	int iErr = 0;

	if(enFilter > NONE || enFilter < SOR)
	{
		enCurrentFileType = ANY;
	}
	else
	{
		enCurrentFileType = enFilter;
	}
	
	return iErr;
}
//设置选中文件状态以及全选按钮的状态
static void SetFileCheckAndSelectAll(char* pCurFileName)
{
    int iReturn = 0;
    iReturn = CheckSpecialQueue(pCurFileName, pEnvQueue);
	/* 链表中不存在该项 */
	if(iReturn)
	{
	    if(uiSelectMode == FILE_SINGLE_MODE)
	    {
	        int i;
		    int iActualPageFileNum = GetActulPageFileNum();
            ClearQueue(pEnvQueue);
            for (i = 0; i < iActualPageFileNum; i++)
            {
				SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_multi_unselect.bmp",
								 pFileBrowseBtnCheck[i]);
			}
	    }
	    
		WriteQueue(pCurFileName, pEnvQueue);
		if (iDisplayFlag)
		{
			SetPictureBitmap(BmpFileDirectory"bg_filebrowse1_multi_select.bmp",
							 pFileBrowseBtnCheck[uiCurrFileIndex]);
		}
	}
	/* 链表中已经存在该项 */
	else
	{
		ClearSpecialQueue(pCurFileName, pEnvQueue);
		SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_multi_unselect.bmp",
						 pFileBrowseBtnCheck[uiCurrFileIndex]);
	}
}

/***
  * 功能：
        执行多选操作
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void PaintMultiSelect(void)
{
	if (g_ucOpenFolderFlg)
	{
		int i = 0;
		int iActualPageFileNum = GetActulPageFileNum();
		g_ucOpenFolderFlg = 0;
		ClearQueue(pEnvQueue);

        for (i = 0; i < iActualPageFileNum; ++i)
        {
			SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_multi_unselect.bmp",
							 pFileBrowseBtnCheck[i]);
		};
        for (i = iActualPageFileNum; i < s_uiMaxFileNum; ++i)
        {
			SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_multi_unselect.bmp",
							 pFileBrowseBtnCheck[i]);
		}
	}
    char currPath[MAXPATHNAME];
    getcwd(currPath, MAXPATHNAME);
    int iLen = strlen(currPath);
    iLen += strlen(__g_pFileName[uiCurrFileIndex]);
	char* pCurFileName = (char *)GuiMemAlloc(iLen + 2);
	if (NULL != pCurFileName)
	{
		sprintf(pCurFileName, "%s/%s", currPath, __g_pFileName[uiCurrFileIndex]);

		//非 sdcard && usb && .. 目录
		if (strcmp(pCurFileName, MntSDcardDataDirectory) && 
            strcmp(pCurFileName, MntUsbDirectory) &&
            strcmp("..", __g_pFileName[uiCurrFileIndex]))
		{
		    SetFileCheckAndSelectAll(pCurFileName);
		}	
        GuiMemFree(pCurFileName);
    }

    if (NULL != __pFuncCallBack)
	{
		(*__pFuncCallBack)();
	}
}

static int ReadFile(const char *pFileName)
{
    int iErr = 0;

	if (NULL == pFileName)
		return -1;

	char pFilePath[MAXPATHNAME] = "";
	getcwd(pFilePath, MAXPATHNAME);
    char cPathBuff[MAXPATHNAME] = {0};
    strcpy(cPathBuff, pFilePath);
	__SetOpenFolder(pFilePath);
    
	strcat(pFilePath, "/");
	strcat(pFilePath, pFileName);

	switch(GetFileExtension(pFileName))
	{
	//显示BMP和JPG图片	
	case BMP:
	case JPG:		
		//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);
	    ImageViewer(pFilePath, pFrmFileBrowse, __pFuncReWincallBack);
		SetImageViewerSort(GetCurSortMethod());
// 		DisableInfoDialog(pFrmFileBrowse);
		break;
        
	case SOR:
//     case CUR:
		//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);
		iErr = SorViewer(pFileName, pFrmFileBrowse, __pFuncReWincallBack);
// 		DisableInfoDialog(pFrmFileBrowse);
        if (iErr) {
            DisplayFile();
            SyncCurrFbmap();
            setOtdrOperateFirstPath(cPathBuff);
            //打开文件失败的弹框
            DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
			0, 0, __pFuncReWincallBack, NULL, NULL);
        }
		break;
        
   case SOLA:
		//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);
		iErr = SolaViewer(pFilePath, pFrmFileBrowse, __pFuncReWincallBack);
// 		DisableInfoDialog(pFrmFileBrowse);        
        if (iErr) {
            DisplayFile();
            SyncCurrFbmap();
            setOperateFirstPath(cPathBuff);
            //打开文件失败的弹框
            DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
			0, 0, __pFuncReWincallBack, NULL, NULL);
        }
        break;
        
   case GDM:
		//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);
		iErr = FiberViewer(pFilePath, pFrmFileBrowse, __pFuncReWincallBack);
// 		DisableInfoDialog(pFrmFileBrowse);        
        if (iErr) {
            DisplayFile();
            SyncCurrFbmap();
            setOperateFirstPath(cPathBuff);
            //打开文件失败的弹框
            DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
			0, 0, __pFuncReWincallBack, NULL, NULL);
        }
        break;
		
	//其他类型文件弹出对话框提示不支持
   default:
	   //打开文件失败的弹框
	   DialogInit(120, 90, TransString("Warning"),
		   TransString("The system is not support this file type!"),
		   0, 0, __pFuncReWincallBack, NULL, NULL);
		break;					
	}

	return 0;
}

void OpenAtIndex(unsigned int index)
{
    if (__g_pFileName[index] == NULL)
        return;
    OpenFileAndFolder(__g_pFileName[index]);
    SyncCurrFbmap();
}


/***
  * 功能：
        打开目录或者系统支持的文件
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		此函数不刷新帧缓冲
***/
int OpenFileAndFolder(const char *pFileName)
{
	int iErr = 0;
	int iReturn = 0;
	struct stat statbuf;	
    char fileName[MAXPATHNAME]="";
     
	if(iErr == 0)
	{
		if(NULL == pFileName)
		{
			iErr = -1;
		}
	}
    
    strncpy(fileName,pFileName,strlen(pFileName));

	if(iErr == 0)
	{
		//目录不存在则创建     
		if(access(fileName, F_OK) != 0)
		{
		    if (0 == strncmp(fileName, MntUsbDirectory, strlen(MntUsbDirectory)) && CheckMountUSB())
            {
                strcpy(fileName, MntUpDirectory);
            }
            else 
            {
                char strBuff[1024] = {0};
			    sprintf(strBuff, "mkdir -p -- \"%s\"", fileName);
			    mysystem(strBuff);
            }
		}
		/* 获得将要打开的文件的详细信息 */
		iReturn = lstat(fileName, &statbuf);
		if (iReturn == 0)
		{			
			/* 是目录 */
			if( S_ISDIR(statbuf.st_mode) )			
			{	
				/* 提示 文件打开中 */
				//ShowInfoDialog(TransString("DIALOG_OPENING"), pFrmFileBrowse);
			    if (!iRefreshFlag)
					OpenFolder(fileName);
				DisplayFile();	
				//DisableInfoDialog(pFrmFileBrowse);
			}
			else if (uiFileOprationMode != FILE_SAVE_MODE) 
            {
				ReadFile(fileName);
			}
		}
		else
		{
			perror("error");
		}
	}

	return iErr;
}


/***
  * 功能：
        刷新文件浏览器界面
  * 参数：
        无
  * 返回：
        无
  * 备注：	
***/
static void DisplayFile(void)
{
	int i = 0;

	/* 没有任何文件，则不显示选中的条目 */
	for (i = 0; i < s_uiMaxFileNum; i++)
	{
		SetPictureBitmap(BmpFileDirectory"btn_filebrowse1_file_unselect.bmp", pFileBrowseBtn[i]);
	}

	if (GetCurFileNum() > 0)
	{
		if (getSelectedMenu()) //设置左侧文件列表选中项(光标焦点不在)
			SetPictureBitmap(BmpFileDirectory "btn_filebrowse1_file_select1.bmp", pFileBrowseBtn[uiCurrFileIndex]);
		else //设置左侧文件列表选中项(光标焦点)
			SetPictureBitmap(BmpFileDirectory "btn_filebrowse1_file_select.bmp", pFileBrowseBtn[uiCurrFileIndex]);
	}
	
	/* 更新路径名 */
	DisplayPicture(pFileBrowseBtnPath);
	DisplayLabel(pFileBrowseLblPathTitle);
	DisplayLabel(pFileBrowseLblPath);
	
	/* 更新每个文件条目 */
	for (i = 0; i < s_uiMaxFileNum; i++)
	{
		DisplayPicture(pFileBrowseBtn[i]);
		DisplayPicture(pFileBrowseBtnIco[i]);
		DisplayLabel(pFileBrowseLbl[i]);
		DisplayLabel(pFileBrowseLblTimeInfo[i]);
		DisplayPicture(pFileBrowseBtnCheck[i]);
	}
}


/***
  * 功能：
        进入新的目录或向上向下翻页时更新多选图标资源
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateMultiSelectICO(const char *pFileName, int iIndex)
{
	int iErr = 0;

    if (NULL == pFileName)
        return -1;

    char currPath[MAXPATHNAME];
    getcwd(currPath, MAXPATHNAME);
	int iLen = strlen(currPath) + strlen(pFileName);
    
	char* filePath = (char *)GuiMemAlloc(iLen + 2);	
	if (NULL != filePath)
	{
		sprintf(filePath, "%s/%s", currPath, pFileName);
		int iReturn = CheckSpecialQueue(filePath, pEnvQueue);
		//链表中不存在该项
		if (iReturn)
		{
			SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_multi_unselect.bmp",
							 pFileBrowseBtnCheck[iIndex]);
		}
	    //链表中已经存在该项
		else
		{
			if (iDisplayFlag)
			{
				SetPictureBitmap(BmpFileDirectory"bg_filebrowse1_multi_select.bmp",
								 pFileBrowseBtnCheck[iIndex]);
			}
		}
        GuiMemFree(filePath);
	}
    
    return iErr;	
}

static void SetFileIco(FILETYPE enExtension, char* pIcoPicPath)
{
    char* icon[] = {
        BmpFileDirectory"ico_filebrowse_sor.bmp",
        BmpFileDirectory"ico_filebrowse_picture.bmp",
        BmpFileDirectory"ico_filebrowse_pdf.bmp",
        BmpFileDirectory"ico_filebrowse_other.bmp"
    };
    if(enExtension == SOR)
    {
        strcpy(pIcoPicPath, icon[0]);
    }
    else if(enExtension == BMP || enExtension == JPG)
    {
        strcpy(pIcoPicPath, icon[1]);
    }
	else if(enExtension == PDF)
	{
		strcpy(pIcoPicPath, icon[2]);
	}
    else
    {
        strcpy(pIcoPicPath, icon[3]);
    }
}

/***
  * 功能：
        生成文件大小字符串，加上单位的最大长度为6字节
  * 参数：
        无
  * 返回：
        
  * 备注：
***/
static void FileSize2Str(long size, char* str)
{
    static char* UnitArry[4] = { "B", "KB", "MB", "GB" };
    static int coff[2] = { 10, 100 };
    char buf[16] = { 0 };
    float tmp = 0;
    float result = size;
    int i = 0;
    int len = 0;
    int decimal = 0;

    for (i = 0; i < 4; i++)
    {
        tmp = result / 1024;
        if (tmp < 1.0)
        {
            break;
        }
        result = tmp;
    }
    if (4==i)
    {
        i = 3;
    }
    len = sprintf(str, "%d", (int)result);
    if (len < 3) 
    {
        decimal = (result - (int)result)*coff[2-len];
        if (decimal != 0)
        {
            if((1 == len)&&(decimal<10))
            {
                sprintf(buf, ".%02d", decimal);
            }
            else
            {
                sprintf(buf, ".%d", decimal);
            }
            strcat(str, buf);
        }
    }
    strcat(str, UnitArry[i]);
    return;
}


/***
  * 功能：
        更新文件的文件类型图标、文件名、文件内容修改时间、多选框等资源
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int UpdateFileRes(void)
{
	/* 错误标志、返回值定义 */
	int iErr = 0;
	int iReturn = 0;
	
	/* 临时变量 */
	int i = 0;
	static struct stat statbuf;	
	struct tm *pTm;
	char cFileTimeInfo[30] = {0};
    char cFileTypeInfo[30] = {0};
    char cFileSizeInfo[30] = {0};
	FILETYPE enExtension;
	int iActualPageFileNum = 0;
	static struct dirent **NamelistPage = NULL;	

	char *pIcoFolderPath = BmpFileDirectory"ico_filebrowse_folder.bmp";
	char *pIcoOtherPath  = BmpFileDirectory"ico_filebrowse_other.bmp";
	char *pIcoNullPath   = BmpFileDirectory"ico_filebrowse_null.bmp";
    
	iActualPageFileNum = GetActulPageFileNum();
	NamelistPage = GetCurPageFileNamelist();
	
	/* 更新每个文件在文件浏览器上的显示资源 */
	for(i=0; i<iActualPageFileNum; i++)
	{
		GuiMemFree(__g_pFileName[i]);

		__g_pFileName[i] = GuiMemAlloc( strlen(NamelistPage[i]->d_name)+1 );
		if(NULL != __g_pFileName[i])
		{
			strcpy(__g_pFileName[i], NamelistPage[i]->d_name);
		}
        bzero(cFileTypeInfo, sizeof(cFileTypeInfo));
		/* 获取文件的详细信息，是否为目录，文件内容修改时间 */
		iReturn = lstat(NamelistPage[i]->d_name,&statbuf);
		if(iReturn == 0)
		{
			//更新文件类型图标
			if (S_ISDIR(statbuf.st_mode))
			{
				SetPictureBitmap((char *)pIcoFolderPath, pFileBrowseBtnIco[i]);
			}
			else
			{
				enExtension = (FILETYPE)GetFileExtension(NamelistPage[i]->d_name);
                FileTypeToFileString(enExtension, cFileTypeInfo);
                char pIcoPath[256] = {0};
                SetFileIco(enExtension, pIcoPath);
                SetPictureBitmap(pIcoPath, pFileBrowseBtnIco[i]);
			}
				
			/* 更新文件名 */
			pFileBrowseStr[i] = TransString(NamelistPage[i]->d_name);	
			SetLabelText(pFileBrowseStr[i], pFileBrowseLbl[i]);
            GuiMemFree(pFileBrowseStr[i]);
				
			/* 更新文件内容修改时间 */
			pTm = gmtime(&statbuf.st_mtime);
			bzero(cFileTimeInfo, sizeof(cFileTimeInfo));
            strftime(cFileTimeInfo, sizeof(cFileTimeInfo), "%Y/%m/%d", pTm);
			pFileBrowseStrTimeInfo[i] = TransString(cFileTimeInfo);
			SetLabelText(pFileBrowseStrTimeInfo[i], pFileBrowseLblTimeInfo[i]);
            GuiMemFree(pFileBrowseStrTimeInfo[i]);

            bzero(cFileSizeInfo, sizeof(cFileSizeInfo));
            if (!S_ISDIR(statbuf.st_mode))
            {
                FileSize2Str(statbuf.st_size, cFileSizeInfo);
            }
		}
		/* 获取文件详细信息失败，未知文件类型 */
		else
		{
			/* 更新文件类型图标 */
			SetPictureBitmap((char *)pIcoOtherPath, pFileBrowseBtnIco[i]);
			/* 更新文件名 */
			pFileBrowseStr[i] = TransString(NamelistPage[i]->d_name);	
			SetLabelText(pFileBrowseStr[i], pFileBrowseLbl[i]);
            GuiMemFree(pFileBrowseStr[i]);
			/* 更新文件内容修改时间 */
			pFileBrowseStrTimeInfo[i] = TransString("");
			SetLabelText(pFileBrowseStrTimeInfo[i], pFileBrowseLblTimeInfo[i]);
            GuiMemFree(pFileBrowseStrTimeInfo[i]);
		}	

		/* 更新多选图标 */
		UpdateMultiSelectICO(NamelistPage[i]->d_name, i);

	}		
	for(; i<s_uiMaxFileNum; i++)
	{
		GuiMemFree(__g_pFileName[i]);
		pFileBrowseStr[i] = TransString("");
		SetLabelText(pFileBrowseStr[i], pFileBrowseLbl[i]);
        GuiMemFree(pFileBrowseStr[i]);
		pFileBrowseStrTimeInfo[i] = TransString("");
		SetLabelText(pFileBrowseStrTimeInfo[i], pFileBrowseLblTimeInfo[i]); 
        GuiMemFree(pFileBrowseStrTimeInfo[i]);
		SetPictureBitmap((char *)pIcoNullPath, pFileBrowseBtnIco[i]);
		SetPictureBitmap(BmpFileDirectory "bg_filebrowse1_multi_unselect.bmp", pFileBrowseBtnCheck[i]);
	}	
	return iErr;
}


/***
  * 功能：
		删除显示路径缓存中的/mnt
  * 参数：
		1、char *pBuff:		待处理的路径
  * 返回：
		无
  * 备注：	
***/
void DeletePathTitleBuff(char *pBuff)
{
	unsigned int uiLength = 0;
	unsigned int i = 0;
    char* cTopPath = MntUpDirectory;
	unsigned int uiStartPos = strlen(cTopPath);
	char *pTmp = NULL;
	
	uiLength = strlen(pBuff);

	pTmp = strstr(pBuff, cTopPath);
	if(NULL != pTmp)
	{
		for(i=0; i<(uiLength - uiStartPos); i++)
		{
			pBuff[i] = pBuff[i+uiStartPos];
		}
		pBuff[i] = 0;		
	}
}


/***
  * 功能：
        打开指定目录，并更新每个显示条目的资源
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
static int OpenFolder(const char *pFilePath)
{
	//错误标志、返回值定义
	int iErr = 0;
    int iReturn = 0;

	static char cLastPath[MAXPATHNAME];
	char cTmpPathBuff[MAXPATHNAME];
	char cPathBuff[MAXPATHNAME];						// 当前绝对路径的缓存
	//char* pTmpCap = NULL;

	if(iErr == 0)
	{
		if( chdir(pFilePath) )
		{
			iErr = -1;
		}
	}

	/* 遍历目录 */
	if(iErr == 0)
	{
		getcwd(cPathBuff, MAXPATHNAME);	
		/* 防止突然拔掉SD卡或U盘，getcwd系统调用返回其他目录 */
		if(strstr(cPathBuff, MntUpDirectory) == NULL)
		{
			strcpy(cPathBuff, MntUpDirectory);
		}	
		if( ScanDirectory(cPathBuff, s_uiMaxFileNum) )
		{
			iErr = -1;
		}
	}	
	
	/* 进入新的目录，对多选链表进行操作 */
	if(iErr == 0)
	{
		if(strcmp(cLastPath, cPathBuff))
		{
			memset(cLastPath, 0, MAXPATHNAME);
			strcpy(cLastPath, cPathBuff);
		}
		g_ucOpenFolderFlg = 1;		
	}

	/* 进入新的目录，检测sd卡或u盘是否挂载
	 * 若sd卡或u盘拔掉，但却没有删除mnt目录下的sdcard和usb目录，弹出对话框提示
	 */
	if (iErr == 0)
	{
		if(strstr(pFilePath, "sdcard") || strstr(pFilePath, "usb"))
		{
			iReturn = FirstCheckMount();
			if(iReturn)
			{				
				DialogInit(120, 90, TransString("Warning"), 
						   GetCurrLanguageText(FILE_LBL_NO_DISK),
						   0, 0, __pFuncReWincallBack, NULL, NULL);				

				iErr = -1;
			}
		}		
	}
	//设置路径显示文本信息
    if(!iErr)
    {
		strcpy(cTmpPathBuff, cPathBuff);
		DeletePathTitleBuff(cTmpPathBuff);
		pFileBrowseStrPath = TransString(cTmpPathBuff);
		SetLabelText(pFileBrowseStrPath, pFileBrowseLblPath);
        GuiMemFree(pFileBrowseStrPath);
    }
	/* 更新每个文件在文件浏览器上的显示资源 */
	if(iErr == 0)
	{
		
		uiCurrFileIndex = 0;
		iSelectedOption = 0; //默认光标停留在返回上一级目录上
		// SetPictureBitmap(BmpFileDirectory"bg_filebrowse_updir_press.bmp", pFileBrowseBtnUpDir);
		// DisplayPicture(pFileBrowseBtnUpDir);
		/* 更新每个文件在文件浏览器上的显示资源 */
		UpdateFileRes();	
	}			
	
	return iErr;
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
int SetFileOperationType(unsigned int enFileOperation)
{
	int iErr = 0;

	uiFileOprationMode = enFileOperation;

	return iErr;
}

/***
  * 功能：
        设置当前文件浏览器的文件选择模式
  * 参数：
        1、FILE_SELECT_MODE enSelectMode:	将要设置的文件选择模式
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetFileSelectMode(FILE_SELECT_MODE enSelectMode)
{
	int iErr = 0;

	uiSelectMode = enSelectMode;

	return iErr;
}

/***
  * 功能：
        设置首次进入文件浏览器应该打开的目录
  * 参数：
        1、const char *pPath:	将要打开的目录
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetFirstOpenFolder(const char *pPath)
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
	    if(access(pPath, F_OK) == -1)
        {
            strcpy(__g_cFirstFilePath, "/mnt");
        }
        else
        {
            strcpy(__g_cFirstFilePath, pPath);
        }
	}

	return iErr;
}


/***
  * 功能：
        设置多选状态发生变化时，用户控件状态更新的回调函数
  * 参数：
        1、FILEBROWSECALLLBACK pFuncCallBack:	更新回调函数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetWidgetUpdateCallBack(FILEBROWSECALLLBACK pFuncCallBack)
{
	__pFuncCallBack = pFuncCallBack;
	return 0;
}


/***
  * 功能：
        获取当前选中的文件名
  * 参数：
        1、char *pFileName:	存贮当前选中的文件名(带路径)
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		用户自己保证pFileName足够大
***/
int GetSelectFileName(char *pFileName)
{
	int iErr = 0;

	if(iErr == 0)
	{
		if(NULL == pFileName)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		/* 当前文件夹下为空 */
		if(NULL == __g_pFileName[uiCurrFileIndex])
		{
			pFileName[0] = '\0';
		}
		else
		{
		    char path[512] = {0};
		    char name[512] = {0};
		    getcwd(path, 512);
			strcpy(name, __g_pFileName[uiCurrFileIndex]);
			sprintf(pFileName, "%s/%s", path, name);
		}
	}	

	return iErr;	
}



/***
  * 功能：
        更新全选显示
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		此函数不刷新帧缓冲
***/
int UpdateFullSelect(void)
{
	UpdateFileRes();	
	DisplayFile();	
    SyncCurrFbmap();
	return 0;
}


/***
  * 功能：
        设置打开文件夹标志为0
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：

***/
int ResetOpenFolderFlag(void)
{
	if(g_ucOpenFolderFlg)
		g_ucOpenFolderFlg = 0;

	return 0;
}

/***
  * 功能：
        复制选中的文件到剪切板上
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：

***/
void CopyFileToClipboard(void)
{
    ClearQueue(pClipboardQueue); 
    
    int count = pEnvQueue->pMsgQueue->iCount;
    char** ppselectedList = (char**)malloc(count * sizeof(char*));
    GetFileListFromQueue(pEnvQueue, ppselectedList);
    int i;
    for(i = 0; i < count; ++i)
    {
        if(CheckSpecialQueue(ppselectedList[i], pClipboardQueue))
        {
            WriteQueue(ppselectedList[i], pClipboardQueue);
        }
        else
        {
            ClearSpecialQueue(ppselectedList[i], pClipboardQueue);
        }
    }
    for(i = 0; i < count; ++i)
    {
        GuiMemFree(ppselectedList[i]);
    }
    GuiMemFree(ppselectedList);
    ClearQueue(pEnvQueue);
	isClickEnter = 0;//取消选中标志位
}

static int isClipboardFileDeleted(MULTISELECT *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
	char *pTmp = NULL;
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
            if (access(pTmp, F_OK) == -1)
            {
				LOG(LOG_ERROR, "-------file not exist--------\n");
				break;
            }
        }

        if(iPos == -1)
        {
            iErr = -3;
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));		

    return iErr;
}

//检测是否在同一目录
static int CheckSameDirctory(MULTISELECT *pMsgObj, const char* pDestDirectory)
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
			    //检测同目录拷贝文件
			    char path[MAXPATHNAME];
                memcpy(path, pTmp, strlen(pTmp)-strlen(pTmp1));
                if(!strcmp(path, pDestDirectory))
                {
                    break;
                }
			}	
        }

		/* 不存在同一目录 */
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
        复制剪切板上的内容到指定目录
  * 参数：
        1、const char* pDestDirectory:  指定目录
        2、int iPasteFlag:              是否直接粘贴 
  * 返回：
        成功返回零，失败返回非零值
  * 备注：

***/
int PasteFromClipboard(const char* pDestDirectory, int iPasteFlag)
{
    int iErr = -1;

    if(iPasteFlag)
    {
        iErr = CopyMultiSelect(pClipboardQueue, pDestDirectory);
    }
    else
    {
        //剪贴板上文件是否被删除
        if(!isClipboardFileDeleted(pClipboardQueue))
        {
            iErr = -3;
        }
        else
        {
            //是否在当前目录下
            if(!CheckSameDirctory(pClipboardQueue, pDestDirectory))
            {
                iErr = -4;
            }
            else
            {
                if(CheckSameName(pClipboardQueue, "."))
                {
                    //ShowInfoDialog(GetCurrLanguageText(DIALOG_COPYING), pFrmFileBrowse);
                    iErr = CopyMultiSelect(pClipboardQueue, pDestDirectory);
                }
            }
        }
    }
    return iErr;
}
/***
  * 功能：
        显示剪切板上的内容作测试使用
  * 参数：
        无
  * 返回:
  * 备注：

***/
void ShowClipboard(void)
{
	LOG(LOG_INFO, "clipboard content is following:\n");
	ShowQueue(pClipboardQueue);
}
/***
  * 功能：
        重新绘制右侧菜单栏
  * 参数：
        CALLLBACKWINDOW1 func
  * 返回：
        无
  * 备注：

***/
void RepaintWnd(CALLLBACKWINDOW1 func)
{
    __pFuncRePaintWincallBack = func;
}
/***
  * 功能：
        设置文件类型以及全选按钮的显示
  * 参数：
        int flag(0:不显示，1:显示)
  * 返回：
        无
  * 备注：

***/
void SetDisplayFileTypeAndSelectAll(int flag)
{
    iDisplayFlag = flag;
}
