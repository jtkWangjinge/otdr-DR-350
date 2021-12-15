/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolafilenamed.c
* 摘    要：  声明主窗体frmsolafilenamed 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v0.0.1
* 作    者：  	 wjg
* 完成日期：  2018-3-30
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/

#include "wnd_frmsolafilenamed.h"

#include "wnd_frmmenubak.h"
#include "wnd_frmime.h"
#include "wnd_frmsola.h"
#include "wnd_frmsolaidentify.h"
#include "wnd_frmmain.h"
#include "wnd_frmsolaident.h"

#include "app_frmsolamessager.h"
#include "app_systemsettings.h"
#include "wnd_frmsolasave.h"
/***************************************************************
* 			窗体frmfilenamed中的窗体控件
****************************************************************/
static int iFiberNameSetupIndex;
static SOLA_FILE_NAME_SETTING* pSolaFilenameSetting = NULL;
extern SOLA_MESSAGER*       pSolaMessager;
/***************************************************************
* 			窗体frmfilenamed中的窗体控件
****************************************************************/
static GUIWINDOW *pFrmSolaFileNamed = NULL;
static WNDMENU1 *pFrmSolaFileNamedMenu = NULL;
/*********************桌面背景定义************************/
static GUIPICTURE *pFrmSolaFileNamedBG = NULL;
//模块标题
static GUILABEL *pSolaFileNamedLblTitle = NULL;
static GUICHAR *pSolaFileNamedStrTitle = NULL;
//光纤文件名设置模块
static GUILABEL *pSolaFileNameLblFiberNameSetup = NULL;
static GUILABEL *pSolaFileNameLblPrefix = NULL;
static GUILABEL *pSolaFileNameLblSuffix = NULL;
static GUILABEL *pSolaFileNameLblSuffixWidth = NULL;

static GUICHAR *pSolaFileNamedStrFiberNameSetup = NULL;
static GUICHAR *pSolaFileNamedStrPrefix = NULL;
static GUICHAR *pSolaFileNamedStrSuffix = NULL;
static GUICHAR *pSolaFileNamedStrSuffixWidth = NULL;

static GUIPICTURE *pSolaFileNameInputBtn[3] = {NULL};
static GUILABEL *pSolaFileNameInputLblText[3] = {NULL};
static GUICHAR *pSolaFileNameInputStrText[3] = {NULL};

//文件格式模块
static GUILABEL *pSolaFileNameLblFileFormat = NULL;
static GUICHAR *pSolaFileNamedStrFileFormat = NULL;

GUIPICTURE *pSolaFileNameFileFormatRadioBtn[2] = {NULL};
static GUILABEL *pSolaFileNameLblFormat[2] = {NULL};
static GUICHAR *pSolaFileNamedStrFormat[2] = {NULL};

//升序或降序
static GUILABEL *pSolaFileNameLblSort = NULL;
static GUICHAR *pSolaFileNamedStrSort = NULL;

GUIPICTURE *pSolaFileNameSortRadioBtn[2] = {NULL};
static GUILABEL *pSolaFileNameLblSortText[2] = {NULL};
static GUICHAR *pSolaFileNamedStrSortText[2] = {NULL};

//预览
static GUILABEL *pSolaFileNameLblPreview = NULL;
static GUICHAR *pSolaFileNamedStrPreview = NULL;

static GUILABEL *pSolaFileNameLblPreviewText = NULL;
static GUICHAR *pSolaFileNamedStrPreviewText = NULL;

