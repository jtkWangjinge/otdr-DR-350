/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmbmpfilenamed.c
* 摘    要：  声明主窗体frmbmpfilenamed 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v0.0.1
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/

#include "wnd_frmbmpfilenamed.h"

/**************************************************************
* 	为实现窗体frmbmpfilenamed而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmotdrsave.h"
#include "app_frminit.h"
#include "wnd_frmime.h"
#include "wnd_frmmenubak.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "app_sola.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmabout.h"
#include "wnd_frmwifi.h"

/**************************************************************
* 		定义frmbmpfilenamed中内部使用的数据结构
**************************************************************/

//自定义的全局变量
extern SOLA_FILE_NAME_SETTING *pBmpFileNamedSetting;

static int bmpFilenamedSetupIndex;
/***************************************************************
* 			窗体frmbmpfilenamed中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmBmpFileNamed = NULL;

static WNDMENU1 *pFrmBmpFileNamedMenu = NULL;
/*********************桌面背景定义************************/
static GUIPICTURE *pFrmBmpFileNamedBG = NULL;
static GUIPICTURE *pFrmBmpFileNamedBGTableTitle = NULL;
static GUIPICTURE *pFrmBmpFileNamedBGTableTitleStandbySet = NULL;
//模块标题
static GUILABEL *pBmpFileNamedLblLabel = NULL;
static GUICHAR *pBmpFileNamedStrLabel = NULL;

static GUILABEL *pBmpFileNamedLblStandbySet = NULL;
static GUICHAR *pBmpFileNamedStrStandbySet = NULL;


/**********************桌面控件定义***********************/
//桌面控件
static GUIPICTURE *pBmpFileNamedBtnSelectPoint = NULL;
static GUIPICTURE *pBmpFileNamedBgPreview = NULL;
static GUIPICTURE *pBmpFileNamedBtnUserInput[3];

static GUIPICTURE *pBmpFileNamedBtnIncrement = NULL;
static GUIPICTURE *pBmpFileNamedBtnDecrement = NULL;

/************************桌面标签定义*********************/
//桌面标签

static GUILABEL *pBmpFileNamedLblFiberNamingSetup = NULL ;     //光纤名设置
static GUILABEL *pBmpFileNamedLblPrefixLeft = NULL ;           //前缀左边文本
static GUILABEL *pBmpFileNamedLblPrefixRight = NULL ;          //前缀内容
static GUILABEL *pBmpFileNamedLblSuffixLeft = NULL;            //后缀左边文本  
static GUILABEL *pBmpFileNamedLblSuffixRight = NULL;           //后缀内容
static GUILABEL *pBmpFileNamedLblNumLeft = NULL;               //后缀数量左边文本
static GUILABEL *pBmpFileNamedLblNumRight = NULL;              //后缀数量内容

static GUILABEL *pBmpFileNamedLblFileFormat = NULL ;           //文件格式标题
static GUILABEL *pBmpFileNamedLblFileFormatRadio = NULL ;           //bmp

static GUILABEL *pBmpFileNamedLblInOrDeValue = NULL;           //升序或降序
static GUILABEL *pBmpFileNamedLblIncrement = NULL;             //升序
static GUILABEL *pBmpFileNamedLblDecrement = NULL;             //降序

static GUILABEL *pBmpFileNamedLblPreview = NULL ;              //预览标题
static GUILABEL *pBmpFileNamedLblNamePreview = NULL ;          //预览内容

/************************桌面标签文本*********************/
//桌面标签
static GUICHAR *pBmpFileNamedStrFiberNamingSetup = NULL ;      //光纤名设置
static GUICHAR *pBmpFileNamedStrPrefixLeft = NULL ;            //前缀左边文本
static GUICHAR *pBmpFileNamedStrPrefixRight = NULL ;           //前缀内容
static GUICHAR *pBmpFileNamedStrSuffixLeft = NULL;             //后缀左边文本    
static GUICHAR *pBmpFileNamedStrSuffixRight = NULL;            //后缀内容
static GUICHAR *pBmpFileNamedStrNumLeft = NULL;                //后缀数量左边文本    
static GUICHAR *pBmpFileNamedStrNumRight = NULL;               //后缀数量内容

