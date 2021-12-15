/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_global.h
* 摘    要：  声明各窗体的公共数据、公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2012-12-31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_GLOBAL_H
#define _WND_GLOBAL_H

/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"
#include "global_index.h"
#include "guimyglobal.h"

#include "app_text.h"
#include "app_frmfilebrowse.h"
#include "app_memory_detecter.h"
#include "app_speaker.h"

#include "wnd_frmupdate.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmstandbyset.h"
#include "wnd_stack.h"
#include "wnd_frmmain.h"
#include "wnd_signal.h"

/********************************
* 定义GUI中当前使用的按键映射码值
* 注：以下内容需根据实际情况修改
********************************/
#define KEYCODE_SHIFT         316 
#define KEYCODE_OTDR_LEFT     310 
#define KEYCODE_OTDR_RIGHT    311 
#define KEYCODE_START         315

#define KEYCODE_UP      	103
#define KEYCODE_DOWN    	108
#define KEYCODE_LEFT    	105
#define KEYCODE_RIGHT   	106
#define KEYCODE_ENTER   	28

#define KEYCODE_FILE    	144
#define KEYCODE_VFL    		141
#define KEYCODE_ESC	    	1 
#define KEYCODE_HOME    	139

#define MAX_KEY_COUNT       12

//临时变量数组大小
#define TEMP_ARRAY_SIZE     128

//声明公共资源
typedef enum _font_color
{
	EN_FONT_BLACK  = 0,
	EN_FONT_YELLOW = 1,
	EN_FONT_GRAY   = 2,
	EN_FONT_GRAY1  = 3,
	EN_FONT_WHITE  = 4,
	EN_FONT_RED    = 5,
	EN_FONT_GREEN  = 6,
	EN_FONT_BLUE   = 7
}FontColor;

extern unsigned char *pFrameBufferBak;		//帧缓冲备份区域
//交换两个数,此函数必须在两个数不相等的情况下使用
#define GuiSwap(x, y) (((x)=(x)^(y)),((y)=(x)^(y)),((x)=(x)^(y)))
#define COLOR_BLOCK_SIZE 12

typedef struct _selectarea
{
	int iSx;
	int iEx;
	int iSy;
	int iEy;
}SELECTAREA;

typedef struct _guibutton
{
	int startX;
	int startY;
	int length;
	int high;
	char bgPath[150];
	MSGFUNC fBtnDown;
	MSGFUNC fBtnUp;
	int iVisible;
}GUIBUTTON1;

typedef struct _guilabel
{
	int startX;
	int startY;
	int length;
	int high;
	unsigned char ucSize;
	unsigned char ucStyle;
	GUICHAR **pStrObj;
	MSGFUNC fBtnDown;
	MSGFUNC fBtnUp;
	int iVisible;
}GUILABEL1;


extern unsigned char GucClearFlg;

/* 定义初始化控件时，传入的回调函数指针 */
typedef void (*CALLLBACKHANDLE)(void);

/* 
 * 回调函数指针
 * 主要用于控件退出时还原窗体的回调函数
 */
typedef void (*CALLLBACKWINDOW)(GUIWINDOW **);

/* 
 * 回调函数指针
 * 主要用于控件退出时还原窗体的回调函数
 */
typedef int (*CALLLBACKWINDOW1)(void *);

/***************************************
* 定义用于完成功能按钮显示的公共操作函数
***************************************/
//显示功能按钮按下
int ShowBtn_FuncDownExtra(GUIPICTURE *pBtnFx, GUILABEL *pLblFx, GUILABEL *pLblFx1);
//显示功能按钮弹起
int ShowBtn_FuncUpExtra(GUIPICTURE *pBtnFx, GUILABEL *pLblFx, GUILABEL *pLblFx1);

/***********************************
* 定义用于完成状态显示的公共操作函数
***********************************/
//显示电源信息
int ShowSta_PowerInfo(GUIPICTURE *pIcoPower);
//刷新日期(iLoop：0：不循环，1：循环但只在日期变化的时候刷新)
int ShowSta_DateInfo(int iLoop);
//更新LABEL的文本资源
int UpdateLabelRes( GUICHAR *pTargetStr, GUILABEL *pTargetLbl, GUIPICTURE *pBackGround);

/***
  * 功能：
        获取FPGA的序列号
  * 参数：
		unsigned long long *pDNA: 指向存贮序列号的指针
  * 返回：
        0			成功
        -1			失败
  * 备注：
***/
int GetFPGASerialNum(unsigned long long *pDNA);