GUIPICTURE *pSolaFileNameBgPreview = NULL;
/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int SolaFileNamedTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int SolaFileNamedTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/*************************************************************
* 		                功能函数声明
**************************************************************/
static void InitSolaFileNamedPara(void);
static void SolaFilenamedMenuCallBack(int iOpt);
static int  SolaFileNamedBtnQuit_Fun(void);
static void ReCreateSolaFileNamedWindow(GUIWINDOW **pWnd);
static void RefreshFileFormat(void);
static void RefreshIncrementOrDecrement(void);
static void RefreshPreviewName();
/*************************************************************
* 		                响应函数声明
**************************************************************/
//文件名输入框响应函数
static int  SolaFileNamedInputBtn_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  SolaFileNamedInputBtn_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
//文件格式按钮响应函数
static int  SolaFileNamedFileFormatRadioBtn_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  SolaFileNamedFileFormatRadioBtn_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
//升序或降序按钮响应函数
static int  SolaFileNamedSortRadioBtn_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int  SolaFileNamedSortRadioBtn_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
//窗体frmfilenamed的初始化函数，申请资源
int FrmSolaFileNamedInit(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;

    //临时变量定义
	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
	pFrmSolaFileNamed = (GUIWINDOW *) pWndObj;
    InitSolaFileNamedPara();
	SolaFileNamedTextRes_Init(NULL, 0, NULL, 0);
	/* 窗体背景图片 */
	pFrmSolaFileNamedBG = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_filenamed1.bmp");
    //标题
	pSolaFileNamedLblTitle = CreateLabel(0, 22, 100, 24, pSolaFileNamedStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaFileNamedLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSolaFileNamedLblTitle);
    //光纤文件名设置
    pSolaFileNameLblFiberNameSetup = CreateLabel(50, 120-55, 200, 24, pSolaFileNamedStrFiberNameSetup);
    pSolaFileNameLblPrefix = CreateLabel(70, 100, 130, 24, pSolaFileNamedStrPrefix);
    pSolaFileNameLblSuffix = CreateLabel(70, 156, 130, 24, pSolaFileNamedStrSuffix);
    pSolaFileNameLblSuffixWidth = CreateLabel(70, 156+36, 130, 24, pSolaFileNamedStrSuffixWidth);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblFiberNameSetup);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblPrefix);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblSuffix);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblSuffixWidth);
    
    int i;
    pSolaFileNameInputBtn[0] = CreatePicture(190, 95, 200, 50,
                        BmpFileDirectory"autoNameInputBG1.bmp");
    pSolaFileNameInputLblText[0] = CreateLabel(210, 102, 150, 50,
                        pSolaFileNameInputStrText[0]);
    for (i = 1; i < 3; ++i)
    {
        pSolaFileNameInputBtn[i] = CreatePicture(190, 120+36*i, 200, 25,
                        BmpFileDirectory"autoNameInputBG.bmp");
        pSolaFileNameInputLblText[i] = CreateLabel(210, 125+36*i, 200, 24,
                        pSolaFileNameInputStrText[i]);
    }
    
    //文件格式
    pSolaFileNameLblFileFormat = CreateLabel(431, 120-55, 200, 24, pSolaFileNamedStrFileFormat);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblFileFormat);

    for (i = 0; i < 2; ++i)
    {
        pSolaFileNameFileFormatRadioBtn[i] = CreatePicture(469, 163+36*i-45, 25, 25,
                        BmpFileDirectory"autoNameRadio_unselect.bmp");
        pSolaFileNameLblFormat[i] = CreateLabel(510, 163+36*i+4-45, 130, 24, 
                        pSolaFileNamedStrFormat[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblFormat[i]);
    }
    
    //升序或降序
    pSolaFileNameLblSort = CreateLabel(40, 289-20, 300, 24, pSolaFileNamedStrSort);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblSort);
    
    for (i = 0; i < 2; ++i)
    {
        pSolaFileNameSortRadioBtn[i] = CreatePicture(78+149*i, 333-20, 25, 25,
                        BmpFileDirectory"autoNameRadio_unselect.bmp");
        pSolaFileNameLblSortText[i] = CreateLabel(119+149*i, 333+4-20, 100, 24,
                        pSolaFileNamedStrSortText[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblSortText[i]);
    }
    
    //预览
    pSolaFileNameLblPreview = CreateLabel(32, 419+4-10, 70, 24, pSolaFileNamedStrPreview);
    pSolaFileNameLblPreviewText = CreateLabel(110, 419+4-6, 400, 24, pSolaFileNamedStrPreviewText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFileNameLblPreview);

    pSolaFileNameBgPreview = CreatePicture(100, 419-10, 540, 30,
                        BmpFileDirectory"autoNamePreviewBG.bmp");


    pFrmSolaFileNamedMenu = CreateWndMenu1(0, 0, NULL,
									MENU_BACK, -1, 0, 40, SolaFilenamedMenuCallBack);
									
	AddWndMenuToComp1(pFrmSolaFileNamedMenu,pFrmSolaFileNamed);	
	LoginWndMenuToMsg1(pFrmSolaFileNamedMenu, pFrmSolaFileNamed);	

    //注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSolaFileNamed, 
	              pFrmSolaFileNamed);

    //光纤文件名设置
	for(i = 0; i < 3; ++i)
	{
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaFileNameInputBtn[i], pFrmSolaFileNamed);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaFileNameInputLblText[i], pFrmSolaFileNamed);
	}

    for (i = 0; i < 2; ++i)
    {
        //文件格式
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaFileNameFileFormatRadioBtn[i], pFrmSolaFileNamed);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaFileNameLblFormat[i], pFrmSolaFileNamed);
        //升序或降序
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaFileNameSortRadioBtn[i], pFrmSolaFileNamed);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaFileNameLblSortText[i], pFrmSolaFileNamed);
    }

    //注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();

    //光纤文件名设置
	for(i = 0; i < 3; ++i)
	{
    	LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFileNameInputBtn[i], 
    	                SolaFileNamedInputBtn_Up, NULL, i, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaFileNameInputBtn[i], 
    	                SolaFileNamedInputBtn_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFileNameInputLblText[i], 
    	                SolaFileNamedInputBtn_Up, NULL, i, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaFileNameInputLblText[i], 
    	                SolaFileNamedInputBtn_Down, NULL, i, pMsg);
	}
    
    for (i = 0; i < 2; ++i)
    {
        //文件格式
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFileNameFileFormatRadioBtn[i], 
    	                SolaFileNamedFileFormatRadioBtn_Up, NULL, i, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaFileNameFileFormatRadioBtn[i], 
    	                SolaFileNamedFileFormatRadioBtn_Down, NULL, i, pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFileNameLblFormat[i], 
	                    SolaFileNamedFileFormatRadioBtn_Up, NULL, i, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaFileNameLblFormat[i], 
    	                SolaFileNamedFileFormatRadioBtn_Down, NULL, i, pMsg);

        //升序或降序
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFileNameSortRadioBtn[i], 
    	                SolaFileNamedSortRadioBtn_Up, NULL, i, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaFileNameSortRadioBtn[i], 
    	                SolaFileNamedSortRadioBtn_Down, NULL, i, pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFileNameLblSortText[i], 
    	                SolaFileNamedSortRadioBtn_Up, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaFileNameLblSortText[i], 
	                    SolaFileNamedSortRadioBtn_Down, NULL, i, pMsg);
    }

    return iReturn;
}
//窗体frmfilenamed的退出函数，释放所有资源
int FrmSolaFileNamedExit(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;

    //临时变量定义
	GUIMESSAGE *pMsg = NULL; 
	//得到当前窗体对象
	pFrmSolaFileNamed = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmSolaFileNamed);

    //标题栏
    DestroyPicture(&pFrmSolaFileNamedBG);
    DestroyLabel(&pSolaFileNamedLblTitle);

    //光纤文件名设置
    DestroyLabel(&pSolaFileNameLblFiberNameSetup);
    DestroyLabel(&pSolaFileNameLblPrefix);
    DestroyLabel(&pSolaFileNameLblSuffix);
    DestroyLabel(&pSolaFileNameLblSuffixWidth);

    int i;
    for (i = 0; i < 3; ++i)
    {
        DestroyPicture(&pSolaFileNameInputBtn[i]);
        DestroyLabel(&pSolaFileNameInputLblText[i]);
    }
    
    DestroyLabel(&pSolaFileNameLblFileFormat);
    for (i = 0; i < 2; ++i)
    {
        //文件格式
        DestroyPicture(&pSolaFileNameFileFormatRadioBtn[i]);
        DestroyLabel(&pSolaFileNameLblFormat[i]); 
        //升序或降序
        DestroyPicture(&pSolaFileNameSortRadioBtn[i]);
        DestroyLabel(&pSolaFileNameLblSortText[i]);
    }
    
    DestroyLabel(&pSolaFileNameLblSort);
    
    //预览
    DestroyLabel(&pSolaFileNameLblPreview);
    DestroyLabel(&pSolaFileNameLblPreviewText);
    DestroyPicture(&pSolaFileNameBgPreview);

    //右侧菜单栏
    DestroyWndMenu1(&pFrmSolaFileNamedMenu);

    SolaFileNamedTextRes_Exit(NULL, 0, NULL, 0);
    
    return iReturn;
}
//窗体frmfilenamed的绘制函数，绘制整个窗体
int FrmSolaFileNamedPaint(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;

    //标题栏
    DisplayPicture(pFrmSolaFileNamedBG);
    DisplayLabel(pSolaFileNamedLblTitle);

    //光纤文件名设置
    DisplayLabel(pSolaFileNameLblFiberNameSetup);
    DisplayLabel(pSolaFileNameLblPrefix);
    DisplayLabel(pSolaFileNameLblSuffix);
    DisplayLabel(pSolaFileNameLblSuffixWidth);

    int i;
    for (i = 0; i < 3; ++i)
    {
        DisplayPicture(pSolaFileNameInputBtn[i]);
        DisplayLabel(pSolaFileNameInputLblText[i]);
    }
    
    //文件格式
    RefreshFileFormat();
    DisplayLabel(pSolaFileNameLblFileFormat);
    
    //升序或降序    
    DisplayLabel(pSolaFileNameLblSort);
    RefreshIncrementOrDecrement();
    
    //预览
    DisplayLabel(pSolaFileNameLblPreview);
    DisplayPicture(pSolaFileNameBgPreview);
    DisplayLabel(pSolaFileNameLblPreviewText);
    RefreshPreviewName();

    DisplayWndMenu1(pFrmSolaFileNamedMenu);

	SetPowerEnable(1, 1);

    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}
