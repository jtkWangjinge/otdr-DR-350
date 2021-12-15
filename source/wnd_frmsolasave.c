/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmSOLASave.c
* 摘    要：  声明主窗体FrmSOLASave 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.1
* 作    者： 
* 完成日期：
*
* 取代版本：v1.0.0
* 原 作 者：
* 完成日期：
**************************************************************/

#include "wnd_frmsolasave.h"

/**************************************************************
* 	为实现窗体wnd_frmotdrsave而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmsola.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmmain.h"
#include "wnd_frmsolaident.h"
#include "wnd_frmotdrfileopen.h"
#include "wnd_frmsolafilenamed.h"
#include "app_frminit.h"
#include "wnd_frmoperatesor.h"
#include "guiphoto.h"
/**************************************************************
* 		定义wnd_frmotdrsave中内部使用的数据结构
**************************************************************/
//引用的全局变量

char* pSOLASaveCurrPath = NULL;
SolaEvents *solaEvents = NULL;
char *pSolaOpenCurrPath = NULL;
extern char cSorQuickSaveName[100];
/***************************************************************
* 			窗体wnd_frmotdrsave中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmSOLASave = NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pSOLASaveBG = NULL;
static GUIPICTURE *pSOLASaveBGTableTitle = NULL;

/*********************右侧控件定义************************/
static WNDMENU1 *pSOLASaveMenu = NULL;

/**********************桌面控件定义***********************/
//桌面控件
static GUILABEL *pSOLASaveLblTopOpen = NULL; 
static GUILABEL *pSOLASaveLblTopSave = NULL;

static GUIPICTURE *pSOLAFileSaveBtnTopOpen = NULL;

static GUICHAR *pSOLASaveStrTopOpen = NULL; 
static GUICHAR *pSOLASaveStrTopSave = NULL; 

//中间选择框
static GUIPICTURE *pSOLASavePicSaveItem = NULL;

static GUIPICTURE *pSOLASavePicActItem = NULL;
static GUILABEL	  *pSOLASaveLblActItem = NULL;
static GUICHAR    *pSOLASaveStrActItem = NULL;

//当前活动曲线
static GUIPICTURE *pSOLASaveBgActCur = NULL;

static GUILABEL   *pSOLASaveLblActCur = NULL;
static GUICHAR    *pSOLASaveStrActCur = NULL;
static GUILABEL   *pSOLASaveLblActiveCurveLbl = NULL;
static GUICHAR    *pSOLASaveStrActiveCurveLbl = NULL;
//设置当前活动曲线 和 保存按钮

//打开sola文件
static void OpenSolaFile(void);
//按清除曲线按钮函数
static void ClearSelectCurve(void);
/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int SOLASaveTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int SOLASaveTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**************************************************************
* 				控件函数声明
**************************************************************/
//顶部打开按钮声明
static int  SOLASaveBtnTopOpen_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  SOLASaveBtnTopOpen_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
//右侧按键区菜单
static void SOLASaveMenuCallBack(int iOption);

//用于对话框重新还原窗体时的回调函数
static void ReCreateSOLASaveWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSOLASaveInit, FrmSOLASaveExit, 
						FrmSOLASavePaint, FrmSOLASaveLoop, 
						FrmSOLASavePause, FrmSOLASaveResume,
						NULL);
}

