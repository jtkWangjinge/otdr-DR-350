/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfipidentify.c
* 摘    要：  实现主窗体frmfipidentify的窗体处理相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2017-2-7
*
* 取代版本： 
* 原 作 者：	
* 完成日期： 
*******************************************************************************/

#include "wnd_frmfipidentify.h"

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
#include "wnd_frmfipresult.h"
#include "wnd_frmime.h"
#include "guiphoto.h"
/*******************************************************************************
***         				为实现窗体frmfipresult而定义的变量		    		 ***
*******************************************************************************/
extern OpticalFiberEndSur OptFiberEndSurCheck;
extern int iCheckErrFlag;
extern FIPCALLLBACK FipCallBackFunc;

FipIdentify fipIdentify;
static int iExitFlag = 0;
//创建右边menu菜单所需数组
static unsigned int StrFipIdentifyMenuRealtime[] = {	
1
};

/*******************************************************************************
***                                窗体中的窗体控件                          ***
*******************************************************************************/
static GUIWINDOW *pFrmFipIdentify = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pFipIdentifyBg = NULL;
static GUILABEL *pFipIdentifyLblTitle = NULL;
static GUICHAR *pFipIdentifyStrTitle = NULL;
static GUIPICTURE *pFipIdentifyFileName = NULL;
static GUILABEL *pFipIdentifyLblFileName = NULL;
static GUICHAR *pFipIdentifyStrFileName = NULL;

static WNDMENU1 *pFipIdentifyMenu = NULL;

static GUIPICTURE *pFipIdentifyBtnTabImage = NULL;
static GUIPICTURE *pFipIdentifyBtnTabResult = NULL;
static GUIPICTURE *pFipIdentifyBtnTabIdentify = NULL;
static GUILABEL *pFipIdentifyLblTabResult = NULL;
static GUILABEL *pFipIdentifyLblTabIdentify = NULL;
static GUICHAR *pFipIdentifyStrTabResult = NULL;
static GUICHAR *pFipIdentifyStrTabIdentify = NULL;

static GUIPICTURE *pFipIdentifyBgTable = NULL;

static GUILABEL *pFipIdentifyLblTableIdentify = NULL;
static GUILABEL *pFipIdentifyLblTableValue = NULL;
static GUILABEL *pFipIdentifyLblTableFilename = NULL;
static GUILABEL *pFipIdentifyLblTableTask = NULL;
static GUILABEL *pFipIdentifyLblTableClient = NULL;
static GUILABEL *pFipIdentifyLblTableCompany = NULL;
static GUILABEL *pFipIdentifyLblTableNote = NULL;
static GUILABEL *pFipIdentifyLblTablePlaceA = NULL;
static GUILABEL *pFipIdentifyLblTablePlaceB = NULL;
static GUILABEL *pFipIdentifyLblTableCableID = NULL;
static GUILABEL *pFipIdentifyLblTableFiberID = NULL;
static GUILABEL *pFipIdentifyLblTableConnectorID = NULL;

static GUILABEL *pFipIdentifyLblTableTaskValue = NULL;
static GUILABEL *pFipIdentifyLblTableClientValue = NULL;
static GUILABEL *pFipIdentifyLblTableCompanyValue = NULL;
static GUILABEL *pFipIdentifyLblTableNoteValue = NULL;
static GUILABEL *pFipIdentifyLblTablePlaceAValue = NULL;
static GUILABEL *pFipIdentifyLblTablePlaceBValue = NULL;
static GUILABEL *pFipIdentifyLblTableCableIDValue = NULL;
static GUILABEL *pFipIdentifyLblTableFiberIDValue = NULL;
static GUILABEL *pFipIdentifyLblTableConnectorIDValue = NULL;

static GUICHAR *pFipIdentifyStrTableIdentify = NULL;
static GUICHAR *pFipIdentifyStrTableValue = NULL;
static GUICHAR *pFipIdentifyStrTableFilename = NULL;
static GUICHAR *pFipIdentifyStrTableTask = NULL;
static GUICHAR *pFipIdentifyStrTableClient = NULL;
static GUICHAR *pFipIdentifyStrTableCompany = NULL;
static GUICHAR *pFipIdentifyStrTableNote = NULL;
static GUICHAR *pFipIdentifyStrTablePlaceA = NULL;
static GUICHAR *pFipIdentifyStrTablePlaceB = NULL;
static GUICHAR *pFipIdentifyStrTableCableID = NULL;
static GUICHAR *pFipIdentifyStrTableFiberID = NULL;
static GUICHAR *pFipIdentifyStrTableConnectorID = NULL;