//窗体frmfilenamed的循环函数，进行窗体循环
int FrmSolaFileNamedLoop(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;
    return iReturn;
}
//窗体frmfilenamed的挂起函数，进行窗体挂起前预处理
int FrmSolaFileNamedPause(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;
    return iReturn;
}
//窗体frmfilenamed的恢复函数，进行窗体恢复前预处理
int FrmSolaFileNamedResume(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;
    return iReturn;
}

/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int SolaFileNamedTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    char temp[10] = {0};
    char buff[128] = {0};

    pSolaFileNamedStrTitle = TransString("OTDR_SAVE_LBL_AUTONAME");

    //光纤文件名设置
    pSolaFileNamedStrFiberNameSetup = TransString("OTDR_AUTONAME_NAMESET");
    pSolaFileNamedStrPrefix = TransString("OTDR_AUTONAME_PREFIX");
    pSolaFileNamedStrSuffix = TransString("OTDR_AUTONAME_SUFFIX");
    pSolaFileNamedStrSuffixWidth = TransString("OTDR_AUTONAME_NUMOFDIGITS");

    pSolaFileNameInputStrText[0] = TransString(pSolaFilenameSetting->prefix);
    sprintf(temp, "%%0%dd", pSolaFilenameSetting->suffixWidth);
	sprintf(buff, temp, pSolaFilenameSetting->suffix % (int)pow(10, pSolaFilenameSetting->suffixWidth));
	pSolaFileNameInputStrText[1] = TransString(buff);
    char suffixWidth[10] = {0};
    sprintf(suffixWidth, "%d", pSolaFilenameSetting->suffixWidth);
    pSolaFileNameInputStrText[2] = TransString(suffixWidth);
    //文件格式
    pSolaFileNamedStrFileFormat = TransString("CURVEINFO_LBL_FILE_FORMAT");
    pSolaFileNamedStrFormat[0] = TransString("OTDR_AUTONAME_PDF");
    pSolaFileNamedStrFormat[1] = TransString("SOLA");

    //升序或降序
    pSolaFileNamedStrSort = TransString("OTDR_AUTONAME_INORDEVALUE");
    pSolaFileNamedStrSortText[0] = TransString("OTDR_AUTONAME_INCREMENT");
    pSolaFileNamedStrSortText[1] = TransString("OTDR_AUTONAME_DECREMENT");

    //预览
    pSolaFileNamedStrPreview = TransString("OTDR_AUTONAME_PREVIEW");
    char preview[128] = {0};
    if(pSolaFilenameSetting->fileFormat == 1)
    {
        sprintf(preview, "%s%s%s", pSolaFilenameSetting->prefix, buff, ".PDF");
    }
    else
    {
        sprintf(preview, "%s%s%s", pSolaFilenameSetting->prefix, buff, ".SOLA");
    }
    pSolaFileNamedStrPreviewText = TransString(preview); 
    
    return iReturn;
}
//释放文本资源
static int SolaFileNamedTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    GuiMemFree(pSolaFileNamedStrTitle);

    //光纤文件名设置
    GuiMemFree(pSolaFileNamedStrFiberNameSetup);
    GuiMemFree(pSolaFileNamedStrPrefix);
    GuiMemFree(pSolaFileNamedStrSuffix);
    GuiMemFree(pSolaFileNamedStrSuffixWidth);

    int i;
    for (i = 0; i < 3; ++i)
    {
        GuiMemFree(pSolaFileNameInputStrText[i]);
    }
    
    //文件格式
    GuiMemFree(pSolaFileNamedStrFileFormat);
    GuiMemFree(pSolaFileNamedStrFormat[0]);
    GuiMemFree(pSolaFileNamedStrFormat[1]);

    //升序或降序
    GuiMemFree(pSolaFileNamedStrSort);
    GuiMemFree(pSolaFileNamedStrSortText[0]);
    GuiMemFree(pSolaFileNamedStrSortText[1]);

    //预览
    GuiMemFree(pSolaFileNamedStrPreview);
    GuiMemFree(pSolaFileNamedStrPreviewText);

    return iReturn;
}