static GUICHAR *pBmpFileNamedStrFileFormat = NULL;             //文件格式标题
static GUICHAR *pBmpFileNamedStrFileFormatRadio = NULL;        //bmp

static GUICHAR *pBmpFileNamedStrInOrDeValue = NULL;
static GUICHAR *pBmpFileNamedStrIncrement = NULL;
static GUICHAR *pBmpFileNamedStrDecrement = NULL;

static GUICHAR *pBmpFileNamedStrPreview = NULL ;               //预览标题
static GUICHAR *pBmpFileNamedStrNamePreview = NULL ;           //预览内容
/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int BmpFileNamedTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int BmpFileNamedTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

#ifdef F214 
static int BmpFileNamedWndKey_Down(void *pInArg, int iInLen, 
                           		void *pOutArg, int iOutLen);
static int BmpFileNamedWndKey_Up(void *pInArg, int iInLen, 
                         		void *pOutArg, int iOutLen);
#endif

/**************************************************************
* 				控件函数声明
**************************************************************/
//退出按钮
static int BmpFileNamedBtnQuit_Fun();

void BmpFileNameddMenuCallBack(int iOpt);

//刷新预览名
static void RefreshPreviewName();

/**************************************************************
* 				函数定义
**************************************************************/

/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateBmpFileNamedWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmBmpFileNamedInit, FrmBmpFileNamedExit, 
		FrmBmpFileNamedPaint, FrmBmpFileNamedLoop, 
		FrmBmpFileNamedPause, FrmBmpFileNamedResume,
		NULL);
}


//刷新文件名设置部分
static void RefreshFiberNamingSteup(int index)
{
    char buff[128] = {0};
    if(index == 1)  //前缀
    {
        if(pBmpFileNamedStrPrefixRight)
        {
            GuiMemFree(pBmpFileNamedStrPrefixRight);
        }
        pBmpFileNamedStrPrefixRight = TransString(pBmpFileNamedSetting->prefix);
        SetLabelText(pBmpFileNamedStrPrefixRight, pBmpFileNamedLblPrefixRight);
        DisplayPicture(pBmpFileNamedBtnUserInput[0]);
        DisplayLabel(pBmpFileNamedLblPrefixRight);
    }

    if(index == 2)  //后缀
    {
        char format[10];
        if(pBmpFileNamedStrSuffixRight)
        {
            GuiMemFree(pBmpFileNamedStrSuffixRight);
        }
        sprintf(format, "%%0%dd", pBmpFileNamedSetting->suffixWidth);
    	sprintf(buff, format, (int)pBmpFileNamedSetting->suffix % (int)pow(10, pBmpFileNamedSetting->suffixWidth));
        pBmpFileNamedStrSuffixRight = TransString(buff);
        SetLabelText(pBmpFileNamedStrSuffixRight, pBmpFileNamedLblSuffixRight);
        DisplayPicture(pBmpFileNamedBtnUserInput[1]);
        DisplayLabel(pBmpFileNamedLblSuffixRight);
    }
    
    if(index == 3)  //数字位数
    {
        if(pBmpFileNamedStrNumRight)
        {
            GuiMemFree(pBmpFileNamedStrNumRight);
        }
    	sprintf(buff, "%d", pBmpFileNamedSetting->suffixWidth);
        pBmpFileNamedStrNumRight = TransString(buff);
        SetLabelText(pBmpFileNamedStrNumRight, pBmpFileNamedLblNumRight);
        DisplayPicture(pBmpFileNamedBtnUserInput[2]);
        DisplayLabel(pBmpFileNamedLblNumRight);
    }
}

//刷新升降序
static void RefreshInOrDeValue()
{
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pBmpFileNamedBtnIncrement);
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pBmpFileNamedBtnDecrement);

    pBmpFileNamedSetting->suffixRule ? 
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pBmpFileNamedBtnIncrement) :
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pBmpFileNamedBtnDecrement);
    DisplayPicture(pBmpFileNamedBtnIncrement);
    DisplayPicture(pBmpFileNamedBtnDecrement);
}

