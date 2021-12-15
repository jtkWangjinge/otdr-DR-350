/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfipresult.c
* 摘    要：  实现主窗体frmfipresult的窗体处理相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2017-1-19
*
* 取代版本： 
* 原 作 者：	
* 完成日期： 
*******************************************************************************/

#include "wnd_frmfipresult.h"

/*******************************************************************************
***                  为实现窗体frmfipresult而需要引用的其他头文件                  ***
*******************************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "app_frmfip.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmdialog.h"
#include "app_algorithm_analysis.h"
#include "wnd_frmfip.h"
#include "wnd_frmfipidentify.h"
#include "guiphoto.h"

/*******************************************************************************
***         				为实现窗体frmfipresult而定义的变量		    		 ***
*******************************************************************************/
extern OpticalFiberEndSur OptFiberEndSurCheck;
extern int iCheckErrFlag;
extern FIPCALLLBACK FipCallBackFunc;

static int iExitFlag = 0;
//创建右边menu菜单所需数组
static unsigned int StrFipResultMenuRealtime[] = {	
1
};

/*******************************************************************************
***                                窗体中的窗体控件                          ***
*******************************************************************************/
static GUIWINDOW *pFrmFipResult = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pFipResultBg = NULL;
static GUILABEL *pFipResultLblTitle = NULL;
static GUICHAR *pFipResultStrTitle = NULL;
static GUIPICTURE *pFipResultFileName = NULL;
static GUILABEL *pFipResultLblFileName = NULL;
static GUICHAR *pFipResultStrFileName = NULL;

static WNDMENU1 *pFipResultMenu = NULL;

static GUIPICTURE *pFipResultBtnTabImage = NULL;
static GUIPICTURE *pFipResultBtnTabResult = NULL;
static GUIPICTURE *pFipResultBtnTabIdentify = NULL;
static GUILABEL *pFipResultLblTabResult = NULL;
static GUILABEL *pFipResultLblTabIdentify = NULL;
static GUICHAR *pFipResultStrTabResult = NULL;
static GUICHAR *pFipResultStrTabIdentify = NULL;

static GUIPICTURE *pFipResultBgTable = NULL;
//表头标签
static GUILABEL *pTableLblHeaderLayering = NULL; 								//分层
static GUILABEL *pTableLblHeaderLayeringUnit = NULL; 						    //分层单位
static GUILABEL *pTableLblHeaderScracth = NULL; 								//划痕
static GUILABEL *pTableLblHeaderScracthStandard = NULL; 						//标准
static GUILABEL *pTableLblHeaderScracthThreshold = NULL; 						//阈值
static GUILABEL *pTableLblHeaderScracthNum = NULL; 								//数量
static GUILABEL *pTableLblHeaderDefect = NULL; 									//缺陷
static GUILABEL *pTableLblHeaderDefectStandard = NULL; 							//标准
static GUILABEL *pTableLblHeaderDefectThreshold = NULL; 						//阈值
static GUILABEL *pTableLblHeaderDefectNum = NULL; 								//数量

//表头文本
static GUICHAR *pTableStrHeaderLayering = NULL; 								//分层
static GUICHAR *pTableStrHeaderLayeringUnit = NULL; 							//分层单位
static GUICHAR *pTableStrHeaderScracth = NULL; 									//划痕
static GUICHAR *pTableStrHeaderScracthStandard = NULL; 							//标准
static GUICHAR *pTableStrHeaderScracthThreshold = NULL; 						//阈值
static GUICHAR *pTableStrHeaderScracthNum = NULL; 								//数量
static GUICHAR *pTableStrHeaderDefect = NULL; 									//缺陷
static GUICHAR *pTableStrHeaderDefectStandard = NULL; 							//标准
static GUICHAR *pTableStrHeaderDefectThreshold = NULL; 							//阈值
static GUICHAR *pTableStrHeaderDefectNum = NULL; 								//数量


//纤芯标签
static GUILABEL *pTableLblCore = NULL;											//纤芯
static GUILABEL *pTableLblCoreRange = NULL;									    //纤芯范围
static GUILABEL *pTableLblCoreScracthStandard = NULL;							//纤芯划痕标准
static GUILABEL *pTableLblCoreScracthThreshold = NULL;							//纤芯划痕阈值
static GUILABEL *pTableLblCoreScracthNum = NULL;								//纤芯划痕数量
static GUILABEL *pTableLblCoreDefectStandard = NULL;							//纤芯缺陷标准
static GUILABEL *pTableLblCoreDefectThreshold = NULL;							//纤芯缺陷阈值
static GUILABEL *pTableLblCoreDefectNum  = NULL;								//纤芯缺陷数量

//纤芯文本
static GUICHAR *pTableStrCore = NULL;											//纤芯
static GUICHAR *pTableStrCoreRange = NULL;									    //纤芯范围
static GUICHAR *pTableStrCoreScracthStandard = NULL;							//纤芯划痕标准
static GUICHAR *pTableStrCoreScracthThreshold = NULL;							//纤芯划痕阈值
static GUICHAR *pTableStrCoreScracthNum = NULL;									//纤芯划痕数量
static GUICHAR *pTableStrCoreDefectStandard = NULL;								//纤芯缺陷标准
static GUICHAR *pTableStrCoreDefectThreshold = NULL;							//纤芯缺陷阈值
static GUICHAR *pTableStrCoreDefectNum  = NULL;									//纤芯缺陷数量

//包层标签
static GUILABEL *pTableLblCladding = NULL;										//包层
static GUILABEL *pTableLblCladdingRange = NULL;									//包层范围
static GUILABEL *pTableLblCladdingSmallScracthStandard = NULL;					//包层小划痕标准
static GUILABEL *pTableLblCladdingSmallScracthThreshold = NULL;					//包层小划痕阈值
static GUILABEL *pTableLblCladdingSmallScracthNum = NULL;						//包层小划痕数量
static GUILABEL *pTableLblCladdingSmallDefectStandard = NULL;					//包层小缺陷标准
static GUILABEL *pTableLblCladdingSmallDefectThreshold = NULL;					//包层小缺陷阈值
static GUILABEL *pTableLblCladdingSmallDefectNum  = NULL;						//包层小缺陷数量