/*************************************************************
* 		                功能函数实现
**************************************************************/
//初始化设置参数
static void InitSolaFileNamedPara(void)
{
    pSolaFilenameSetting = &pSolaMessager->autoFilename;
}
//右侧菜单栏回调函数
static void SolaFilenamedMenuCallBack(int iOpt)
{
	switch(iOpt)
	{
	    case 0:
            break;
        case 1:
            break;
		case BACK_DOWN:
			SolaFileNamedBtnQuit_Fun();
			break;
		default:
		    break;
	}
}


//退出当前界面响应函数
static int SolaFileNamedBtnQuit_Fun(void)
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
    SetSettingsData((void*)&pSolaMessager->autoFilename, sizeof(SOLA_FILE_NAME_SETTING), SOLA_FILENAME_SET);
    SaveSettings(SOLA_FILENAME_SET);

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    				FrmSOLASaveInit, FrmSOLASaveExit, 
    				FrmSOLASavePaint, FrmSOLASaveLoop, 
    				FrmSOLASavePause, FrmSOLASaveResume,
    				NULL); 		 
    SendWndMsg_WindowExit(pFrmSolaFileNamed);  
    SendSysMsg_ThreadCreate(pWnd); 

	return iReturn;
}

/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateSolaFileNamedWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmSolaFileNamedInit, FrmSolaFileNamedExit, 
		FrmSolaFileNamedPaint, FrmSolaFileNamedLoop, 
		FrmSolaFileNamedPause, FrmSolaFileNamedResume,
		NULL);
}