static GUICHAR *pFipIdentifyStrTableTaskValue = NULL;
static GUICHAR *pFipIdentifyStrTableClientValue = NULL;
static GUICHAR *pFipIdentifyStrTableCompanyValue = NULL;
static GUICHAR *pFipIdentifyStrTableNoteValue = NULL;
static GUICHAR *pFipIdentifyStrTablePlaceAValue = NULL;
static GUICHAR *pFipIdentifyStrTablePlaceBValue = NULL;
static GUICHAR *pFipIdentifyStrTableCableIDValue = NULL;
static GUICHAR *pFipIdentifyStrTableFiberIDValue = NULL;
static GUICHAR *pFipIdentifyStrTableConnectorIDValue = NULL;

static GUIPICTURE *pFipIdentifyBtnTask = NULL;
static GUIPICTURE *pFipIdentifyBtnClient = NULL;
static GUIPICTURE *pFipIdentifyBtnCompany = NULL;
static GUIPICTURE *pFipIdentifyBtnNote = NULL;
static GUIPICTURE *pFipIdentifyBtnPlaceA = NULL;
static GUIPICTURE *pFipIdentifyBtnPlaceB = NULL;
static GUIPICTURE *pFipIdentifyBtnCableID = NULL;
static GUIPICTURE *pFipIdentifyBtnFiberID = NULL;
static GUIPICTURE *pFipIdentifyBtnConnectorID = NULL;

static GUIPICTURE *pFipIdentifyBtnTaskTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnClientTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnCompanyTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnNoteTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnPlaceATouch = NULL;
static GUIPICTURE *pFipIdentifyBtnPlaceBTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnCableIDTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnFiberIDTouch = NULL;
static GUIPICTURE *pFipIdentifyBtnConnectorIDTouch = NULL;

/*******************************************************************************
***                        窗体中的文本资源处理函数                          ***
*******************************************************************************/

//初始化文本资源
static int FipIdentifyTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
//释放文本资源
static int FipIdentifyTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

static void FipIdentifyMenuCallBack(int iOption);

static int FipIdentifyBtnTabImage_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int FipIdentifyBtnTabResult_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);

static int FipIdentifyBtnValue_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int FipIdentifyBtnValue_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);

static int FipIdentifyBtnSelect_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int FipIdentifyBtnSelect_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);