static GUILABEL *pTableLblCladdingMediumScracthStandard = NULL;					//包层中划痕标准
static GUILABEL *pTableLblCladdingMediumScracthThreshold = NULL;				//包层中划痕阈值
static GUILABEL *pTableLblCladdingMediumScracthNum = NULL;						//包层中划痕数量
static GUILABEL *pTableLblCladdingMediumDefectStandard = NULL;					//包层中缺陷标准
static GUILABEL *pTableLblCladdingMediumDefectThreshold = NULL;					//包层中缺陷阈值
static GUILABEL *pTableLblCladdingMediumDefectNum  = NULL;						//包层中缺陷数量

static GUILABEL *pTableLblCladdingLargeDefectStandard = NULL;					//包层大缺陷标准
static GUILABEL *pTableLblCladdingLargeDefectThreshold = NULL;					//包层大缺陷阈值
static GUILABEL *pTableLblCladdingLargeDefectNum  = NULL;						//包层大缺陷数量

//包层文本
static GUICHAR *pTableStrCladding = NULL;										//包层
static GUICHAR *pTableStrCladdingRange = NULL;									//包层范围
static GUICHAR *pTableStrCladdingSmallScracthStandard = NULL;					//包层小划痕标准
static GUICHAR *pTableStrCladdingSmallScracthThreshold = NULL;					//包层小划痕阈值
static GUICHAR *pTableStrCladdingSmallScracthNum = NULL;						//包层小划痕数量
static GUICHAR *pTableStrCladdingSmallDefectStandard = NULL;					//包层小缺陷标准
static GUICHAR *pTableStrCladdingSmallDefectThreshold = NULL;					//包层小缺陷阈值
static GUICHAR *pTableStrCladdingSmallDefectNum  = NULL;						//包层小缺陷数量

static GUICHAR *pTableStrCladdingMediumScracthStandard = NULL;					//包层中划痕标准
static GUICHAR *pTableStrCladdingMediumScracthThreshold = NULL;					//包层中划痕阈值
static GUICHAR *pTableStrCladdingMediumScracthNum = NULL;						//包层中划痕数量
static GUICHAR *pTableStrCladdingMediumDefectStandard = NULL;					//包层中缺陷标准
static GUICHAR *pTableStrCladdingMediumDefectThreshold = NULL;					//包层中缺陷阈值
static GUICHAR *pTableStrCladdingMediumDefectNum  = NULL;						//包层中缺陷数量

static GUICHAR *pTableStrCladdingLargeDefectStandard = NULL;					//包层大缺陷标准
static GUICHAR *pTableStrCladdingLargeDefectThreshold = NULL;					//包层大缺陷阈值
static GUICHAR *pTableStrCladdingLargeDefectNum  = NULL;						//包层大缺陷数量

//涂层标签
static GUILABEL *pTableLblCoating = NULL;										//涂层
static GUILABEL *pTableLblCoatingRange = NULL;									//涂层范围
static GUILABEL *pTableLblCoatingScracthStandard = NULL;						//涂层划痕标准
static GUILABEL *pTableLblCoatingScracthThreshold = NULL;						//涂层划痕阈值
static GUILABEL *pTableLblCoatingScracthNum = NULL;								//涂层划痕数量
static GUILABEL *pTableLblCoatingDefectStandard = NULL;							//涂层缺陷标准
static GUILABEL *pTableLblCoatingDefectThreshold = NULL;						//涂层缺陷阈值
static GUILABEL *pTableLblCoatingDefectNum  = NULL;								//涂层缺陷数量

//涂层文本
static GUICHAR *pTableStrCoating = NULL;										//涂层
static GUICHAR *pTableStrCoatingRange = NULL;									//涂层范围
static GUICHAR *pTableStrCoatingScracthStandard = NULL;							//涂层划痕标准
static GUICHAR *pTableStrCoatingScracthThreshold = NULL;						//涂层划痕阈值
static GUICHAR *pTableStrCoatingScracthNum = NULL;								//涂层划痕数量
static GUICHAR *pTableStrCoatingDefectStandard = NULL;							//涂层缺陷标准
static GUICHAR *pTableStrCoatingDefectThreshold = NULL;							//涂层缺陷阈值
static GUICHAR *pTableStrCoatingDefectNum  = NULL;								//涂层缺陷数量

//套层标签
static GUILABEL *pTableLblJacket = NULL;										//套层
static GUILABEL *pTableLblJacketRange = NULL;									//套层范围
static GUILABEL *pTableLblJacketSmallScracthStandard = NULL;					//套层小划痕标准
static GUILABEL *pTableLblJacketSmallScracthThreshold = NULL;					//套层小划痕阈值
static GUILABEL *pTableLblJacketSmallScracthNum = NULL;							//套层小划痕数量
static GUILABEL *pTableLblJacketSmallDefectStandard = NULL;						//套层小缺陷标准
static GUILABEL *pTableLblJacketSmallDefectThreshold = NULL;					//套层小缺陷阈值
static GUILABEL *pTableLblJacketSmallDefectNum  = NULL;							//套层小缺陷数量

static GUILABEL *pTableLblJacketLargeDefectStandard = NULL;						//套层大缺陷标准
static GUILABEL *pTableLblJacketLargeDefectThreshold = NULL;					//套层大缺陷阈值
static GUILABEL *pTableLblJacketLargeDefectNum  = NULL;							//套层大缺陷数量

//套层文本
static GUICHAR *pTableStrJacket = NULL;											//套层
static GUICHAR *pTableStrJacketRange = NULL;									//套层范围
static GUICHAR *pTableStrJacketSmallScracthStandard = NULL;						//套层小划痕标准
static GUICHAR *pTableStrJacketSmallScracthThreshold = NULL;					//套层小划痕阈值
static GUICHAR *pTableStrJacketSmallScracthNum = NULL;							//套层小划痕数量
static GUICHAR *pTableStrJacketSmallDefectStandard = NULL;						//套层小缺陷标准
static GUICHAR *pTableStrJacketSmallDefectThreshold = NULL;						//套层小缺陷阈值
static GUICHAR *pTableStrJacketSmallDefectNum  = NULL;							//套层小缺陷数量

