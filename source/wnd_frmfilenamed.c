/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfilenamed.c
* 摘    要：  声明主窗体frmfilenamed 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v0.0.1
* 作    者：  	 
* 完成日期：  2015-1-22
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/

#include "wnd_frmfilenamed.h"

/**************************************************************
* 	为实现窗体frmfilenamed而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmotdrsave.h"
#include "app_frminit.h"
#include "wnd_frmime.h"
#include "wnd_frmmenubak.h"
#include "guiphoto.h"
#include "app_systemsettings.h"

/**************************************************************
* 		定义frmfilenamed中内部使用的数据结构
**************************************************************/

//引用的全局变量
extern POTDR_TOP_SETTINGS pOtdrTopSettings;
//自定义的全局变量
FILE_NAME_SETTING *pFilenameSetting;

static int filenameSetupIndex;
/***************************************************************
* 			窗体frmfilenamed中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmFileNamed = NULL;

static WNDMENU1 *pFrmFileNamedMenu = NULL;
/*********************桌面背景定义************************/
static GUIPICTURE *pFrmFileNamedBG = NULL;
static GUIPICTURE *pFrmFileNamedBGTableTitle = NULL;
//模块标题
static GUILABEL *pFileNamedLblLabel = NULL;
static GUICHAR *pFileNamedStrLabel = NULL;


/**********************桌面控件定义***********************/
//桌面控件
static GUIPICTURE *pFileNamedBtnSelectPoint[3];
static GUIPICTURE *pFileNamedBgPreview = NULL;
static GUIPICTURE *pFileNamedBtnUserInput[3];

static GUIPICTURE *pFileNamedBtnIncrement = NULL;
static GUIPICTURE *pFileNamedBtnDecrement = NULL;

/************************桌面标签定义*********************/
//桌面标签

static GUILABEL *pFileNamedLblFiberNamingSetup = NULL ;     //光纤名设置
static GUILABEL *pFileNamedLblPrefixLeft = NULL ;           //前缀左边文本
static GUILABEL *pFileNamedLblPrefixRight = NULL ;          //前缀内容
static GUILABEL *pFileNamedLblSuffixLeft = NULL;            //后缀左边文本  
static GUILABEL *pFileNamedLblSuffixRight = NULL;           //后缀内容
static GUILABEL *pFileNamedLblNumLeft = NULL;               //后缀数量左边文本
static GUILABEL *pFileNamedLblNumRight = NULL;              //后缀数量内容

static GUILABEL *pFileNamedLblFileFormat = NULL ;           //文件格式标题
static GUILABEL *pFileNamedLblFileFormatRadio[3];           //pdf sor inno

static GUILABEL *pFileNamedLblInOrDeValue = NULL;           //升序或降序
static GUILABEL *pFileNamedLblIncrement = NULL;             //升序
static GUILABEL *pFileNamedLblDecrement = NULL;             //降序

static GUILABEL *pFileNamedLblPreview = NULL ;              //预览标题
static GUILABEL *pFileNamedLblNamePreview = NULL ;          //预览内容

/************************桌面标签文本*********************/
//桌面标签
static GUICHAR *pFileNamedStrFiberNamingSetup = NULL ;      //光纤名设置
static GUICHAR *pFileNamedStrPrefixLeft = NULL ;            //前缀左边文本
static GUICHAR *pFileNamedStrPrefixRight = NULL ;           //前缀内容
static GUICHAR *pFileNamedStrSuffixLeft = NULL;             //后缀左边文本    
static GUICHAR *pFileNamedStrSuffixRight = NULL;            //后缀内容
static GUICHAR *pFileNamedStrNumLeft = NULL;                //后缀数量左边文本    
static GUICHAR *pFileNamedStrNumRight = NULL;               //后缀数量内容

static GUICHAR *pFileNamedStrFileFormat = NULL;             //文件格式标题
static GUICHAR *pFileNamedStrFileFormatRadio[3];            //pdf sor inno


static GUICHAR *pFileNamedStrInOrDeValue = NULL;
static GUICHAR *pFileNamedStrIncrement = NULL;
static GUICHAR *pFileNamedStrDecrement = NULL;

static GUICHAR *pFileNamedStrPreview = NULL ;               //预览标题
static GUICHAR *pFileNamedStrNamePreview = NULL ;           //预览内容
/*************************************************************
* 		文本资源初始化及销毁函数声明
**************************************************************/
//初始化文本资源
static int FileNamedTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int FileNamedTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/**************************************************************
* 				控件函数声明
**************************************************************/
//退出按钮
static int FileNamedBtnQuit_Fun();

