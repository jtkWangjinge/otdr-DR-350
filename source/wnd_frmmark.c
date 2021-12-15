/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmMark.c
* 摘    要：  实现主窗体FrmMark 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/8
*
*******************************************************************************/

#include "wnd_frmmark.h"

/**************************************************************
* 	为实现窗体wnd_FrmMark而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmime.h"
#include "wnd_frmabout.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmotdr.h"
#include "wnd_frmotdrsave.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"
#include "app_systemsettings.h"
#include "guiphoto.h"
/**************************************************************
* 			定义wnd_FrmMark中内部使用的数据结构
**************************************************************/

//引用的全局变量
static int iDistABFlag = 0;		//记录当前是0:a->b 还是1:b->a
//自定义的全局变量
static int g_iInputMarkNum = -1;
MarkParam g_markParam;

extern SerialNum* pCurSN;
extern PSYSTEMSET pCurSystemSet;
extern MarkParam* pMarkParam;
/**************************************************************
* 					窗体wnd_FrmMark中的窗体控件
**************************************************************/

static GUIWINDOW *pFrmMark = NULL;
static WNDMENU1 *pFrmMarkMenu = NULL;
/*********************桌面背景定义************************/
static GUIPICTURE *pFrmMarkBG = NULL;
static GUILABEL *pFrmMarkLabel = NULL;
static GUICHAR *pFrmMarkString = NULL;
/*********************右侧控件定义************************/

/**********************桌面控件定义***********************/
//桌面控件
static GUIPICTURE *pMarkBtnFiberInput[4] = {NULL} ;
static GUIPICTURE *pMarkBtnTaskInput[5] = {NULL};
static GUIPICTURE *pMarkBtnMeasureInput[5] = {NULL};
static GUIPICTURE *pMarkBtnClear = NULL ;
static GUIPICTURE *pMarkBtnNoteInput = NULL ;
static GUIPICTURE *pMarkBtnFiberdirectionAB = NULL ;
static GUIPICTURE *pMarkBtnFiberdirectionBA = NULL ;

/************************桌面标签定义*********************/
//桌面标签
static GUILABEL *pMarkLblFiber = NULL; 
static GUILABEL *pMarkLblFiberMark = NULL; 
static GUILABEL *pMarkLblCableMark = NULL; 
static GUILABEL *pMarkLblFiberLocation1 = NULL; 
static GUILABEL *pMarkLblFiberLocation2 = NULL; 
static GUILABEL *pMarkLblFiberdirection = NULL; 
static GUILABEL *pMarkLblFiberdirectionAB = NULL; 
static GUILABEL *pMarkLblFiberdirectionBA = NULL; 
static GUILABEL *pMarkLblFiberString[4] = {NULL}; 
//测量栏标签
static GUILABEL *pMarkLblMeas = NULL; 
static GUILABEL *pMarkLblMeasDate = NULL; 
static GUILABEL *pMarkLblMeasTime = NULL; 
static GUILABEL *pMarkLblMeasModule = NULL; 
static GUILABEL *pMarkLblMeasNum = NULL; 
static GUILABEL *pMarkLblMeasModuleVal = NULL; 
static GUILABEL *pMarkLblMeasNumVal = NULL; 
static GUILABEL *pMarkLblMeasDateString = NULL; 
static GUILABEL *pMarkLblMeasTimeString = NULL; 

//任务栏标签
static GUILABEL *pMarkLblTask = NULL; 
static GUILABEL *pMarkLblTaskID = NULL; 
static GUILABEL *pMarkLblTaskOperatorA = NULL; 
static GUILABEL *pMarkLblTaskOperatorB = NULL; 
static GUILABEL *pMarkLblTaskCompany = NULL; 
static GUILABEL *pMarkLblTaskClient = NULL; 
static GUILABEL *pMarkLblTaskString[5] = {NULL}; 

//注释以下标签
static GUILABEL *pMarkLblNote = NULL; 
static GUILABEL *pMarkLblClear = NULL; 
static GUILABEL *pMarkLblNoteString = NULL; 