static GUICHAR *pTableStrJacketLargeDefectStandard = NULL;						//套层大缺陷标准
static GUICHAR *pTableStrJacketLargeDefectThreshold = NULL;						//套层大缺陷阈值
static GUICHAR *pTableStrJacketLargeDefectNum  = NULL;							//套层大缺陷数量

/*******************************************************************************
***                        窗体中的文本资源处理函数                          ***
*******************************************************************************/

//初始化文本资源
static int FipResultTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
//释放文本资源
static int FipResultTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

static void FipResultMenuCallBack(int iOption);

static int FipResultBtnTabImage_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int FipResultBtnTabIdentify_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);

/*  侧边菜单控件回调函数 */
static void FipResultMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOption)
	{
	case 0:
		break;
	case 1:
	
		break;
	case 2:
		
		break;
	case 3:								//居中
		break;
	case 4:								//放大
		break;
	case 5:
		break;
	case BACK_DOWN:	
		if(iExitFlag)
		{
			break;
		}
		iExitFlag = 1;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmFipInit, FrmFipExit, 
							FrmFipPaint, FrmFipLoop, 
							FrmFipPause, FrmFipResume,
							NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFipResult); 	//发送消息以便退出前一个窗体
		SendSysMsg_ThreadCreate(pWnd);
	break;
	case HOME_DOWN:
		if(iExitFlag)
		{
			break;
		}
		iExitFlag = 1;
		PthreadJoin();
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFipResult);  
	    SendSysMsg_ThreadCreate(pWnd);
	 	break;
	default:
		break;
	}
}