/***
  * 功能：
        实现按钮、Label等控件状态的改变
  * 参数：
        1.char *strBitmapFile:   	需要显示的图片控件路径，可以为NULL
        2.GUIPICTURE *pPicObj:     	需要显示的功能按钮标签，可以为NULL
        3.GUICHAR *pStr:       		需要显示的文字信息，可以为NULL
        3.GUILABEL *pLblObj:       	需要显示的功能按钮标签，可以为NULL
        4.int color:     		   	字体颜色选择1是黑色，2是黄色,0是系统默认颜色
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int TouchChange(char *strBitmapFile, GUIPICTURE *pPicObj, 
				GUICHAR *pStr, GUILABEL *pLblObj, int color);

void ChangeAbleInArea(int iEnableFlag, int iTypeOfGUI, int x1, int y1, int x2, int y2);

/***
  * 功能：
        设置指定区域内控件与设定的颜色进行透明显示
  * 参数：
        1.unsigned char ucAlpha:	Alpha值越大，原背景色越亮
        2.unsigned long ulColour:	24位颜色代码
        3.unsigned int uiPlaceX:    指定区域的X坐标
        4.unsigned int uiPlaceY:    指定区域的Y坐标
        5.unsigned int uiWidth:  	指定区域的宽度
        6.unsigned int uiHeight: 	指定区域的高度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2014.09.01
  		此函数只是将指定区域进行透明色处理，如果要将区域内的控件置为无效，
  		请调用ChangeAbleInArea函数
***/
int DispTransparent(unsigned char ucAlpha, unsigned long ulColour,
					 unsigned int uiPlaceX, unsigned int uiPlaceY, 
                     unsigned int uiWidth, unsigned int uiHeight);
/***
  * 功能：
        设置指定区域内控件与设定的颜色进行透明显示
  * 参数：
        1.unsigned char ucAlpha:	Alpha值越大，原背景色越亮
        2.unsigned long ulColour:	24位颜色代码
        3.unsigned int uiPlaceX:    指定区域的X坐标
        4.unsigned int uiPlaceY:    指定区域的Y坐标
        5.unsigned int uiWidth:  	指定区域的宽度
        6.unsigned int uiHeight: 	指定区域的高度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2018.06.27
  		此函数只是将指定区域进行透明色处理，如果要将区域内的控件置为无效，
  		请调用ChangeAbleInArea函数
  		区别于老的函数，该函数直接修改帧缓冲的数据，有可能出现闪烁现象
***/
int DispTransparentNew(unsigned char ucAlpha, unsigned long ulColour,
					 unsigned int uiPlaceX, unsigned int uiPlaceY, 
                     unsigned int uiWidth, unsigned int uiHeight);
                     
int CreateThread(void);

/***
  * 功能：
        显示WiFi连接状态
  * 参数：
        1、GUIPICTURE *pIcoWiFi	:    	需要显示的WiFi状态图形框
        2、int iWinType			:		当前所处的窗体，0、主窗体；1、其他窗体；5、窗体没有发生变化
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by 
***/
int ShowSta_WiFiInfo(GUIPICTURE *pIcoWiFi, int iWinType);
//显示u盘状态
int ShowSta_UDiskInfo(int iWinType);

//显示光纤端面插入状态
int ShowSta_FiberMicroscopeInfo(int iWinType);

/*** 
 * 功能： 
 	在屏幕上画一个选择框
 * 参数： 
 	1.char *pfbsave :帧缓冲区的备份缓存起始地址
 	2.SELECTAREA *pSelctArea:指向选择的区域的数据指针
 * 返回： 
 	成功 0，失败负值
 * 备注
 	使用之前需要先备份帧缓冲的数据
	默认会把选择框画到当前的帧缓冲里
***/ 
int DrawSeclectBox(unsigned char *pfbsave, SELECTAREA *pSelctArea);

//用于创建各窗体标题LABEL的底色
void CreateTitle(GUILABEL *lbl);

#define GuiMemAlloc(lSize)  malloc(lSize)

#define GuiMemFree(pMem) if(pMem) {free(pMem); pMem=NULL;}

//关机
void TurnOffDEV(void);


//获取公共资源 包括字体等
int GetGlobalResource(void);

//释放全局资源区域
int ReleasGlobalResource(void);
/***
  * 功能：
		初始化提示信息框
  * 参数：
		无
  * 返回：
		无
  * 备注：
  		类似于安卓的小的提示信息框
***/
void InitInfoDialog(void);