/*  侧边菜单控件回调函数 */
static void FipIdentifyMenuCallBack(int iOption)
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
		SendWndMsg_WindowExit(pFrmFipIdentify); 	//发送消息以便退出前一个窗体
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
	    SendWndMsg_WindowExit(pFrmFipIdentify);  
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
int FrmFipIdentifyInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	
	//得到当前窗体对象 
    pFrmFipIdentify = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FipIdentifyTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件
    ***************************************************************************/
    pFipIdentifyBg = CreatePhoto("fipidentify_bg");
    pFipIdentifyFileName = CreatePhoto("otdr_top5");

	pFipIdentifyLblFileName = CreateLabel(400, 22, 281, 24, pFipIdentifyStrFileName);
	SetLabelAlign(2, pFipIdentifyLblFileName);
    pFipIdentifyLblTitle = CreateLabel(0, 22, 100, 24, pFipIdentifyStrTitle);
	pFipIdentifyLblTabResult = CreateLabel(100, 22, 100, 24, pFipIdentifyStrTabResult);
	pFipIdentifyLblTabIdentify= CreateLabel(200, 22, 100, 24, pFipIdentifyStrTabIdentify);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFipIdentifyLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFipIdentifyLblTabResult);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFipIdentifyLblTabIdentify);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTabResult);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFipIdentifyLblTabIdentify);
    
    pFipIdentifyBtnTabImage = CreatePhoto("otdr_top1");
    pFipIdentifyBtnTabResult = CreatePhoto("otdr_top2");
    pFipIdentifyBtnTabIdentify = CreatePhoto("otdr_top3f");
    pFipIdentifyBgTable = CreatePhoto("fip_identify_table");
    
	pFipIdentifyLblTableIdentify = CreateLabel(150, 55, 120, 24, pFipIdentifyStrTableIdentify);
	pFipIdentifyLblTableValue = CreateLabel(271, 55, 180, 24, pFipIdentifyStrTableValue);
	pFipIdentifyLblTableFilename = CreateLabel(451, 55, 106, 24, pFipIdentifyStrTableFilename);
    SetLabelAlign(2, pFipIdentifyLblTableIdentify);
    SetLabelAlign(2, pFipIdentifyLblTableValue);
    SetLabelAlign(2, pFipIdentifyLblTableFilename);
    
	pFipIdentifyLblTableTask = CreateLabel(150, 90, 120, 24, pFipIdentifyStrTableTask);
	pFipIdentifyLblTableClient = CreateLabel(150, 90+43, 120, 24, pFipIdentifyStrTableClient);
	pFipIdentifyLblTableCompany = CreateLabel(150, 90+43*2, 120, 24, pFipIdentifyStrTableCompany);
	pFipIdentifyLblTableNote = CreateLabel(150, 90+43*3, 120, 24, pFipIdentifyStrTableNote);
	pFipIdentifyLblTablePlaceA = CreateLabel(150, 90+43*4, 120, 24, pFipIdentifyStrTablePlaceA);
	pFipIdentifyLblTablePlaceB = CreateLabel(150, 90+43*5, 120, 24, pFipIdentifyStrTablePlaceB);
	pFipIdentifyLblTableCableID = CreateLabel(150, 90+43*6, 120, 24, pFipIdentifyStrTableCableID);
	pFipIdentifyLblTableFiberID = CreateLabel(150, 90+43*7, 120, 24, pFipIdentifyStrTableFiberID);
	pFipIdentifyLblTableConnectorID = CreateLabel(150, 90+43*8, 120, 24, pFipIdentifyStrTableConnectorID);
    
	pFipIdentifyLblTableTaskValue = CreateLabel(275, 90, 180, 28, pFipIdentifyStrTableTaskValue);
	pFipIdentifyLblTableClientValue = CreateLabel(275, 90+43, 180, 28, pFipIdentifyStrTableClientValue);
	pFipIdentifyLblTableCompanyValue = CreateLabel(275, 90+43*2, 180, 28, pFipIdentifyStrTableCompanyValue);
	pFipIdentifyLblTableNoteValue = CreateLabel(275, 90+43*3, 180, 28, pFipIdentifyStrTableNoteValue);
	pFipIdentifyLblTablePlaceAValue = CreateLabel(275, 90+43*4, 180, 28, pFipIdentifyStrTablePlaceAValue);
	pFipIdentifyLblTablePlaceBValue = CreateLabel(275, 90+43*5, 180, 28, pFipIdentifyStrTablePlaceBValue);
	pFipIdentifyLblTableCableIDValue = CreateLabel(275, 90+43*6, 180, 28, pFipIdentifyStrTableCableIDValue);
	pFipIdentifyLblTableFiberIDValue = CreateLabel(275, 90+43*7, 180, 28, pFipIdentifyStrTableFiberIDValue);
	pFipIdentifyLblTableConnectorIDValue = CreateLabel(275, 90+43*8, 180, 28, pFipIdentifyStrTableConnectorIDValue);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableTask);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableClient);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableCompany);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableNote);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTablePlaceA);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTablePlaceB);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableCableID);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableFiberID);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableConnectorID);
	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableTaskValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableClientValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableCompanyValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableNoteValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTablePlaceAValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTablePlaceBValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableCableIDValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableFiberIDValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFipIdentifyLblTableConnectorIDValue);

    SetLabelAlign(2, pFipIdentifyLblTableTask);
    SetLabelAlign(2, pFipIdentifyLblTableClient);
    SetLabelAlign(2, pFipIdentifyLblTableCompany);
    SetLabelAlign(2, pFipIdentifyLblTableNote);
    SetLabelAlign(2, pFipIdentifyLblTablePlaceA);
    SetLabelAlign(2, pFipIdentifyLblTablePlaceB);
    SetLabelAlign(2, pFipIdentifyLblTableCableID);
    SetLabelAlign(2, pFipIdentifyLblTableFiberID);
    SetLabelAlign(2, pFipIdentifyLblTableConnectorID);

    SetLabelAlign(2, pFipIdentifyLblTableTaskValue);
    SetLabelAlign(2, pFipIdentifyLblTableClientValue);
    SetLabelAlign(2, pFipIdentifyLblTableCompanyValue);
    SetLabelAlign(2, pFipIdentifyLblTableNoteValue);
    SetLabelAlign(2, pFipIdentifyLblTablePlaceAValue);
    SetLabelAlign(2, pFipIdentifyLblTablePlaceBValue);
    SetLabelAlign(2, pFipIdentifyLblTableCableIDValue);
    SetLabelAlign(2, pFipIdentifyLblTableFiberIDValue);
    SetLabelAlign(2, pFipIdentifyLblTableConnectorIDValue);

	pFipIdentifyBtnTask = CreatePicture(492, 85, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnClient = CreatePicture(492, 85+43, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnCompany = CreatePicture(492, 85+43*2, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnNote = CreatePicture(492, 85+43*3, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnPlaceA = CreatePicture(492, 85+43*4, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnPlaceB = CreatePicture(492, 85+43*5, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnCableID = CreatePicture(492, 85+43*6, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnFiberID = CreatePicture(492, 85+43*7, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");
	pFipIdentifyBtnConnectorID = CreatePicture(492, 85+43*8, 30, 30,BmpFileDirectory"fip_identify_uncheck.bmp");

	pFipIdentifyBtnTaskTouch = CreatePicture(452, 78, 100, 32, NULL);
	pFipIdentifyBtnClientTouch = CreatePicture(452, 78+43, 100, 32, NULL);
	pFipIdentifyBtnCompanyTouch = CreatePicture(452, 78+43*2, 100, 32, NULL);
	pFipIdentifyBtnNoteTouch = CreatePicture(452, 78+43*3, 100, 32, NULL);
	pFipIdentifyBtnPlaceATouch = CreatePicture(452, 78+43*4, 100, 32, NULL);
	pFipIdentifyBtnPlaceBTouch = CreatePicture(452, 78+43*5, 100, 32, NULL);
	pFipIdentifyBtnCableIDTouch = CreatePicture(452, 78+43*6, 100, 32, NULL);
	pFipIdentifyBtnFiberIDTouch = CreatePicture(452, 78+43*7, 100, 32, NULL);
	pFipIdentifyBtnConnectorIDTouch = CreatePicture(450, 78+43*8, 100, 32, NULL);

    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFipIdentifyMenu = CreateWndMenu1(0, sizeof(StrFipIdentifyMenuRealtime), StrFipIdentifyMenuRealtime, 0xffff,
								 -1, 0, 40, FipIdentifyMenuCallBack);

    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFipIdentify, pFrmFipIdentify);
	
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnTabImage, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnTabResult, pFrmFipIdentify);
	
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableTaskValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableClientValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableCompanyValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableNoteValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTablePlaceAValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTablePlaceBValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableCableIDValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableFiberIDValue, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFipIdentifyLblTableConnectorIDValue, pFrmFipIdentify);
	
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnTaskTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnClientTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnCompanyTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnNoteTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnPlaceATouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnPlaceBTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnCableIDTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnFiberIDTouch, pFrmFipIdentify);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipIdentifyBtnConnectorIDTouch, pFrmFipIdentify);

    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFipIdentifyMenu, pFrmFipIdentify);
	
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	LoginWndMenuToMsg1(pFipIdentifyMenu, pFrmFipIdentify);
	GUIMESSAGE *pMsg = GetCurrMessage();
    
	LoginMessageReg(GUIMESSAGE_TCH_DOWN,pFipIdentifyBtnTabImage,
            		FipIdentifyBtnTabImage_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnTabResult,
            		FipIdentifyBtnTabResult_Down, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableTaskValue,
            		FipIdentifyBtnValue_Down, NULL, 1, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableTaskValue,
            		FipIdentifyBtnValue_Up, NULL, 1, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableClientValue,
            		FipIdentifyBtnValue_Down, NULL, 2, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableClientValue,
            		FipIdentifyBtnValue_Up, NULL, 2, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableCompanyValue,
            		FipIdentifyBtnValue_Down, NULL, 3, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableCompanyValue,
            		FipIdentifyBtnValue_Up, NULL, 3, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableNoteValue,
            		FipIdentifyBtnValue_Down, NULL, 4, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableNoteValue,
            		FipIdentifyBtnValue_Up, NULL, 4, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTablePlaceAValue,
            		FipIdentifyBtnValue_Down, NULL, 5, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTablePlaceAValue,
            		FipIdentifyBtnValue_Up, NULL, 5, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTablePlaceBValue,
            		FipIdentifyBtnValue_Down, NULL, 6, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTablePlaceBValue,
            		FipIdentifyBtnValue_Up, NULL, 6, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableCableIDValue,
            		FipIdentifyBtnValue_Down, NULL, 7, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableCableIDValue,
            		FipIdentifyBtnValue_Up, NULL, 7, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableFiberIDValue,
            		FipIdentifyBtnValue_Down, NULL, 8, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableFiberIDValue,
            		FipIdentifyBtnValue_Up, NULL, 8, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyLblTableConnectorIDValue,
            		FipIdentifyBtnValue_Down, NULL, 9, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyLblTableConnectorIDValue,
            		FipIdentifyBtnValue_Up, NULL, 9, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnTaskTouch,
            		FipIdentifyBtnSelect_Down, NULL, 1, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnTaskTouch,
            		FipIdentifyBtnSelect_Up, NULL, 1, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnClientTouch,
            		FipIdentifyBtnSelect_Down, NULL, 2, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnClientTouch,
            		FipIdentifyBtnSelect_Up, NULL, 2, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnCompanyTouch,
            		FipIdentifyBtnSelect_Down, NULL, 3, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnCompanyTouch,
            		FipIdentifyBtnSelect_Up, NULL, 3, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnNoteTouch,
            		FipIdentifyBtnSelect_Down, NULL, 4, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnNoteTouch,
            		FipIdentifyBtnSelect_Up, NULL, 4, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnPlaceATouch,
            		FipIdentifyBtnSelect_Down, NULL, 5, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnPlaceATouch,
            		FipIdentifyBtnSelect_Up, NULL, 5, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnPlaceBTouch,
            		FipIdentifyBtnSelect_Down, NULL, 6, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnPlaceBTouch,
            		FipIdentifyBtnSelect_Up, NULL, 6, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnCableIDTouch,
            		FipIdentifyBtnSelect_Down, NULL, 7, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnCableIDTouch,
            		FipIdentifyBtnSelect_Up, NULL, 7, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnFiberIDTouch,
            		FipIdentifyBtnSelect_Down, NULL, 8, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnFiberIDTouch,
            		FipIdentifyBtnSelect_Up, NULL, 8, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFipIdentifyBtnConnectorIDTouch,
            		FipIdentifyBtnSelect_Down, NULL, 9, pMsg);	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFipIdentifyBtnConnectorIDTouch,
            		FipIdentifyBtnSelect_Up, NULL, 9, pMsg);
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
int FrmFipIdentifyExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    	
    //得到当前窗体对象
    pFrmFipIdentify = (GUIWINDOW *) pWndObj;

    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFipIdentify);
	
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFipIdentifyBg);
    DestroyPicture(&pFipIdentifyFileName);

	DestroyLabel(&pFipIdentifyLblFileName);
	DestroyLabel(&pFipIdentifyLblTitle);	
	DestroyLabel(&pFipIdentifyLblTabResult);
	DestroyLabel(&pFipIdentifyLblTabIdentify);
    
	DestroyPicture(&pFipIdentifyBtnTabImage);
	DestroyPicture(&pFipIdentifyBtnTabResult);
	DestroyPicture(&pFipIdentifyBtnTabIdentify);
	DestroyPicture(&pFipIdentifyBgTable);
	
	DestroyLabel(&pFipIdentifyLblTableIdentify);
	DestroyLabel(&pFipIdentifyLblTableValue);
	DestroyLabel(&pFipIdentifyLblTableFilename);
	DestroyLabel(&pFipIdentifyLblTableTask);
	DestroyLabel(&pFipIdentifyLblTableClient);
	DestroyLabel(&pFipIdentifyLblTableCompany);
	DestroyLabel(&pFipIdentifyLblTableNote);
	DestroyLabel(&pFipIdentifyLblTablePlaceA);
	DestroyLabel(&pFipIdentifyLblTablePlaceB);
	DestroyLabel(&pFipIdentifyLblTableCableID);
	DestroyLabel(&pFipIdentifyLblTableFiberID);
	DestroyLabel(&pFipIdentifyLblTableConnectorID);
	
	DestroyLabel(&pFipIdentifyLblTableTaskValue);
	DestroyLabel(&pFipIdentifyLblTableClientValue);
	DestroyLabel(&pFipIdentifyLblTableCompanyValue);
	DestroyLabel(&pFipIdentifyLblTableNoteValue);
	DestroyLabel(&pFipIdentifyLblTablePlaceAValue);
	DestroyLabel(&pFipIdentifyLblTablePlaceBValue);
	DestroyLabel(&pFipIdentifyLblTableCableIDValue);
	DestroyLabel(&pFipIdentifyLblTableFiberIDValue);
	DestroyLabel(&pFipIdentifyLblTableConnectorIDValue);
	
	DestroyPicture(&pFipIdentifyBtnTask);
	DestroyPicture(&pFipIdentifyBtnClient);
	DestroyPicture(&pFipIdentifyBtnCompany);
	DestroyPicture(&pFipIdentifyBtnNote);
	DestroyPicture(&pFipIdentifyBtnPlaceA);
	DestroyPicture(&pFipIdentifyBtnPlaceB);
	DestroyPicture(&pFipIdentifyBtnCableID);
	DestroyPicture(&pFipIdentifyBtnFiberID);
	DestroyPicture(&pFipIdentifyBtnConnectorID);
	DestroyPicture(&pFipIdentifyBtnTaskTouch);
	DestroyPicture(&pFipIdentifyBtnClientTouch);
	DestroyPicture(&pFipIdentifyBtnCompanyTouch);
	DestroyPicture(&pFipIdentifyBtnNoteTouch);
	DestroyPicture(&pFipIdentifyBtnPlaceATouch);
	DestroyPicture(&pFipIdentifyBtnPlaceBTouch);
	DestroyPicture(&pFipIdentifyBtnCableIDTouch);
	DestroyPicture(&pFipIdentifyBtnFiberIDTouch);
	DestroyPicture(&pFipIdentifyBtnConnectorIDTouch);

    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFipIdentifyMenu);

    //释放文本资源
    FipIdentifyTextRes_Exit(NULL, 0, NULL, 0);
	
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
int FrmFipIdentifyPaint(void *pWndObj)
{
    ///错误标志、返回值定义
    int iRet = 0;

    //得到当前窗体对象
    pFrmFipIdentify = (GUIWINDOW *) pWndObj;
	iExitFlag = 0;
    /***************************************************************************
    *                      显示桌面上各个区域的背景控件
    ***************************************************************************/
    DisplayPicture(pFipIdentifyBg);
	DisplayPicture(pFipIdentifyBtnTabImage);
	DisplayPicture(pFipIdentifyBtnTabIdentify);

	DisplayLabel(pFipIdentifyLblTitle);
	DisplayLabel(pFipIdentifyLblFileName);
	DisplayLabel(pFipIdentifyLblTabResult);
	DisplayLabel(pFipIdentifyLblTabIdentify);

	DisplayPicture(pFipIdentifyBgTable);
	
	DisplayLabel(pFipIdentifyLblTableIdentify);
	DisplayLabel(pFipIdentifyLblTableValue);
	DisplayLabel(pFipIdentifyLblTableFilename);
	DisplayLabel(pFipIdentifyLblTableTask);
	DisplayLabel(pFipIdentifyLblTableClient);
	DisplayLabel(pFipIdentifyLblTableCompany);
	DisplayLabel(pFipIdentifyLblTableNote);
	DisplayLabel(pFipIdentifyLblTablePlaceA);
	DisplayLabel(pFipIdentifyLblTablePlaceB);
	DisplayLabel(pFipIdentifyLblTableCableID);
	DisplayLabel(pFipIdentifyLblTableFiberID);
	DisplayLabel(pFipIdentifyLblTableConnectorID);
	DisplayLabel(pFipIdentifyLblTableTaskValue);
	DisplayLabel(pFipIdentifyLblTableClientValue);
	DisplayLabel(pFipIdentifyLblTableCompanyValue);
	DisplayLabel(pFipIdentifyLblTableNoteValue);
	DisplayLabel(pFipIdentifyLblTablePlaceAValue);
	DisplayLabel(pFipIdentifyLblTablePlaceBValue);
	DisplayLabel(pFipIdentifyLblTableCableIDValue);
	DisplayLabel(pFipIdentifyLblTableFiberIDValue);
	DisplayLabel(pFipIdentifyLblTableConnectorIDValue);
	
	if(fipIdentify.taskIdentifyIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnTask);
	}
	if(fipIdentify.clientIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnClient);
	}	
	if(fipIdentify.companyIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnCompany);
	}	
	if(fipIdentify.noteIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnNote);
	}	
	if(fipIdentify.positionAIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnPlaceA);
	}	
	if(fipIdentify.positionBIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnPlaceB);
	}	
	if(fipIdentify.cableIDIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnCableID);
	}	
	if(fipIdentify.fiberIDIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnFiberID);
	}	
	if(fipIdentify.connectorIDIsSelect)
	{
		SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnConnectorID);
	}
	
	DisplayPicture(pFipIdentifyBtnTask);
	DisplayPicture(pFipIdentifyBtnClient);
	DisplayPicture(pFipIdentifyBtnCompany);
	DisplayPicture(pFipIdentifyBtnNote);
	DisplayPicture(pFipIdentifyBtnPlaceA);
	DisplayPicture(pFipIdentifyBtnPlaceB);
	DisplayPicture(pFipIdentifyBtnCableID);
	DisplayPicture(pFipIdentifyBtnFiberID);
	DisplayPicture(pFipIdentifyBtnConnectorID);

    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFipIdentifyMenu);
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
int FrmFipIdentifyLoop(void *pWndObj)
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
int FrmFipIdentifyPause(void *pWndObj)
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
int FrmFipIdentifyResume(void *pWndObj)
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
static int FipIdentifyTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
	/***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	pFipIdentifyStrFileName = TransString(OptFiberEndSurCheck.cName);
	pFipIdentifyStrTitle = TransString("FIBERMICROSCOPE_TITLE");
	
	pFipIdentifyStrTabResult = TransString("FIBEREND_RESULT");
	pFipIdentifyStrTabIdentify = TransString("FIBEREND_IDENTIFY");
	
	pFipIdentifyStrTableIdentify = TransString("FIBEREND_IDENTIFY");
	pFipIdentifyStrTableValue = TransString("FIBER_VALUE");
	pFipIdentifyStrTableFilename = TransString("FIBER_FILENAME");
	pFipIdentifyStrTableTask = TransString("FIBER_TASK");
	pFipIdentifyStrTableClient = TransString("FIBER_CLIENT");
	pFipIdentifyStrTableCompany = TransString("FIBER_COMPANY");
	pFipIdentifyStrTableNote = TransString("FIBER_NOTE");
	pFipIdentifyStrTablePlaceA = TransString("FIBER_POSITIONA");
	pFipIdentifyStrTablePlaceB = TransString("FIBER_POSITIONB");
	pFipIdentifyStrTableCableID = TransString("SOLA_MANAGE_CABLEID");
	pFipIdentifyStrTableFiberID = TransString("SOLA_MANAGE_FIBERID");
	pFipIdentifyStrTableConnectorID = TransString("FIP_REPORT_CONNECTOR_ID");

	pFipIdentifyStrTableTaskValue = TransString(fipIdentify.taskIdentify);
	pFipIdentifyStrTableClientValue = TransString(fipIdentify.client);
	pFipIdentifyStrTableCompanyValue = TransString(fipIdentify.company);
	pFipIdentifyStrTableNoteValue = TransString(fipIdentify.note);
	pFipIdentifyStrTablePlaceAValue = TransString(fipIdentify.positionA);
	pFipIdentifyStrTablePlaceBValue = TransString(fipIdentify.positionB);
	pFipIdentifyStrTableCableIDValue = TransString(fipIdentify.cableID);
	pFipIdentifyStrTableFiberIDValue = TransString(fipIdentify.fiberID);
	pFipIdentifyStrTableConnectorIDValue = TransString(fipIdentify.connectorID);
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
static int FipIdentifyTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    /***************************************************************************
    *                           释放桌面上的文本
    ***************************************************************************/
	GuiMemFree(pFipIdentifyStrTitle);
	GuiMemFree(pFipIdentifyStrFileName);
	GuiMemFree(pFipIdentifyStrTabResult);
	GuiMemFree(pFipIdentifyStrTabIdentify);

	GuiMemFree(pFipIdentifyStrTableIdentify);
	GuiMemFree(pFipIdentifyStrTableValue);
	GuiMemFree(pFipIdentifyStrTableFilename);
	GuiMemFree(pFipIdentifyStrTableTask);
	GuiMemFree(pFipIdentifyStrTableClient);
	GuiMemFree(pFipIdentifyStrTableCompany);
	GuiMemFree(pFipIdentifyStrTableNote);
	GuiMemFree(pFipIdentifyStrTablePlaceA);
	GuiMemFree(pFipIdentifyStrTablePlaceB);
	GuiMemFree(pFipIdentifyStrTableCableID);
	GuiMemFree(pFipIdentifyStrTableFiberID);
	GuiMemFree(pFipIdentifyStrTableConnectorID);
	
	GuiMemFree(pFipIdentifyStrTableTaskValue);
	GuiMemFree(pFipIdentifyStrTableClientValue);
	GuiMemFree(pFipIdentifyStrTableCompanyValue);
	GuiMemFree(pFipIdentifyStrTableNoteValue);
	GuiMemFree(pFipIdentifyStrTablePlaceAValue);
	GuiMemFree(pFipIdentifyStrTablePlaceBValue);
	GuiMemFree(pFipIdentifyStrTableCableIDValue);
	GuiMemFree(pFipIdentifyStrTableFiberIDValue);
	GuiMemFree(pFipIdentifyStrTableConnectorIDValue);

	return 0;
}