/***
  * 功能：
        窗体frmfip的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmFipResultInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	int isAllRed = 0;
	//得到当前窗体对象 
    pFrmFipResult = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FipResultTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件
    ***************************************************************************/
    pFipResultBg = CreatePhoto("fip_bg");
    pFipResultFileName = CreatePhoto("otdr_top5");
	pFipResultLblTitle = CreateLabel(0, 22, 100, 24, pFipResultStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipResultLblTitle);
	pFipResultLblFileName = CreateLabel(400, 22, 281, 24, pFipResultStrFileName);
	SetLabelAlign(2, pFipResultLblFileName);

	pFipResultLblTabResult = CreateLabel(100, 22, 100, 24, pFipResultStrTabResult);
	pFipResultLblTabIdentify= CreateLabel(200, 22, 100, 24, pFipResultStrTabIdentify);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFipResultLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFipResultLblTabResult);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFipResultLblTabIdentify);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFipResultLblTabResult);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipResultLblTabIdentify);
    pFipResultBtnTabImage = CreatePhoto("otdr_top1");
    pFipResultBtnTabResult = CreatePhoto("otdr_top2f");
    pFipResultBgTable = CreatePhoto("fip_result_table");
    pFipResultBtnTabIdentify = CreatePicture(200, 0, 100, 50,NULL);
    //表头
	pTableLblHeaderLayering = CreateLabel(5, 59, 83, 24, pTableStrHeaderLayering);
	pTableLblHeaderLayeringUnit = CreateLabel(5, 83, 83, 24, pTableStrHeaderLayeringUnit);
	pTableLblHeaderScracth = CreateLabel(94, 45, 292, 60, pTableStrHeaderScracth);
	pTableLblHeaderScracthStandard = CreateLabel(94, 78+4, 100, 60, pTableStrHeaderScracthStandard);
	pTableLblHeaderScracthThreshold = CreateLabel(195, 78+4, 96, 60, pTableStrHeaderScracthThreshold);
	pTableLblHeaderScracthNum = CreateLabel(291, 78+4, 96, 60, pTableStrHeaderScracthNum);
	pTableLblHeaderDefect = CreateLabel(387, 45, 292, 60, pTableStrHeaderDefect);
	pTableLblHeaderDefectStandard = CreateLabel(387, 78+4, 100, 60, pTableStrHeaderDefectStandard);
	pTableLblHeaderDefectThreshold = CreateLabel(488, 78+4, 96, 60, pTableStrHeaderDefectThreshold);
	pTableLblHeaderDefectNum = CreateLabel(584, 78+4, 96, 60, pTableStrHeaderDefectNum);

	//纤芯
	pTableLblCore = CreateLabel(5, 123, 83, 24, pTableStrCore);
	pTableLblCoreRange = CreateLabel(5, 147, 83, 24, pTableStrCoreRange);
	pTableLblCoreScracthStandard = CreateLabel(99, 126+4, 90, 60, pTableStrCoreScracthStandard);
	pTableLblCoreScracthThreshold = CreateLabel(195, 126+4, 96, 60, pTableStrCoreScracthThreshold);
	pTableLblCoreScracthNum = CreateLabel(291, 126+4, 96, 60, pTableStrCoreScracthNum);
	pTableLblCoreDefectStandard = CreateLabel(392, 126+4, 90, 60, pTableStrCoreDefectStandard);
	pTableLblCoreDefectThreshold = CreateLabel(488, 126+4, 96, 60, pTableStrCoreDefectThreshold);
	pTableLblCoreDefectNum = CreateLabel(584, 126+4, 96, 60, pTableStrCoreDefectNum);

	
	//包层
	pTableLblCladding = CreateLabel(5, 229, 83, 24, pTableStrCladding);
	pTableLblCladdingRange = CreateLabel(5, 253, 83, 24, pTableStrCladdingRange);
	pTableLblCladdingSmallScracthStandard = CreateLabel(99, 181+4, 90, 60, pTableStrCladdingSmallScracthStandard);
	pTableLblCladdingSmallScracthThreshold = CreateLabel(195, 181+4, 96, 60, pTableStrCladdingSmallScracthThreshold);
	pTableLblCladdingSmallScracthNum = CreateLabel(291, 181+4, 96, 60, pTableStrCladdingSmallScracthNum);
	pTableLblCladdingSmallDefectStandard = CreateLabel(392, 181+4, 90, 60, pTableStrCladdingSmallDefectStandard);
	pTableLblCladdingSmallDefectThreshold = CreateLabel(488, 181+4, 96, 60, pTableStrCladdingSmallDefectThreshold);
	pTableLblCladdingSmallDefectNum = CreateLabel(584, 181+4, 96, 60, pTableStrCladdingSmallDefectNum);

	pTableLblCladdingMediumScracthStandard = CreateLabel(99, 233+4, 90, 60, pTableStrCladdingMediumScracthStandard);
	pTableLblCladdingMediumScracthThreshold = CreateLabel(195, 233+4, 96, 60, pTableStrCladdingMediumScracthThreshold);
	pTableLblCladdingMediumScracthNum = CreateLabel(291, 233+4, 96, 60, pTableStrCladdingMediumScracthNum);
	pTableLblCladdingMediumDefectStandard = CreateLabel(392, 233+4, 90, 60, pTableStrCladdingMediumDefectStandard);
	pTableLblCladdingMediumDefectThreshold = CreateLabel(488, 233+4, 96, 60, pTableStrCladdingMediumDefectThreshold);
	pTableLblCladdingMediumDefectNum = CreateLabel(584, 233+4, 96, 60, pTableStrCladdingMediumDefectNum);

	pTableLblCladdingLargeDefectStandard = CreateLabel(392, 287+4, 90, 60, pTableStrCladdingLargeDefectStandard);
	pTableLblCladdingLargeDefectThreshold = CreateLabel(488, 287+4, 96, 60, pTableStrCladdingLargeDefectThreshold);
	pTableLblCladdingLargeDefectNum = CreateLabel(584, 287+4, 96, 60, pTableStrCladdingLargeDefectNum);

	//涂层
	pTableLblCoating = CreateLabel(5, 335, 83, 24, pTableStrCoating);
	pTableLblCoatingRange = CreateLabel(5, 359, 83, 24, pTableStrCoatingRange);
	pTableLblCoatingScracthStandard = CreateLabel(99, 340+4, 90, 60, pTableStrCoatingScracthStandard);
	pTableLblCoatingScracthThreshold = CreateLabel(195, 340+4, 96, 60, pTableStrCoatingScracthThreshold);
	pTableLblCoatingScracthNum = CreateLabel(291, 340+4, 96, 60, pTableStrCoatingScracthNum);
	pTableLblCoatingDefectStandard = CreateLabel(392, 340+4, 90, 60, pTableStrCoatingDefectStandard);
	pTableLblCoatingDefectThreshold = CreateLabel(488, 340+4, 96, 60, pTableStrCoatingDefectThreshold);
	pTableLblCoatingDefectNum = CreateLabel(584, 340+4, 96, 60, pTableStrCoatingDefectNum);

	//套层
	pTableLblJacket = CreateLabel(5, 414, 83, 24, pTableStrJacket);
	pTableLblJacketRange = CreateLabel(5, 438, 83, 24, pTableStrJacketRange);
	pTableLblJacketSmallScracthStandard = CreateLabel(99, 393+4, 90, 60, pTableStrJacketSmallScracthStandard);
	pTableLblJacketSmallScracthThreshold = CreateLabel(195, 393+4, 96, 60, pTableStrJacketSmallScracthThreshold);
	pTableLblJacketSmallScracthNum = CreateLabel(291, 393+4, 96, 60, pTableStrJacketSmallScracthNum);
	pTableLblJacketSmallDefectStandard = CreateLabel(392, 393+4, 90, 60, pTableStrJacketSmallDefectStandard);
	pTableLblJacketSmallDefectThreshold = CreateLabel(488, 393+4, 96, 60, pTableStrJacketSmallDefectThreshold);
	pTableLblJacketSmallDefectNum = CreateLabel(584, 393+4, 96, 60, pTableStrJacketSmallDefectNum);

	pTableLblJacketLargeDefectStandard = CreateLabel(392, 446+4, 90, 60, pTableStrJacketLargeDefectStandard);
	pTableLblJacketLargeDefectThreshold = CreateLabel(488, 446+4, 96, 60, pTableStrJacketLargeDefectThreshold);
	pTableLblJacketLargeDefectNum = CreateLabel(584, 446+4, 96, 60, pTableStrJacketLargeDefectNum);
	
	//表头
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderLayering);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderLayeringUnit);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderScracth);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderScracthStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderScracthThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderScracthNum);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderDefect);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblHeaderDefectNum);
	//纤芯
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCore);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreRange);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreScracthStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreScracthThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreScracthNum);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoreDefectNum);
	//包层
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladding);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingRange);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingSmallScracthStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingSmallScracthThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingSmallScracthNum);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingSmallDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingSmallDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingSmallDefectNum);
	
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingMediumScracthStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingMediumScracthThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingMediumScracthNum);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingMediumDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingMediumDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingMediumDefectNum);
	
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingLargeDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingLargeDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCladdingLargeDefectNum);
	//涂层
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoating);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingRange);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingScracthStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingScracthThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingScracthNum);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblCoatingDefectNum);
	//套层
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacket);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketRange);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketSmallScracthStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketSmallScracthThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketSmallScracthNum);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketSmallDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketSmallDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketSmallDefectNum);
		
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketLargeDefectStandard);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketLargeDefectThreshold);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pTableLblJacketLargeDefectNum);

	//表头
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderLayering);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderLayeringUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderScracth);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderScracthStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderScracthThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderScracthNum);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderDefect);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderDefectThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblHeaderDefectNum);
	//纤芯
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCore);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreRange);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreScracthStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreScracthThreshold);
	//若没有找到光纤端面则全显示黑色
	if(AnalyzeResult(OptFiberEndSurCheck.ResultInfo) && OptFiberEndSurCheck.ResultInfo.Isqualified != 1)
	{
        isAllRed = 1;
	}

    if(OptFiberEndSurCheck.ResultInfo.FiberCoreScracth == 0 && !isAllRed)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCoreScracthNum);
	}
	else
	{
		SetLabelFont(getGlobalFnt(EN_FONT_RED),pTableLblCoreScracthNum);
	}
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreDefectThreshold);

    if(OptFiberEndSurCheck.ResultInfo.FiberCoreDefect == 0 && !isAllRed)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCoreDefectNum);
	}
	else
	{
		SetLabelFont(getGlobalFnt(EN_FONT_RED),pTableLblCoreDefectNum);
	}
	
	//包层
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladding);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingRange);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingSmallScracthStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingSmallScracthThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCladdingSmallScracthNum);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingSmallDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingSmallDefectThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCladdingSmallDefectNum);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingMediumScracthStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingMediumScracthThreshold);

    if(OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge == 0 && !isAllRed)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCladdingMediumScracthNum);
	}
	else
	{
		SetLabelFont(getGlobalFnt(EN_FONT_RED),pTableLblCladdingMediumScracthNum);
	}
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingMediumDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingMediumDefectThreshold);

    if(OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium<= 5 && !isAllRed)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCladdingMediumDefectNum);
	}
	else
	{
		SetLabelFont(getGlobalFnt(EN_FONT_RED),pTableLblCladdingMediumDefectNum);
	}
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingLargeDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingLargeDefectThreshold);

    if(OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge == 0 && !isAllRed)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblCladdingLargeDefectNum);
	}
	else
	{
		SetLabelFont(getGlobalFnt(EN_FONT_RED),pTableLblCladdingLargeDefectNum);
	}
	
	//涂层
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoating);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingRange);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingScracthStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingScracthThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingScracthNum);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingDefectThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoatingDefectNum);
	//套层
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacket);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketRange);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketSmallScracthStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketSmallScracthThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblJacketSmallScracthNum);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketSmallDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketSmallDefectThreshold);
	SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblJacketSmallDefectNum);
		
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketLargeDefectStandard);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketLargeDefectThreshold);

    if(OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge == 0 && !isAllRed)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLUE),pTableLblJacketLargeDefectNum);
	}
	else
	{
		SetLabelFont(getGlobalFnt(EN_FONT_RED),pTableLblJacketLargeDefectNum);
	}
	
	if(isAllRed)
	{
	    GuiMemFree(pTableStrCoreScracthNum);
	    GuiMemFree(pTableStrCoreDefectNum);
	    GuiMemFree(pTableStrCladdingSmallScracthNum);
	    GuiMemFree(pTableStrCladdingSmallDefectNum);
	    GuiMemFree(pTableStrCladdingMediumScracthNum);
	    GuiMemFree(pTableStrCladdingMediumDefectNum);
	    GuiMemFree(pTableStrCladdingLargeDefectNum);	
	    GuiMemFree(pTableStrJacketSmallScracthNum);
	    GuiMemFree(pTableStrJacketSmallDefectNum);
	    GuiMemFree(pTableStrJacketLargeDefectNum);
	    
        pTableStrCoreScracthNum = TransString("N/A");
        pTableStrCoreDefectNum = TransString("N/A");
        pTableStrCladdingSmallScracthNum = TransString("N/A");
        pTableStrCladdingSmallDefectNum = TransString("N/A");
        pTableStrCladdingMediumScracthNum = TransString("N/A");
        pTableStrCladdingMediumDefectNum = TransString("N/A");
        pTableStrCladdingLargeDefectNum = TransString("N/A");
        pTableStrJacketSmallScracthNum = TransString("N/A");
        pTableStrJacketSmallDefectNum = TransString("N/A");
        pTableStrJacketLargeDefectNum = TransString("N/A");

        SetLabelText(pTableStrCoreScracthNum, pTableLblCoreScracthNum);
        SetLabelText(pTableStrCoreDefectNum, pTableLblCoreDefectNum);
        SetLabelText(pTableStrCladdingSmallScracthNum, pTableLblCladdingSmallScracthNum);
        SetLabelText(pTableStrCladdingSmallDefectNum, pTableLblCladdingSmallDefectNum);
        SetLabelText(pTableStrCladdingMediumScracthNum, pTableLblCladdingMediumScracthNum);
        SetLabelText(pTableStrCladdingMediumDefectNum, pTableLblCladdingMediumDefectNum);
        SetLabelText(pTableStrCladdingLargeDefectNum, pTableLblCladdingLargeDefectNum);
        SetLabelText(pTableStrJacketSmallScracthNum, pTableLblJacketSmallScracthNum);
        SetLabelText(pTableStrJacketSmallDefectNum, pTableLblJacketSmallDefectNum);
        SetLabelText(pTableStrJacketLargeDefectNum, pTableLblJacketLargeDefectNum);

        SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreScracthNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCoreDefectNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingSmallScracthNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingSmallDefectNum);
        
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingMediumScracthNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingMediumDefectNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblCladdingLargeDefectNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketSmallScracthNum);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketSmallDefectNum);
    	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pTableLblJacketLargeDefectNum);
	}
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFipResultMenu = CreateWndMenu1(0, sizeof(StrFipResultMenuRealtime), StrFipResultMenuRealtime, 0xffff,
								 -1, 0, 40, FipResultMenuCallBack);

    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFipResult, pFrmFipResult);
	
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipResultBtnTabImage, pFrmFipResult);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipResultBtnTabIdentify, pFrmFipResult);

    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFipResultMenu, pFrmFipResult);
	
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	LoginWndMenuToMsg1(pFipResultMenu, pFrmFipResult);
	GUIMESSAGE *pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_TCH_DOWN,pFipResultBtnTabImage,
            		FipResultBtnTabImage_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipResultBtnTabIdentify,
            		FipResultBtnTabIdentify_Down, NULL, 0, pMsg);
	return iRet;
}