/***
  * 功能：
		在屏幕上显示提示信息
  * 参数：
		1、GUICHAR *str	:	将要显示的提示信息
		2、GUIWINDOW *pWndObj:   需要添加GUI窗体控件的窗体对象
  * 返回：
		0、成功
		非0、失败
  * 备注：
  		类似于安卓的小的提示信息框，显示后其他控件不使能
***/
int ShowInfoDialog(GUICHAR *str, GUIWINDOW *pWndObj);
/***
  * 功能：
		不使能提示信息框
  * 参数：
		1、GUIWINDOW *pWndObj:   需要添加GUI窗体控件的窗体对象
  * 返回：
		0、成功
		非0、失败
  * 备注：
  		类似于安卓的小的提示信息框
***/
int DisableInfoDialog(GUIWINDOW *pWndObj);


//在显示弹出提示的函数
//返回值>=0 成功；<0失败
typedef int (*InCueFun)(int arg, void *pArg);
typedef GUICHAR* (*InCueSetStr)(int arg);

int DisplayCue(GUIWINDOW *pWnd, InCueFun fCue, InCueSetStr fCurStr);
int MsgNoSdcard(GUIWINDOW *pWnd);
//宏定义图层级别
#define OTDRCOOR_LAYER 1	//Otdr坐标系
#define DROPLIST_LAYER 3	//上/下拉列表
#define DIALOG_LAYER 5		//对话框
/***
  * 功能：
        显示进度条
  * 参数：
        1.unsigned int uiPlaceX	:       进度条水平放置位置，以左上角为基点
        2.unsigned int uiPlaceY	:       进度条垂直放置位置，以左上角为基点
        3.unsigned int uiWidth	:      	进度条水平宽度
        4.unsigned int uiHeight	:     	进度条垂直高度  
        5、int iProgress		:		当前进度，0-100
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowProgressBar(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                    unsigned int uiWidth, unsigned int uiHeight, 
                    int iProgress);


/***
  * 功能：
        分期付款功能是否有效
  * 参数：
		无
  * 返回：
        加密为非0 ，不加密为0
  * 备注：
***/
int InstIsEncrypted();

/***
  * 功能：
        分期付款信息是否已经过期
  * 参数：

  * 返回：
        过期返回1，不过期返回0
  * 备注：
***/
int InstOutOfDate(int shutdown);
int SetPowerEnable(int iEnable, int Wnd);
//为了秒记数，输入框时候光标闪烁
int getTimeCount();
int DisableAllWidgets(GUIWINDOW *pWnd);
int GetLabelRealWidth(GUILABEL *pLblObj);
int EnableAllWidgets(GUIWINDOW *pWnd);
int KnobFrequency(unsigned int uiMsec);
int InitMsg(void);
int SendMsg(char *cmdBuf);
int RecMsg();
int ScreenShot(void);

/***
  * 功能：
        判断是否可以写指定文件
        若文件存在，内部弹出对话框提示用户
  * 参数：
        filePath 指定的文件全路径
  * 返回：
        成功返回1，否则返回0
  * 备注：
***/
int CanWriteFile(const char* filePath);
//保存到U盘(针对sor与sola)
void SaveToUsb(CALLLBACKWINDOW func, char* lastPath, char* fileName, FILETYPE fileType);
typedef int (*Operate)(char *argument);
void GuiOperateSave(unsigned char *pFbBackup, GUIWINDOW *pCallerWnd, Operate operateFunc, char *absolutePath);

int GetFirstPath(char* pCurrPath, char *pFirstPath);

GUIFONT * getGlobalFnt(FontColor color);
int isArabic();
int isThai();
int isFont24();
unsigned short * thaiLangProcess(unsigned short * unicode);
//检测字符串是否只包含字母数字和下划线  返回0"name"合格
int isNameStr(char *name);
//判断给的文件和文件名是否存在
int isFileExist(const char *name, const char *path);
//处理长按按键移动标记线响应
int HandleMoveOtdrMark(int KeyState, int KeyCode);
// MENU 长按截图功能
void KeyDownScreenShot(void);
//输出错误信息到文件中
void PrintErrorInfo(char* fileName, char* log, int iCreateFile);

//返回主界面
void ReturnMenuOK(void);
//跳转到vfl界面
void JumpVflWindow(void);

/***
  * 功能：
        保存系统设置
  * 参数：
		1.int iResetSystemFlag:是否要重置系统参数的标志位，0：不重置，1：重置
		2.unsigned short languageRecover：当iResetSystemFlag=1，该参数会被使用，否则不用（只有重置系统的时候，系统设置的语言不能修改）
        无
  * 备注：
***/

void SaveSystemSet(int iResetSystemFlag, unsigned short languageRecover);

//租赁是否到期,到期返回0，未到期返回1
int IsLeaseExpiration();
#endif
