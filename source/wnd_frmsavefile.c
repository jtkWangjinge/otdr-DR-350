/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsavefile.c
* 摘    要：  实现主窗体frmsavefile的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/26
*
*******************************************************************************/

#include "wnd_frmsavefile.h"

#include "app_curve.h"
#include "app_frmotdr.h"
#include "app_pdfreportotdr.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"
#include "guimessage.h"

#include "wnd_frmmenubak.h"
#include "wnd_frmdialog.h"
#include "wnd_frmotdrfileopen.h"
#include "wnd_frmotdr.h"

typedef struct _otdr_save_curve
{
	int iSaveCurveNum;					//显示曲线的个数
	int iActiveCurve;					//当前活动曲线的序号
	int iActCurItemSelect;				//当前活动曲线Item选中条
	int iSaveCurveFlag[CURVE_MAX];		//每条曲线是否保存的标志
}OTDR_SAVE_CURVE;

static GUIWINDOW* pFrmSaveFile = NULL;

static GUIPICTURE* pSaveFileBg = NULL;										//背景
static GUIPICTURE *pSaveFileBgLeft = NULL;									//左侧背景
static GUIPICTURE* pSaveFileBgIcon = NULL;									//图标

static GUILABEL* pSaveFileLblTitle = NULL;									//标题
static GUICHAR* pSaveFileStrTitle = NULL;

static GUIPICTURE* pSaveFileBtnFunctionTitle[FILE_OPTION_NUM] = {NULL};		//标题背景（保存文件+打开文件）
static GUILABEL* pSaveFileLblFunctionTitle[FILE_OPTION_NUM] = {NULL};		//功能栏标题
static GUICHAR* pSaveFileStrFunctionTitle[FILE_OPTION_NUM] = {NULL};
//返回按钮
static GUIPICTURE* pSaveFileBtnBack = NULL;
//
static WNDMENU1	*pFrmSaveFileMenu = NULL;									//菜单栏
//各项非选中背景资源
static char *pSaveFileUnpressBmp[2] =
{
    BmpFileDirectory "bg_savefile_save.bmp",
    BmpFileDirectory "bg_savefile_exit.bmp"
};

//各项选中背景资源
static char *pSaveFileSelectBmp[2] =
{
    BmpFileDirectory "bg_savefile_saveSelect.bmp",
    BmpFileDirectory "bg_savefile_exitSelect.bmp"
};
extern CURR_WINDOW_TYPE enCurWindow;										//当前的窗口

//中间选择框
static GUIPICTURE *pSaveFilePicSaveItem[8] = { NULL };						//图标
static GUIPICTURE *pSaveFilePicActItem[8] = { NULL };						//曲线名称背景
static GUIPICTURE *pSaveFilePicPdfItem[8] = { NULL };						//pdf名称背景
static GUILABEL	  *pSaveFileLblActItem[8] = { NULL };						//曲线名文本
static GUICHAR    *pSaveFileStrActItem[8] = { NULL };
static GUILABEL	  *pSaveFileLblPdfItem[8] = { NULL };						//曲线名文本
static GUICHAR    *pSaveFileStrPdfItem[8] = { NULL };

//活动曲线背景和文本
static GUILABEL* pSaveFileLblActiveCurveTitle = NULL;						//标题
static GUICHAR* pSaveFileStrActiveCurveTitle = NULL;
static GUILABEL* pSaveFileLblActiveCurve = NULL;							//文件名
static GUICHAR* pSaveFileStrActiveCurve = NULL;
static GUIPICTURE* pSaveFileBgActiveCurve = NULL;

char* pStrSaveFileButton_unpress[FILE_OPTION_NUM] =
{
	BmpFileDirectory"btn_saveFile_unpress.bmp",
	BmpFileDirectory"btn_openFile_unpress.bmp"
};

char* pStrSaveFileButton_press[FILE_OPTION_NUM] =
{
	BmpFileDirectory"btn_saveFile_press.bmp",
	BmpFileDirectory"btn_openFile_press.bmp"
};

static CALLLBACKWINDOW pCallFunc = NULL;		//回调函数
static OTDR_SAVE_CURVE stSaveCurve;				//保存曲线结构体声明
extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//顶层结构
static PDISPLAY_INFO pDisplay = NULL;			//显示信息
static int iSelectedOption = 0;					//当前选中的项
char* pOtdrFileCurrentPath = NULL;