void FilenamedMenuCallBack(int iOpt);

//刷新预览名
static void RefreshPreviewName();

/**************************************************************
* 				函数定义
**************************************************************/

/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateFileNamedWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmFileNamedInit, FrmFileNamedExit, 
		FrmFileNamedPaint, FrmFileNamedLoop, 
		FrmFileNamedPause, FrmFileNamedResume,
		NULL);
}

//参数初始化
static void InitFilaNamedPara()
{
    pFilenameSetting = &pOtdrTopSettings->pUser_Setting->sFileNameSetting;
}

//刷新文件名设置部分
static void RefreshFiberNamingSteup(int index)
{
    char buff[128] = {0};
    if(index == 1)  //前缀
    {
        if(pFileNamedStrPrefixRight)
        {
            GuiMemFree(pFileNamedStrPrefixRight);
        }
        pFileNamedStrPrefixRight = TransString(pFilenameSetting->prefix);
        SetLabelText(pFileNamedStrPrefixRight, pFileNamedLblPrefixRight);
        DisplayPicture(pFileNamedBtnUserInput[0]);
        DisplayLabel(pFileNamedLblPrefixRight);
    }

    if(index == 2)  //后缀
    {
        char format[10];
        if(pFileNamedStrSuffixRight)
        {
            GuiMemFree(pFileNamedStrSuffixRight);
        }
        sprintf(format, "%%0%dd", pFilenameSetting->suffixWidth);
    	sprintf(buff, format, (int)pFilenameSetting->suffix % (int)pow(10, pFilenameSetting->suffixWidth));
        pFileNamedStrSuffixRight = TransString(buff);
        SetLabelText(pFileNamedStrSuffixRight, pFileNamedLblSuffixRight);
        DisplayPicture(pFileNamedBtnUserInput[1]);
        DisplayLabel(pFileNamedLblSuffixRight);
    }
    
    if(index == 3)  //数字位数
    {
        if(pFileNamedStrNumRight)
        {
            GuiMemFree(pFileNamedStrNumRight);
        }
    	sprintf(buff, "%d", pFilenameSetting->suffixWidth);
        pFileNamedStrNumRight = TransString(buff);
        SetLabelText(pFileNamedStrNumRight, pFileNamedLblNumRight);
        DisplayPicture(pFileNamedBtnUserInput[2]);
        DisplayLabel(pFileNamedLblNumRight);
    }
}

//刷新文件格式
static void RefreshFileFormat()
{
    int i;
    for(i = 0; i < 3; i++)
    {
        SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pFileNamedBtnSelectPoint[i]);
        if(pFilenameSetting->fileFormat == i+1)
        {
            SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pFileNamedBtnSelectPoint[i]);
        }
        DisplayPicture(pFileNamedBtnSelectPoint[i]);
    }
    RefreshPreviewName();
}

//刷新升降序
static void RefreshInOrDeValue()
{
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pFileNamedBtnIncrement);
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_unselect.bmp", pFileNamedBtnDecrement);

    pFilenameSetting->suffixRule ? 
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pFileNamedBtnIncrement) :
    SetPictureBitmap(BmpFileDirectory"autoNameRadio_select.bmp", pFileNamedBtnDecrement);
    DisplayPicture(pFileNamedBtnIncrement);
    DisplayPicture(pFileNamedBtnDecrement);
}