/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateFipIdentifyWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmFipIdentifyInit, FrmFipIdentifyExit, 
		FrmFipIdentifyPaint, FrmFipIdentifyLoop, 
		FrmFipIdentifyPause, FrmFipIdentifyResume,
		NULL);
}
/***
  * 功能：
        		用户自定义输入字符串回调函数
  * 参数：
		无
  * 返回：
        		无
  * 备注：
***/
static char *SelectItem = NULL;
static void FipIdentifyUserInputCallFun(void)
{
	char cTmpInputBuff[512];

	GetIMEInputBuff(cTmpInputBuff);
	if(SelectItem)
	{
		strcpy(SelectItem, cTmpInputBuff);
	}
}

static int FipIdentifyBtnTabImage_Down(void *pInArg,   int iInLen, 
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
		SendWndMsg_WindowExit(pFrmFipIdentify); 	//发送消息以便退出前一个窗体
		SendSysMsg_ThreadCreate(pWnd);
	}
	return iReturn;
}

static int FipIdentifyBtnTabResult_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	if(iExitFlag == 0)
	{
		iExitFlag = 1;
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmFipResultInit, FrmFipResultExit, 
							FrmFipResultPaint, FrmFipResultLoop, 
							FrmFipResultPause, FrmFipResultResume,
							NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFipIdentify); 	//发送消息以便退出前一个窗体
		SendSysMsg_ThreadCreate(pWnd);
	}
	return iReturn;
}