/*******************************************************************************
*                   窗体FrmSaveFile内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int SaveFileTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int SaveFileTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//功能栏按下响应处理函数
static int SaveFileBtnFunctionTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//功能栏弹起响应处理函数
static int SaveFileBtnFunctionTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//返回按钮响应处理函数
static int SaveFileBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SaveFileBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//曲线按下响应处理函数
static int SaveFilePicActItem_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SaveFilePicActItem_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//初始化参数
static void SaveFileInitPara(void);
//菜单栏回调函数
static void SaveFileMenuCall(int option);
//设置活动曲线名
static int SetActiveCurveFileName(void);
//保存曲线
static void SaveCurveFile(void);
//设置保存曲线的结构
static void SetSaveCurveStructure(void);
//重绘窗体
static void RecreateSaveFileWindow(GUIWINDOW** pWnd);
//退出当前窗体
static void BackCurrentWindow(void);
/*******************************************************************************
**							窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
		窗体frmsavefile的初始化函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		建立窗体控件、注册消息处理
***/
static int FrmSaveFileInit(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	pFrmSaveFile = (GUIWINDOW*)pWndObj;

	pSaveFileBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
								BmpFileDirectory "bg_saveFile.bmp");
	pSaveFileBgLeft = CreatePicture(8, 48, 519, 396,
									BmpFileDirectory "bg_saveFile_left_unpress.bmp");
	pSaveFileBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_saveFile_icon.bmp");//待定
	//初始化参数
	SaveFileInitPara();
	//初始化文本
	SaveFileTextRes_Init(NULL, 0, NULL, 0);
	//标题栏
	pSaveFileLblTitle = CreateLabel(36, 12, 100, 16, pSaveFileStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSaveFileLblTitle);
	//返回按钮
	pSaveFileBtnBack = CreatePicture(0, 20, 35, 21, BmpFileDirectory"btn_file_back.bmp");
	//功能区
	int i;
	for (i = 0; i < FILE_OPTION_NUM; ++i)
	{
		pSaveFileBtnFunctionTitle[i] = CreatePicture(3 + 259 * i, 47, 255, 35, BmpFileDirectory"btn_file_unpress.bmp");
		pSaveFileLblFunctionTitle[i] = CreateLabel(104 + 259 * i, 56, 100, 24, pSaveFileStrFunctionTitle[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSaveFileLblFunctionTitle[i]);
		// SetLabelAlign(GUILABEL_ALIGN_CENTER, pSaveFileLblFunctionTitle[i]);
	}

	//中间区域
	int iCurveNum = stSaveCurve.iSaveCurveNum;

	for (i = 0; i < iCurveNum; i++)
	{
		if (i == stSaveCurve.iActiveCurve)
		{
			pSaveFilePicActItem[i] = CreatePicture(0, 83 + i * 40, 519, 38,
												   BmpFileDirectory "btn_filebrowse1_file_select1.bmp");
		}
		else
		{
			pSaveFilePicActItem[i] = CreatePicture(0, 83 + i * 40, 519, 38,
												   BmpFileDirectory "btn_filebrowse1_file_unselect.bmp");
		}

		pSaveFilePicSaveItem[i] = CreatePicture(36, 93 + i * 40, 20, 20,
												BmpFileDirectory "ico_filebrowse_sor.bmp");
		pSaveFilePicSaveItem[i + iCurveNum] = CreatePicture(36, 93 + (i + iCurveNum) * 40, 20, 20,
															BmpFileDirectory "ico_filebrowse_pdf.bmp");
		pSaveFilePicPdfItem[i] = CreatePicture(0, 83 + (i + iCurveNum) * 40, 519, 38,
											   BmpFileDirectory "btn_filebrowse1_file_unselect.bmp");
		pSaveFileLblActItem[i] = CreateLabel(67, 93 + i * 40, 180, 16,
											 pSaveFileStrActItem[i]);
		pSaveFileLblPdfItem[i] = CreateLabel(67, 93 + (i + iCurveNum) * 40, 180, 16,
											 pSaveFileStrPdfItem[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSaveFileLblActItem[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSaveFileLblPdfItem[i]);
	}

	//活动曲线
	pSaveFileLblActiveCurveTitle = CreateLabel(15, 200 + 8, 100, 24, pSaveFileStrActiveCurveTitle);
	pSaveFileBgActiveCurve = CreatePicture(105, 200, 150, 40, BmpFileDirectory"bg_active_curve.bmp");
	pSaveFileLblActiveCurve = CreateLabel(105+5, 200+8, 150, 24, pSaveFileStrActiveCurve);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSaveFileLblActiveCurveTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSaveFileLblActiveCurve);
	/***************************************************************************
	*                       创建右侧的菜单栏控件							   *
	***************************************************************************/
	//默认光标停留在菜单栏
	isInSmallFileBrowse = 0;
	unsigned int strFileMenu[FILE_OPTION_NUM] = { FIP_LBL_SAVE, OTDR_LBL_EXIT };
	pFrmSaveFileMenu = CreateWndMenu1(FILE_OPTION_NUM, sizeof(strFileMenu), strFileMenu, 0xffff,
										0, 1, 41, SaveFileMenuCall);
	//设置菜单栏背景
	for (i = 0; i < 2; i++)
	{
		SetWndMenuItemBg(i, pSaveFileUnpressBmp[i], pFrmSaveFileMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pSaveFileSelectBmp[i], pFrmSaveFileMenu, MENU_SELECT);
	}

	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSaveFile, pFrmSaveFile);
	for (i = 0; i < FILE_OPTION_NUM; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSaveFileBtnFunctionTitle[i], pFrmSaveFile);
	}

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSaveFileBtnBack, pFrmSaveFile);

	for (i = 0; i < iCurveNum; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSaveFilePicActItem[i], pFrmSaveFile);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSaveFileLblActItem[i], pFrmSaveFile);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSaveFilePicPdfItem[i], pFrmSaveFile);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSaveFileLblPdfItem[i], pFrmSaveFile);
	}
	/***************************************************************************
	*                       注册右侧菜单栏各个菜单控件
	***************************************************************************/

	AddWndMenuToComp1(pFrmSaveFileMenu, pFrmSaveFile);

	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	//功能区
	for (i = 0; i < FILE_OPTION_NUM; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSaveFileBtnFunctionTitle[i], SaveFileBtnFunctionTitle_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSaveFileBtnFunctionTitle[i], SaveFileBtnFunctionTitle_Up, NULL, i, pMsg);
	}

	//返回按钮
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSaveFileBtnBack, SaveFileBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSaveFileBtnBack, SaveFileBtnBack_Up, NULL, 0, pMsg);

	//菜单栏
	LoginWndMenuToMsg1(pFrmSaveFileMenu, pFrmSaveFile);
	//中间区域控件
	for (i = 0; i < iCurveNum; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSaveFilePicActItem[i], SaveFilePicActItem_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSaveFilePicActItem[i], SaveFilePicActItem_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSaveFileLblActItem[i], SaveFilePicActItem_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSaveFileLblActItem[i], SaveFilePicActItem_Up, NULL, i, pMsg);

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSaveFilePicPdfItem[i], SaveFilePicActItem_Down, NULL, i + iCurveNum, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSaveFilePicPdfItem[i], SaveFilePicActItem_Up, NULL, i + iCurveNum, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSaveFileLblPdfItem[i], SaveFilePicActItem_Down, NULL, i+iCurveNum, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSaveFileLblPdfItem[i], SaveFilePicActItem_Up, NULL, i + iCurveNum, pMsg);
	}

	return iRet;
}