//刷新预览名
static void RefreshPreviewName()
{
    char buff[128] = {0};
    char temp[128];
    
    if(pBmpFileNamedStrNamePreview)
    {
        GuiMemFree(pBmpFileNamedStrNamePreview);
    }
    sprintf(temp, "%s%%0%dd.bmp", pBmpFileNamedSetting->prefix, pBmpFileNamedSetting->suffixWidth);
    sprintf(buff, temp, pBmpFileNamedSetting->suffix % (int)pow(10, pBmpFileNamedSetting->suffixWidth));
    pBmpFileNamedStrNamePreview = TransString(buff);
    SetLabelText(pBmpFileNamedStrNamePreview, pBmpFileNamedLblNamePreview);
    DisplayPicture(pBmpFileNamedBgPreview);
    DisplayLabel(pBmpFileNamedLblNamePreview);
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
static void UserInputCallFun(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);
	
	switch(bmpFilenamedSetupIndex)
	{
        case 0:
            if(!isNameStr(cTmpBuff))
            {
                strcpy(pBmpFileNamedSetting->prefix, cTmpBuff);
            }
            else
            {
        		CreateIMEDialog(TransString("DIALOG_FILE_INPUT_INCORRECT"));
        		return;
            }
        break;
        case 1:
        {
            pBmpFileNamedSetting->suffix = atoi(cTmpBuff);
            if(0 == pBmpFileNamedSetting->suffix)
            {
                pBmpFileNamedSetting->suffix = 1;
            }
        }
        break;
        case 2:
        {
            int width = atoi(cTmpBuff);
            if(width > 5 || width < 3)
            {
        		CreateIMEDialog(TransString("OTDR_AUTONAME_DIGIT"));
        		return;
            }
            else
            {
                pBmpFileNamedSetting->suffixWidth = width;
            }
        }
        break;
        default:
        break;
	}
}