/***
  * 功能：
        窗体frmotdrcurve的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmFipResultExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    	
    //得到当前窗体对象
    pFrmFipResult = (GUIWINDOW *) pWndObj;

    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFipResult);
	
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFipResultBg);
    DestroyPicture(&pFipResultFileName);
	

	DestroyLabel(&pFipResultLblFileName);
	DestroyLabel(&pFipResultLblTitle);	

	DestroyLabel(&pFipResultLblTabResult);
	DestroyLabel(&pFipResultLblTabIdentify);
	DestroyPicture(&pFipResultBtnTabImage);
	DestroyPicture(&pFipResultBtnTabResult);
	DestroyPicture(&pFipResultBtnTabIdentify);

	DestroyPicture(&pFipResultBgTable);

	//表头
    DestroyLabel(&pTableLblHeaderLayering);
    DestroyLabel(&pTableLblHeaderLayeringUnit);
    DestroyLabel(&pTableLblHeaderScracth);
    DestroyLabel(&pTableLblHeaderScracthStandard);
    DestroyLabel(&pTableLblHeaderScracthThreshold);
    DestroyLabel(&pTableLblHeaderScracthNum);
    DestroyLabel(&pTableLblHeaderDefect);
    DestroyLabel(&pTableLblHeaderDefectStandard);
    DestroyLabel(&pTableLblHeaderDefectThreshold);
    DestroyLabel(&pTableLblHeaderDefectNum);
	//纤芯
    DestroyLabel(&pTableLblCore);
    DestroyLabel(&pTableLblCoreRange);
    DestroyLabel(&pTableLblCoreScracthStandard);
    DestroyLabel(&pTableLblCoreScracthThreshold);
    DestroyLabel(&pTableLblCoreScracthNum);
    DestroyLabel(&pTableLblCoreDefectStandard);
    DestroyLabel(&pTableLblCoreDefectThreshold);
    DestroyLabel(&pTableLblCoreDefectNum);
	//包层
    DestroyLabel(&pTableLblCladding);
    DestroyLabel(&pTableLblCladdingRange);
    DestroyLabel(&pTableLblCladdingSmallScracthStandard);
    DestroyLabel(&pTableLblCladdingSmallScracthThreshold);
    DestroyLabel(&pTableLblCladdingSmallScracthNum);
    DestroyLabel(&pTableLblCladdingSmallDefectStandard);
    DestroyLabel(&pTableLblCladdingSmallDefectThreshold);
    DestroyLabel(&pTableLblCladdingSmallDefectNum);
	
    DestroyLabel(&pTableLblCladdingMediumScracthStandard);
    DestroyLabel(&pTableLblCladdingMediumScracthThreshold);
    DestroyLabel(&pTableLblCladdingMediumScracthNum);
    DestroyLabel(&pTableLblCladdingMediumDefectStandard);
    DestroyLabel(&pTableLblCladdingMediumDefectThreshold);
    DestroyLabel(&pTableLblCladdingMediumDefectNum);
	
    DestroyLabel(&pTableLblCladdingLargeDefectStandard);
    DestroyLabel(&pTableLblCladdingLargeDefectThreshold);
    DestroyLabel(&pTableLblCladdingLargeDefectNum);
	//涂层
    DestroyLabel(&pTableLblCoating);
    DestroyLabel(&pTableLblCoatingRange);
    DestroyLabel(&pTableLblCoatingScracthStandard);
    DestroyLabel(&pTableLblCoatingScracthThreshold);
    DestroyLabel(&pTableLblCoatingScracthNum);
    DestroyLabel(&pTableLblCoatingDefectStandard);
    DestroyLabel(&pTableLblCoatingDefectThreshold);
    DestroyLabel(&pTableLblCoatingDefectNum);
	//套层
    DestroyLabel(&pTableLblJacket);
    DestroyLabel(&pTableLblJacketRange);
    DestroyLabel(&pTableLblJacketSmallScracthStandard);
    DestroyLabel(&pTableLblJacketSmallScracthThreshold);
    DestroyLabel(&pTableLblJacketSmallScracthNum);
    DestroyLabel(&pTableLblJacketSmallDefectStandard);
    DestroyLabel(&pTableLblJacketSmallDefectThreshold);
    DestroyLabel(&pTableLblJacketSmallDefectNum);
		
    DestroyLabel(&pTableLblJacketLargeDefectStandard);
    DestroyLabel(&pTableLblJacketLargeDefectThreshold);
    DestroyLabel(&pTableLblJacketLargeDefectNum);
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFipResultMenu);

    //释放文本资源
    FipResultTextRes_Exit(NULL, 0, NULL, 0);
	
    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipResultPaint(void *pWndObj)
{
    ///错误标志、返回值定义
    int iRet = 0;

    //得到当前窗体对象
    pFrmFipResult = (GUIWINDOW *) pWndObj;
	iExitFlag = 0;
    /***************************************************************************
    *                      显示桌面上各个区域的背景控件
    ***************************************************************************/
    DisplayPicture(pFipResultBg);
	DisplayPicture(pFipResultBtnTabImage);
	DisplayPicture(pFipResultBtnTabResult);

	DisplayLabel(pFipResultLblTitle);
	DisplayLabel(pFipResultLblFileName);

	DisplayLabel(pFipResultLblTabResult);
	DisplayLabel(pFipResultLblTabIdentify);

	DisplayPicture(pFipResultBgTable);

	
	//表头
	DisplayLabel(pTableLblHeaderLayering);
	DisplayLabel(pTableLblHeaderLayeringUnit);
	DisplayLabel(pTableLblHeaderScracth);
	DisplayLabel(pTableLblHeaderScracthStandard);
	DisplayLabel(pTableLblHeaderScracthThreshold);
	DisplayLabel(pTableLblHeaderScracthNum);
	DisplayLabel(pTableLblHeaderDefect);
	DisplayLabel(pTableLblHeaderDefectStandard);
	DisplayLabel(pTableLblHeaderDefectThreshold);
	DisplayLabel(pTableLblHeaderDefectNum);
	//纤芯
	DisplayLabel(pTableLblCore);
	DisplayLabel(pTableLblCoreRange);
	DisplayLabel(pTableLblCoreScracthStandard);
	DisplayLabel(pTableLblCoreScracthThreshold);
	DisplayLabel(pTableLblCoreScracthNum);
	DisplayLabel(pTableLblCoreDefectStandard);
	DisplayLabel(pTableLblCoreDefectThreshold);
	DisplayLabel(pTableLblCoreDefectNum);
	//包层
	DisplayLabel(pTableLblCladding);
	DisplayLabel(pTableLblCladdingRange);
	DisplayLabel(pTableLblCladdingSmallScracthStandard);
	DisplayLabel(pTableLblCladdingSmallScracthThreshold);
	DisplayLabel(pTableLblCladdingSmallScracthNum);
	DisplayLabel(pTableLblCladdingSmallDefectStandard);
	DisplayLabel(pTableLblCladdingSmallDefectThreshold);
	DisplayLabel(pTableLblCladdingSmallDefectNum);
	
	DisplayLabel(pTableLblCladdingMediumScracthStandard);
	DisplayLabel(pTableLblCladdingMediumScracthThreshold);
	DisplayLabel(pTableLblCladdingMediumScracthNum);
	DisplayLabel(pTableLblCladdingMediumDefectStandard);
	DisplayLabel(pTableLblCladdingMediumDefectThreshold);
	DisplayLabel(pTableLblCladdingMediumDefectNum);
	
	DisplayLabel(pTableLblCladdingLargeDefectStandard);
	DisplayLabel(pTableLblCladdingLargeDefectThreshold);
	DisplayLabel(pTableLblCladdingLargeDefectNum);
	//涂层
	DisplayLabel(pTableLblCoating);
	DisplayLabel(pTableLblCoatingRange);
	DisplayLabel(pTableLblCoatingScracthStandard);
	DisplayLabel(pTableLblCoatingScracthThreshold);
	DisplayLabel(pTableLblCoatingScracthNum);
	DisplayLabel(pTableLblCoatingDefectStandard);
	DisplayLabel(pTableLblCoatingDefectThreshold);
	DisplayLabel(pTableLblCoatingDefectNum);
	//套层
	DisplayLabel(pTableLblJacket);
	DisplayLabel(pTableLblJacketRange);
	DisplayLabel(pTableLblJacketSmallScracthStandard);
	DisplayLabel(pTableLblJacketSmallScracthThreshold);
	DisplayLabel(pTableLblJacketSmallScracthNum);
	DisplayLabel(pTableLblJacketSmallDefectStandard);
	DisplayLabel(pTableLblJacketSmallDefectThreshold);
	DisplayLabel(pTableLblJacketSmallDefectNum);
		
	DisplayLabel(pTableLblJacketLargeDefectStandard);
	DisplayLabel(pTableLblJacketLargeDefectThreshold);
	DisplayLabel(pTableLblJacketLargeDefectNum);
    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFipResultMenu);
	SetPowerEnable(1, 1);

	RefreshScreen(__FILE__, __func__, __LINE__);
	
    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipResultLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;

	MsecSleep(10);

    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipResultPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipResultResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        初始化文本资源
  * 参数：
  		...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FipResultTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
	/***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	char tmp[32];
	pFipResultStrFileName = TransString(OptFiberEndSurCheck.cName);
	pFipResultStrTitle = TransString("FIBERMICROSCOPE_TITLE");
	
	pFipResultStrTabResult = TransString("FIBEREND_RESULT");
	pFipResultStrTabIdentify = TransString("FIBEREND_IDENTIFY");

	//表头
	pTableStrHeaderLayeringUnit = TransString("(um)");
	pTableStrHeaderLayering = TransString("FIBER_LAYERING");
	pTableStrHeaderScracth = TransString("FIBER_SCRACTH");
	pTableStrHeaderScracthStandard = TransString("FIBER_CRITERION");
	pTableStrHeaderScracthThreshold = TransString("FIBER_THRESHOLD");
	pTableStrHeaderScracthNum = TransString("FIBER_AMOUNT");
	pTableStrHeaderDefect = TransString("FIBER_DEFECT");
	pTableStrHeaderDefectStandard = TransString("FIBER_CRITERION");
	pTableStrHeaderDefectThreshold = TransString("FIBER_THRESHOLD");
	pTableStrHeaderDefectNum = TransString("FIBER_AMOUNT");
	//纤芯
	pTableStrCore = TransString("FIBER_CORENOUNIT");
	pTableStrCoreRange = TransString("(0-25)");
	pTableStrCoreScracthStandard = TransString("FIBER_0_INF");
	pTableStrCoreScracthThreshold = TransString("0");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCoreScracth);
	pTableStrCoreScracthNum = TransString(tmp);
	pTableStrCoreDefectStandard = TransString("FIBER_0_INF");
	pTableStrCoreDefectThreshold = TransString("0");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCoreDefect);
	pTableStrCoreDefectNum = TransString(tmp);

	//包层
	pTableStrCladding = TransString("FIBER_CLADDINGNOUNIT");
	pTableStrCladdingRange = TransString("(25-120)");
	pTableStrCladdingSmallScracthStandard = TransString("FIBER_0_3");
	pTableStrCladdingSmallScracthThreshold = TransString("FIBER_ANY");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthSmall);
	pTableStrCladdingSmallScracthNum = TransString(tmp);
	pTableStrCladdingSmallDefectStandard = TransString("FIBER_0_2");
	pTableStrCladdingSmallDefectThreshold = TransString("FIBER_ANY");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectSmall);
	pTableStrCladdingSmallDefectNum = TransString(tmp);
	
	pTableStrCladdingMediumScracthStandard = TransString("FIBER_3_INF");
	pTableStrCladdingMediumScracthThreshold = TransString("0");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge);
	pTableStrCladdingMediumScracthNum = TransString(tmp);
	pTableStrCladdingMediumDefectStandard = TransString("FIBER_2_5");
	pTableStrCladdingMediumDefectThreshold = TransString("5");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium);
	pTableStrCladdingMediumDefectNum = TransString(tmp);
	
	pTableStrCladdingLargeDefectStandard = TransString("FIBER_5_INF");
	pTableStrCladdingLargeDefectThreshold = TransString("0");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge);
	pTableStrCladdingLargeDefectNum = TransString(tmp);

	//涂层
	pTableStrCoating = TransString("FIBER_BUFFERNOUNIT");
	pTableStrCoatingRange = TransString("(120-130)");
	pTableStrCoatingScracthStandard = TransString("--");
	pTableStrCoatingScracthThreshold = TransString("--");
	pTableStrCoatingScracthNum = TransString("--");
	pTableStrCoatingDefectStandard = TransString("--");
	pTableStrCoatingDefectThreshold = TransString("--");
	pTableStrCoatingDefectNum = TransString("--");

	//套层
	pTableStrJacket = TransString("FIBER_JACKETNOUNIT");
	pTableStrJacketRange = TransString("(130-250)");
	pTableStrJacketSmallScracthStandard = TransString("FIBER_0_INF");
	pTableStrJacketSmallScracthThreshold = TransString("FIBER_ANY");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberJacketScracth);
	pTableStrJacketSmallScracthNum = TransString(tmp);
	pTableStrJacketSmallDefectStandard = TransString("FIBER_0_10");
	pTableStrJacketSmallDefectThreshold = TransString("FIBER_ANY");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberJacketDefectSmall);
	pTableStrJacketSmallDefectNum = TransString(tmp);
	
	pTableStrJacketLargeDefectStandard = TransString("FIBER_10_INF");
	pTableStrJacketLargeDefectThreshold = TransString("0");
	memset(tmp,0,32);
	sprintf(tmp,"%d",OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge);
	pTableStrJacketLargeDefectNum = TransString(tmp);
    return 0;
}

/***
  * 功能：
        释放文本资源
  * 参数：
  		...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FipResultTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    /***************************************************************************
    *                           释放桌面上的文本
    ***************************************************************************/
	GuiMemFree(pFipResultStrTitle);
	GuiMemFree(pFipResultStrFileName);
	GuiMemFree(pFipResultStrTabResult);
	GuiMemFree(pFipResultStrTabIdentify);
	//表头
	GuiMemFree(pTableStrHeaderLayering);
	GuiMemFree(pTableStrHeaderLayeringUnit);
	GuiMemFree(pTableStrHeaderScracth);
	GuiMemFree(pTableStrHeaderScracthStandard);
	GuiMemFree(pTableStrHeaderScracthThreshold);
	GuiMemFree(pTableStrHeaderScracthNum);
	GuiMemFree(pTableStrHeaderDefect);
	GuiMemFree(pTableStrHeaderDefectStandard);
	GuiMemFree(pTableStrHeaderDefectThreshold);
	GuiMemFree(pTableStrHeaderDefectNum);

	//纤芯
	GuiMemFree(pTableStrCore);
	GuiMemFree(pTableStrCoreRange);
	GuiMemFree(pTableStrCoreScracthStandard);
	GuiMemFree(pTableStrCoreScracthThreshold);
	GuiMemFree(pTableStrCoreScracthNum);
	GuiMemFree(pTableStrCoreDefectStandard);
	GuiMemFree(pTableStrCoreDefectThreshold);
	GuiMemFree(pTableStrCoreDefectNum);

	//包层
	GuiMemFree(pTableStrCladding);
	GuiMemFree(pTableStrCladdingRange);
	GuiMemFree(pTableStrCladdingSmallScracthStandard);
	GuiMemFree(pTableStrCladdingSmallScracthThreshold);
	GuiMemFree(pTableStrCladdingSmallScracthNum);
	GuiMemFree(pTableStrCladdingSmallDefectStandard);
	GuiMemFree(pTableStrCladdingSmallDefectThreshold);
	GuiMemFree(pTableStrCladdingSmallDefectNum);
	
	GuiMemFree(pTableStrCladdingMediumScracthStandard);
	GuiMemFree(pTableStrCladdingMediumScracthThreshold);
	GuiMemFree(pTableStrCladdingMediumScracthNum);
	GuiMemFree(pTableStrCladdingMediumDefectStandard);
	GuiMemFree(pTableStrCladdingMediumDefectThreshold);
	GuiMemFree(pTableStrCladdingMediumDefectNum);
	
	GuiMemFree(pTableStrCladdingLargeDefectStandard);
	GuiMemFree(pTableStrCladdingLargeDefectThreshold);
	GuiMemFree(pTableStrCladdingLargeDefectNum);

	//涂层
	GuiMemFree(pTableStrCoating);
	GuiMemFree(pTableStrCoatingRange);
	GuiMemFree(pTableStrCoatingScracthStandard);
	GuiMemFree(pTableStrCoatingScracthThreshold);
	GuiMemFree(pTableStrCoatingScracthNum);
	GuiMemFree(pTableStrCoatingDefectStandard);
	GuiMemFree(pTableStrCoatingDefectThreshold);
	GuiMemFree(pTableStrCoatingDefectNum);

	//套层
	GuiMemFree(pTableStrJacket);
	GuiMemFree(pTableStrJacketRange);
	GuiMemFree(pTableStrJacketSmallScracthStandard);
	GuiMemFree(pTableStrJacketSmallScracthThreshold);
	GuiMemFree(pTableStrJacketSmallScracthNum);
	GuiMemFree(pTableStrJacketSmallDefectStandard);
	GuiMemFree(pTableStrJacketSmallDefectThreshold);
	GuiMemFree(pTableStrJacketSmallDefectNum);
	
	GuiMemFree(pTableStrJacketLargeDefectStandard);
	GuiMemFree(pTableStrJacketLargeDefectThreshold);
	GuiMemFree(pTableStrJacketLargeDefectNum);
	return 0;
}

static int FipResultBtnTabImage_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	if(iExitFlag == 0)
	{
		iExitFlag = 1;
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmFipInit, FrmFipExit, 
							FrmFipPaint, FrmFipLoop, 
							FrmFipPause, FrmFipResume,
							NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFipResult); 	//发送消息以便退出前一个窗体
		SendSysMsg_ThreadCreate(pWnd);
	}

	return iReturn;
}

static int FipResultBtnTabIdentify_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	if(iExitFlag == 0)
	{
		iExitFlag = 1;
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmFipIdentifyInit, FrmFipIdentifyExit, 
							FrmFipIdentifyPaint, FrmFipIdentifyLoop, 
							FrmFipIdentifyPause, FrmFipIdentifyResume,
							NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFipResult); 	//发送消息以便退出前一个窗体
		SendSysMsg_ThreadCreate(pWnd);
	}
	return iReturn;
}