/***
  * 功能：
		窗体frmsavefile的退出函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		释放所有资源
***/
static int FrmSaveFileExit(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	//得到当前窗体对象
	pFrmSaveFile = (GUIWINDOW*)pWndObj;
	//清空消息队列中的消息注册项
	GUIMESSAGE* pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	//从当前窗体中注销窗体控件
	ClearWindowComp(pFrmSaveFile);

	DestroyPicture(&pSaveFileBg);
	DestroyPicture(&pSaveFileBgLeft);
	DestroyPicture(&pSaveFileBgIcon);
	DestroyLabel(&pSaveFileLblTitle);
	//返回按钮
	DestroyPicture(&pSaveFileBtnBack);

	//活动曲线
	DestroyPicture(&pSaveFileBgActiveCurve);
	DestroyLabel(&pSaveFileLblActiveCurveTitle);
	DestroyLabel(&pSaveFileLblActiveCurve);

	int i;
	int iCurveNum = stSaveCurve.iSaveCurveNum;
	//中间区域
	for (i = 0; i < iCurveNum; i++)
	{
		DestroyPicture(&pSaveFilePicSaveItem[i]);
		DestroyPicture(&pSaveFilePicActItem[i]);
		DestroyLabel(&pSaveFileLblActItem[i]);

		DestroyPicture(&pSaveFilePicSaveItem[i+iCurveNum]);
		DestroyPicture(&pSaveFilePicPdfItem[i]);
		DestroyLabel(&pSaveFileLblPdfItem[i]);
	}

	for (i = 0; i < FILE_OPTION_NUM; ++i)
	{
		DestroyPicture(&pSaveFileBtnFunctionTitle[i]);
		DestroyLabel(&pSaveFileLblFunctionTitle[i]);
	}

	DestroyWndMenu1(&pFrmSaveFileMenu);

	//释放文本
	SaveFileTextRes_Exit(NULL, 0, NULL, 0);

	//记录当前的窗口
	enCurWindow = ENUM_OTHER_WIN;

	return iRet;
}