/***
  * 功能：
        		Quit 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  BmpFileNamedBtnQuit_Fun()
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
    SetSettingsData((void*)pBmpFileNamedSetting, sizeof(SOLA_FILE_NAME_SETTING), BMP_FILENAME_SET);
    SaveSettings(BMP_FILENAME_SET);

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    				FrmMainInit, FrmMainExit, 
    				FrmMainPaint, FrmMainLoop, 
    				FrmMainPause, FrmMainResume,
    				NULL); 		 
    SendWndMsg_WindowExit(pFrmBmpFileNamed);  
    SendSysMsg_ThreadCreate(pWnd); 

	return iReturn;
}

/***
  * 功能：
        		用户输入框 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int BmpFileNamedLblTitleStandbySet_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
    SetSettingsData((void*)pBmpFileNamedSetting, sizeof(SOLA_FILE_NAME_SETTING), BMP_FILENAME_SET);
    SaveSettings(BMP_FILENAME_SET);

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    				FrmStandbysetInit, FrmStandbysetExit, 
    				FrmStandbysetPaint, FrmStandbysetLoop, 
    				FrmStandbysetPause, FrmStandbysetResume,
    				NULL); 		 
    SendWndMsg_WindowExit(pFrmBmpFileNamed);  
    SendSysMsg_ThreadCreate(pWnd); 

	return iReturn;
}


/***
  * 功能：
        		用户输入框 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int BmpFileNamedLblUserInput_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        		UserDef 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  BmpFileNamedLblUserInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	char buff[128] = {0};
	char temp[128];
	bmpFilenamedSetupIndex = iOutLen;
	switch(bmpFilenamedSetupIndex)
	{
        case 0:
        IMEInit(pBmpFileNamedSetting->prefix, 25, 0, ReCreateBmpFileNamedWindow, UserInputCallFun, NULL);
        break;
        case 1:
        sprintf(temp, "%%0%dd", pBmpFileNamedSetting->suffixWidth);
        sprintf(buff, temp, pBmpFileNamedSetting->suffix % (int)pow(10, pBmpFileNamedSetting->suffixWidth));
        IMEInit(buff, pBmpFileNamedSetting->suffixWidth, 1, ReCreateBmpFileNamedWindow, UserInputCallFun, NULL);
        break;
        case 2:
        sprintf(buff, "%d", pBmpFileNamedSetting->suffixWidth);
        IMEInit(buff, 1, 1, ReCreateBmpFileNamedWindow, UserInputCallFun, NULL);
        break;
        default:
        break;
	}
	
    return iReturn;
}


/***
  * 功能：
        升序或降序 按钮按下处理函数
  * 参数：
       	无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int BmpFileNamedLblInOrDeValue_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    pBmpFileNamedSetting->suffixRule = !iOutLen;
    RefreshInOrDeValue();
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}


/***
  * 功能：
        		升序或降序 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  BmpFileNamedLblInOrDeValue_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        		窗体frmbmpFilenamedd 的初始化函数，建立窗体控件、
        		注册消息处理
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmBmpFileNamedInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int loop;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL; 
    #ifndef MINI2
	unsigned int StrStandbyMenu[] = {
		1
	};
	#else
    unsigned int StrStandbyMenu[] = {
	SYSTEMSET_STANDBY,
	SYSTEMSET_TIME,
	SYSTEMSET_LANGUAGE,
	MAIN_LBL_WIFI,
	SYSTEMSET_SYSMAINTAIN,
	SYSTEM_ABOUT,
	};
    #endif
	//得到当前窗体对象
	pFrmBmpFileNamed = (GUIWINDOW *) pWndObj;	
	
	BmpFileNamedTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体背景图片 */
	pFrmBmpFileNamedBG = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_filenamed1.bmp");
    pFrmBmpFileNamedBGTableTitleStandbySet = CreatePhoto("otdr_top1");
    pFrmBmpFileNamedBGTableTitle = CreatePhoto("otdr_top2f");
    //标题
	pBmpFileNamedLblStandbySet = CreateLabel(0, 24, 100, 24, pBmpFileNamedStrStandbySet);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pBmpFileNamedLblStandbySet);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pBmpFileNamedLblStandbySet);
    
	pBmpFileNamedLblLabel = CreateLabel(100, 24, 100, 24, pBmpFileNamedStrLabel);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pBmpFileNamedLblLabel);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pBmpFileNamedLblLabel);
    
    //光纤名设置
    pBmpFileNamedLblFiberNamingSetup = CreateLabel(50, 120-55, 300, 24, pBmpFileNamedStrFiberNamingSetup);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblFiberNamingSetup);
    
    pBmpFileNamedLblPrefixLeft = CreateLabel(70, 100, 130, 24, pBmpFileNamedStrPrefixLeft);
    pBmpFileNamedLblSuffixLeft = CreateLabel(70, 156, 130, 24, pBmpFileNamedStrSuffixLeft);
    pBmpFileNamedLblNumLeft = CreateLabel(70, 156+36, 130, 24, pBmpFileNamedStrNumLeft);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblPrefixLeft);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblSuffixLeft);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblNumLeft);
    
    pBmpFileNamedLblPrefixRight = CreateLabel(210, 102, 150, 50, pBmpFileNamedStrPrefixRight);
    pBmpFileNamedLblSuffixRight = CreateLabel(210, 125+36*1, 200, 24, pBmpFileNamedStrSuffixRight);
    pBmpFileNamedLblNumRight = CreateLabel(210, 125+36*2, 200, 24, pBmpFileNamedStrNumRight);

    pBmpFileNamedBtnUserInput[0] = CreatePicture(190, 95, 200, 50, BmpFileDirectory"autoNameInputBG1.bmp");
    pBmpFileNamedBtnUserInput[1] = CreatePicture(190, 120+36*1, 200, 25, BmpFileDirectory"autoNameInputBG.bmp");
    pBmpFileNamedBtnUserInput[2] = CreatePicture(190, 120+36*2, 200, 25, BmpFileDirectory"autoNameInputBG.bmp");

    //文件格式选择
	pBmpFileNamedLblFileFormat = CreateLabel(431, 120-55, 200, 24, pBmpFileNamedStrFileFormat);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblFileFormat);

    pBmpFileNamedLblFileFormatRadio = CreateLabel(510, 163+4-55, 130, 24, pBmpFileNamedStrFileFormatRadio);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblFileFormatRadio);

    pBmpFileNamedBtnSelectPoint = CreatePicture(469, 163-55, 25, 25, BmpFileDirectory"autoNameRadio_select.bmp");

	//升降序
	pBmpFileNamedLblInOrDeValue = CreateLabel(40, 289-20, 500, 24, pBmpFileNamedStrInOrDeValue);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblInOrDeValue);

    pBmpFileNamedLblIncrement = CreateLabel(119, 333+4-20, 100, 24, pBmpFileNamedStrIncrement);
    pBmpFileNamedLblDecrement = CreateLabel(268, 333+4-20, 100, 24, pBmpFileNamedStrDecrement);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblIncrement);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblDecrement);
	
    pBmpFileNamedBtnIncrement = CreatePicture(78, 333-20, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");
    pBmpFileNamedBtnDecrement = CreatePicture(227, 333-20, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");
    
    //预览
	pBmpFileNamedBgPreview = CreatePicture(100, 419-10, 540, 30, BmpFileDirectory"autoNamePreviewBG.bmp");
	pBmpFileNamedLblPreview = CreateLabel(32, 419+4-10, 70, 24, pBmpFileNamedStrPreview);
	pBmpFileNamedLblNamePreview = CreateLabel(110, 419+4-6, 400, 24, pBmpFileNamedStrNamePreview);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBmpFileNamedLblPreview);
    
    
	/* 建立右侧菜单栏控件 */	
	#ifndef MINI2
	pFrmBmpFileNamedMenu = CreateWndMenu1(5, sizeof(StrStandbyMenu), StrStandbyMenu,
									0xffff, 0, 0, 40, BmpFileNameddMenuCallBack);
    #else
    pFrmBmpFileNamedMenu = CreateWndMenu1(6, sizeof(StrStandbyMenu), StrStandbyMenu,
									0xffff, 0, 0, 40, BmpFileNameddMenuCallBack);
    #endif
									
	AddWndMenuToComp1(pFrmBmpFileNamedMenu,pFrmBmpFileNamed);	
	LoginWndMenuToMsg1(pFrmBmpFileNamedMenu, pFrmBmpFileNamed);	
	
	//注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmBmpFileNamed, 
	              pFrmBmpFileNamed);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmBmpFileNamedBGTableTitleStandbySet, 
	              pFrmBmpFileNamed);

	for(loop=0;loop<3;loop++)
	{
    	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pBmpFileNamedBtnUserInput[loop], 
    	              pFrmBmpFileNamed);
	}

	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pBmpFileNamedLblIncrement, 
	              pFrmBmpFileNamed);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pBmpFileNamedLblDecrement, 
	              pFrmBmpFileNamed);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pBmpFileNamedBtnIncrement, 
	              pFrmBmpFileNamed);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pBmpFileNamedBtnDecrement, 
	              pFrmBmpFileNamed);
	
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmBmpFileNamedBGTableTitleStandbySet, 
	               BmpFileNamedLblTitleStandbySet_Down, NULL, 0, pMsg);

	for(loop=0;loop<3;loop++)
	{
    	LoginMessageReg(GUIMESSAGE_TCH_UP, pBmpFileNamedBtnUserInput[loop], 
    	               BmpFileNamedLblUserInput_Up, NULL, loop, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBmpFileNamedBtnUserInput[loop], 
    	               BmpFileNamedLblUserInput_Down, NULL, loop, pMsg);
	}
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pBmpFileNamedLblIncrement, 
	               BmpFileNamedLblInOrDeValue_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBmpFileNamedLblIncrement, 
	               BmpFileNamedLblInOrDeValue_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pBmpFileNamedBtnIncrement,
                    BmpFileNamedLblInOrDeValue_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBmpFileNamedBtnIncrement,
                    BmpFileNamedLblInOrDeValue_Down, NULL, 0, pMsg);
                    
	LoginMessageReg(GUIMESSAGE_TCH_UP, pBmpFileNamedLblDecrement, 
	               BmpFileNamedLblInOrDeValue_Up, NULL, 1, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBmpFileNamedLblDecrement, 
	               BmpFileNamedLblInOrDeValue_Down, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pBmpFileNamedBtnDecrement,
                    BmpFileNamedLblInOrDeValue_Up, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBmpFileNamedBtnDecrement,
                    BmpFileNamedLblInOrDeValue_Down, NULL, 1, pMsg);