//用户自定义输入字符串回调函数
static void UserInputCallFun(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);
	
	switch(iFiberNameSetupIndex)
	{
        case 0:
        if(!isNameStr(cTmpBuff))
        {
            strcpy(pSolaFilenameSetting->prefix, cTmpBuff);
        }
        else
        {
    		CreateIMEDialog(TransString("DIALOG_FILE_INPUT_INCORRECT"));
    		return;
        }
        break;
        case 1:
        {
            pSolaFilenameSetting->suffix = atoi(cTmpBuff);
            if(0 == pSolaFilenameSetting->suffix)
            {
                pSolaFilenameSetting->suffix = 1;
            }
        }
        break;
        case 2:
        {
            int suffixWidth = atoi(cTmpBuff);
            if (suffixWidth >= 3 && suffixWidth <= 5)
                pSolaFilenameSetting->suffixWidth = atoi(cTmpBuff);
            else
                CreateIMEDialog(TransString("OTDR_AUTONAME_DIGIT"));
        } 
        break;
        default:
        break;
	}
}

//刷新文件格式
static void RefreshFileFormat(void)
{
    //文件格式
    if (pSolaFilenameSetting->fileFormat == 1)
    {
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pSolaFileNameFileFormatRadioBtn[0]);
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pSolaFileNameFileFormatRadioBtn[1]);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pSolaFileNameFileFormatRadioBtn[0]);
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pSolaFileNameFileFormatRadioBtn[1]);
    }

    int i;
    for (i = 0; i < 2; ++i)
    {
        DisplayPicture(pSolaFileNameFileFormatRadioBtn[i]);
        DisplayLabel(pSolaFileNameLblFormat[i]);
    }  
    
    RefreshPreviewName();
}