/***
  * 功能：
		窗体frmsavefile的绘制函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveFilePaint(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	DisplayPicture(pSaveFileBg);
	// DisplayPicture(pSaveFileBgLeft);
	// DisplayPicture(pSaveFileBgIcon);
	DisplayLabel(pSaveFileLblTitle);
	//刷新菜单栏
	DisplayWndMenu1(pFrmSaveFileMenu);
	//刷新菜单栏
	int i;
	for (i = 0; i < 2; ++i)
	{
		SetWndMenuItemEnble(i, 1, pFrmSaveFileMenu);
	}
	//返回按钮
	// DisplayPicture(pSaveFileBtnBack);

	//活动曲线
// 	DisplayLabel(pSaveFileLblActiveCurveTitle);
// 	DisplayPicture(pSaveFileBgActiveCurve);
// 	DisplayLabel(pSaveFileLblActiveCurve);

	int iCurveNum = stSaveCurve.iSaveCurveNum;
	//中间区域
	for (i = 0; i < iCurveNum; i++)
	{
		DisplayPicture(pSaveFilePicActItem[i]);
		DisplayPicture(pSaveFilePicSaveItem[i]);	
		DisplayLabel(pSaveFileLblActItem[i]);

		DisplayPicture(pSaveFilePicPdfItem[i]);
		DisplayPicture(pSaveFilePicSaveItem[i+iCurveNum]);
		DisplayLabel(pSaveFileLblPdfItem[i]);
	}

	for (i = 0; i < FILE_OPTION_NUM; ++i)
	{
		if (i == SAVE_FILE)
		{
			SetPictureBitmap(BmpFileDirectory"btn_file_press.bmp", pSaveFileBtnFunctionTitle[i]);
		}
		DisplayPicture(pSaveFileBtnFunctionTitle[i]);
		DisplayLabel(pSaveFileLblFunctionTitle[i]);
	}

	//记录当前的窗口
	enCurWindow = ENUM_SAVE_OTDR_WIN;

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

/***
  * 功能：
		窗体frmsavefile的循环函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveFileLoop(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	SendWndMsg_LoopDisable(pWndObj);

	return iRet;
}

/***
  * 功能：
		窗体frmsavefile的挂起函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveFilePause(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

/***
  * 功能：
		窗体frmsavefile的恢复函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int FrmSaveFileResume(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

/*******************************************************************************
*                   窗体FrmSaveFile内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int SaveFileTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	pSaveFileStrTitle = GetCurrLanguageText(OTDR_LBL_SAVE_FILE);
	pSaveFileStrFunctionTitle[SAVE_FILE] = GetCurrLanguageText(OTDR_LBL_SAVE_FILE);
	pSaveFileStrFunctionTitle[OPEN_FILE] = GetCurrLanguageText(OTDR_LBL_OPEN_FILE);

	int i;
	char buff[F_NAME_MAX] = { 0 };
	char strFile[F_NAME_MAX] = { 0 };

	for (i = 0; i < stSaveCurve.iSaveCurveNum; ++i)
	{
		GetCurveFile(i, pDisplay, strFile);
		pSaveFileStrActItem[i] = TransString(strFile);
		//设置pdf文件名
		char *position = strrchr(strFile, '.');
		strcpy(position, ".PDF");
		pSaveFileStrPdfItem[i] = TransString(strFile);
	}
	
	pSaveFileStrActiveCurveTitle = TransString("OTDR_SAVE_LBL_ACTIVECURVE");

	GetCurveFile(stSaveCurve.iActiveCurve, pDisplay, strFile);
	snprintf(buff, sizeof(buff), "  %s", strFile);
	pSaveFileStrActiveCurve = TransString(buff);

	return 0;
}

//释放文本资源
static int SaveFileTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GuiMemFree(pSaveFileStrTitle);
	GuiMemFree(pSaveFileStrFunctionTitle[SAVE_FILE]);
	GuiMemFree(pSaveFileStrFunctionTitle[OPEN_FILE]);

	int i;
	for (i = 0; i < stSaveCurve.iSaveCurveNum; ++i)
	{
		GuiMemFree(pSaveFileStrActItem[i]);
		GuiMemFree(pSaveFileStrPdfItem[i]);
	}

	GuiMemFree(pSaveFileStrActiveCurveTitle);
	GuiMemFree(pSaveFileStrActiveCurve);

	return 0;
}

//功能栏按下响应处理函数
static int SaveFileBtnFunctionTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

//功能栏弹起响应处理函数
static int SaveFileBtnFunctionTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	if (iOutLen)
	{
		//临时变量定义
		if (access("/mnt/dataDisk", F_OK) && access("/tmp/usb", F_OK))
		{
			MsgNoSdcard(pFrmSaveFile);
		}
		else
		{
			if (pOtdrFileCurrentPath == NULL)
			{
				pOtdrFileCurrentPath = GuiMemAlloc(512);
			}

			strcpy(pOtdrFileCurrentPath, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);//每次打开都是OTDR设置的路径
			OtdrFileDialog(FILE_OPEN_MODE, MntUpDirectory, pOtdrFileCurrentPath, RecreateSaveFileWindow, SOR);
		}
	}

	return iReturn;
}

//返回按钮响应处理函数
static int SaveFileBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;

}

static int SaveFileBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	BackCurrentWindow();

	return 0;
}


//功能栏按下响应处理函数
static int SaveFilePicActItem_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	int i = 0;
	int iCurveNum = stSaveCurve.iSaveCurveNum;
	iSelectedOption = iOutLen;

	if (iOutLen < iCurveNum)//设置sor
	{
		stSaveCurve.iActCurItemSelect = iOutLen;
	}
	else//设置pdf
	{
		stSaveCurve.iActCurItemSelect = iOutLen-iCurveNum;
	}

	for (i = 0; i < iCurveNum*2; i++)
	{
		if (iOutLen != i)
		{
			if (i < iCurveNum)
			{
				SetPictureBitmap(BmpFileDirectory"btn_filebrowse1_file_unselect.bmp", pSaveFilePicActItem[i]);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"btn_filebrowse1_file_unselect.bmp", pSaveFilePicPdfItem[i-iCurveNum]);
			}
		}
		else
		{
			if (!getSelectedMenu())//光标停留在左侧文件列表
			{
				if (i < iCurveNum)
				{
					SetPictureBitmap(BmpFileDirectory "btn_filebrowse1_file_select.bmp", pSaveFilePicActItem[i]);
				}
				else
				{
					SetPictureBitmap(BmpFileDirectory "btn_filebrowse1_file_select.bmp", pSaveFilePicPdfItem[i - iCurveNum]);
				}
			}
			else//光标停留在右侧菜单栏,左侧选中条灰色选项
			{
				if (i < iCurveNum)
				{
					SetPictureBitmap(BmpFileDirectory "btn_filebrowse1_file_select1.bmp", pSaveFilePicActItem[i]);
				}
				else
				{
					SetPictureBitmap(BmpFileDirectory "btn_filebrowse1_file_select1.bmp", pSaveFilePicPdfItem[i - iCurveNum]);
				}
			}
			

			SetActiveCurveFileName();
		}

		if (i < iCurveNum)
		{
			DisplayPicture(pSaveFilePicActItem[i]);
			DisplayLabel(pSaveFileLblActItem[i]);
		}
		else
		{
			DisplayPicture(pSaveFilePicPdfItem[i - iCurveNum]);
			DisplayLabel(pSaveFileLblPdfItem[i - iCurveNum]);
		}

		DisplayPicture(pSaveFilePicSaveItem[i]);
	}

	// RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

static int SaveFilePicActItem_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

//初始化参数
static void SaveFileInitPara(void)
{
	pDisplay = pOtdrTopSettings->pDisplayInfo;
	SetSaveCurveStructure();
}

static void SavePdfReport(char *absolutePath, Operate operateFunc)
{
	if (!absolutePath)
	{
		return;
	}
	if (CanWriteFile(absolutePath))
	{
		MsecSleep(500);
		//创建报告
		GuiOperateSave(pFrameBufferBak, pFrmSaveFile, operateFunc, absolutePath);
	}
}

//菜单栏回调函数
static void SaveFileMenuCall(int option)
{
	switch (option)
	{
	case 0://OK
		if (iSelectedOption < stSaveCurve.iSaveCurveNum)
		{
			SaveCurveFile();
		}
		else
		{
			char *reportPath = otdrPdfReportGetPathOrName(0);
			if (reportPath)
			{
				SavePdfReport(reportPath, CreateOtdrReport);
				GuiMemFree(reportPath);
			}
		}
		break;
	case 1://Cancel
		BackCurrentWindow();
		break;
	case HOME_DOWN:
		BackCurrentWindow();
		break;
	default:
		break;
	}
}

//设置保存曲线的结构
static void SetSaveCurveStructure(void)
{
	int i;
	stSaveCurve.iSaveCurveNum = GetCurveNum(pDisplay);

	if (0 >= stSaveCurve.iSaveCurveNum)
	{
		stSaveCurve.iActiveCurve = 0;
		stSaveCurve.iActCurItemSelect = 0;

		for (i = 0; i < CURVE_MAX; i++)
		{
			stSaveCurve.iSaveCurveFlag[i] = 0;
		}

		stSaveCurve.iSaveCurveFlag[stSaveCurve.iActiveCurve] = 0;
	}
	else
	{
		stSaveCurve.iActiveCurve = GetCurrCurvePos(pOtdrTopSettings->pDisplayInfo);
		stSaveCurve.iActCurItemSelect = stSaveCurve.iActiveCurve;

		for (i = 0; i < CURVE_MAX; i++)
		{
			stSaveCurve.iSaveCurveFlag[i] = 0;
		}

		stSaveCurve.iSaveCurveFlag[stSaveCurve.iActiveCurve] = 1;
	}
}

//设置活动曲线名
static int SetActiveCurveFileName(void)
{
	int iRet = 0;
	char buff[F_NAME_MAX];
	char strFile[F_NAME_MAX];

	if (0 >= stSaveCurve.iSaveCurveNum)
		return 0;

	stSaveCurve.iActiveCurve = stSaveCurve.iActCurItemSelect;

	GetCurveFile(stSaveCurve.iActiveCurve, pDisplay, strFile);
	snprintf(buff, sizeof(buff), "  %s", strFile);
	pSaveFileStrActiveCurve = TransString(buff);
// 	UpdateLabelRes(pSaveFileStrActiveCurve, pSaveFileLblActiveCurve, pSaveFileBgActiveCurve);
	SetCurrCurvePos(stSaveCurve.iActiveCurve, pDisplay);
	pOtdrTopSettings->pUser_Setting->enWaveCurPos = stSaveCurve.iActiveCurve;

	return iRet;
}

//保存曲线文件
static void SaveCurveFile(void)
{
	if (0 >= stSaveCurve.iSaveCurveNum)
		return;
	int iReturn = SaveSorFile(stSaveCurve.iActiveCurve, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);

	if (iReturn == 0)
	{
		DialogInit(120, 90, GetCurrLanguageText(OTDR_LBL_SAVE_FILE),
				   GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
				   0, 0, RecreateSaveFileWindow, NULL, NULL);
	}
	else if (iReturn == -2)
	{
		DialogInit(120, 90, GetCurrLanguageText(OTDR_LBL_SAVE_FILE),
				   GetCurrLanguageText(DIALOG_LBL_FILEOPEN_ERR),
				   0, 0, RecreateSaveFileWindow, NULL, NULL);
	}
	else
	{
		if (iReturn != -1)
		{
			DialogInit(120, 90, GetCurrLanguageText(OTDR_LBL_SAVE_FILE),
					   GetCurrLanguageText(DIALOG_SOR_SYSTEM_ERR),
					   0, 0, RecreateSaveFileWindow, NULL, NULL);
		}
	}
}

//重绘窗体
static void RecreateSaveFileWindow(GUIWINDOW** pWnd)
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmSaveFileInit, FrmSaveFileExit,
		FrmSaveFilePaint, FrmSaveFileLoop,
		FrmSaveFilePause, FrmSaveFileResume,
		NULL);
}

//退出当前窗体
static void BackCurrentWindow(void)
{
	if (pCallFunc)
	{
		GUIWINDOW *pWnd = NULL;
		(*pCallFunc)(&pWnd);
		SendWndMsg_WindowExit(pFrmSaveFile);      //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
	}
	else
	{
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmOtdrInit, FrmOtdrExit,
			FrmOtdrPaint, FrmOtdrLoop,
			FrmOtdrPause, FrmOtdrResume,
			NULL);         //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmSaveFile);	//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
	}
}

/***
  * 功能：
		创建外部调用保存文件的窗体
  * 参数：
		CALLLBACKWINDOW func:回调函数
  * 返回：
		无
  * 备注：
***/
void CreateSaveFileDialog(CALLLBACKWINDOW func)
{
	pCallFunc = func;

	GUIWINDOW* pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmSaveFileInit, FrmSaveFileExit,
		FrmSaveFilePaint, FrmSaveFileLoop,
		FrmSaveFilePause, FrmSaveFileResume,
		NULL);
	SendWndMsg_WindowExit(GetCurrWindow());
	SendSysMsg_ThreadCreate(pWnd);
}

