/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmOtdrSave.c
* 摘    要：  声明主窗体FrmOtdrSave 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.1
* 作    者：  	     
* 完成日期：  2015-3-2
*
* 取代版本：v1.0.0
* 原 作 者：	   
* 完成日期：2015-1-9
**************************************************************/

#include "wnd_frmotdrsave.h"

/**************************************************************
* 	为实现窗体wnd_frmotdrsave而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmotdr.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmmain.h"
#include "wnd_frmmark.h"
#include "wnd_frmotdrfileopen.h"
#include "wnd_frmfilenamed.h"
#include "wnd_frmquicksave.h"
#include "app_frmotdr.h"
#include "app_frminit.h"
#include "wnd_frmsavedefdir.h"
#include "guiphoto.h"
/**************************************************************
* 		定义wnd_frmotdrsave中内部使用的数据结构
**************************************************************/
//引用的全局变量

extern POTDR_TOP_SETTINGS pOtdrTopSettings;	//当前OTDR的工作设置

char* pOtdrFileCurrPath = NULL;
static PDISPLAY_INFO pDisplay = NULL;
static SAVE_CURVE stSaveCur;
extern char cSorQuickSaveName[100];
extern char* pOtdrFileCurrPath;
/***************************************************************
* 			窗体wnd_frmotdrsave中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmOtdrSave = NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pOtdrSaveBG = NULL;
//static GUIPICTURE *pOtdrSaveBGTableTitle = NULL;

/*********************右侧控件定义************************/
static WNDMENU1 *pOtdrSaveMenu = NULL;

/**********************桌面控件定义***********************/
//桌面控件
static GUILABEL *pOtdrSaveLblTopOpen = NULL; 
static GUILABEL *pOtdrSaveLblTopSave = NULL;

static GUIPICTURE *pOtdrFileSaveBtnTopOpen = NULL;

static GUICHAR *pOtdrSaveStrTopOpen = NULL; 
static GUICHAR *pOtdrSaveStrTopSave = NULL; 

//中间选择框
static GUIPICTURE *pOtdrSavePicSaveItem[8] = {NULL};

static GUIPICTURE *pOtdrSavePicActItem[8] = {NULL};
static GUILABEL	  *pOtdrSaveLblActItem[8] = {NULL};
static GUICHAR    *pOtdrSaveStrActItem[8] = {NULL};

//当前活动曲线
static GUIPICTURE *pOtdrSaveBgActCur = NULL;

static GUILABEL   *pOtdrSaveLblActCur = NULL;
static GUICHAR    *pOtdrSaveStrActCur = NULL;
static GUILABEL   *pOtdrSaveLblActiveCurveLbl = NULL;
static GUICHAR    *pOtdrSaveStrActiveCurveLbl = NULL;
//设置当前活动曲线 和 保存按钮



/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int OtdrSaveTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int OtdrSaveTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**************************************************************
* 				控件函数声明
**************************************************************/
//顶部打开按钮声明
static int  OtdrSaveBtnTopOpen_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  OtdrSaveBtnTopOpen_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);

//中间部分选择函数

static int  OtdrSaveActItem_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  OtdrSaveActItem_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  SetActiveCurve();
//右侧按键区菜单
static void OtdrSaveMenuCallBack(int iOption);


//保存曲线
static int SaveSor(int arg, void *pArg);
static void ClearSelectCurve(void);


//用于对话框重新还原窗体时的回调函数
static void ReCreateSaveWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmOtdrSaveInit, FrmOtdrSaveExit, 
						FrmOtdrSavePaint, FrmOtdrSaveLoop, 
						FrmOtdrSavePause, FrmOtdrSaveResume,
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
static int  OtdrSaveBtnTopOpen_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	if (access("/mnt/dataDisk", F_OK) && access("/tmp/usb", F_OK))
	{
		MsgNoSdcard(pFrmOtdrSave);
	}
	else
	{
        if(pOtdrFileCurrPath == NULL)
        {
            pOtdrFileCurrPath = GuiMemAlloc(512);
            //strcpy(pOtdrFileCurrPath, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
        }
        strcpy(pOtdrFileCurrPath, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);//每次打开都是OTDR设置的路径
        OtdrFileDialog(FILE_OPEN_MODE, MntUpDirectory, pOtdrFileCurrPath, ReCreateSaveWindow, SORANDCUR);        
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
static int  OtdrSaveBtnTopOpen_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	
	return iReturn;
}


