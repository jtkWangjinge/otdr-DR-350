/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmSolaIdent.c
* 摘    要：  声明主窗体FrmSolaIdent 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v0.0.1
* 作    者：   
* 完成日期：  2014-12-30
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/

#include "wnd_frmsolaident.h"

#include "wnd_frmmark.h"
/**************************************************************
* 	为实现窗体wnd_FrmSolaIdent而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmime.h"
#include "wnd_frmabout.h"
#include "wnd_frmmenubak.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"
#include "app_systemsettings.h"
#include "guiphoto.h"
#include "wnd_frmsolalinedefine.h"
#include "wnd_frmsola.h"
#include "wnd_frmsolalinepassthreshold.h"
#include "wnd_frmsolaitempassthreshold.h"
#include "wnd_frmsolafilenamed.h"
/**************************************************************
* 			定义wnd_FrmSolaIdent中内部使用的数据结构
**************************************************************/

//引用的全局变量
static int iDistABFlag = 0;		//记录当前是0:a->b 还是1:b->a
//自定义的全局变量
static int g_iInputSolaIdentNum = -1;

extern SerialNum* pCurSN;
extern PSYSTEMSET pCurSystemSet;
extern SOLA_MESSAGER* pSolaMessager;
/**************************************************************
* 					窗体wnd_FrmSolaIdent中的窗体控件
**************************************************************/

static GUIWINDOW *pFrmSolaIdent = NULL;
static WNDMENU1 *pFrmSolaIdentMenu = NULL;
/*********************桌面背景定义************************/
static GUIPICTURE *pFrmSolaIdentBG = NULL;

static GUIPICTURE * identifyTitlePicture =NULL;
static GUIPICTURE * lineDefineTitlePicture =NULL;
static GUIPICTURE * linePassThresholdTitlePicture =NULL;
static GUIPICTURE * itemPassThresholdTitlePicture =NULL;

static GUICHAR* pSolaSettingStrIdentifyTitle = NULL;
static GUICHAR* pSolaSettingStrLineDefineTitle = NULL;
static GUICHAR* pSolaSettingStrLinePassThresholdTitle = NULL;
static GUICHAR* pSolaSettingStrItemPassThresholdTitle = NULL;

static GUILABEL* pSolaSettingLblLineDefineTitle = NULL;
static GUILABEL* pSolaSettingLblLinePassThresholdTitle = NULL;
static GUILABEL* pSolaSettingLblItemPassThresholdTitle = NULL;
static GUILABEL* pSolaSettingLblIdentifyTitle = NULL;
/*********************右侧控件定义************************/

/**********************桌面控件定义***********************/
//桌面控件
static GUIPICTURE *pSolaIdentBtnFiberInput[4] = {NULL} ;
static GUIPICTURE *pSolaIdentBtnTaskInput[5] = {NULL};
static GUIPICTURE *pSolaIdentBtnMeasureInput[5] = {NULL};
static GUIPICTURE *pSolaIdentBtnClear = NULL ;
static GUIPICTURE *pSolaIdentBtnNoteInput = NULL ;
static GUIPICTURE *pSolaIdentBtnFiberdirectionAB = NULL ;
static GUIPICTURE *pSolaIdentBtnFiberdirectionBA = NULL ;

/************************桌面标签定义*********************/
//桌面标签
static GUILABEL *pSolaIdentLblFiber = NULL; 
static GUILABEL *pSolaIdentLblFiberMark = NULL; 
static GUILABEL *pSolaIdentLblCableMark = NULL; 
static GUILABEL *pSolaIdentLblFiberLocation1 = NULL; 
static GUILABEL *pSolaIdentLblFiberLocation2 = NULL; 
static GUILABEL *pSolaIdentLblFiberdirection = NULL; 
static GUILABEL *pSolaIdentLblFiberdirectionAB = NULL; 
static GUILABEL *pSolaIdentLblFiberdirectionBA = NULL; 
static GUILABEL *pSolaIdentLblFiberString[4] = {NULL}; 
//测量栏标签
static GUILABEL *pSolaIdentLblMeas = NULL; 
static GUILABEL *pSolaIdentLblMeasDate = NULL; 
static GUILABEL *pSolaIdentLblMeasTime = NULL; 
static GUILABEL *pSolaIdentLblMeasModule = NULL; 
static GUILABEL *pSolaIdentLblMeasNum = NULL; 
static GUILABEL *pSolaIdentLblMeasModuleVal = NULL; 
static GUILABEL *pSolaIdentLblMeasNumVal = NULL; 
static GUILABEL *pSolaIdentLblMeasDateString = NULL; 
static GUILABEL *pSolaIdentLblMeasTimeString = NULL; 


//任务栏标签
static GUILABEL *pSolaIdentLblTask = NULL; 
static GUILABEL *pSolaIdentLblTaskID = NULL; 
static GUILABEL *pSolaIdentLblTaskOperatorA = NULL; 
static GUILABEL *pSolaIdentLblTaskOperatorB = NULL; 
static GUILABEL *pSolaIdentLblTaskCompany = NULL; 
static GUILABEL *pSolaIdentLblTaskClient = NULL; 
static GUILABEL *pSolaIdentLblTaskString[5] = {NULL}; 

//注释以下标签
static GUILABEL *pSolaIdentLblNote = NULL; 
static GUILABEL *pSolaIdentLblClear = NULL; 
static GUILABEL *pSolaIdentLblNoteString = NULL; 