static int FipIdentifyBtnValue_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;


	return iReturn;
}
static int FipIdentifyBtnValue_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	char buff[128] = {0};
	switch(iOutLen)
	{
		case 1:
			if(strcmp(fipIdentify.taskIdentify,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.taskIdentify);
			}
			SelectItem = fipIdentify.taskIdentify;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 2:
			if(strcmp(fipIdentify.client,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.client);
			}
			SelectItem = fipIdentify.client;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 3:
			if(strcmp(fipIdentify.company,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.company);
			}
			SelectItem = fipIdentify.company;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 4:
			if(strcmp(fipIdentify.note,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.note);
			}
			SelectItem = fipIdentify.note;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 5:
			if(strcmp(fipIdentify.positionA,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.positionA);
			}
			SelectItem = fipIdentify.positionA;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 6:
			if(strcmp(fipIdentify.positionB,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.positionB);
			}
			SelectItem = fipIdentify.positionB;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 7:
			if(strcmp(fipIdentify.cableID,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.cableID);
			}
			SelectItem = fipIdentify.cableID;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 8:
			if(strcmp(fipIdentify.fiberID,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.fiberID);
			}
			SelectItem = fipIdentify.fiberID;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		case 9:
			if(strcmp(fipIdentify.connectorID,"") != 0)
			{
				sprintf(buff, "%s", fipIdentify.connectorID);
			}
			SelectItem = fipIdentify.connectorID;
			IMEInit(buff, 10, 0, ReCreateFipIdentifyWindow, FipIdentifyUserInputCallFun, NULL);
			break;
		default:
			break;
	}
	return iReturn;
}