/***
  * 功能：
        		TopOpen 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  SOLASaveBtnTopOpen_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	if (access("/tmp/sdcard", F_OK) && access("/tmp/usb", F_OK))
	{
		MsgNoSdcard(pFrmSOLASave);
	}
	else
	{
        OpenSolaFile();
	}		 
	return iReturn;
}


/***
  * 功能：
        		TopOpen 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  SOLASaveBtnTopOpen_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	
	return iReturn;
}


static int GetSOLAData()
{
	solaEvents = getCurrSolaEvents();
	
	return 0;
}


//窗体初始化
int FrmSOLASaveInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;

	unsigned int StrSOLASaveMenu[] = {
		1	
	};

	//得到当前窗体对象
	pFrmSOLASave = (GUIWINDOW *) pWndObj;

	GetSOLAData();
	
	SOLASaveTextRes_Init(NULL, 0, NULL, 0);
	
	//背景
    pSOLASaveBG = CreatePhoto("bg_otdr_save");
    pSOLASaveBGTableTitle = CreatePhoto("otdr_top1f");
	//选项卡标签
	pSOLASaveLblTopSave = CreateLabel(0, 24, 100, 24,   
								  pSOLASaveStrTopSave);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pSOLASaveLblTopSave);
	pSOLASaveLblTopOpen = CreateLabel(101, 24, 100, 24,   
								  pSOLASaveStrTopOpen);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pSOLASaveLblTopOpen);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSOLASaveLblTopSave);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSOLASaveLblTopOpen);
	
	//中间选择部分

    pSOLASavePicSaveItem = CreatePicture(44, 64 , 60, 60,
							BmpFileDirectory"btn_save_select.bmp");
	pSOLASavePicActItem = CreatePicture(90, 62, 510, 32,
							BmpFileDirectory"btn_save_act_select.bmp");

	pSOLASaveLblActItem = CreateLabel(95, 62 + 8, 505, 24, pSOLASaveStrActItem);

	//当前曲线
	pSOLASaveBgActCur = CreatePicture(120, 430, 505, 20,
									BmpFileDirectory"bg_save_actcur.bmp");
	pSOLASaveLblActiveCurveLbl = CreateLabel(12, 430, 150, 24,   
								  pSOLASaveStrActiveCurveLbl);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSOLASaveLblActiveCurveLbl);
	pSOLASaveLblActCur = CreateLabel(165, 430, 505, 24, pSOLASaveStrActCur);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSOLASaveLblActCur);

	//设置活动曲线 和 保存按钮
	
	pSOLAFileSaveBtnTopOpen = CreatePicture(100, 0, 100, 39, NULL);

	pSOLASaveMenu = CreateWndMenu1(sizeof(StrSOLASaveMenu) / sizeof(StrSOLASaveMenu[0]), 
							sizeof(StrSOLASaveMenu),StrSOLASaveMenu,
							MENU_BACK, -1, 0, 40, SOLASaveMenuCallBack);	

	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSOLASave, 
		pFrmSOLASave);

	//打开选项卡
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUIPICTURE), pSOLAFileSaveBtnTopOpen, 
						pFrmSOLASave);
	//右边菜单
	AddWndMenuToComp1(pSOLASaveMenu, pFrmSOLASave);


	pMsg = GetCurrMessage();
	//桌面按钮注册
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSOLAFileSaveBtnTopOpen, 
	               SOLASaveBtnTopOpen_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSOLAFileSaveBtnTopOpen, 
	               SOLASaveBtnTopOpen_Down, NULL, 0, pMsg);
	
	//注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pSOLASaveMenu, pFrmSOLASave);
    
	return iReturn;
}


//窗体退出
int FrmSOLASaveExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pFrmSOLASave = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmSOLASave);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pSOLASaveBG);
    DestroyPicture(&pSOLASaveBGTableTitle);
	DestroyPicture(&pSOLAFileSaveBtnTopOpen);
	DestroyPicture(&pSOLASaveBgActCur);
	DestroyLabel(&pSOLASaveLblTopOpen);
	DestroyLabel(&pSOLASaveLblTopSave);

	DestroyPicture(&pSOLASavePicSaveItem);
	DestroyPicture(&pSOLASavePicActItem);
	DestroyLabel(&pSOLASaveLblActItem);
	
	DestroyLabel(&pSOLASaveLblActCur);
	DestroyLabel(&pSOLASaveLblActiveCurveLbl);

	DestroyWndMenu1(&pSOLASaveMenu);

	//文本内容销毁
	SOLASaveTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}


//窗体绘制
int FrmSOLASavePaint(void *pWndObj)
{

	int iReturn = 0;

	pFrmSOLASave = (GUIWINDOW *) pWndObj;

	//显示状态栏、桌面、信息栏控件
	DisplayPicture(pSOLASaveBG);
	DisplayPicture(pSOLASaveBGTableTitle);
	DisplayLabel(pSOLASaveLblTopOpen);
	DisplayLabel(pSOLASaveLblTopSave);
	
	DisplayLabel(pSOLASaveLblActCur);
	DisplayLabel(pSOLASaveLblActiveCurveLbl);

	DisplayWndMenu1(pSOLASaveMenu);
	if(solaEvents == NULL)
	{
        SetWndMenuItemEnble(0, 0, pSOLASaveMenu);
        SetWndMenuItemEnble(1, 0, pSOLASaveMenu);
        SetWndMenuItemEnble(3, 0, pSOLASaveMenu);
    }
    else
    {
    	DisplayPicture(pSOLASavePicSaveItem);
    	DisplayPicture(pSOLASavePicActItem);
    	DisplayLabel(pSOLASaveLblActItem);    
    }

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}


//窗体循环
int FrmSOLASaveLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	SendWndMsg_LoopDisable(pWndObj);
	
	return iReturn;
}



//窗体挂起函数
int FrmSOLASavePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}



//窗体恢复函数
int FrmSOLASaveResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


//初始化文本资源
static int SOLASaveTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	char buff[512] = {0};
	
	pSOLASaveStrTopOpen = TransString("OTDR_SAVE_LBL_OPENFILE");
	pSOLASaveStrTopSave = TransString("OTDR_SAVE_LBL_SAVEFILE"); 

    if(solaEvents)
    {
        char *name = strrchr(solaEvents->absolutePath, '/');
        if(name)
	        snprintf(buff, sizeof(buff), "%s", ++name);
	}
	pSOLASaveStrActItem = TransString(buff);
	
	pSOLASaveStrActiveCurveLbl = TransString(" ");
	
	pSOLASaveStrActCur = TransString(" ");


	return iReturn;
}


//释放文本资源
static int SOLASaveTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	GuiMemFree(pSOLASaveStrTopOpen);
	GuiMemFree(pSOLASaveStrTopSave);

	GuiMemFree(pSOLASaveStrActItem);
	GuiMemFree(pSOLASaveStrActCur);
	GuiMemFree(pSOLASaveStrActiveCurveLbl);

	return iReturn;
}

//打开sola文件
static void OpenSolaFile(void)
{
    if(pSolaOpenCurrPath == NULL)
    {
        pSolaOpenCurrPath = GuiMemAlloc(512);
        //strcpy(pSolaOpenCurrPath, pOtdrTopSettings->sDefSavePath.cSolaSavePath);
    }
    strcpy(pSolaOpenCurrPath, pOtdrTopSettings->sDefSavePath.cSolaSavePath);//每次打开都是SOLA路径
    FileDialog(FILE_OPEN_MODE, MntUpDirectory, pSolaOpenCurrPath, ReCreateSOLASaveWindow, SOLA);
}

/*  侧边菜单控件回调函数 */
static void SOLASaveMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOption)
	{
	case 0://保存曲线
        if (NULL == solaEvents)
			break;
        if(WriteSola(NULL, 1))
        {
            DialogInit(120, 90, TransString("DIALOG_NOTE"),
				TransString("SOLA_SAVE_FAILED"),
				0, 0, ReCreateSOLASaveWindow, NULL, NULL);
        }
        else
        {
            DialogInit(120, 90, TransString("DIALOG_NOTE"),
				GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
				0, 0, ReCreateSOLASaveWindow, NULL, NULL);
        }
		break;
	case 1://保存到U盘
        {
            if(solaEvents)
            {
                char *fileName = strrchr(solaEvents->absolutePath, '/');
                if(fileName)
                {
                    fileName++;
                    
                    if(pSOLASaveCurrPath == NULL)
                    {
                        pSOLASaveCurrPath = GuiMemAlloc(512);
                        strcpy(pSOLASaveCurrPath, pOtdrTopSettings->sDefSavePath.cSolaSavePath);
                    }
            		SaveToUsb(ReCreateSOLASaveWindow, pSOLASaveCurrPath, fileName, SOLA);
        		}
            }
	    }
		break;
	case 2://自动命名
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
					FrmSolaFileNamedInit, FrmSolaFileNamedExit, 
					FrmSolaFileNamedPaint, FrmSolaFileNamedLoop, 
					FrmSolaFileNamedPause, FrmSolaFileNamedResume,
			        NULL);          				//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmSOLASave);      	//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);           	//发送消息以便调用新的窗体
		
		break;
	case 3://清除SOLA
        ClearSelectCurve();
		break;
	case 4:
		break;
	case BACK_DOWN:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmSolaInit, FrmSolaExit, 
	                        FrmSolaPaint, FrmSolaLoop, 
					        FrmSolaPause, FrmSolaResume,
	                        NULL);           
	    SendWndMsg_WindowExit(pFrmSOLASave);  
   	 	SendSysMsg_ThreadCreate(pWnd); 
		break;
	default:
		break;
	}
	
}


//按清除曲线按钮函数
static void ClearSelectCurve(void)
{
	GUIWINDOW *pWnd = NULL;

    clearSolaEvents();

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSOLASaveInit, FrmSOLASaveExit, 
						FrmSOLASavePaint, FrmSOLASaveLoop, 
						FrmSOLASavePause, FrmSOLASaveResume,
						NULL);			 
	SendWndMsg_WindowExit(pFrmSOLASave);  
	SendSysMsg_ThreadCreate(pWnd); 
}