/************************桌面标签文本*********************/
//桌面标签文本
static GUICHAR *pSolaIdentStrFiber = NULL; 
static GUICHAR *pSolaIdentStrFiberMark = NULL; 
static GUICHAR *pSolaIdentStrCableMark = NULL; 
static GUICHAR *pSolaIdentStrFiberLocation1 = NULL; 
static GUICHAR *pSolaIdentStrFiberLocation2 = NULL; 
static GUICHAR *pSolaIdentStrFiberdirection = NULL; 
static GUICHAR *pSolaIdentStrFiberdirectionAB = NULL; 
static GUICHAR *pSolaIdentStrFiberdirectionBA = NULL; 
static GUICHAR *pSolaIdentStrFiberString[4] = {NULL}; 
//测量栏标签文本
static GUICHAR *pSolaIdentStrMeas = NULL; 
static GUICHAR *pSolaIdentStrMeasDate = NULL; 
static GUICHAR *pSolaIdentStrMeasTime = NULL; 
static GUICHAR *pSolaIdentStrMeasModule = NULL; 
static GUICHAR *pSolaIdentStrMeasNum = NULL; 
static GUICHAR *pSolaIdentStrMeasModuleVal = NULL; 
static GUICHAR *pSolaIdentStrMeasNumVal = NULL; 
static GUICHAR *pSolaIdentStrMeasDateString = NULL; 
static GUICHAR *pSolaIdentStrMeasTimeString = NULL; 

//任务栏标签文本
static GUICHAR *pSolaIdentStrTask = NULL; 
static GUICHAR *pSolaIdentStrTaskID = NULL; 
static GUICHAR *pSolaIdentStrTaskOperatorA = NULL; 
static GUICHAR *pSolaIdentStrTaskOperatorB = NULL; 
static GUICHAR *pSolaIdentStrTaskCompany = NULL; 
static GUICHAR *pSolaIdentStrTaskClient = NULL; 
static GUICHAR *pSolaIdentStrTaskString[5] = {NULL}; 

//注释以下标签文本
static GUICHAR *pSolaIdentStrNote = NULL; 
static GUICHAR *pSolaIdentStrNoteString = NULL; 
static GUICHAR *pSolaIdentStrClear = NULL; 

/**************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int SolaIdentTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int SolaIdentTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/**************************************************************
* 				控件函数声明
**************************************************************/


//清除按钮
static int SolaIdentBtnClear_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int SolaIdentBtnClear_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
                               
static int SolaIdentifyTabItemPassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabItemPassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);

static int SolaIdentifyTabLineDefine_Down(void *pInArg, int iInLen, 
                                          void *pOutArg, int iOutLen);
static int SolaIdentifyTabLineDefine_Up(void *pInArg, int iInLen, 
                                        void *pOutArg, int iOutLen);

static int SolaIdentifyTabLinePassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabLinePassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);

static void SolaIdentMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	
	switch (iOption)
	{    
    	case 0:
            break;
        case 1:
            break;
    	case BACK_DOWN:
        	{
				LOG(LOG_INFO, "#### markParam Deration is %d\n", pSolaMessager->newIdentify.Deraction);
				SetSettingsData((void*)&pSolaMessager->newIdentify, sizeof(MarkParam), SOLA_IDENT);
                SaveSettings(SOLA_IDENT);
                pWnd = CreateSolaWindow();
                SendWndMsg_WindowExit(pFrmSolaIdent);	//发送消息以便退出当前窗体
                SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
            }
    		break;
    	case HOME_DOWN:
    		break;
    		
    	default:
    		break;
	}
}


/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateSolaIdentWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmSolaIdentInit, FrmSolaIdentExit, 
		FrmSolaIdentPaint, FrmSolaIdentLoop, 
		FrmSolaIdentPause, FrmSolaIdentResume,
		NULL);
}

//创建solaIdentify window
GUIWINDOW* CreateSolaIdentWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmSolaIdentInit, FrmSolaIdentExit,
                                   FrmSolaIdentPaint, FrmSolaIdentLoop,
                                   FrmSolaIdentPause, FrmSolaIdentResume,
                                   NULL);
    return pWnd;
}
/*
 * 执行输入的回调函数
 */
static void InputOK(void)
{
	MarkParam *para = &pSolaMessager->newIdentify;
	switch(g_iInputSolaIdentNum)
	{
		case 1:
			memset(para->FiberMark,0,sizeof(para->FiberMark));
			GetIMEInputBuff(para->FiberMark);
			break;
		case 2:
			memset(para->CableMark,0,sizeof(para->CableMark));
			GetIMEInputBuff(para->CableMark);
			break;
		case 3:
			memset(para->Location_A,0,sizeof(para->Location_A));
			GetIMEInputBuff(para->Location_A);
			break;
		case 4:
			memset(para->Location_B,0,sizeof(para->Location_B));
			GetIMEInputBuff(para->Location_B);
			break;
		case 11:
			memset(para->TaskID_Unicode,0,sizeof(para->TaskID_Unicode));
			GetIMEInputBuff(para->TaskID_Unicode);
			break;
		case 12:
			memset(para->Operator_A,0,sizeof(para->Operator_A));
			GetIMEInputBuff(para->Operator_A);
			break;
		case 13:
			memset(para->Operator_B,0,sizeof(para->Operator_B));
			GetIMEInputBuff(para->Operator_B);
			break;
		case 14:
			memset(para->Company,0,sizeof(para->Company));
			GetIMEInputBuff(para->Company);
			break;
		case 15:
			memset(para->Customer,0,sizeof(para->Customer));
			GetIMEInputBuff(para->Customer);
			break;
		case 10:
			memset(para->Note,0,sizeof(para->Note));
			GetIMEInputBuff(para->Note);
			break;
	}
}