static int GetSaveCur()
{
	int i;
	stSaveCur.iSaveCurveNum =GetCurveNum(pOtdrTopSettings->pDisplayInfo);
	if (0 >= stSaveCur.iSaveCurveNum)
	{
		stSaveCur.iActiveCurve = 0;
		stSaveCur.iActCurItemSelect = 0;
		for(i = 0; i < CURVE_MAX; i++)
		{
			stSaveCur.iSaveCurveFlag[i] = 0;
		}
		stSaveCur.iSaveCurveFlag[stSaveCur.iActiveCurve] = 0;
	}
	else
	{
		stSaveCur.iActiveCurve = GetCurrCurvePos(pOtdrTopSettings->pDisplayInfo);
		stSaveCur.iActCurItemSelect = stSaveCur.iActiveCurve;
		for(i = 0; i < CURVE_MAX; i++)
		{
			stSaveCur.iSaveCurveFlag[i] = 0;
		}
		stSaveCur.iSaveCurveFlag[stSaveCur.iActiveCurve] = 1;
	}
	return 0;
}


//窗体初始化
int FrmOtdrSaveInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	int i;

	pDisplay = pOtdrTopSettings->pDisplayInfo;

	unsigned int StrOtdrSaveMenu[] = {
		1
	};

	//得到当前窗体对象
	pFrmOtdrSave = (GUIWINDOW *) pWndObj;

	GetSaveCur();
	
	OtdrSaveTextRes_Init(NULL, 0, NULL, 0);
	
	//背景
    pOtdrSaveBG = CreatePhoto("bg_otdr_save");
	//选项卡标签
	pOtdrSaveLblTopSave = CreateLabel(0, 24, 100, 24,   
								  pOtdrSaveStrTopSave);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pOtdrSaveLblTopSave);
	pOtdrSaveLblTopOpen = CreateLabel(101, 24, 100, 24,   
								  pOtdrSaveStrTopOpen);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pOtdrSaveLblTopOpen);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pOtdrSaveLblTopSave);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pOtdrSaveLblTopOpen);
	
	//中间选择部分
	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		if(i == stSaveCur.iActiveCurve)
		{
            pOtdrSavePicSaveItem[i] = CreatePicture(43, 63 + i * 60, 60, 60,
									BmpFileDirectory"btn_save_select.bmp");
			pOtdrSavePicActItem[i] = CreatePicture(90, 62 + i * 60, 510, 32,
									BmpFileDirectory"btn_save_act_select.bmp");
		}
		else
		{
            pOtdrSavePicSaveItem[i] = CreatePicture(44, 64 + i * 60, 60, 60,
									BmpFileDirectory"btn_save_unselect.bmp");
			pOtdrSavePicActItem[i] = CreatePicture(90, 62 + i * 60, 510, 32,
									BmpFileDirectory"btn_save_act_unselect.bmp");
        }
		pOtdrSaveLblActItem[i] = CreateLabel(95, 62 + i * 60 + 8, 505, 24, pOtdrSaveStrActItem[i]);
	}

	//当前曲线
	pOtdrSaveBgActCur = CreatePicture(120, 430, 505, 20,
									BmpFileDirectory"bg_save_actcur.bmp");
	pOtdrSaveLblActiveCurveLbl = CreateLabel(12, 430, 150, 24,   
								  pOtdrSaveStrActiveCurveLbl);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pOtdrSaveLblActiveCurveLbl);
	pOtdrSaveLblActCur = CreateLabel(165, 430, 505, 24, pOtdrSaveStrActCur);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pOtdrSaveLblActCur);

	//设置活动曲线 和 保存按钮
	
	pOtdrFileSaveBtnTopOpen = CreatePicture(100, 0, 100, 39, NULL);

	pOtdrSaveMenu = CreateWndMenu1(sizeof(StrOtdrSaveMenu) / sizeof(StrOtdrSaveMenu[0]), 
							sizeof(StrOtdrSaveMenu),StrOtdrSaveMenu,
							MENU_BACK, -1, 0, 40, OtdrSaveMenuCallBack);	

	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmOtdrSave, 
		pFrmOtdrSave);

	//打开选项卡
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUIPICTURE), pOtdrFileSaveBtnTopOpen, 
						pFrmOtdrSave);
	
	//中间部分控件
	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrSavePicActItem[i], 
						pFrmOtdrSave);
	}

	
	//右边菜单
	AddWndMenuToComp1(pOtdrSaveMenu, pFrmOtdrSave);


	pMsg = GetCurrMessage();
	//桌面按钮注册
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrFileSaveBtnTopOpen, 
	               OtdrSaveBtnTopOpen_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrFileSaveBtnTopOpen, 
	               OtdrSaveBtnTopOpen_Down, NULL, 0, pMsg);
	
	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrSavePicActItem[i], 
					   OtdrSaveActItem_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrSavePicActItem[i], 
					   OtdrSaveActItem_Down, NULL, i, pMsg);
	}

	//注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pOtdrSaveMenu, pFrmOtdrSave);
    
	return iReturn;
}