#ifdef F214 
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmBmpFileNamed, 
	               BmpFileNamedWndKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmBmpFileNamed, 
	               BmpFileNamedWndKey_Up, NULL, 0, pMsg);

#endif
	
	return iReturn;
}
/***
  * 功能：
        		窗体frmbmpFilenamedd 的退出函数，释放所有资源
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmBmpFileNamedExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL; 
	int i = 0;
	//得到当前窗体对象
	pFrmBmpFileNamed = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmBmpFileNamed);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pFrmBmpFileNamedBG);
    DestroyPicture(&pFrmBmpFileNamedBGTableTitle);
    DestroyPicture(&pFrmBmpFileNamedBGTableTitleStandbySet);
	DestroyPicture(&pBmpFileNamedBgPreview);
	DestroyPicture(&pBmpFileNamedBtnIncrement);
	DestroyPicture(&pBmpFileNamedBtnDecrement);
	for(i = 0;i<3; i++)
	{
	    DestroyPicture(&pBmpFileNamedBtnUserInput[i]);
	}
	DestroyPicture(&pBmpFileNamedBtnSelectPoint);
	
	DestroyLabel(&pBmpFileNamedLblLabel);
	DestroyLabel(&pBmpFileNamedLblStandbySet);
	

	DestroyLabel(&pBmpFileNamedLblFiberNamingSetup);
	DestroyLabel(&pBmpFileNamedLblPrefixLeft);
	DestroyLabel(&pBmpFileNamedLblPrefixRight);
	DestroyLabel(&pBmpFileNamedLblSuffixLeft);
	DestroyLabel(&pBmpFileNamedLblSuffixRight);
	DestroyLabel(&pBmpFileNamedLblNumLeft);
	DestroyLabel(&pBmpFileNamedLblNumRight);
	
	DestroyLabel(&pBmpFileNamedLblFileFormat);
	DestroyLabel(&pBmpFileNamedLblFileFormatRadio);
	
	DestroyLabel(&pBmpFileNamedLblInOrDeValue);
	DestroyLabel(&pBmpFileNamedLblIncrement);
	DestroyLabel(&pBmpFileNamedLblDecrement);

	DestroyLabel(&pBmpFileNamedLblPreview);
	DestroyLabel(&pBmpFileNamedLblNamePreview);

	DestroyWndMenu1(&pFrmBmpFileNamedMenu);
	
	//文本内容销毁
	BmpFileNamedTextRes_Exit(NULL, 0, NULL, 0);

   return iReturn;
}


/***
  * 功能：
        		窗体frmbmpFilenamedd 的绘制函数，绘制整个窗体
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmBmpFileNamedPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0; 
	int i = 0;

	//得到当前窗体对象
	pFrmBmpFileNamed = (GUIWINDOW *) pWndObj;
	//显示状态栏、桌面、信息栏控件
	DisplayPicture(pFrmBmpFileNamedBG);
    DisplayPicture(pFrmBmpFileNamedBGTableTitle);
    DisplayPicture(pFrmBmpFileNamedBGTableTitleStandbySet);
    DisplayPicture(pBmpFileNamedBgPreview);

    RefreshInOrDeValue();	
	for(i = 0;i<3; i++)
	{
        RefreshFiberNamingSteup(i+1);
	}
	RefreshPreviewName();
	
	//按钮标签显示
	DisplayLabel(pBmpFileNamedLblLabel);
	DisplayLabel(pBmpFileNamedLblStandbySet);

	//桌面标签
	DisplayLabel(pBmpFileNamedLblFiberNamingSetup);
	DisplayLabel(pBmpFileNamedLblPrefixLeft);
	DisplayLabel(pBmpFileNamedLblSuffixLeft);
	DisplayLabel(pBmpFileNamedLblNumLeft);
	
	DisplayLabel(pBmpFileNamedLblFileFormat);
	DisplayLabel(pBmpFileNamedLblFileFormatRadio);
    DisplayPicture(pBmpFileNamedBtnSelectPoint);
	
	DisplayLabel(pBmpFileNamedLblInOrDeValue);
	DisplayLabel(pBmpFileNamedLblIncrement);
	DisplayLabel(pBmpFileNamedLblDecrement);

	DisplayLabel(pBmpFileNamedLblPreview);
	
	DisplayWndMenu1(pFrmBmpFileNamedMenu);

	SetPowerEnable(1, 1);

	//FileNameRefresh_Fun();
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}


/***
  * 功能：
        	窗体frmbmpFilenamedd 的循环函数，进行窗体循环
  * 参数：
       	 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmBmpFileNamedLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	SendWndMsg_LoopDisable(pWndObj);
	
	return iReturn;
}


/***
  * 功能：
        窗体frmbmpFilenamedd 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmBmpFileNamedPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体frmbmpFilenamedd 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmBmpFileNamedResume(void *pWndObj)
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
static int BmpFileNamedTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    char buff[128];
    char temp[10];
    
	pBmpFileNamedStrLabel = TransString("SCREENSHOT_SET");
	pBmpFileNamedStrStandbySet = TransString("SYSTEMSET_STANDBY");
	
	pBmpFileNamedStrFiberNamingSetup = TransString("SCREENSHOT_NAME_SET");
	pBmpFileNamedStrPrefixLeft = TransString("OTDR_AUTONAME_PREFIX");
	pBmpFileNamedStrPrefixRight = TransString(pBmpFileNamedSetting->prefix);
	pBmpFileNamedStrSuffixLeft = TransString("OTDR_AUTONAME_SUFFIX");
	sprintf(temp, "%%0%dd", pBmpFileNamedSetting->suffixWidth);
	sprintf(buff, temp, pBmpFileNamedSetting->suffix % (int)pow(10, pBmpFileNamedSetting->suffixWidth));
	pBmpFileNamedStrSuffixRight = TransString(buff);
	pBmpFileNamedStrNumLeft = TransString("OTDR_AUTONAME_NUMOFDIGITS");
	sprintf(buff, "%d", pBmpFileNamedSetting->suffix);
	pBmpFileNamedStrNumRight = TransString(buff);

	pBmpFileNamedStrFileFormat = TransString("CURVEINFO_LBL_FILE_FORMAT");
	pBmpFileNamedStrFileFormatRadio = TransString("bmp");

	pBmpFileNamedStrInOrDeValue = TransString("OTDR_AUTONAME_INORDEVALUE");
	pBmpFileNamedStrIncrement = TransString("OTDR_AUTONAME_INCREMENT");
	pBmpFileNamedStrDecrement = TransString("OTDR_AUTONAME_DECREMENT");

	pBmpFileNamedStrPreview = TransString("OTDR_AUTONAME_PREVIEW");
    pBmpFileNamedStrNamePreview = TransString("Fiber000.SOR");
	
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
static int BmpFileNamedTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0; 
	//释放状态栏上的文本
	//...
	//释放按钮区的文本
	//...
	GuiMemFree(pBmpFileNamedStrLabel);
	GuiMemFree(pBmpFileNamedStrStandbySet);
	
	//释放桌面上的文本
    GuiMemFree(pBmpFileNamedStrFiberNamingSetup);
    GuiMemFree(pBmpFileNamedStrPrefixLeft);
    GuiMemFree(pBmpFileNamedStrPrefixRight);
    GuiMemFree(pBmpFileNamedStrSuffixLeft);
    GuiMemFree(pBmpFileNamedStrSuffixRight);
    GuiMemFree(pBmpFileNamedStrNumLeft);
    GuiMemFree(pBmpFileNamedStrNumRight);

    GuiMemFree(pBmpFileNamedStrFileFormat);
    GuiMemFree(pBmpFileNamedStrFileFormatRadio);

    GuiMemFree(pBmpFileNamedStrInOrDeValue);
    GuiMemFree(pBmpFileNamedStrIncrement);
    GuiMemFree(pBmpFileNamedStrDecrement);

    GuiMemFree(pBmpFileNamedStrPreview);
    GuiMemFree(pBmpFileNamedStrNamePreview);

	return iReturn;
}

void BmpFileNameddMenuCallBack(int iOpt)
{
	GUIWINDOW *pWnd = NULL;
    
	switch (iOpt)
	{
	case 0:
		break;
	case 1://调取时间窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmTimeSetInit, FrmTimeSetExit, 
		                    FrmTimeSetPaint, FrmTimeSetLoop, 
					        FrmTimeSetPause, FrmTimeSetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmBmpFileNamed);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调取语言设置窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmLanguageSetInit, FrmLanguageSetExit, 
							FrmLanguageSetPaint, FrmLanguageSetLoop, 
							FrmLanguageSetPause, FrmLanguageSetResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmBmpFileNamed);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmWiFiInit, FrmWiFiExit, 
								FrmWiFiPaint, FrmWiFiLoop, 
								FrmWiFiPause, FrmWiFiResume,
								NULL);			
		SendWndMsg_WindowExit(pFrmBmpFileNamed);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 4:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSysMaintainInit, FrmSysMaintainExit, 
		                    FrmSysMaintainPaint, FrmSysMaintainLoop, 
					        FrmSysMaintainPause, FrmSysMaintainResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmBmpFileNamed);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 5:
    #ifndef MINI2
    #else
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAboutInit, FrmAboutExit, 
							FrmAboutPaint, FrmAboutLoop, 
							FrmAboutPause, FrmAboutResume,
							NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmBmpFileNamed);	  //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			 //发送消息以便调用新的窗体
    #endif
		break;		
	case BACK_DOWN://退出时间设置
	case HOME_DOWN://退出时间设置
	    BmpFileNamedBtnQuit_Fun(); 
		break;	
	default:
		break;
	}
}
#ifdef F214 
#if 1
static int BmpFileNamedWndKey_Down(void *pInArg, int iInLen, 
								void *pOutArg, int iOutLen)
{
	 //错误标志、返回值定义
	 int iReturn = 0;
	 
	 unsigned int uiValue;

	 uiValue = (unsigned int)pInArg;
	 {
		 switch (uiValue)
		 {
		 case KEYCODE_ESC:
			 break;
		 case KEYCODE_MENU:
			 break;
		 case KEYCODE_RT:
			 break;
		 case KEYCODE_AVG:
			 break;
		 case KEYCODE_SETUP:
			 break;
		 case KEYCODE_KEY1:
			 break;
		 case KEYCODE_KEY2:
			 break;
		 case KEYCODE_BRIGHT:
			 break;
		 case KEYCODE_UP:
			 break;
		 case KEYCODE_DOWN:
			 break;
		 case KEYCODE_LEFT:
			 break;
		 case KEYCODE_RIGHT:
			 break;
		 case KEYCODE_ENTER:
			 break;
		 default:
			 break;
		 }
	 }
 return iReturn;
}

static int BmpFileNamedWndKey_Up(void *pInArg, int iInLen, 
					  		  void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	unsigned int uiValue;

	uiValue = (unsigned int)pInArg;
	 {
		 switch (uiValue)
		 {
		 case KEYCODE_ESC:
		 case KEYCODE_MENU:
			 BmpFileNameddMenuCallBack(BACK_DOWN);
			 break;
		 case KEYCODE_RT:
			 break;
		 case KEYCODE_AVG:
			 break;
		 case KEYCODE_SETUP:
			 break;
		 case KEYCODE_KEY1:
		 	Keykey1();
			 break;
		 case KEYCODE_KEY2:
		 	Keykey2();
			 break;
		 case KEYCODE_BRIGHT:
			 KeyBright();
			 break;
		 case KEYCODE_UP:
			 break;
		 case KEYCODE_DOWN:
			 break;
		 case KEYCODE_LEFT:
			 break;
		 case KEYCODE_RIGHT:
			 break;
		 case KEYCODE_ENTER:
            ScreenShot();
			 break;
		 default:
			 break;
		 }
	 }
	 return iReturn;
}
#endif
#endif