//刷新预览名
static void RefreshPreviewName()
{
    char buff[128] = {0};
    char temp[128];
    
    if(pFileNamedStrNamePreview)
    {
        GuiMemFree(pFileNamedStrNamePreview);
    }
    if (pFilenameSetting->fileFormat == 1)
    {
        sprintf(temp, "%s%%0%dd.PDF", pFilenameSetting->prefix, pFilenameSetting->suffixWidth);
    }
    else if(pFilenameSetting->fileFormat == 2)
    {
        sprintf(temp, "%s%%0%dd.SOR", pFilenameSetting->prefix, pFilenameSetting->suffixWidth);
    }
    else
    {
        sprintf(temp, "%s%%0%dd.CUR", pFilenameSetting->prefix, pFilenameSetting->suffixWidth);
    }
    sprintf(buff, temp, pFilenameSetting->suffix % (int)pow(10, pFilenameSetting->suffixWidth));
    pFileNamedStrNamePreview = TransString(buff);
    SetLabelText(pFileNamedStrNamePreview, pFileNamedLblNamePreview);
    DisplayPicture(pFileNamedBgPreview);
    DisplayLabel(pFileNamedLblNamePreview);
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
	
	switch(filenameSetupIndex)
	{
        case 0:
            if(!isNameStr(cTmpBuff))
            {
                strcpy(pFilenameSetting->prefix, cTmpBuff);
            }
            else
            {
        		CreateIMEDialog(TransString("DIALOG_FILE_INPUT_INCORRECT"));
        		return;
            }
        break;
        case 1:
        {
            pFilenameSetting->suffix = atoi(cTmpBuff);
            if(0 == pFilenameSetting->suffix)
            {
                pFilenameSetting->suffix = 1;
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
                pFilenameSetting->suffixWidth = width;
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
static int  FileNamedBtnQuit_Fun()
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
    SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sFileNameSetting, sizeof(FILE_NAME_SETTING), FILENAME_SET);
    SaveSettings(FILENAME_SET);

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    				FrmOtdrSaveInit, FrmOtdrSaveExit, 
    				FrmOtdrSavePaint, FrmOtdrSaveLoop, 
    				FrmOtdrSavePause, FrmOtdrSaveResume,
    				NULL); 		 
    SendWndMsg_WindowExit(pFrmFileNamed);  
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
static int FileNamedLblUserInput_Down(void *pInArg, int iInLen, 
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
static int  FileNamedLblUserInput_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	char buff[128] = {0};
	char temp[128];
	filenameSetupIndex = iOutLen;
	switch(filenameSetupIndex)
	{
        case 0:
        IMEInit(pFilenameSetting->prefix, 25, 0, ReCreateFileNamedWindow, UserInputCallFun, NULL);
        break;
        case 1:
        sprintf(temp, "%%0%dd", pFilenameSetting->suffixWidth);
        sprintf(buff, temp, pFilenameSetting->suffix % (int)pow(10, pFilenameSetting->suffixWidth));
        IMEInit(buff, pFilenameSetting->suffixWidth, 1, ReCreateFileNamedWindow, UserInputCallFun, NULL);
        break;
        case 2:
        sprintf(buff, "%d", pFilenameSetting->suffixWidth);
        IMEInit(buff, 1, 1, ReCreateFileNamedWindow, UserInputCallFun, NULL);
        break;
        default:
        break;
	}
	
    return iReturn;
}
/***
  * 功能：
        		文件格式 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int FileNamedLblFileFormat_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    pFilenameSetting->fileFormat = iOutLen + 1;
    RefreshFileFormat();
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}


/***
  * 功能：
        		文件格式 按钮抬起处理函数
  * 参数：
       		 无
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
static int  FileNamedLblFileFormat_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

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
static int FileNamedLblInOrDeValue_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    pFilenameSetting->suffixRule = !iOutLen;
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
static int  FileNamedLblInOrDeValue_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        		窗体frmfilenamed 的初始化函数，建立窗体控件、
        		注册消息处理
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileNamedInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int loop;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL; 
	InitFilaNamedPara();
	//得到当前窗体对象
	pFrmFileNamed = (GUIWINDOW *) pWndObj;	
	
	FileNamedTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体背景图片 */
	pFrmFileNamedBG = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_filenamed1.bmp");
    pFrmFileNamedBGTableTitle = CreatePhoto("otdr_top1f");
    //标题
	pFileNamedLblLabel = CreateLabel(0, 24, 100, 24, pFileNamedStrLabel);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pFileNamedLblLabel);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pFileNamedLblLabel);
    
    //光纤名设置
    pFileNamedLblFiberNamingSetup = CreateLabel(50, 120-55, 300, 24, pFileNamedStrFiberNamingSetup);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblFiberNamingSetup);
    
    pFileNamedLblPrefixLeft = CreateLabel(70, 100, 130, 24, pFileNamedStrPrefixLeft);
    pFileNamedLblSuffixLeft = CreateLabel(70, 156, 130, 24, pFileNamedStrSuffixLeft);
    pFileNamedLblNumLeft = CreateLabel(70, 156+36, 130, 24, pFileNamedStrNumLeft);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblPrefixLeft);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblSuffixLeft);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblNumLeft);
    
    pFileNamedLblPrefixRight = CreateLabel(210, 102, 150, 50, pFileNamedStrPrefixRight);
    pFileNamedLblSuffixRight = CreateLabel(210, 125+36*1, 200, 24, pFileNamedStrSuffixRight);
    pFileNamedLblNumRight = CreateLabel(210, 125+36*2, 200, 24, pFileNamedStrNumRight);

    pFileNamedBtnUserInput[0] = CreatePicture(190, 95, 200, 50, BmpFileDirectory"autoNameInputBG1.bmp");
    pFileNamedBtnUserInput[1] = CreatePicture(190, 120+36*1, 200, 25, BmpFileDirectory"autoNameInputBG.bmp");
    pFileNamedBtnUserInput[2] = CreatePicture(190, 120+36*2, 200, 25, BmpFileDirectory"autoNameInputBG.bmp");

    //文件格式选择
	pFileNamedLblFileFormat = CreateLabel(431, 120-55, 200, 24, pFileNamedStrFileFormat);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblFileFormat);

    pFileNamedLblFileFormatRadio[0] = CreateLabel(510, 163+4-55, 130, 24, pFileNamedStrFileFormatRadio[0]);
    pFileNamedLblFileFormatRadio[1] = CreateLabel(510, 199+4-55, 130, 24, pFileNamedStrFileFormatRadio[1]);
    pFileNamedLblFileFormatRadio[2] = CreateLabel(510, 235+4-55, 130, 24, pFileNamedStrFileFormatRadio[2]);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblFileFormatRadio[0]);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblFileFormatRadio[1]);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblFileFormatRadio[2]);

    pFileNamedBtnSelectPoint[0] = CreatePicture(469, 163-55, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");
    pFileNamedBtnSelectPoint[1] = CreatePicture(469, 199-55, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");
    pFileNamedBtnSelectPoint[2] = CreatePicture(469, 235-55, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");

	//升降序
	pFileNamedLblInOrDeValue = CreateLabel(40, 289-20, 500, 24, pFileNamedStrInOrDeValue);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblInOrDeValue);

    pFileNamedLblIncrement = CreateLabel(119, 333+4-20, 100, 24, pFileNamedStrIncrement);
    pFileNamedLblDecrement = CreateLabel(268, 333+4-20, 100, 24, pFileNamedStrDecrement);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblIncrement);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblDecrement);
	
    pFileNamedBtnIncrement = CreatePicture(78, 333-20, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");
    pFileNamedBtnDecrement = CreatePicture(227, 333-20, 25, 25, BmpFileDirectory"autoNameRadio_unselect.bmp");
    
    //预览
	pFileNamedBgPreview = CreatePicture(100, 419-10, 540, 30, BmpFileDirectory"autoNamePreviewBG.bmp");
	pFileNamedLblPreview = CreateLabel(32, 419+4-10, 70, 24, pFileNamedStrPreview);
	pFileNamedLblNamePreview = CreateLabel(110, 419+4-6, 400, 24, pFileNamedStrNamePreview);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFileNamedLblPreview);
    
    
	/* 建立右侧菜单栏控件 */
	pFrmFileNamedMenu = CreateWndMenu1(0, 0,NULL,MENU_BACK, -1, 0, 40, FilenamedMenuCallBack);
									
	AddWndMenuToComp1(pFrmFileNamedMenu,pFrmFileNamed);	
	LoginWndMenuToMsg1(pFrmFileNamedMenu, pFrmFileNamed);	
	
	//注册窗体(因为所有的按键事件都统一由窗体进
	//行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFileNamed, 
	              pFrmFileNamed);

	for(loop = 0;loop < 3; loop++)
	{
    	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileNamedBtnUserInput[loop], 
    	              pFrmFileNamed);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFileNamedLblFileFormatRadio[loop], 
    	              pFrmFileNamed);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileNamedBtnSelectPoint[loop], 
    	              pFrmFileNamed);
	}

	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFileNamedLblIncrement, 
	              pFrmFileNamed);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFileNamedLblDecrement, 
	              pFrmFileNamed);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileNamedBtnIncrement, 
	              pFrmFileNamed);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFileNamedBtnDecrement, 
	              pFrmFileNamed);
	
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();
	
	for(loop = 0; loop < 3; loop++)
	{
    	LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedBtnUserInput[loop], 
    	               FileNamedLblUserInput_Up, NULL, loop, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedBtnUserInput[loop], 
    	               FileNamedLblUserInput_Down, NULL, loop, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedBtnSelectPoint[loop], 
    	               FileNamedLblFileFormat_Up, NULL, loop, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedBtnSelectPoint[loop], 
    	               FileNamedLblFileFormat_Down, NULL, loop, pMsg);

    	LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedLblFileFormatRadio[loop], 
    	               FileNamedLblFileFormat_Up, NULL, loop, pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedLblFileFormatRadio[loop], 
    	               FileNamedLblFileFormat_Down, NULL, loop, pMsg);
	}
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedLblIncrement, 
	               FileNamedLblInOrDeValue_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedLblIncrement, 
	               FileNamedLblInOrDeValue_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedBtnIncrement,
                    FileNamedLblInOrDeValue_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedBtnIncrement,
                    FileNamedLblInOrDeValue_Down, NULL, 0, pMsg);
                    
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedLblDecrement, 
	               FileNamedLblInOrDeValue_Up, NULL, 1, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedLblDecrement, 
	               FileNamedLblInOrDeValue_Down, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFileNamedBtnDecrement,
                    FileNamedLblInOrDeValue_Up, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFileNamedBtnDecrement,
                    FileNamedLblInOrDeValue_Down, NULL, 1, pMsg);
	
	return iReturn;
}
/***
  * 功能：
        		窗体frmfilenamed 的退出函数，释放所有资源
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileNamedExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL; 
	int i = 0;
	//得到当前窗体对象
	pFrmFileNamed = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmFileNamed);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pFrmFileNamedBG);
    DestroyPicture(&pFrmFileNamedBGTableTitle);
	DestroyPicture(&pFileNamedBgPreview);
	DestroyPicture(&pFileNamedBtnIncrement);
	DestroyPicture(&pFileNamedBtnDecrement);
	for(i = 0; i < 3; i++)
	{
	    DestroyPicture(&pFileNamedBtnUserInput[i]);
        DestroyPicture(&pFileNamedBtnSelectPoint[i]);
	}
	
	DestroyLabel(&pFileNamedLblLabel);
	DestroyLabel(&pFileNamedLblFiberNamingSetup);
	DestroyLabel(&pFileNamedLblPrefixLeft);
	DestroyLabel(&pFileNamedLblPrefixRight);
	DestroyLabel(&pFileNamedLblSuffixLeft);
	DestroyLabel(&pFileNamedLblSuffixRight);
	DestroyLabel(&pFileNamedLblNumLeft);
	DestroyLabel(&pFileNamedLblNumRight);
	
	DestroyLabel(&pFileNamedLblFileFormat);
	DestroyLabel(&pFileNamedLblFileFormatRadio[0]);
	DestroyLabel(&pFileNamedLblFileFormatRadio[1]);
	DestroyLabel(&pFileNamedLblFileFormatRadio[2]);
	
	DestroyLabel(&pFileNamedLblInOrDeValue);
	DestroyLabel(&pFileNamedLblIncrement);
	DestroyLabel(&pFileNamedLblDecrement);

	DestroyLabel(&pFileNamedLblPreview);
	DestroyLabel(&pFileNamedLblNamePreview);

	DestroyWndMenu1(&pFrmFileNamedMenu);
	
	//文本内容销毁
	FileNamedTextRes_Exit(NULL, 0, NULL, 0);

   return iReturn;
}


/***
  * 功能：
        		窗体frmfilenamed 的绘制函数，绘制整个窗体
  * 参数：
       		 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileNamedPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0; 
	int i = 0;

	//得到当前窗体对象
	pFrmFileNamed = (GUIWINDOW *) pWndObj;
	//显示状态栏、桌面、信息栏控件
	DisplayPicture(pFrmFileNamedBG);
    DisplayPicture(pFrmFileNamedBGTableTitle);    
    DisplayPicture(pFileNamedBgPreview);

    RefreshInOrDeValue();	
	for(i = 0;i<3; i++)
	{
        RefreshFiberNamingSteup(i+1);
	}
	RefreshFileFormat();
	RefreshPreviewName();
	
	//按钮标签显示
	DisplayLabel(pFileNamedLblLabel);

	//桌面标签
	DisplayLabel(pFileNamedLblFiberNamingSetup);
	DisplayLabel(pFileNamedLblPrefixLeft);
	DisplayLabel(pFileNamedLblSuffixLeft);
	DisplayLabel(pFileNamedLblNumLeft);
	
	DisplayLabel(pFileNamedLblFileFormat);
	DisplayLabel(pFileNamedLblFileFormatRadio[0]);
	DisplayLabel(pFileNamedLblFileFormatRadio[1]);
	DisplayLabel(pFileNamedLblFileFormatRadio[2]);
	
	DisplayLabel(pFileNamedLblInOrDeValue);
	DisplayLabel(pFileNamedLblIncrement);
	DisplayLabel(pFileNamedLblDecrement);

	DisplayLabel(pFileNamedLblPreview);
	
	DisplayWndMenu1(pFrmFileNamedMenu);

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
    
	return iReturn;
}


/***
  * 功能：
        	窗体frmfilenamed 的循环函数，进行窗体循环
  * 参数：
       	 1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileNamedLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	SendWndMsg_LoopDisable(pWndObj);
	
	return iReturn;
}


/***
  * 功能：
        窗体frmfilenamed 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileNamedPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体frmfilenamed 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileNamedResume(void *pWndObj)
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
static int FileNamedTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    char buff[128];
    char temp[10];
    
	pFileNamedStrLabel = TransString("OTDR_AUTONAME_AUTONAME");
	
	pFileNamedStrFiberNamingSetup = TransString("OTDR_AUTONAME_NAMESET");
	pFileNamedStrPrefixLeft = TransString("OTDR_AUTONAME_PREFIX");
	pFileNamedStrPrefixRight = TransString(pFilenameSetting->prefix);
	pFileNamedStrSuffixLeft = TransString("OTDR_AUTONAME_SUFFIX");
	sprintf(temp, "%%0%dd", pFilenameSetting->suffixWidth);
	sprintf(buff, temp, pFilenameSetting->suffix % (int)pow(10, pFilenameSetting->suffixWidth));
	pFileNamedStrSuffixRight = TransString(buff);
	pFileNamedStrNumLeft = TransString("OTDR_AUTONAME_NUMOFDIGITS");
	sprintf(buff, "%d", pFilenameSetting->suffix);
	pFileNamedStrNumRight = TransString(buff);

	pFileNamedStrFileFormat = TransString("CURVEINFO_LBL_FILE_FORMAT");
	pFileNamedStrFileFormatRadio[0] = TransString("OTDR_AUTONAME_PDF");
	pFileNamedStrFileFormatRadio[1] = TransString("OTDR_AUTONAME_SOR");
	pFileNamedStrFileFormatRadio[2] = TransString("OTDR_AUTONAME_INNO");

	pFileNamedStrInOrDeValue = TransString("OTDR_AUTONAME_INORDEVALUE");
	pFileNamedStrIncrement = TransString("OTDR_AUTONAME_INCREMENT");
	pFileNamedStrDecrement = TransString("OTDR_AUTONAME_DECREMENT");

	pFileNamedStrPreview = TransString("OTDR_AUTONAME_PREVIEW");
    pFileNamedStrNamePreview = TransString("Fiber000.SOR");
	
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
static int FileNamedTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0; 
	//释放状态栏上的文本
	//...
	//释放按钮区的文本
	//...
	GuiMemFree(pFileNamedStrLabel);
	
	//释放桌面上的文本
    GuiMemFree(pFileNamedStrFiberNamingSetup);
    GuiMemFree(pFileNamedStrPrefixLeft);
    GuiMemFree(pFileNamedStrPrefixRight);
    GuiMemFree(pFileNamedStrSuffixLeft);
    GuiMemFree(pFileNamedStrSuffixRight);
    GuiMemFree(pFileNamedStrNumLeft);
    GuiMemFree(pFileNamedStrNumRight);

    GuiMemFree(pFileNamedStrFileFormat);
    GuiMemFree(pFileNamedStrFileFormatRadio[0]);
    GuiMemFree(pFileNamedStrFileFormatRadio[1]);
    GuiMemFree(pFileNamedStrFileFormatRadio[2]);

    GuiMemFree(pFileNamedStrInOrDeValue);
    GuiMemFree(pFileNamedStrIncrement);
    GuiMemFree(pFileNamedStrDecrement);

    GuiMemFree(pFileNamedStrPreview);
    GuiMemFree(pFileNamedStrNamePreview);

	return iReturn;
}

void FilenamedMenuCallBack(int iOpt)
{
	switch(iOpt)
	{
		case BACK_DOWN:
			FileNamedBtnQuit_Fun();
			break;
	}
}