//窗体退出
int FrmOtdrSaveExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pFrmOtdrSave = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmOtdrSave);


	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pOtdrSaveBG);
	DestroyPicture(&pOtdrFileSaveBtnTopOpen);
	DestroyPicture(&pOtdrSaveBgActCur);
	DestroyLabel(&pOtdrSaveLblTopOpen);
	DestroyLabel(&pOtdrSaveLblTopSave);

	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		DestroyPicture(&pOtdrSavePicSaveItem[i]);
		DestroyPicture(&pOtdrSavePicActItem[i]);
		DestroyLabel(&pOtdrSaveLblActItem[i]);
	}
	DestroyLabel(&pOtdrSaveLblActCur);
	DestroyLabel(&pOtdrSaveLblActiveCurveLbl);

	DestroyWndMenu1(&pOtdrSaveMenu);

	//文本内容销毁
	OtdrSaveTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}


//窗体绘制
int FrmOtdrSavePaint(void *pWndObj)
{

	int iReturn = 0;
	int  i;

	pFrmOtdrSave = (GUIWINDOW *) pWndObj;

	//显示状态栏、桌面、信息栏控件
	DisplayPicture(pOtdrSaveBG);
	DisplayLabel(pOtdrSaveLblTopOpen);
	DisplayLabel(pOtdrSaveLblTopSave);

	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		DisplayPicture(pOtdrSavePicSaveItem[i]);
		DisplayPicture(pOtdrSavePicActItem[i]);
		DisplayLabel(pOtdrSaveLblActItem[i]);
	}
	DisplayLabel(pOtdrSaveLblActCur);
	DisplayLabel(pOtdrSaveLblActiveCurveLbl);

	DisplayWndMenu1(pOtdrSaveMenu);
	if(GetCurveNum(pDisplay) <= 0)
	{
        SetWndMenuItemEnble(0, 0, pOtdrSaveMenu);
        SetWndMenuItemEnble(1, 0, pOtdrSaveMenu);
        SetWndMenuItemEnble(4, 0, pOtdrSaveMenu);
    }

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}


//窗体循环
int FrmOtdrSaveLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	SendWndMsg_LoopDisable(pWndObj);
    
	return iReturn;
}