/***
  * 功能：
        		Clear 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnClear_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("btn_sure.bmp", 
				pSolaIdentBtnClear, NULL, pSolaIdentLblClear, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}


/***
  * 功能：
        		Clear 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnClear_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	TouchChange("btn_unsure.bmp", 
				pSolaIdentBtnClear, NULL, pSolaIdentLblClear, 1);
	for(i = 0;i<4;i++)
	{
		GuiMemFree(pSolaIdentStrFiberString[i]);
		pSolaIdentStrFiberString[i] = TransString(""); 
		SetLabelText(    
					pSolaIdentStrFiberString[i],pSolaIdentLblFiberString[i]);
		DisplayPicture(pSolaIdentBtnFiberInput[i]);
		DisplayLabel(pSolaIdentLblFiberString[i]);
	}
	for(i = 0;i<5;i++)
	{
		GuiMemFree(pSolaIdentStrTaskString[i]);
		pSolaIdentStrTaskString[i] = TransString(""); 
		SetLabelText(    
					pSolaIdentStrTaskString[i],pSolaIdentLblTaskString[i]);
		DisplayPicture(pSolaIdentBtnTaskInput[i]);
		DisplayLabel(pSolaIdentLblTaskString[i]);
	}
	GuiMemFree(pSolaIdentStrNoteString);
	pSolaIdentStrNoteString = TransString(""); 
	SetLabelText(    
				pSolaIdentStrNoteString, pSolaIdentLblNoteString);
	DisplayPicture(pSolaIdentBtnNoteInput);
	DisplayLabel(pSolaIdentLblNoteString);
	
	memset(&pSolaMessager->newIdentify,0,sizeof(MarkParam));

	RefreshScreen(__FILE__, __func__, __LINE__);
	 
	
	return iReturn;
}

/***
  * 功能：
        		NoteInput 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnNoteInput_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	return iReturn;
}

/***
  * 功能：
        		NoteInput 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnNoteInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	char buff[512] = {0};
	g_iInputSolaIdentNum = 10;

	sprintf(buff, "%s", pSolaMessager->newIdentify.Note);
	IMEInit(buff, 100, 0, ReCreateSolaIdentWindow, InputOK, NULL);
	return iReturn;
}

/***
  * 功能：
        		FiberInput 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnFiberInput_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	
	return iReturn;
}


/***
  * 功能：
        		FiberInput 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnFiberInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int iTemp = (int)pOutArg;
	char buff[128] = {0};
	
	if (iTemp > 4 || iTemp < 1)
	{
		return -1;
	}
	g_iInputSolaIdentNum = iTemp;
	iTemp--;
	switch(g_iInputSolaIdentNum)
	{
		case 1:
			sprintf(buff, "%s", pSolaMessager->newIdentify.FiberMark);
			IMEInit(buff, 8, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 2:
			sprintf(buff, "%s", pSolaMessager->newIdentify.CableMark);
			IMEInit(buff, 8, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 3:
			sprintf(buff, "%s", pSolaMessager->newIdentify.Location_A);
			IMEInit(buff, 8, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 4:
			sprintf(buff, "%s", pSolaMessager->newIdentify.Location_B);
			IMEInit(buff, 8, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;			
	}
	return iReturn;

}


/***
  * 功能：
        		TaskInput 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnTaskInput_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	
	return iReturn;
}


/***
  * 功能：
        		TaskInput 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnTaskInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int iTemp = (int)pOutArg;
	char buff[512] = {0};
	if (iTemp > 5 || iTemp < 1)
	{
		return -1;
	}
	g_iInputSolaIdentNum = iTemp+10;
	iTemp--;
	switch(g_iInputSolaIdentNum)
	{
		case 11:
			sprintf(buff, "%s", pSolaMessager->newIdentify.TaskID_Unicode);
			IMEInit(buff, 10, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 12:
			sprintf(buff, "%s", pSolaMessager->newIdentify.Operator_A);
			IMEInit(buff, 10, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 13:
			sprintf(buff, "%s", pSolaMessager->newIdentify.Operator_B);
			IMEInit(buff, 10, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 14:
			sprintf(buff, "%s", pSolaMessager->newIdentify.Company);
			IMEInit(buff, 10, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
		case 15:
			sprintf(buff, "%s", pSolaMessager->newIdentify.Customer);
			IMEInit(buff, 10, 0, ReCreateSolaIdentWindow, InputOK, NULL);
			break;
			
	}
	return iReturn;
}

/***
  * 功能：
        		方向AB 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnFiberDirectionAB_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("id_opt_select.bmp", 
				pSolaIdentBtnFiberdirectionAB, NULL, NULL, 0);
	TouchChange("id_opt_unselect.bmp", 
				pSolaIdentBtnFiberdirectionBA, NULL, NULL, 0);
	pSolaMessager->newIdentify.Deraction = 0;
	iDistABFlag = 0;
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}

/***
  * 功能：
        		方向AB 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnFiberDirectionAB_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	return iReturn;
}

/***
  * 功能：
        		方向BA 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnFiberDirectionBA_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("id_opt_unselect.bmp", 
				pSolaIdentBtnFiberdirectionAB, NULL, NULL, 0);
	TouchChange("id_opt_select.bmp", 
				pSolaIdentBtnFiberdirectionBA, NULL, NULL, 0);
	//memset(&pSolaIdentParam->Deraction,0,sizeof(pSolaIdentParam->Deraction));
    pSolaMessager->newIdentify.Deraction = 1;
	iDistABFlag = 1;
	
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

/***
  * 功能：
        		方向AB 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int SolaIdentBtnFiberDirectionBA_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	return iReturn;
}
/***
  * 功能：
        		窗体frmSolaIdent 的初始化函数，建立窗体控件、
        		注册消息处理
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaIdentInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	int i = 0;
	//初始化文本资源
	//如果GUI存在多国语言，在此处获得对应语言的文
	//本资源
	//初始化文本资源必须在建立文本CreateText()或建立标
	//签CreateLabel()之前
	/**********************************************************/
	//得到当前窗体对象
	pFrmSolaIdent = (GUIWINDOW *) pWndObj;
	SolaIdentTextRes_Init(NULL, 0, NULL, 0);

	//建立桌面上的控件
    pFrmSolaIdentBG = CreatePhoto("bg_global");
    identifyTitlePicture =CreatePhoto("otdr_top1f");
    lineDefineTitlePicture =CreatePhoto("otdr_top2");
    linePassThresholdTitlePicture =CreatePhoto("otdr_top3");
    itemPassThresholdTitlePicture =CreatePhoto("otdr_top4");
    
    pSolaSettingLblIdentifyTitle = CreateLabel(0, 22, 100, 24, pSolaSettingStrIdentifyTitle);
    pSolaSettingLblLineDefineTitle = CreateLabel(100, 22, 100, 24, pSolaSettingStrLineDefineTitle);
    pSolaSettingLblLinePassThresholdTitle = CreateLabel(200, 22, 100, 24, pSolaSettingStrLinePassThresholdTitle);
    pSolaSettingLblItemPassThresholdTitle = CreateLabel(300, 22, 100, 24, pSolaSettingStrItemPassThresholdTitle);
    
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaSettingLblIdentifyTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblIdentifyTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLineDefineTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLineDefineTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLinePassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLinePassThresholdTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblItemPassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblItemPassThresholdTitle);
    
	
	pFrmSolaIdentMenu = CreateWndMenu1(0, 0, NULL, (UINT16)(MENU_BACK|~MENU_HOME),
									-1, 0, 40, SolaIdentMenuCallBack);

    CreateLoopPhoto(pSolaIdentBtnFiberInput,"btn_mark_input",0,0,4);

	for(i = 0; i<4; i++)
	{
		pSolaIdentLblFiberString[i] = CreateLabel(180, 100+i*30,135,25,   
										  pSolaIdentStrFiberString[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberString[i]); 
	}
    
    CreateLoopPhoto(pSolaIdentBtnTaskInput,"btn_mark_input1",0,0,5);
	for(i = 0; i<5; i++)
	{
		pSolaIdentLblTaskString[i] = CreateLabel(508, 100+i*30,135,25,   
										  pSolaIdentStrTaskString[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTaskString[i]);
	}
    CreateLoopPhoto(pSolaIdentBtnMeasureInput,"btn_mark_input2",0,0,4);

	//清除
    pSolaIdentBtnClear = CreatePhoto("btn_unsure");
    //注释
    pSolaIdentBtnNoteInput = CreatePhoto("btn_mark_note1");
	pSolaIdentLblNoteString = CreateLabel(410, 322,230,70,     
								pSolaIdentStrNoteString);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblNoteString); 
	
	//建立按钮区的控件
	
    pSolaIdentBtnFiberdirectionAB = CreatePhoto("id_opt_select");
    pSolaIdentBtnFiberdirectionBA = CreatePhoto("id_opt_unselect");


	/******************建立按钮区的标签******************/
	//桌面标签
	pSolaIdentLblFiber = CreateLabel(35,55,300,24,   
									  pSolaIdentStrFiber);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiber);
	pSolaIdentLblFiberMark = CreateLabel(80,100,100,24,      
								pSolaIdentStrFiberMark);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberMark);
	pSolaIdentLblCableMark = CreateLabel(80,130,100,24,     
								pSolaIdentStrCableMark);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblCableMark);
	pSolaIdentLblFiberLocation1 = CreateLabel(80,160, 100, 24,   
									  pSolaIdentStrFiberLocation1);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberLocation1);
	pSolaIdentLblFiberLocation2 = CreateLabel(80, 190, 100, 24,   
									  pSolaIdentStrFiberLocation2);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberLocation2);
	pSolaIdentLblFiberdirection = CreateLabel(80, 220, 100, 24,   
									 pSolaIdentStrFiberdirection);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberdirection);
	pSolaIdentLblFiberdirectionAB = CreateLabel(205, 225, 50, 24,  
									 pSolaIdentStrFiberdirectionAB);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberdirectionAB);
	pSolaIdentLblFiberdirectionBA = CreateLabel(285, 225, 50, 24,   
									 pSolaIdentStrFiberdirectionBA);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblFiberdirectionBA);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiber);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiberMark);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblCableMark);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiberLocation1);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiberLocation2);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiberdirection);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiberdirectionAB);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblFiberdirectionBA);
	
	//测量栏标签
	pSolaIdentLblMeas = CreateLabel(35, 290, 300, 24,   
							 pSolaIdentStrMeas);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeas);
	pSolaIdentLblMeasDate = CreateLabel(80, 335, 80, 24,   
								 pSolaIdentStrMeasDate);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasDate);
	pSolaIdentLblMeasTime = CreateLabel(80, 365, 80, 24,  
								 pSolaIdentStrMeasTime);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasTime);
	pSolaIdentLblMeasModule = CreateLabel(80, 395, 80, 24,   
									 pSolaIdentStrMeasModule);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasModule);
	pSolaIdentLblMeasNum = CreateLabel(80, 425, 80, 24,    
								pSolaIdentStrMeasNum);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasNum);
	
	pSolaIdentLblMeasDateString = CreateLabel(153, 335, 150, 24,   
									 pSolaIdentStrMeasDateString);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasDateString);
	pSolaIdentLblMeasTimeString = CreateLabel(153, 365, 150, 24,   
									 pSolaIdentStrMeasTimeString);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasTimeString);

	pSolaIdentLblMeasModuleVal = CreateLabel(153, 395, 150, 24,   
									 pSolaIdentStrMeasModuleVal);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasModuleVal);
	pSolaIdentLblMeasNumVal = CreateLabel(153, 425, 150, 24,   
									 pSolaIdentStrMeasNumVal);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblMeasNumVal);
	
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblMeas);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblMeasDate);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblMeasTime);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblMeasModule);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblMeasNum);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSolaIdentLblMeasDateString);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSolaIdentLblMeasTimeString);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSolaIdentLblMeasModuleVal);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSolaIdentLblMeasNumVal);
	
	//任务栏标签
	pSolaIdentLblTask = CreateLabel(360, 55, 300, 24,  
							 pSolaIdentStrTask);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTask);
	pSolaIdentLblTaskID = CreateLabel(410, 100, 90, 24,   
								 pSolaIdentStrTaskID);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTaskID);
	pSolaIdentLblTaskOperatorA = CreateLabel(410, 130, 90, 24,   
									 pSolaIdentStrTaskOperatorA);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTaskOperatorA);
	pSolaIdentLblTaskOperatorB = CreateLabel(410, 160, 90, 24,   
									 pSolaIdentStrTaskOperatorB);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTaskOperatorB);
	pSolaIdentLblTaskCompany = CreateLabel(410, 190, 90, 24,   
									 pSolaIdentStrTaskCompany);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTaskCompany);
	pSolaIdentLblTaskClient = CreateLabel(410, 220, 90, 24,   
								 pSolaIdentStrTaskClient);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblTaskClient);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblTask);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblTaskID);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblTaskOperatorA);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblTaskOperatorB);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblTaskCompany);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblTaskClient);

	//注释以下标签
	pSolaIdentLblNote = CreateLabel(360, 290, 300, 24,  
							 pSolaIdentStrNote);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaIdentLblNote);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblNote);
	
    pSolaIdentLblClear = CreateLabel(525, 430, 142, 43,
							 pSolaIdentStrClear);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaIdentLblClear);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pSolaIdentLblClear);
	
	//注册窗体控件,只对那些需要接收事件的控件进行
	//即如果该控件没有或者不需要响应输入事件，可
	//以无需注册
	//注册窗体控件必须在注册消息处理函数之前进行
	//*********************************************************/
	//注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSolaIdent, 
	              pFrmSolaIdent);
	//使用默认曲线按钮注册
	//清除按钮注册
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentBtnClear, 
	              pFrmSolaIdent);
	              
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), itemPassThresholdTitlePicture,pFrmSolaIdent);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), lineDefineTitlePicture,pFrmSolaIdent);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linePassThresholdTitlePicture,pFrmSolaIdent);

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentBtnNoteInput, 
	              pFrmSolaIdent);
	for(i = 0;i<4;i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentBtnFiberInput[i], 
		              pFrmSolaIdent);
	}
	for(i = 0;i<5;i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentBtnTaskInput[i], 
		              pFrmSolaIdent);
	}
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentLblFiberdirectionAB, 
	              pFrmSolaIdent);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentLblFiberdirectionBA, 
	              pFrmSolaIdent);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentBtnFiberdirectionAB, 
	              pFrmSolaIdent);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentBtnFiberdirectionBA, 
	              pFrmSolaIdent);

	AddWndMenuToComp1(pFrmSolaIdentMenu,pFrmSolaIdent);
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();
	//使用默认曲线按钮
	//清除按钮
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentBtnClear, 
	               SolaIdentBtnClear_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentBtnClear, 
	               SolaIdentBtnClear_Down, NULL, 0, pMsg);
	//注释响应
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentBtnNoteInput, 
	               SolaIdentBtnNoteInput_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentBtnNoteInput, 
	               SolaIdentBtnNoteInput_Down, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Up, NULL, 0, pMsg);

	for(i = 0;i<4;i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentBtnFiberInput[i], 
		               SolaIdentBtnFiberInput_Up, (void*)(i+1), 0, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentBtnFiberInput[i], 
		               SolaIdentBtnFiberInput_Down, (void*)(i+1), 0, pMsg);
	}
	for(i = 0;i<5;i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentBtnTaskInput[i], 
		               SolaIdentBtnTaskInput_Up, (void*)(i+1), 0, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentBtnTaskInput[i], 
		               SolaIdentBtnTaskInput_Down, (void*)(i+1), 0, pMsg);
	}

	LoginMessageReg(GUIMESSAGE_TCH_DOWN,pSolaIdentLblFiberdirectionAB, 
	               SolaIdentBtnFiberDirectionAB_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pSolaIdentLblFiberdirectionAB, 
	               SolaIdentBtnFiberDirectionAB_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN,pSolaIdentBtnFiberdirectionAB, 
	               SolaIdentBtnFiberDirectionAB_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pSolaIdentBtnFiberdirectionAB, 
	               SolaIdentBtnFiberDirectionAB_Up, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentLblFiberdirectionBA, 
	               SolaIdentBtnFiberDirectionBA_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pSolaIdentLblFiberdirectionAB, 
	               SolaIdentBtnFiberDirectionBA_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentBtnFiberdirectionBA, 
	               SolaIdentBtnFiberDirectionBA_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pSolaIdentLblFiberdirectionBA, 
	               SolaIdentBtnFiberDirectionBA_Up, NULL, 0, pMsg);

	LoginWndMenuToMsg1(pFrmSolaIdentMenu, pFrmSolaIdent);
	return iReturn;
}
/***
  * 功能：
        		窗体frmSolaIdent 的退出函数，释放所有资源
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaIdentExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	int i = 0;
	//得到当前窗体对象
	pFrmSolaIdent = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmSolaIdent);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pFrmSolaIdentBG);
	DestroyPicture(&identifyTitlePicture);
    DestroyPicture(&lineDefineTitlePicture);
    DestroyPicture(&linePassThresholdTitlePicture);
    DestroyPicture(&itemPassThresholdTitlePicture);
    
    DestroyLabel(&pSolaSettingLblIdentifyTitle);
    DestroyLabel(&pSolaSettingLblLineDefineTitle);
    DestroyLabel(&pSolaSettingLblLinePassThresholdTitle);
    DestroyLabel(&pSolaSettingLblItemPassThresholdTitle);

	for(i = 0;i<4;i++)
	{
		DestroyPicture(&pSolaIdentBtnFiberInput[i]);
		DestroyLabel(&pSolaIdentLblFiberString[i]);
        DestroyPicture(&pSolaIdentBtnMeasureInput[i]);
	}
	for(i = 0;i<5;i++)
	{
		DestroyPicture(&pSolaIdentBtnTaskInput[i]);
		DestroyLabel(&pSolaIdentLblTaskString[i]);
	}
	//DestroyPicture(&pSolaIdentBtnDefault);
	DestroyPicture(&pSolaIdentBtnClear);
	DestroyPicture(&pSolaIdentBtnNoteInput);
	DestroyPicture(&pSolaIdentBtnFiberdirectionAB);
	DestroyPicture(&pSolaIdentBtnFiberdirectionBA);
	//按钮标签销毁
	DestroyLabel(&pSolaIdentLblFiber);
	
	//桌面标签销毁
	DestroyLabel(&pSolaIdentLblFiberMark);
	DestroyLabel(&pSolaIdentLblCableMark);
	DestroyLabel(&pSolaIdentLblFiberLocation1);
	DestroyLabel(&pSolaIdentLblFiberLocation2);
	DestroyLabel(&pSolaIdentLblFiberdirection);
	DestroyLabel(&pSolaIdentLblFiberdirectionAB);
	DestroyLabel(&pSolaIdentLblFiberdirectionBA);
	//测量栏标签销毁
	DestroyLabel(&pSolaIdentLblMeas); 
	DestroyLabel(&pSolaIdentLblMeasDate); 
	DestroyLabel(&pSolaIdentLblMeasTime); 
	DestroyLabel(&pSolaIdentLblMeasModule); 
	DestroyLabel(&pSolaIdentLblMeasNum); 
	DestroyLabel(&pSolaIdentLblMeasModuleVal); 
	DestroyLabel(&pSolaIdentLblMeasNumVal); 

	DestroyLabel(&pSolaIdentLblMeasDateString); 
	DestroyLabel(&pSolaIdentLblMeasTimeString); 
	//任务栏标签销毁
	DestroyLabel(&pSolaIdentLblTask); 
	DestroyLabel(&pSolaIdentLblTaskID); 
	DestroyLabel(&pSolaIdentLblTaskOperatorA); 
	DestroyLabel(&pSolaIdentLblTaskOperatorB); 
	DestroyLabel(&pSolaIdentLblTaskCompany); 
	DestroyLabel(&pSolaIdentLblTaskClient); 
	//注释以下标签销毁
	DestroyLabel(&pSolaIdentLblNote); 
	//DestroyLabel(&pSolaIdentLblDefault); 
	DestroyLabel(&pSolaIdentLblClear); 
	DestroyLabel(&pSolaIdentLblNoteString); 

	DestroyWndMenu1(&pFrmSolaIdentMenu);
	
	//文本内容销毁
	SolaIdentTextRes_Exit(NULL, 0, NULL, 0);

           return iReturn;
}


/***
  * 功能：
        		窗体frmSolaIdent 的绘制函数，绘制整个窗体
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaIdentPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	//得到当前窗体对象
	pFrmSolaIdent = (GUIWINDOW *) pWndObj;
	//按钮显示
	DisplayPicture(pFrmSolaIdentBG);
    DisplayPicture(identifyTitlePicture);
    DisplayPicture(lineDefineTitlePicture);
    DisplayPicture(linePassThresholdTitlePicture);
    DisplayPicture(itemPassThresholdTitlePicture);

    DisplayLabel(pSolaSettingLblIdentifyTitle);
    DisplayLabel(pSolaSettingLblLineDefineTitle);
    DisplayLabel(pSolaSettingLblLinePassThresholdTitle);
    DisplayLabel(pSolaSettingLblItemPassThresholdTitle);
    
	for(i = 0;i<4;i++)
	{
		DisplayPicture(pSolaIdentBtnFiberInput[i]);
		DisplayLabel(pSolaIdentLblFiberString[i]);
        DisplayPicture(pSolaIdentBtnMeasureInput[i]);
	}
	for(i = 0;i<5;i++)
	{
		DisplayPicture(pSolaIdentBtnTaskInput[i]);
		DisplayLabel(pSolaIdentLblTaskString[i]);
	}
	
	DisplayPicture(pSolaIdentBtnClear);
	DisplayPicture(pSolaIdentBtnNoteInput);
	LOG(LOG_INFO, "pSolaIdentParam->Deraction is %d \n", pSolaMessager->newIdentify.Deraction);

	if(pSolaMessager->newIdentify.Deraction) //b->a
	{
		TouchChange("id_opt_unselect.bmp", 
					pSolaIdentBtnFiberdirectionAB, NULL, NULL, 0);
		TouchChange("id_opt_select.bmp", 
					pSolaIdentBtnFiberdirectionBA, NULL, NULL, 0);
	}
	else	//a->b
	{
		TouchChange("id_opt_select.bmp", 
					pSolaIdentBtnFiberdirectionAB, NULL, NULL, 0);
		TouchChange("id_opt_unselect.bmp", 
					pSolaIdentBtnFiberdirectionBA, NULL, NULL, 0);
	}

	DisplayPicture(pSolaIdentBtnFiberdirectionAB);
	DisplayPicture(pSolaIdentBtnFiberdirectionBA);
	//按钮标签显示
	DisplayLabel(pSolaIdentLblFiber);
	
	//桌面标签显示
	DisplayLabel(pSolaIdentLblFiberMark);
	DisplayLabel(pSolaIdentLblCableMark);
	DisplayLabel(pSolaIdentLblFiberLocation1);
	DisplayLabel(pSolaIdentLblFiberLocation2);
	DisplayLabel(pSolaIdentLblFiberdirection);
	DisplayLabel(pSolaIdentLblFiberdirectionAB);
	DisplayLabel(pSolaIdentLblFiberdirectionBA);
	//测量栏标签显示
	DisplayLabel(pSolaIdentLblMeas); 
	DisplayLabel(pSolaIdentLblMeasDate); 
	DisplayLabel(pSolaIdentLblMeasTime); 
	DisplayLabel(pSolaIdentLblMeasModule); 
	DisplayLabel(pSolaIdentLblMeasNum); 
	DisplayLabel(pSolaIdentLblMeasModuleVal); 
	DisplayLabel(pSolaIdentLblMeasNumVal); 
	
	DisplayLabel(pSolaIdentLblMeasDateString); 
	DisplayLabel(pSolaIdentLblMeasTimeString); 
	//任务栏标签显示
	DisplayLabel(pSolaIdentLblTask); 
	DisplayLabel(pSolaIdentLblTaskID); 
	DisplayLabel(pSolaIdentLblTaskOperatorA); 
	DisplayLabel(pSolaIdentLblTaskOperatorB); 
	DisplayLabel(pSolaIdentLblTaskCompany); 
	DisplayLabel(pSolaIdentLblTaskClient); 
	//注释以下标签显示
	DisplayLabel(pSolaIdentLblNote); 
	//DisplayLabel(pSolaIdentLblDefault); 
	DisplayLabel(pSolaIdentLblClear); 
	DisplayLabel(pSolaIdentLblNoteString); 
	DisplayWndMenu1(pFrmSolaIdentMenu);

	SetPowerEnable(1, 1);
	
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}


/***
  * 功能：
        	窗体frmSolaIdent 的循环函数，进行窗体循环
  * 参数：
       	 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaIdentLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	
	return iReturn;
}


/***
  * 功能：
        窗体frmSolaIdent 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaIdentPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体frmSolaIdent 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaIdentResume(void *pWndObj)
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
static int SolaIdentTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	MarkParam *para = &pSolaMessager->newIdentify;
	if(getSolaMeasureDate(para->Test_date, para->Test_time, pCurSystemSet->uiTimeShowFlag))
	{
    	//临时变量定义
    	DEVFD *pDevFd = NULL;
    	RTCVAL *pRtc = NULL;
    	//获取当前系统时间
    	pDevFd = GetGlb_DevFd();
    	pRtc = Rtc_GetTime(pDevFd->iRtcDev);
    	pRtc->tm_year += 1900;
    	pRtc->tm_mon++;
    	if(pCurSystemSet->uiTimeShowFlag == 0)
        {
        	sprintf(para->Test_date,"%.2d/%.2d/%.4d",
			pRtc->tm_mday,pRtc->tm_mon,pRtc->tm_year);
        }
        else if(pCurSystemSet->uiTimeShowFlag == 1)
        {
        	sprintf(para->Test_date,"%.2d/%.2d/%.4d",
			pRtc->tm_mon,pRtc->tm_mday,pRtc->tm_year);
        }
        else
        {
        	sprintf(para->Test_date,"%.4d/%.2d/%.2d",
			pRtc->tm_year,pRtc->tm_mon,pRtc->tm_mday);
        }
    	sprintf(para->Test_time,"%.2d:%.2d:%.2d",
    			pRtc->tm_hour,pRtc->tm_min,pRtc->tm_sec);
    			
        GuiMemFree(pRtc);
	}
	//初始化状态栏上的文本
	//...
	//初始化桌面上的文本
	//...
	//初始化桌面按钮的文本
	//...
    pSolaSettingStrIdentifyTitle = TransString("SOLA_IDENTITY");
    pSolaSettingStrLineDefineTitle = TransString("SOLA_LINE_DEFINE");
    pSolaSettingStrLinePassThresholdTitle = TransString("SOLA_LINEPASS");
    pSolaSettingStrItemPassThresholdTitle = TransString("SOLA_ITEMPASS");
	pSolaIdentStrClear = TransString("IDETY_CLEAR"); 
	//初始化桌面标签上的文本
	pSolaIdentStrFiber = TransString("IDETY_FIBER"); 
	pSolaIdentStrFiberMark = TransString("IDETY_FIBER_MARK"); 
	pSolaIdentStrCableMark = TransString("IDETY_CABLE_MARK"); 
	pSolaIdentStrFiberLocation1 = TransString("IDETY_LOCATION_A"); 
	pSolaIdentStrFiberLocation2 = TransString("IDETY_LOCATION_B"); 
	pSolaIdentStrFiberdirection = TransString("IDETY_DIRECTION"); 
	pSolaIdentStrFiberdirectionAB = TransString("A->B"); 
	pSolaIdentStrFiberdirectionBA = TransString("B->A"); 

	//初始化测量栏标签文本
	pSolaIdentStrMeas = TransString("IDETY_MEASURE"); 
	pSolaIdentStrMeasDate = TransString("IDETY_DATE"); 
	pSolaIdentStrMeasTime = TransString("IDETY_TIME"); 
	pSolaIdentStrMeasModule = TransString("IDETY_MODULE"); 
	pSolaIdentStrMeasNum = TransString("OTDR_ABOUT_SERIALNUM"); 

    //初始化任务栏标签文本
    pSolaIdentStrTask = TransString("IDETY_TASK"); 
    pSolaIdentStrTaskID = TransString("IDETY_TASK_ID"); 
    pSolaIdentStrTaskOperatorA = TransString("IDETY_OPER_A"); 
    pSolaIdentStrTaskOperatorB = TransString("IDETY_OPER_B"); 
    pSolaIdentStrTaskCompany = TransString("IDETY_COMPANY"); 
    pSolaIdentStrTaskClient = TransString("IDETY_CUSTOMER"); 
    pSolaIdentStrNote = TransString("IDETY_NOTE"); 
    //初始化注释以下标签文本
    pSolaIdentStrMeasDateString = TransString(para->Test_date); 
    pSolaIdentStrMeasTimeString = TransString(para->Test_time); 
    
	pSolaIdentStrNoteString = TransString(para->Note); 
	//...
	pSolaIdentStrFiberString[0] = TransString(para->FiberMark); 
	pSolaIdentStrFiberString[1] = TransString(para->CableMark); 
	pSolaIdentStrFiberString[2] = TransString(para->Location_A); 
	pSolaIdentStrFiberString[3] = TransString(para->Location_B); 
	
	pSolaIdentStrTaskString[0] = TransString(para->TaskID_Unicode); 
	pSolaIdentStrTaskString[1] = TransString(para->Operator_A); 
	pSolaIdentStrTaskString[2] = TransString(para->Operator_B); 
	pSolaIdentStrTaskString[3] = TransString(para->Company); 
	pSolaIdentStrTaskString[4] = TransString(para->Customer);
	
	GenerateSerialNumber(para->Serial_num, pCurSN);
	sprintf(para->Module,"%s","SOLA");
	pSolaIdentStrMeasNumVal = TransString(para->Serial_num);
	pSolaIdentStrMeasModuleVal = TransString(para->Module);
	

	//memset(&pSolaIdentParam->Deraction,0,sizeof(pSolaIdentParam->Deraction));
    //pSolaIdentParam->Deraction = pSolaIdentParam->Deraction;
	//strcpy(pSolaIdentParam->Deraction,"A->B");
	#if 0
	pSolaIdentStrNoteString = TransString("0"); 
		
	pSolaIdentStrMeasDateString = TransString("0"); 
	pSolaIdentStrMeasTimeString = TransString("0"); 
	//...
	pSolaIdentStrFiberString[0] = TransString("0"); 
	pSolaIdentStrFiberString[1] = TransString("0"); 
	pSolaIdentStrFiberString[2] = TransString("0"); 
	pSolaIdentStrFiberString[3] = TransString("0"); 
	
	pSolaIdentStrTaskString[0] = TransString("0"); 
	pSolaIdentStrTaskString[1] = TransString("0"); 
	pSolaIdentStrTaskString[2] = TransString("0"); 
	pSolaIdentStrTaskString[3] = TransString("0"); 
	pSolaIdentStrTaskString[4] = TransString("0"); 
	#endif
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
static int SolaIdentTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i = 0;
	//释放状态栏上的文本
	//...
	//释放按钮区的文本
	//...
    GuiMemFree(pSolaSettingStrIdentifyTitle);
    GuiMemFree(pSolaSettingStrLineDefineTitle);
    GuiMemFree(pSolaSettingStrLinePassThresholdTitle);
    GuiMemFree(pSolaSettingStrItemPassThresholdTitle);
	//GuiMemFree(pSolaIdentStrDefault);
	GuiMemFree(pSolaIdentStrClear ); 
	//释放桌面上的文本
	//...
	GuiMemFree(pSolaIdentStrFiber ); 
	GuiMemFree(pSolaIdentStrFiberMark ); 
	GuiMemFree(pSolaIdentStrCableMark ); 
	GuiMemFree(pSolaIdentStrFiberLocation1 ); 
	GuiMemFree(pSolaIdentStrFiberLocation2) ; 
	GuiMemFree(pSolaIdentStrFiberdirection ); 
	GuiMemFree(pSolaIdentStrFiberdirectionAB ); 
	GuiMemFree(pSolaIdentStrFiberdirectionBA) ; 
	GuiMemFree(pSolaIdentStrMeas ); 
	GuiMemFree(pSolaIdentStrMeasDate ); 
	GuiMemFree(pSolaIdentStrMeasTime) ; 
	GuiMemFree(pSolaIdentStrMeasModule) ; 
	GuiMemFree(pSolaIdentStrMeasNum ); 
	GuiMemFree(pSolaIdentStrMeasModuleVal ); 
	GuiMemFree(pSolaIdentStrMeasNumVal ); 
	
	GuiMemFree(pSolaIdentStrTask ); 
	GuiMemFree(pSolaIdentStrTaskID ); 
	GuiMemFree(pSolaIdentStrTaskOperatorA ); 
	GuiMemFree(pSolaIdentStrTaskOperatorB) ; 
	GuiMemFree(pSolaIdentStrTaskCompany) ; 
	GuiMemFree(pSolaIdentStrTaskClient) ; 
	GuiMemFree(pSolaIdentStrNote ); 
	GuiMemFree(pSolaIdentStrMeasDateString);
	GuiMemFree(pSolaIdentStrMeasTimeString);	
	//释放信息栏上的文本
	for(i = 0;i<4;i++)
	{
		GuiMemFree(pSolaIdentStrFiberString[i]);
	}
	for(i = 0;i<5;i++)
	{
		GuiMemFree(pSolaIdentLblTaskString[i]);
	}
	GuiMemFree(pSolaIdentStrNoteString);	
	//...

	return iReturn;
}

//切换到LineDefine窗体
static int SolaIdentifyTabLineDefine_Down(void *pInArg, int iInLen, 
                                          void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTabLineDefine_Up(void *pInArg, int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    SetSettingsData((void*)&pSolaMessager->newIdentify, sizeof(MarkParam), SOLA_IDENT);
    SaveSettings(SOLA_IDENT);

    GUIWINDOW *pWnd = CreateSolaLineDefineWindow();
    SendWndMsg_WindowExit(pFrmSolaIdent);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

//切换到ItemPass窗体
static int SolaIdentifyTabItemPassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTabItemPassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    SetSettingsData((void*)&pSolaMessager->newIdentify, sizeof(MarkParam), SOLA_IDENT);
    SaveSettings(SOLA_IDENT);

    GUIWINDOW *pWnd = CreateSolaItemPassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaIdent);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

//切换到LinePass窗体
static int SolaIdentifyTabLinePassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTabLinePassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    SetSettingsData((void*)&pSolaMessager->newIdentify, sizeof(MarkParam), SOLA_IDENT);
    SaveSettings(SOLA_IDENT);

    GUIWINDOW *pWnd = CreateSolaLinePassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaIdent);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}