//创建打开文件窗体（应用于菜单栏调用）
void CreateOpenFileDialog(void)
{
	//临时变量定义
	if (access("/mnt/dataDisk", F_OK) && access("/tmp/usb", F_OK))
	{
		MsgNoSdcard(pFrmSaveFile);
	}
	else
	{
		if (pOtdrFileCurrentPath == NULL)
		{
			pOtdrFileCurrentPath = GuiMemAlloc(512);
		}

		strcpy(pOtdrFileCurrentPath, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);//每次打开都是OTDR设置的路径
		OtdrFileDialog(FILE_OPEN_MODE, MntUpDirectory, pOtdrFileCurrentPath, RecreateSaveFileWindow, SOR);
	}
}

//设置按键焦点位置
void SaveFileSetKeyFocusPos(int option)
{
	int iCurveNum = stSaveCurve.iSaveCurveNum;
	//当无曲线时，上下键无可选内容
	if (iCurveNum == 0)
		return;

	switch (option)
	{
	case 0://up
		if (iSelectedOption)
		{
			iSelectedOption--;
		}
		break;
	case 1:
		if (iSelectedOption != 2*iCurveNum-1)
		{
			iSelectedOption++;
		}
		break;
	default:
		break;
	}

	SaveFilePicActItem_Down(NULL, 0, NULL, iSelectedOption);
	SaveFilePicActItem_Up(NULL, 0, NULL, iSelectedOption);
    RefreshScreen(__FILE__, __func__, __LINE__);
}

//设置左侧文件列表背景选中框(iEnable:0,未选中，1，选中)
void SaveFileSetLeftBackground(int iEnable)
{
	iEnable ? SetPictureBitmap(BmpFileDirectory "bg_saveFile_left_select.bmp", pSaveFileBgLeft)
			: SetPictureBitmap(BmpFileDirectory "bg_saveFile_left_unpress.bmp", pSaveFileBgLeft);
	//重新刷新界面
	GetCurrWindow()->fnWndPaint(GetCurrWindow());
}

//刷新左侧列表选中项
void RefreshSelectedFile(void)
{
	SaveFilePicActItem_Down(NULL, 0, NULL, iSelectedOption);
	SaveFilePicActItem_Up(NULL, 0, NULL, iSelectedOption);
}