//窗体挂起函数
int FrmOtdrSavePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//窗体恢复函数
int FrmOtdrSaveResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//初始化文本资源
static int OtdrSaveTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	char buff[F_NAME_MAX] = {0};
	char strFile[F_NAME_MAX] = {0};
	int i;
	
	pOtdrSaveStrTopOpen = TransString("OTDR_SAVE_LBL_OPENFILE");
	pOtdrSaveStrTopSave = TransString("OTDR_SAVE_LBL_SAVEFILE"); 

	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		GetCurveFile(i, pDisplay, strFile);
		snprintf(buff, sizeof(buff), "Trace%d  %s", i + 1, strFile);
		pOtdrSaveStrActItem[i] = TransString(buff);
	}
	
	pOtdrSaveStrActiveCurveLbl = TransString("OTDR_SAVE_LBL_ACTIVECURVE");
	
	GetCurveFile(stSaveCur.iActiveCurve, pDisplay, strFile);
	snprintf(buff, sizeof(buff), "  %s", strFile);
	pOtdrSaveStrActCur = TransString(buff);

	return iReturn;
}

//释放文本资源
static int OtdrSaveTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i;
	GuiMemFree(pOtdrSaveStrTopOpen);
	GuiMemFree(pOtdrSaveStrTopSave);

	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		GuiMemFree(pOtdrSaveStrActItem[i]);
	}
	GuiMemFree(pOtdrSaveStrActCur);
	GuiMemFree(pOtdrSaveStrActiveCurveLbl);

	return iReturn;
}

//选中当前活动条目
static int  OtdrSaveActItem_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
	int iRet = 0;
	int i = 0;
	stSaveCur.iActCurItemSelect = iOutLen;
	for(i = 0; i < stSaveCur.iSaveCurveNum; i++)
	{
		if(iOutLen != i)
		{
			SetPictureBitmap(BmpFileDirectory"btn_save_act_unselect.bmp",
								pOtdrSavePicActItem[i]);
			SetPictureBitmap(BmpFileDirectory"btn_save_unselect.bmp", pOtdrSavePicSaveItem[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"btn_save_act_select.bmp",
								pOtdrSavePicActItem[i]);
			SetPictureBitmap(BmpFileDirectory"btn_save_select.bmp", pOtdrSavePicSaveItem[i]);
			SetActiveCurve();

		}
		DisplayPicture(pOtdrSavePicSaveItem[i]);
		DisplayPicture(pOtdrSavePicActItem[i]);
		DisplayLabel(pOtdrSaveLblActItem[i]);
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iRet;
}
static int  OtdrSaveActItem_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	int iRet = 0;
	return iRet;
}

static int  SetActiveCurve()
{
	int iRet = 0;
	char buff[F_NAME_MAX];
	char strFile[F_NAME_MAX];

	if (0 >= stSaveCur.iSaveCurveNum)
		return 0;

	stSaveCur.iActiveCurve = stSaveCur.iActCurItemSelect;
	
	GetCurveFile(stSaveCur.iActiveCurve, pDisplay, strFile);
	snprintf(buff, sizeof(buff), "  %s", strFile);
	GuiMemFree(pOtdrSaveStrActCur);
	pOtdrSaveStrActCur = TransString(buff);
	UpdateLabelRes(pOtdrSaveStrActCur, pOtdrSaveLblActCur, pOtdrSaveBgActCur);
	SetCurrCurvePos(stSaveCur.iActiveCurve, pDisplay);
	pOtdrTopSettings->pUser_Setting->enWaveCurPos = stSaveCur.iActiveCurve;
	
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iRet;
}