/************************桌面标签文本*********************/
//桌面标签文本
static GUICHAR *pMarkStrFiber = NULL; 
static GUICHAR *pMarkStrFiberMark = NULL; 
static GUICHAR *pMarkStrCableMark = NULL; 
static GUICHAR *pMarkStrFiberLocation1 = NULL; 
static GUICHAR *pMarkStrFiberLocation2 = NULL; 
static GUICHAR *pMarkStrFiberdirection = NULL; 
static GUICHAR *pMarkStrFiberdirectionAB = NULL; 
static GUICHAR *pMarkStrFiberdirectionBA = NULL; 
static GUICHAR *pMarkStrFiberString[4] = {NULL}; 
//测量栏标签文本
static GUICHAR *pMarkStrMeas = NULL; 
static GUICHAR *pMarkStrMeasDate = NULL; 
static GUICHAR *pMarkStrMeasTime = NULL; 
static GUICHAR *pMarkStrMeasModule = NULL; 
static GUICHAR *pMarkStrMeasNum = NULL; 
static GUICHAR *pMarkStrMeasModuleVal = NULL; 
static GUICHAR *pMarkStrMeasNumVal = NULL; 
static GUICHAR *pMarkStrMeasDateString = NULL; 
static GUICHAR *pMarkStrMeasTimeString = NULL; 

//任务栏标签文本
static GUICHAR *pMarkStrTask = NULL; 
static GUICHAR *pMarkStrTaskID = NULL; 
static GUICHAR *pMarkStrTaskOperatorA = NULL; 
static GUICHAR *pMarkStrTaskOperatorB = NULL; 
static GUICHAR *pMarkStrTaskCompany = NULL; 
static GUICHAR *pMarkStrTaskClient = NULL; 
static GUICHAR *pMarkStrTaskString[5] = {NULL}; 

//注释以下标签文本
static GUICHAR *pMarkStrNote = NULL; 
static GUICHAR *pMarkStrNoteString = NULL; 
static GUICHAR *pMarkStrClear = NULL; 

/**************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int MarkTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int MarkTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/**************************************************************
* 				控件函数声明
**************************************************************/
//清除按钮
static int MarkBtnClear_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int MarkBtnClear_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);

static void MarkMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	
	switch (iOption)
	{
	case BACK_DOWN:
		LOG(LOG_INFO, "#### markParam Deration is %d\n", pMarkParam->Deraction);
		SetSettingsData((void*)pMarkParam, sizeof(MarkParam), OTDR_MARK_SET);
        SaveSettings(OTDR_MARK_SET);
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmOtdrSaveInit, FrmOtdrSaveExit, 
							FrmOtdrSavePaint, FrmOtdrSaveLoop, 
							FrmOtdrSavePause, FrmOtdrSaveResume,
							NULL);			 
		SendWndMsg_WindowExit(pFrmMark);  
		SendSysMsg_ThreadCreate(pWnd); 
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
static void ReCreateMarkWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmMarkInit, FrmMarkExit, 
		FrmMarkPaint, FrmMarkLoop, 
		FrmMarkPause, FrmMarkResume,
		NULL);
}

/*
 * 执行输入的回调函数
 */