static int FipIdentifyBtnSelect_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	switch(iOutLen)
	{
		case 1:
			fipIdentify.taskIdentifyIsSelect = fipIdentify.taskIdentifyIsSelect ? 0 : 1;
			if(fipIdentify.taskIdentifyIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnTask);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnTask);
			}
			DisplayPicture(pFipIdentifyBtnTask);
			break;
		case 2:			
			fipIdentify.clientIsSelect = fipIdentify.clientIsSelect ? 0 : 1;
			if(fipIdentify.clientIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnClient);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnClient);
			}
			DisplayPicture(pFipIdentifyBtnClient);
			break;
		case 3:
			fipIdentify.companyIsSelect = fipIdentify.companyIsSelect ? 0 : 1;
			if(fipIdentify.companyIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnCompany);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnCompany);
			}
			DisplayPicture(pFipIdentifyBtnCompany);
			break;
		case 4:
			fipIdentify.noteIsSelect = fipIdentify.noteIsSelect ? 0 : 1;
			if(fipIdentify.noteIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnNote);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnNote);
			}
			DisplayPicture(pFipIdentifyBtnNote);
			break;
		case 5:
			fipIdentify.positionAIsSelect = fipIdentify.positionAIsSelect ? 0 : 1;
			if(fipIdentify.positionAIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnPlaceA);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnPlaceA);
			}
			DisplayPicture(pFipIdentifyBtnPlaceA);
			break;
		case 6:
			fipIdentify.positionBIsSelect = fipIdentify.positionBIsSelect ? 0 : 1;
			if(fipIdentify.positionBIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnPlaceB);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnPlaceB);
			}
			DisplayPicture(pFipIdentifyBtnPlaceB);
			break;
		case 7:
			fipIdentify.cableIDIsSelect = fipIdentify.cableIDIsSelect ? 0 : 1;
			if(fipIdentify.cableIDIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnCableID);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnCableID);
			}
			DisplayPicture(pFipIdentifyBtnCableID);
			break;
		case 8:
			fipIdentify.fiberIDIsSelect = fipIdentify.fiberIDIsSelect ? 0 : 1;
			if(fipIdentify.fiberIDIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnFiberID);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnFiberID);
			}
			DisplayPicture(pFipIdentifyBtnFiberID);
			break;
		case 9:
			fipIdentify.connectorIDIsSelect = fipIdentify.connectorIDIsSelect ? 0 : 1;
			if(fipIdentify.connectorIDIsSelect)
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_check.bmp", pFipIdentifyBtnConnectorID);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"fip_identify_uncheck.bmp", pFipIdentifyBtnConnectorID);
			}
			DisplayPicture(pFipIdentifyBtnConnectorID);
			break;
		default:
			break;
	}
	
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}
static int FipIdentifyBtnSelect_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	int iReturn = 0;

	return iReturn;
}