/*  侧边菜单控件回调函数 */
static void OtdrSaveMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOption)
	{
	case 0://保存曲线
        if (0 >= stSaveCur.iSaveCurveNum)
			break;
        SaveSor(0, NULL);
		break;
	case 1://保存到U盘
        {
    		char strFile[F_NAME_MAX] = {};
            char fileName[F_NAME_MAX] = {};
    		if (GetCurveFile(stSaveCur.iActCurItemSelect,  pDisplay, strFile))
                strcpy(fileName, "Fiber0000.SOR");
    		else
                strcpy(fileName, strFile);
            if(pOtdrFileCurrPath == NULL)
            {
                pOtdrFileCurrPath = GuiMemAlloc(512);
                strcpy(pOtdrFileCurrPath, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
            }
    		SaveToUsb(ReCreateSaveWindow, pOtdrFileCurrPath, fileName, SOR);
	    }
		break;
	case 2://自动命名
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
			        FrmFileNamedInit, FrmFileNamedExit, 
			        FrmFileNamedPaint, FrmFileNamedLoop, 
			        FrmFileNamedPause, FrmFileNamedResume,
			        NULL);          				//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmOtdrSave);      	//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);           	//发送消息以便调用新的窗体
		break;
	case 3://标识
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMarkInit, FrmMarkExit, 
							FrmMarkPaint, FrmMarkLoop, 
							FrmMarkPause, FrmMarkResume,
							NULL);
	    SendWndMsg_WindowExit(pFrmOtdrSave);  
   	 	SendSysMsg_ThreadCreate(pWnd);
		break;
	case 4://清除曲线
        ClearSelectCurve();
        break;
	case BACK_DOWN:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmOtdrInit, FrmOtdrExit, 
	                        FrmOtdrPaint, FrmOtdrLoop, 
					        FrmOtdrPause, FrmOtdrResume,
	                        NULL);           
	    SendWndMsg_WindowExit(pFrmOtdrSave);  
   	 	SendSysMsg_ThreadCreate(pWnd); 
		break;
	default:
		break;
	}
	
}

//保存曲线
static int SaveSor(int arg, void *pArg)
{
	if (access("/mnt/dataDisk", F_OK) != 0)
	{
		return -2;
	}
	
	if( access(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, F_OK) != 0)
	{
		if (mkdir(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, 0666))
			return -3;
	}

    char buff[F_NAME_MAX];
    char strFile[F_NAME_MAX];
    GetCurveFile(stSaveCur.iActiveCurve,  pDisplay, strFile);
    sprintf(buff, "%s/%s", pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, strFile);

    int iErr = 0;
	if(iErr == 0)
	{
        if (!CanWriteFile(buff))
        {
            iErr = -1;
        }
	}

	if(iErr == 0)
	{
		int iReturn = WriteSor(stSaveCur.iActiveCurve, buff);
		if(iReturn == 0)
		{
			DialogInit(120, 90, TransString("DIALOG_NOTE"),
				GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
				0, 0, ReCreateSaveWindow, NULL, NULL);
		}
		else if(iReturn == -2)
		{
			DialogInit(120, 90, TransString("Warning"),
				GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
				0, 0, ReCreateSaveWindow, NULL, NULL);
		}
		else
		{
			DialogInit(120, 90, TransString("Warning"),
				GetCurrLanguageText(DIALOG_SOR_SYSTEM_ERR),
				0, 0, ReCreateSaveWindow, NULL, NULL);
		}
	}

    return iErr;
}

//按清除曲线按钮函数
static void ClearSelectCurve(void)
{
	GUIWINDOW *pWnd = NULL;
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
	CURVE_PRARM Param;

	int idelCur = stSaveCur.iActCurItemSelect;

	CODER_LOG(CoderGu, "idelCur= %d\n",idelCur);

	ClearSaveOtdrCurve(idelCur);
	if(GetCurveNum(pDisplay) >0)
	{
		SetCurrCurvePos(0, pDisplay);
	}
	GetCurvePara(CURR_CURVE, pDisplay, &Param);
	
	pUser_Setting->enWaveCurPos = Param.enWave;

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmOtdrSaveInit, FrmOtdrSaveExit, 
						FrmOtdrSavePaint, FrmOtdrSaveLoop, 
						FrmOtdrSavePause, FrmOtdrSaveResume,
						NULL);			 
	SendWndMsg_WindowExit(pFrmOtdrSave);  
	SendSysMsg_ThreadCreate(pWnd); 
}