static void InputOK(void)
{
	
	switch(g_iInputMarkNum)
	{
		case 1:
			memset(pMarkParam->FiberMark,0,sizeof(pMarkParam->FiberMark));
			GetIMEInputBuff(pMarkParam->FiberMark);
			break;
		case 2:
			memset(pMarkParam->CableMark,0,sizeof(pMarkParam->CableMark));
			GetIMEInputBuff(pMarkParam->CableMark);
			break;
		case 3:
			memset(pMarkParam->Location_A,0,sizeof(pMarkParam->Location_A));
			GetIMEInputBuff(pMarkParam->Location_A);
			break;
		case 4:
			memset(pMarkParam->Location_B,0,sizeof(pMarkParam->Location_B));
			GetIMEInputBuff(pMarkParam->Location_B);
			break;
		case 11:
			memset(pMarkParam->TaskID_Unicode,0,sizeof(pMarkParam->TaskID_Unicode));
			GetIMEInputBuff(pMarkParam->TaskID_Unicode);
			break;
		case 12:
			memset(pMarkParam->Operator_A,0,sizeof(pMarkParam->Operator_A));
			GetIMEInputBuff(pMarkParam->Operator_A);
			break;
		case 13:
			memset(pMarkParam->Operator_B,0,sizeof(pMarkParam->Operator_B));
			GetIMEInputBuff(pMarkParam->Operator_B);
			break;
		case 14:
			memset(pMarkParam->Company,0,sizeof(pMarkParam->Company));
			GetIMEInputBuff(pMarkParam->Company);
			break;
		case 15:
			memset(pMarkParam->Customer,0,sizeof(pMarkParam->Customer));
			GetIMEInputBuff(pMarkParam->Customer);
			break;
		case 10:
			memset(pMarkParam->Note,0,sizeof(pMarkParam->Note));
			GetIMEInputBuff(pMarkParam->Note);
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
static int MarkBtnClear_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("btn_sure.bmp", 
				pMarkBtnClear, NULL, pMarkLblClear, 1);
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
static int MarkBtnClear_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	TouchChange("btn_unsure.bmp", 
				pMarkBtnClear, NULL, pMarkLblClear, 1);
	for(i = 0;i<4;i++)
	{
		GuiMemFree(pMarkStrFiberString[i]);
		pMarkStrFiberString[i] = TransString(""); 
		SetLabelText(    
					pMarkStrFiberString[i],pMarkLblFiberString[i]);
		DisplayPicture(pMarkBtnFiberInput[i]);
		DisplayLabel(pMarkLblFiberString[i]);
	}
	for(i = 0;i<5;i++)
	{
		GuiMemFree(pMarkStrTaskString[i]);
		pMarkStrTaskString[i] = TransString(""); 
		SetLabelText(    
					pMarkStrTaskString[i],pMarkLblTaskString[i]);
		DisplayPicture(pMarkBtnTaskInput[i]);
		DisplayLabel(pMarkLblTaskString[i]);
	}
	GuiMemFree(pMarkStrNoteString);
	pMarkStrNoteString = TransString(""); 
	SetLabelText(    
				pMarkStrNoteString, pMarkLblNoteString);
	DisplayPicture(pMarkBtnNoteInput);
	DisplayLabel(pMarkLblNoteString);
	
	memset(pMarkParam,0,sizeof(MarkParam));

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
static int MarkBtnNoteInput_Down(void *pInArg, int iInLen, 
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
static int MarkBtnNoteInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	char buff[512] = {0};
	g_iInputMarkNum = 10;
	sprintf(buff, "%s", pMarkParam->Note);
	IMEInit(buff, 100, 0, ReCreateMarkWindow, InputOK, NULL);
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
static int MarkBtnFiberInput_Down(void *pInArg, int iInLen, 
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
static int MarkBtnFiberInput_Up(void *pInArg, int iInLen, 
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
	g_iInputMarkNum = iTemp;
	iTemp--;
	switch(g_iInputMarkNum)
	{
		case 1:
			sprintf(buff, "%s", pMarkParam->FiberMark);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 2:
			sprintf(buff, "%s", pMarkParam->CableMark);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 3:
			sprintf(buff, "%s", pMarkParam->Location_A);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 4:
			sprintf(buff, "%s", pMarkParam->Location_B);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
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
static int MarkBtnTaskInput_Down(void *pInArg, int iInLen, 
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
static int MarkBtnTaskInput_Up(void *pInArg, int iInLen, 
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
	g_iInputMarkNum = iTemp+10;
	iTemp--;
	switch(g_iInputMarkNum)
	{
		case 11:
			sprintf(buff, "%s", pMarkParam->TaskID_Unicode);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 12:
			sprintf(buff, "%s", pMarkParam->Operator_A);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 13:
			sprintf(buff, "%s", pMarkParam->Operator_B);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 14:
			sprintf(buff, "%s", pMarkParam->Company);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
			break;
		case 15:
			sprintf(buff, "%s", pMarkParam->Customer);
			IMEInit(buff, 17, 0, ReCreateMarkWindow, InputOK, NULL);
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
static int MarkBtnFiberDirectionAB_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("id_opt_select.bmp", 
				pMarkBtnFiberdirectionAB, NULL, NULL, 0);
	TouchChange("id_opt_unselect.bmp", 
				pMarkBtnFiberdirectionBA, NULL, NULL, 0);
	pMarkParam->Deraction = 0;
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
static int MarkBtnFiberDirectionAB_Up(void *pInArg, int iInLen, 
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
static int MarkBtnFiberDirectionBA_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("id_opt_unselect.bmp", 
				pMarkBtnFiberdirectionAB, NULL, NULL, 0);
	TouchChange("id_opt_select.bmp", 
				pMarkBtnFiberdirectionBA, NULL, NULL, 0);
    pMarkParam->Deraction = 1;
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
static int MarkBtnFiberDirectionBA_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	return iReturn;
}
/***
  * 功能：
        		窗体frmMark 的初始化函数，建立窗体控件、
        		注册消息处理
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmMarkInit(void *pWndObj)
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
	pFrmMark = (GUIWINDOW *) pWndObj;
	MarkTextRes_Init(NULL, 0, NULL, 0);

	//建立桌面上的控件
    pFrmMarkBG = CreatePhoto("bg_global");
    pFrmMarkLabel = CreateLabel(0, 25,100,24,  pFrmMarkString);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pFrmMarkLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pFrmMarkLabel);
	
	pFrmMarkMenu = CreateWndMenu1(0, 0,0, (UINT16)(MENU_BACK|~MENU_HOME),
									-1,0,40, MarkMenuCallBack);

    CreateLoopPhoto(pMarkBtnFiberInput,"btn_mark_input",0,0,4);

	for(i = 0; i<4; i++)
	{
		pMarkLblFiberString[i] = CreateLabel(180, 100+i*30,135,25,   
										  pMarkStrFiberString[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberString[i]); 
	}
    
    CreateLoopPhoto(pMarkBtnTaskInput,"btn_mark_input1",0,0,5);
	for(i = 0; i<5; i++)
	{
		pMarkLblTaskString[i] = CreateLabel(508, 100+i*30,135,25,   
										  pMarkStrTaskString[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTaskString[i]);
	}
    CreateLoopPhoto(pMarkBtnMeasureInput,"btn_mark_input2",0,0,4);
    pMarkBtnClear = CreatePhoto("btn_unsure");
    pMarkBtnNoteInput = CreatePhoto("btn_mark_note1");
	pMarkLblNoteString = CreateLabel(410, 322, 230, 70, pMarkStrNoteString);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblNoteString); 
	
    pMarkBtnFiberdirectionAB = CreatePhoto("id_opt_select");
    pMarkBtnFiberdirectionBA = CreatePhoto("id_opt_unselect");
	/******************建立按钮区的标签******************/
	//桌面标签
	pMarkLblFiber = CreateLabel(35,55,300,24,   
									  pMarkStrFiber);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiber);
	pMarkLblFiberMark = CreateLabel(80,100,100,24,      
								pMarkStrFiberMark);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberMark);
	pMarkLblCableMark = CreateLabel(80,130,100,24,     
								pMarkStrCableMark);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblCableMark);
	pMarkLblFiberLocation1 = CreateLabel(80,160, 100, 24,   
									  pMarkStrFiberLocation1);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberLocation1);
	pMarkLblFiberLocation2 = CreateLabel(80, 190, 100, 24,   
									  pMarkStrFiberLocation2);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberLocation2);
	pMarkLblFiberdirection = CreateLabel(80, 220, 100, 24,   
									 pMarkStrFiberdirection);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberdirection);
	pMarkLblFiberdirectionAB = CreateLabel(205, 225, 50, 24,  
									 pMarkStrFiberdirectionAB);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberdirectionAB);
	pMarkLblFiberdirectionBA = CreateLabel(285, 225, 50, 24,   
									 pMarkStrFiberdirectionBA);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblFiberdirectionBA);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiber);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiberMark);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblCableMark);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiberLocation1);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiberLocation2);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiberdirection);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiberdirectionAB);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblFiberdirectionBA);
	
	//测量栏标签
	pMarkLblMeas = CreateLabel(35, 290, 300, 24,   
							 pMarkStrMeas);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeas);
	pMarkLblMeasDate = CreateLabel(80, 335, 80, 24,   
								 pMarkStrMeasDate);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasDate);
	pMarkLblMeasTime = CreateLabel(80, 365, 80, 24,  
								 pMarkStrMeasTime);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasTime);
	pMarkLblMeasModule = CreateLabel(80, 395, 80, 24,   
									 pMarkStrMeasModule);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasModule);
	pMarkLblMeasNum = CreateLabel(80, 425, 80, 24,    
								pMarkStrMeasNum);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasNum);
	
	pMarkLblMeasDateString = CreateLabel(153, 335, 150, 24,   
									 pMarkStrMeasDateString);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasDateString);
	pMarkLblMeasTimeString = CreateLabel(153, 365, 150, 24,   
									 pMarkStrMeasTimeString);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasTimeString);

	pMarkLblMeasModuleVal = CreateLabel(153, 395, 150, 24,   
									 pMarkStrMeasModuleVal);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasModuleVal);
	pMarkLblMeasNumVal = CreateLabel(153, 425, 150, 24,   
									 pMarkStrMeasNumVal);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblMeasNumVal);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblMeas);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblMeasDate);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblMeasTime);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblMeasModule);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblMeasNum);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pMarkLblMeasDateString);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pMarkLblMeasTimeString);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pMarkLblMeasModuleVal);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pMarkLblMeasNumVal);
	
	//任务栏标签
	pMarkLblTask = CreateLabel(360, 55, 300, 24,  
							 pMarkStrTask);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTask);
	pMarkLblTaskID = CreateLabel(410, 100, 90, 24,   
								 pMarkStrTaskID);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTaskID);
	pMarkLblTaskOperatorA = CreateLabel(410, 130, 90, 24,   
									 pMarkStrTaskOperatorA);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTaskOperatorA);
	pMarkLblTaskOperatorB = CreateLabel(410, 160, 90, 24,   
									 pMarkStrTaskOperatorB);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTaskOperatorB);
	pMarkLblTaskCompany = CreateLabel(410, 190, 90, 24,   
									 pMarkStrTaskCompany);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTaskCompany);
	pMarkLblTaskClient = CreateLabel(410, 220, 90, 24,   
								 pMarkStrTaskClient);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblTaskClient);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblTask);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblTaskID);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblTaskOperatorA);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblTaskOperatorB);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblTaskCompany);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblTaskClient);

	//注释以下标签
	pMarkLblNote = CreateLabel(360, 290, 300, 24,  
							 pMarkStrNote);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pMarkLblNote);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblNote);
	
    pMarkLblClear = CreateLabel(525, 430, 142, 43,
							 pMarkStrClear);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMarkLblClear);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pMarkLblClear);
	
	//注册窗体控件,只对那些需要接收事件的控件进行
	//即如果该控件没有或者不需要响应输入事件，可
	//以无需注册
	//注册窗体控件必须在注册消息处理函数之前进行
	//*********************************************************/
	//注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmMark, 
	              pFrmMark);
	//清除按钮注册
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkBtnClear, 
	              pFrmMark);
	
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkBtnNoteInput, 
	              pFrmMark);
	for(i = 0;i<4;i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkBtnFiberInput[i], 
		              pFrmMark);
	}
	for(i = 0;i<5;i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkBtnTaskInput[i], 
		              pFrmMark);
	}
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkLblFiberdirectionAB, 
	              pFrmMark);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkLblFiberdirectionBA, 
	              pFrmMark);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkBtnFiberdirectionAB, 
	              pFrmMark);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMarkBtnFiberdirectionBA, 
	              pFrmMark);

	AddWndMenuToComp1(pFrmMarkMenu,pFrmMark);
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();
	//清除按钮
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMarkBtnClear, 
	               MarkBtnClear_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMarkBtnClear, 
	               MarkBtnClear_Down, NULL, 0, pMsg);
	//注释响应
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMarkBtnNoteInput, 
	               MarkBtnNoteInput_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMarkBtnNoteInput, 
	               MarkBtnNoteInput_Down, NULL, 0, pMsg);
	
	for(i = 0;i<4;i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_UP, pMarkBtnFiberInput[i], 
		               MarkBtnFiberInput_Up, (void*)(i+1), 0, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMarkBtnFiberInput[i], 
		               MarkBtnFiberInput_Down, (void*)(i+1), 0, pMsg);
	}
	for(i = 0;i<5;i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_UP, pMarkBtnTaskInput[i], 
		               MarkBtnTaskInput_Up, (void*)(i+1), 0, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMarkBtnTaskInput[i], 
		               MarkBtnTaskInput_Down, (void*)(i+1), 0, pMsg);
	}

	LoginMessageReg(GUIMESSAGE_TCH_DOWN,pMarkLblFiberdirectionAB, 
	               MarkBtnFiberDirectionAB_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pMarkLblFiberdirectionAB, 
	               MarkBtnFiberDirectionAB_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN,pMarkBtnFiberdirectionAB, 
	               MarkBtnFiberDirectionAB_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pMarkBtnFiberdirectionAB, 
	               MarkBtnFiberDirectionAB_Up, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMarkLblFiberdirectionBA, 
	               MarkBtnFiberDirectionBA_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pMarkLblFiberdirectionAB, 
	               MarkBtnFiberDirectionBA_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMarkBtnFiberdirectionBA, 
	               MarkBtnFiberDirectionBA_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pMarkLblFiberdirectionBA, 
	               MarkBtnFiberDirectionBA_Up, NULL, 0, pMsg);

	LoginWndMenuToMsg1(pFrmMarkMenu, pFrmMark);
	return iReturn;
}
/***
  * 功能：
        		窗体frmMark 的退出函数，释放所有资源
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmMarkExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	int i = 0;
	//得到当前窗体对象
	pFrmMark = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmMark);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pFrmMarkBG);
	DestroyLabel(&pFrmMarkLabel);
    
	for(i = 0;i<4;i++)
	{
		DestroyPicture(&pMarkBtnFiberInput[i]);
		DestroyLabel(&pMarkLblFiberString[i]);
        DestroyPicture(&pMarkBtnMeasureInput[i]);
	}
    
	for(i = 0;i<5;i++)
	{
		DestroyPicture(&pMarkBtnTaskInput[i]);
		DestroyLabel(&pMarkLblTaskString[i]);
	}
    
	DestroyPicture(&pMarkBtnClear);
	DestroyPicture(&pMarkBtnNoteInput);
	DestroyPicture(&pMarkBtnFiberdirectionAB);
	DestroyPicture(&pMarkBtnFiberdirectionBA);
	//按钮标签销毁
	DestroyLabel(&pMarkLblFiber);
	
	//桌面标签销毁
	DestroyLabel(&pMarkLblFiberMark);
	DestroyLabel(&pMarkLblCableMark);
	DestroyLabel(&pMarkLblFiberLocation1);
	DestroyLabel(&pMarkLblFiberLocation2);
	DestroyLabel(&pMarkLblFiberdirection);
	DestroyLabel(&pMarkLblFiberdirectionAB);
	DestroyLabel(&pMarkLblFiberdirectionBA);
	//测量栏标签销毁
	DestroyLabel(&pMarkLblMeas); 
	DestroyLabel(&pMarkLblMeasDate); 
	DestroyLabel(&pMarkLblMeasTime); 
	DestroyLabel(&pMarkLblMeasModule); 
	DestroyLabel(&pMarkLblMeasNum); 
	DestroyLabel(&pMarkLblMeasModuleVal); 
	DestroyLabel(&pMarkLblMeasNumVal); 

	DestroyLabel(&pMarkLblMeasDateString); 
	DestroyLabel(&pMarkLblMeasTimeString); 
	//任务栏标签销毁
	DestroyLabel(&pMarkLblTask); 
	DestroyLabel(&pMarkLblTaskID); 
	DestroyLabel(&pMarkLblTaskOperatorA); 
	DestroyLabel(&pMarkLblTaskOperatorB); 
	DestroyLabel(&pMarkLblTaskCompany); 
	DestroyLabel(&pMarkLblTaskClient); 
	//注释以下标签销毁
	DestroyLabel(&pMarkLblNote); 
	DestroyLabel(&pMarkLblClear); 
	DestroyLabel(&pMarkLblNoteString); 
	DestroyWndMenu1(&pFrmMarkMenu);
	
	//文本内容销毁
	MarkTextRes_Exit(NULL, 0, NULL, 0);

           return iReturn;
}


/***
  * 功能：
        		窗体frmMark 的绘制函数，绘制整个窗体
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmMarkPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	//得到当前窗体对象
	pFrmMark = (GUIWINDOW *) pWndObj;
	//按钮显示
	DisplayPicture(pFrmMarkBG);
	DisplayLabel(pFrmMarkLabel);
    
	for(i = 0;i<4;i++)
	{
		DisplayPicture(pMarkBtnFiberInput[i]);
		DisplayLabel(pMarkLblFiberString[i]);
        DisplayPicture(pMarkBtnMeasureInput[i]);
	}
    
	for(i = 0;i<5;i++)
	{
		DisplayPicture(pMarkBtnTaskInput[i]);
		DisplayLabel(pMarkLblTaskString[i]);
	}

	DisplayPicture(pMarkBtnClear);
	DisplayPicture(pMarkBtnNoteInput);
    
	if(pMarkParam->Deraction) //b->a
	{
		TouchChange("id_opt_unselect.bmp", 
					pMarkBtnFiberdirectionAB, NULL, NULL, 0);
		TouchChange("id_opt_select.bmp", 
					pMarkBtnFiberdirectionBA, NULL, NULL, 0);
	}
	else	//a->b
	{
		TouchChange("id_opt_select.bmp", 
					pMarkBtnFiberdirectionAB, NULL, NULL, 0);
		TouchChange("id_opt_unselect.bmp", 
					pMarkBtnFiberdirectionBA, NULL, NULL, 0);
	}

	DisplayPicture(pMarkBtnFiberdirectionAB);
	DisplayPicture(pMarkBtnFiberdirectionBA);
	//按钮标签显示
	DisplayLabel(pMarkLblFiber);	
	//桌面标签显示
	DisplayLabel(pMarkLblFiberMark);
	DisplayLabel(pMarkLblCableMark);
	DisplayLabel(pMarkLblFiberLocation1);
	DisplayLabel(pMarkLblFiberLocation2);
	DisplayLabel(pMarkLblFiberdirection);
	DisplayLabel(pMarkLblFiberdirectionAB);
	DisplayLabel(pMarkLblFiberdirectionBA);
	//测量栏标签显示
	DisplayLabel(pMarkLblMeas); 
	DisplayLabel(pMarkLblMeasDate); 
	DisplayLabel(pMarkLblMeasTime); 
	DisplayLabel(pMarkLblMeasModule); 
	DisplayLabel(pMarkLblMeasNum); 
	DisplayLabel(pMarkLblMeasModuleVal); 
	DisplayLabel(pMarkLblMeasNumVal); 	
	DisplayLabel(pMarkLblMeasDateString); 
	DisplayLabel(pMarkLblMeasTimeString); 
	//任务栏标签显示
	DisplayLabel(pMarkLblTask); 
	DisplayLabel(pMarkLblTaskID); 
	DisplayLabel(pMarkLblTaskOperatorA); 
	DisplayLabel(pMarkLblTaskOperatorB); 
	DisplayLabel(pMarkLblTaskCompany); 
	DisplayLabel(pMarkLblTaskClient); 
	//注释以下标签显示
	DisplayLabel(pMarkLblNote); 
	//DisplayLabel(pMarkLblDefault); 
	DisplayLabel(pMarkLblClear); 
	DisplayLabel(pMarkLblNoteString); 
	DisplayWndMenu1(pFrmMarkMenu);

	SetPowerEnable(1, 1);
	
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}


/***
  * 功能：
        	窗体frmMark 的循环函数，进行窗体循环
  * 参数：
       	 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmMarkLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	return iReturn;
}


/***
  * 功能：
        窗体frmMark 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMarkPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体frmMark 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmMarkResume(void *pWndObj)
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
static int MarkTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	if(getCurveMeasureDate(pMarkParam->Test_date, pMarkParam->Test_time, pCurSystemSet->uiTimeShowFlag))
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
        	sprintf(pMarkParam->Test_date,"%.2d/%.2d/%.4d",
			pRtc->tm_mday,pRtc->tm_mon,pRtc->tm_year);
        }
        else if(pCurSystemSet->uiTimeShowFlag == 1)
        {
        	sprintf(pMarkParam->Test_date,"%.2d/%.2d/%.4d",
			pRtc->tm_mon,pRtc->tm_mday,pRtc->tm_year);
        }
        else
        {
        	sprintf(pMarkParam->Test_date,"%.4d/%.2d/%.2d",
			pRtc->tm_year,pRtc->tm_mon,pRtc->tm_mday);
        }
    	sprintf(pMarkParam->Test_time,"%.2d:%.2d:%.2d",
    			pRtc->tm_hour,pRtc->tm_min,pRtc->tm_sec);
    			
        GuiMemFree(pRtc);
    }
	//初始化状态栏上的文本
	//...
	//初始化桌面上的文本
	//...
	//初始化桌面按钮的文本
	//...
	pFrmMarkString = TransString("OTDR_LBL_IDENTITY"); 
	pMarkStrClear = TransString("IDETY_CLEAR"); 
	//初始化桌面标签上的文本
	pMarkStrFiber = TransString("IDETY_FIBER"); 
	pMarkStrFiberMark = TransString("IDETY_FIBER_MARK"); 
	pMarkStrCableMark = TransString("IDETY_CABLE_MARK"); 
	pMarkStrFiberLocation1 = TransString("IDETY_LOCATION_A"); 
	pMarkStrFiberLocation2 = TransString("IDETY_LOCATION_B"); 
	pMarkStrFiberdirection = TransString("IDETY_DIRECTION"); 
	pMarkStrFiberdirectionAB = TransString("A->B"); 
	pMarkStrFiberdirectionBA = TransString("B->A"); 

	//初始化测量栏标签文本
	pMarkStrMeas = TransString("IDETY_MEASURE"); 
	pMarkStrMeasDate = TransString("IDETY_DATE"); 
	pMarkStrMeasTime = TransString("IDETY_TIME"); 
	pMarkStrMeasModule = TransString("IDETY_MODULE"); 
	pMarkStrMeasNum = TransString("OTDR_ABOUT_SERIALNUM"); 

    //初始化任务栏标签文本
    pMarkStrTask = TransString("IDETY_TASK"); 
    pMarkStrTaskID = TransString("IDETY_TASK_ID"); 
    pMarkStrTaskOperatorA = TransString("IDETY_OPER_A"); 
    pMarkStrTaskOperatorB = TransString("IDETY_OPER_B"); 
    pMarkStrTaskCompany = TransString("IDETY_COMPANY"); 
    pMarkStrTaskClient = TransString("IDETY_CUSTOMER"); 
    pMarkStrNote = TransString("IDETY_NOTE"); 
    //初始化注释以下标签文本
    pMarkStrMeasDateString = TransString(pMarkParam->Test_date); 
    pMarkStrMeasTimeString = TransString(pMarkParam->Test_time); 
	pMarkStrNoteString = TransString(pMarkParam->Note); 
	//...
	pMarkStrFiberString[0] = TransString(pMarkParam->FiberMark); 
	pMarkStrFiberString[1] = TransString(pMarkParam->CableMark); 
	pMarkStrFiberString[2] = TransString(pMarkParam->Location_A); 
	pMarkStrFiberString[3] = TransString(pMarkParam->Location_B); 
	
	pMarkStrTaskString[0] = TransString(pMarkParam->TaskID_Unicode); 
	pMarkStrTaskString[1] = TransString(pMarkParam->Operator_A); 
	pMarkStrTaskString[2] = TransString(pMarkParam->Operator_B); 
	pMarkStrTaskString[3] = TransString(pMarkParam->Company); 
	pMarkStrTaskString[4] = TransString(pMarkParam->Customer);
	
	GenerateSerialNumber(pMarkParam->Serial_num, pCurSN);
	sprintf(pMarkParam->Module,"%s","OTDR");
	pMarkStrMeasNumVal = TransString(pMarkParam->Serial_num);
	pMarkStrMeasModuleVal = TransString(pMarkParam->Module);

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
static int MarkTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i = 0;
	//释放状态栏上的文本
	//...
	//释放按钮区的文本
	//...
	GuiMemFree(pFrmMarkString);
	GuiMemFree(pMarkStrClear ); 
	//释放桌面上的文本
	//...
	GuiMemFree(pMarkStrFiber ); 
	GuiMemFree(pMarkStrFiberMark ); 
	GuiMemFree(pMarkStrCableMark ); 
	GuiMemFree(pMarkStrFiberLocation1 ); 
	GuiMemFree(pMarkStrFiberLocation2) ; 
	GuiMemFree(pMarkStrFiberdirection ); 
	GuiMemFree(pMarkStrFiberdirectionAB ); 
	GuiMemFree(pMarkStrFiberdirectionBA) ; 
	GuiMemFree(pMarkStrMeas ); 
	GuiMemFree(pMarkStrMeasDate ); 
	GuiMemFree(pMarkStrMeasTime) ; 
	GuiMemFree(pMarkStrMeasModule) ; 
	GuiMemFree(pMarkStrMeasNum ); 
	GuiMemFree(pMarkStrMeasModuleVal ); 
	GuiMemFree(pMarkStrMeasNumVal ); 
	
	GuiMemFree(pMarkStrTask ); 
	GuiMemFree(pMarkStrTaskID ); 
	GuiMemFree(pMarkStrTaskOperatorA ); 
	GuiMemFree(pMarkStrTaskOperatorB) ; 
	GuiMemFree(pMarkStrTaskCompany) ; 
	GuiMemFree(pMarkStrTaskClient) ; 
	GuiMemFree(pMarkStrNote ); 
	GuiMemFree(pMarkStrMeasDateString);
	GuiMemFree(pMarkStrMeasTimeString);	
	//释放信息栏上的文本
	for(i = 0;i<4;i++)
	{
		GuiMemFree(pMarkStrFiberString[i]);
	}
	for(i = 0;i<5;i++)
	{
		GuiMemFree(pMarkLblTaskString[i]);
	}
	GuiMemFree(pMarkStrNoteString);	
	//...

	return iReturn;
}