//刷新升序降序
static void RefreshIncrementOrDecrement(void)
{
    if (pSolaFilenameSetting->suffixRule == 1)
    {
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pSolaFileNameSortRadioBtn[0]);
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pSolaFileNameSortRadioBtn[1]);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pSolaFileNameSortRadioBtn[0]);
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pSolaFileNameSortRadioBtn[1]);
    }

    int i;
    for (i = 0; i < 2; ++i)
    {
        DisplayPicture(pSolaFileNameSortRadioBtn[i]);
        DisplayLabel(pSolaFileNameLblSortText[i]);
    }
}

/*************************************************************
* 		                响应函数声明
**************************************************************/
//文件名输入框响应函数
static int  SolaFileNamedInputBtn_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    return iReturn;
}
static int  SolaFileNamedInputBtn_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    char buff[128] = {0};
    char temp[10] = {0};
	iFiberNameSetupIndex = iOutLen;
	switch(iFiberNameSetupIndex)
	{
        case 0:
        IMEInit(pSolaFilenameSetting->prefix, 25, 0, ReCreateSolaFileNamedWindow, UserInputCallFun, NULL);
        break;
        case 1:
        sprintf(temp, "%%0%dd", pSolaFilenameSetting->suffixWidth);
        sprintf(buff, temp, pSolaFilenameSetting->suffix % (int)pow(10, pSolaFilenameSetting->suffixWidth));
        IMEInit(buff, pSolaFilenameSetting->suffixWidth, 1, ReCreateSolaFileNamedWindow, UserInputCallFun, NULL);
        break;
        case 2:
        sprintf(buff, "%d", pSolaFilenameSetting->suffixWidth);
        IMEInit(buff, 10, 1, ReCreateSolaFileNamedWindow, UserInputCallFun, NULL);
        break;
        default:
        break;
	}

    return iReturn;
}

//文件格式按钮响应函数
static int SolaFileNamedFileFormatRadioBtn_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    return iReturn;
}
static int  SolaFileNamedFileFormatRadioBtn_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    pSolaFilenameSetting->fileFormat = iOutLen + 1;
    RefreshFileFormat();
    RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//升序或降序按钮响应函数
static int  SolaFileNamedSortRadioBtn_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    return iReturn;
}

static int  SolaFileNamedSortRadioBtn_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    pSolaFilenameSetting->suffixRule = !iOutLen;;
    RefreshIncrementOrDecrement();
    RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//刷新预览名
static void RefreshPreviewName()
{
    char buff[128] = {0};
    char temp[128];
    
    if(pSolaFileNamedStrPreviewText)
    {
        GuiMemFree(pSolaFileNamedStrPreviewText);
    }
    if (pSolaFilenameSetting->fileFormat == 1)
    {
        sprintf(temp, "%s%%0%dd.PDF", pSolaFilenameSetting->prefix, pSolaFilenameSetting->suffixWidth);
    }
    else
    {
        sprintf(temp, "%s%%0%dd.SOLA", pSolaFilenameSetting->prefix, pSolaFilenameSetting->suffixWidth);
    }
    sprintf(buff, temp, pSolaFilenameSetting->suffix % (int)pow(10, pSolaFilenameSetting->suffixWidth));
    pSolaFileNamedStrPreviewText = TransString(buff);
    SetLabelText(pSolaFileNamedStrPreviewText, pSolaFileNameLblPreviewText);
    DisplayPicture(pSolaFileNameBgPreview);
    DisplayLabel(pSolaFileNameLblPreviewText);
}

