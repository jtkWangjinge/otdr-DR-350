/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmdialog.h
* 摘    要：  声明窗体wnd_frmdialog(对话框)的窗体处理线程及相关操作函数。该窗体
*             提供了对话框功能。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*******************************************************************************/
#include "wnd_frmdialog.h"

/**********************************************************************************
**						为实现窗体frmdialog而需要引用的其他头文件				 **
***********************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "app_global.h"

#include "wnd_global.h"

/**********************************************************************************
**								变量定义				 						 **
***********************************************************************************/
static int iDialogWinX = 0; 							// 窗体左上角X
static int iDialogWinY = 0;								// 窗体左上角Y
static int iDialogKeyFlag = 0;
char *pDiologIcoType = NULL;							// 存贮警告类型图片
static unsigned char GucButtonType = 0;					// 按钮的类型，显示一个按钮还是两个按钮
static unsigned char GucJumpWindowFlag = 0;             // 是否跳转界面
CALLLBACKWINDOW DialogcallBackFunc = NULL;				// 重绘前一个窗体的回调函数
CALLLBACKHANDLE OkcallBackFunc     = NULL;				// dialog窗体中按下OK按钮后的回调函数
CALLLBACKHANDLE CancelcallBackFunc = NULL;				// dialog窗体中按下cancel按钮后的回调函数

static int iFocus = 0;									//按键光标选中标志位，默认选中确定按钮（0：确定，1：取消）
/**********************************************************************************
**							窗体frmdialog中的控件定义部分						 **
***********************************************************************************/
static GUIWINDOW *pFrmDialog = NULL;					//窗体控件
/**********************************背景控件定义************************************/ 
static GUIPICTURE *pDialogSetBg = NULL;					//对话框的背景图片
/******************************窗体标题栏控件定义**********************************/
/******************************对话框的控件定义************************************/
static GUICHAR *pDialogStrTitle 	= NULL;				//桌面上对话框的标题文本     			
static GUICHAR *pDialogStrOk     	= NULL;				//桌面上对话框的确定取的文本
static GUICHAR *pDialogStrCancle 	= NULL;				//桌面上对话框的取消键的文本
GUICHAR *pDialogStrInfo     		= NULL;				//按下OK按钮后，弹出的窗体上的文本

static GUILABEL *pDialogLblOk       = NULL;     		//对话框确定按钮的Lbl控件
static GUILABEL *pDialogLblCancle   = NULL;     		//对话框取消按钮的Lbl控件
static GUILABEL *pDialogLblInfo   	= NULL;  			//按下OK按钮后，弹出的窗体上的Label控件
static GUILABEL *pDialogLblTitle 	= NULL;				//按下OK按钮后，弹出的窗体上的Label控件

static GUIPICTURE *pDialogBtnType   = NULL;				//对话框的类型图片
static GUIPICTURE *pDialogBtnOk     = NULL;				//对话框的确定按钮图片
static GUIPICTURE *pDialogBtnCancle = NULL;				//对话框的取消按钮图片

/**********************************************************************************
**	    	窗体frmdialog中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
static int DialogTextRes_Init(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);

static int DialogTextRes_Exit(void *pInArg, int iInLen, 
							   void *pOutArg, int iOutLen);

/**********************************************************************************
**			    	窗体frmdialog中的控件事件处理函数定义部分					 **
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int DialogWndKey_Down(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
static int DialogWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/************************软件升级内的按键事件处理函数******************************/
static int DialogBtnOk_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int DialogBtnCancle_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);

static int DialogBtnOk_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int DialogBtnCancle_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);

/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/
static int	DialogExit(void);
//刷新按键显示
static void FlushKeyDisplay(void);

/***
  * 功能：
        窗体frmdialog的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogInit(void *pWndObj)
{
	//错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmDialog = (GUIWINDOW *) pWndObj;

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    DialogTextRes_Init(NULL, 0, NULL, 0);

    //初始化跳转界面的标志位
    GucJumpWindowFlag = 0;
    //建立窗体控件
    /****************************************************************/
    //建立桌面上的控件
    pDialogSetBg = CreatePicture(iDialogWinX, iDialogWinY, 400, 300,
                                 BmpFileDirectory"bg_dialog.bmp");
	pDialogBtnType  = CreatePicture(iDialogWinX + 170, iDialogWinY + 95,  
									 60, 50, pDiologIcoType);
	pDialogLblTitle = CreateLabel(iDialogWinX + 8, iDialogWinY + 9, 100, 16, pDialogStrTitle);

	switch(GucButtonType)
	{
		/* 仅显示确定按钮 */
		case 0:
			pDialogBtnOk = CreatePicture(iDialogWinX + 101, iDialogWinY + 260, 200, 41,
										 BmpFileDirectory "btn_dialog_press.bmp");
			pDialogBtnCancle = CreatePicture(iDialogWinX + 46, iDialogWinY + 95, 55, 18,
											 BmpFileDirectory "btn_dialog_unpress.bmp");

			pDialogLblOk     = CreateLabel(iDialogWinX + 101 + 50, iDialogWinY + 260+12, 100, 16, pDialogStrOk);
			pDialogLblCancle = CreateLabel(iDialogWinX + 46, iDialogWinY + 95, 55, 18, pDialogStrCancle);
			pDialogLblInfo = CreateLabel(iDialogWinX + 90, iDialogWinY + 177, 220, 90, pDialogStrInfo);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblInfo);
			SetPictureEnable(0, pDialogBtnCancle);
			SetLabelEnable(0, pDialogLblCancle);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblOk);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogLblOk);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogLblInfo);
			break;
			
		/* 显示确定和取消按钮 */
		case 1:
			pDialogBtnOk = CreatePicture(iDialogWinX, iDialogWinY + 259, 200, 41,
										 BmpFileDirectory "btn_dialog_press.bmp");
			pDialogBtnCancle = CreatePicture(iDialogWinX + 200, iDialogWinY + 259, 200, 41,
											 BmpFileDirectory "btn_dialog_unpress.bmp");

			pDialogLblOk     = CreateLabel(iDialogWinX + 84, iDialogWinY + 271, 100, 16, pDialogStrOk);
			pDialogLblCancle = CreateLabel(iDialogWinX + 285, iDialogWinY + 271, 100, 16, pDialogStrCancle);

            pDialogLblInfo   = CreateLabel(iDialogWinX + 90, iDialogWinY + 177, 220, 90, pDialogStrInfo);
	        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblInfo);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblOk);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblCancle);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogLblInfo);
			// SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogLblCancle);
			break;
			
		/* 不显示确定和取消按钮 */
		case 2:
			pDialogBtnOk = CreatePicture(iDialogWinX + 124, iDialogWinY + 95, 55, 18,
										 BmpFileDirectory "btn_dialog_press.bmp");
			pDialogBtnCancle = CreatePicture(iDialogWinX + 46, iDialogWinY + 95, 55, 18,
											 BmpFileDirectory "btn_dialog_unpress.bmp");

			pDialogLblOk = CreateLabel(iDialogWinX + 124, iDialogWinY + 95, 55, 18, pDialogStrOk);
			pDialogLblCancle = CreateLabel(iDialogWinX + 46, iDialogWinY + 95, 55, 18, pDialogStrCancle);

			pDialogLblInfo = CreateLabel(iDialogWinX + 90, iDialogWinY + 177, 220, 90, pDialogStrInfo);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogLblInfo);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblInfo);
			SetPictureEnable(0, pDialogBtnOk);
			SetLabelEnable(0, pDialogLblOk);

			SetPictureEnable(0, pDialogBtnCancle);
			SetLabelEnable(0, pDialogLblCancle);
			break;
		default:
			break;
	}

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件
    //...

    //设置菜单按钮区的标题控件的字体，橘黄色

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmDialog, 
                  pFrmDialog);    
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDialogBtnOk, 
                  pFrmDialog);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDialogBtnCancle, 
                  pFrmDialog);

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();

    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmDialog, 
                    DialogWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmDialog, 
                    DialogWndKey_Up, NULL, 0, pMsg);
	
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDialogBtnOk, 
                    DialogBtnOk_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDialogBtnCancle, 
                    DialogBtnCancle_Down, NULL, 0, pMsg);
	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pDialogBtnOk, 
                    DialogBtnOk_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pDialogBtnCancle, 
                    DialogBtnCancle_Up, NULL, 0, pMsg);

	return iReturn;
}


/***
  * 功能：
        窗体frmdialog的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	DBG_ENTER();
    //得到当前窗体对象
	pFrmDialog = (GUIWINDOW *)pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmDialog);

    //销毁窗体控件
    //***************************************************************/	
    //销毁桌面上的控件
	DestroyPicture(&pDialogSetBg);
	DestroyPicture(&pDialogBtnType);
	DestroyPicture(&pDialogBtnOk);
	DestroyPicture(&pDialogBtnCancle);
	
	DestroyLabel(&pDialogLblOk);
	DestroyLabel(&pDialogLblCancle);
	DestroyLabel(&pDialogLblInfo);
	DestroyLabel(&pDialogLblTitle);

	//释放文本资源
    //***************************************************************/
    DialogTextRes_Exit(NULL, 0, NULL, 0);
    
    //还原标志位的设置，防止下次进入界面点击取消被卡死。
    GucJumpWindowFlag= 0;
	//还原按键光标选中位置
	iFocus = 0;
    //***************************************************************/
    //释放按钮区的字体
    //...
	DBG_EXIT(iReturn);
    return iReturn;
}


/***
  * 功能：
        窗体frmdialog的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogPaint(void *pWndObj)
{
	//错误标志、返回值定义
    int iReturn = 0;
	
    //得到当前窗体对象
    pFrmDialog = (GUIWINDOW *) pWndObj;

	/* 绘制无效区域窗体 */
	DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    //显示桌面上的控件
	DisplayPicture(pDialogSetBg);
	DisplayPicture(pDialogBtnType);
	DisplayPicture(pDialogBtnOk);
	DisplayPicture(pDialogBtnCancle);
	
	DisplayLabel(pDialogLblTitle);
	DisplayLabel(pDialogLblInfo);
	DisplayLabel(pDialogLblOk);
	DisplayLabel(pDialogLblCancle);

	SetPowerEnable(0, 0);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
        窗体frmdialog的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);   
	
    return iReturn;
}


/***
  * 功能：
        窗体frmdialog的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmdialog的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDialogResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/**********************************************************************************
**	    	窗体frmdialog中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/

/***
  * 功能：
        初始化文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogTextRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化桌面上的文本
	pDialogStrOk     = GetCurrLanguageText(SYSTEM_LBL_CONFIRM);
	pDialogStrCancle = GetCurrLanguageText(SYSTEM_LBL_CANCEL);

    return iReturn;
}


/***
  * 功能：
        释放文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogTextRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//对话框标题的文本
	GuiMemFree(pDialogStrTitle);
	
    //对话框上的文本
	GuiMemFree(pDialogStrOk);
	GuiMemFree(pDialogStrCancle);
	GuiMemFree(pDialogStrInfo);

    return iReturn;
}


/**********************************************************************************
**			    			窗体的按键事件处理函数				 				 **
***********************************************************************************/

/***
  * 功能：
        窗体按键的down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogWndKey_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
	
    uiValue = (unsigned int)pInArg;

	if(iDialogKeyFlag == 0)
	{
		iDialogKeyFlag = 1;
			
    	switch (uiValue)
	    {
			case KEYCODE_ESC:
				if(pDialogBtnCancle->Visible.iEnable)
				{
					DialogBtnCancle_Down(NULL, 0, NULL, 0);
				}
				break;
            case KEYCODE_ENTER:
                if (GucButtonType && (iFocus == 1))
                {
                    DialogBtnCancle_Down(NULL, 0, NULL, 0);
                }
                else
                {
                    DialogBtnOk_Down(NULL, 0, NULL, 0);
                }
                break;
			default:
				break;
	    }
	}
 
    return iReturn;
}


/***
  * 功能：
        窗体按键的up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DialogWndKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	if(iDialogKeyFlag == 1)
	{
		iDialogKeyFlag = 0;

		//进入主界面的方式（otdr left 3 times+otdr right 3 times+shift）,
		//该方案适用于租赁日期到了之后，工厂人员无法进入主界面重新设置租赁日期
		static int iOtdrLeftFlag = 0;
		static int iOtdrRightFlag = 0;

		switch (uiValue)
		{
		case KEYCODE_SHIFT:
			if (iOtdrLeftFlag == 3 && iOtdrRightFlag == 3 && GucButtonType == 2)
			{
				LOG(LOG_INFO, "---enter main menu-----\n");
				iOtdrLeftFlag = 0;
				iOtdrRightFlag = 0;
				ReturnMenuOK();
			}
			break;
		case KEYCODE_OTDR_LEFT:
			if (iOtdrLeftFlag < 3 && GucButtonType == 2)
			{
				iOtdrLeftFlag++;
			}
			else
			{
				iOtdrLeftFlag = 0;
			}
			break;
		case KEYCODE_OTDR_RIGHT:
			if (iOtdrLeftFlag == 3 && GucButtonType == 2)
			{
				if (iOtdrRightFlag < 3 && GucButtonType == 2)
				{
					iOtdrRightFlag++;
				}
				else
				{
					iOtdrRightFlag = 0;
				}
			}
			break;
		case KEYCODE_LEFT:
			iOtdrLeftFlag = 0;
			iOtdrRightFlag = 0;
			if (GucButtonType)
			{
				iFocus = (iFocus == 1) ? 0 : 1;
			}
			FlushKeyDisplay();
			break;
		case KEYCODE_RIGHT:
			iOtdrLeftFlag = 0;
			iOtdrRightFlag = 0;
			if (GucButtonType)
			{
				iFocus = (iFocus == 1) ? 0 : 1;
			}
			FlushKeyDisplay();
			break;
		case KEYCODE_ENTER:
			iOtdrLeftFlag = 0;
			iOtdrRightFlag = 0;
			if (GucButtonType && (iFocus == 1))
			{
				DialogBtnCancle_Up(NULL, 0, NULL, 0);
			}
			else
			{
				DialogBtnOk_Up(NULL, 0, NULL, 0);
			}
			break;
		case KEYCODE_ESC:
		{
			iOtdrLeftFlag = 0;
			iOtdrRightFlag = 0;
			if (!GucButtonType)
			{
				if (pDialogBtnOk->Visible.iEnable)
				{
					DialogBtnOk_Up(NULL, 0, NULL, 0);
				}
			}
			else
			{
				if (pDialogBtnCancle->Visible.iEnable)
				{
					DialogBtnCancle_Up(NULL, 0, NULL, 0);
				}
			}
		}
			break;
		default:
			break;
		}
	}
	
    return iReturn;
}


/**********************************************************************************
**							对话框内的控件事件处理函数							 **
***********************************************************************************/

/***
  * 功能：
		OK按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogBtnOk_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    TouchChange("btn_dialog_press.bmp", pDialogBtnOk, pDialogStrOk, pDialogLblOk, 1);
    //刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
    return iReturn;
}


/***
  * 功能：
		cancle按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogBtnCancle_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	TouchChange("btn_dialog_press.bmp", pDialogBtnCancle, pDialogStrCancle, pDialogLblCancle, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
    return iReturn;
}


/***
  * 功能：
		OK按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogBtnOk_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	TouchChange("btn_dialog_unpress.bmp", pDialogBtnOk, pDialogStrOk, pDialogLblOk, 1);
	// RefreshScreen(__FILE__, __func__, __LINE__);	
	
	// SetPictureEnable(0, pDialogBtnOk);
	// SetPictureEnable(0, pDialogBtnCancle);
	// DisplayPicture(pDialogSetBg);
	// DisplayPicture(pDialogBtnType);
	// DisplayPicture(pDialogBtnOk);
	// DisplayPicture(pDialogBtnCancle);
	// DisplayLabel(pDialogLblTitle);
	// DisplayLabel(pDialogLblInfo);

	RefreshScreen(__FILE__, __func__, __LINE__);	

	if(OkcallBackFunc != NULL)
	{
		(*OkcallBackFunc)();	
	}

    //仅当不需要跳转界面(GucJumpWindowFlag=0)，需要回调原有的界面 add by wjg 2020/8/25
    if(!GucJumpWindowFlag)
    {
	    DialogExit();
    }

    return iReturn;
}


/***
  * 功能：
		cancle按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogBtnCancle_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	TouchChange("btn_dialog_unpress.bmp", pDialogBtnCancle, pDialogStrCancle, pDialogLblCancle, 1);
	//刷新屏幕缓存

	// SetPictureEnable(0, pDialogBtnOk);
	// SetPictureEnable(0, pDialogBtnCancle);
	// DisplayPicture(pDialogSetBg);
	// DisplayPicture(pDialogBtnType);
	// DisplayPicture(pDialogBtnOk);
	// DisplayPicture(pDialogBtnCancle);
	// DisplayLabel(pDialogLblTitle);
	// DisplayLabel(pDialogLblInfo);	
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
	if(CancelcallBackFunc != NULL)
	{
		(*CancelcallBackFunc)();	
	}
	//仅当不需要跳转界面(GucJumpWindowFlag=0)，需要回调原有的界面 add by wjg 2020/8/25
    if(!GucJumpWindowFlag)
    {
	    DialogExit();
    }
	
    return iReturn;
}


/***
  * 功能：
		按下OK按钮后需要显示的信息
  * 参数：
		1.info : 	需要显示的信息
  * 返回：
		无
  * 备注：
***/
void ShowDialogExtraInfo(GUICHAR *info)
{
    //临时变量定义
    GUIPEN *pPen;
	unsigned int uiColorBack;

	//画背景
	pPen = GetCurrPen();
	uiColorBack = pPen->uiPenColor;
	pPen->uiPenColor = 0xF4F4F4;
	DrawBlock(iDialogWinX+90, iDialogWinY+170, iDialogWinX+310, iDialogWinY+270);
	pPen->uiPenColor = uiColorBack;

	pDialogStrInfo = info;
	SetLabelText(pDialogStrInfo, pDialogLblInfo);
	SetLabelArea(iDialogWinX + 90, iDialogWinY + 177, 
				  iDialogWinX+90+220, iDialogWinY+177+90, pDialogLblInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDialogLblInfo);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDialogLblInfo);
	DisplayLabel(pDialogLblInfo);

	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
}


/***
  * 功能：
		对话框退出
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DialogExit(void)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;
    if(DialogcallBackFunc)
    {
        (*DialogcallBackFunc)(&pWnd);
        SendWndMsg_WindowExit(pFrmDialog);      //发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
    }
    return iReturn;
}

//刷新按键显示
static void FlushKeyDisplay(void)
{
	if (GucButtonType)
	{
		if (iFocus == 0)
		{
			TouchChange("btn_dialog_press.bmp", pDialogBtnOk, pDialogStrOk, pDialogLblOk, 1);
			TouchChange("btn_dialog_unpress.bmp", pDialogBtnCancle, pDialogStrCancle, pDialogLblCancle, 1);
		}
		else
		{
			TouchChange("btn_dialog_unpress.bmp", pDialogBtnOk, pDialogStrOk, pDialogLblOk, 1);
			TouchChange("btn_dialog_press.bmp", pDialogBtnCancle, pDialogStrCancle, pDialogLblCancle, 1);
		}
	}

	RefreshScreen(__FILE__, __func__, __LINE__);
}


/***
  * 功能：
		建立对话框
  * 参数：
		1.x         : 	对话框左上角的横坐标
		2.y         : 	对话框左上角的纵坐标
		3.pTitleStr : 	对话框的标题文本
		4.pTextStr  : 	对话框上要显示的信息
		5.ucIcoType : 	对话框类型 0:警告对话框
		6.ucButtonType:	0、仅显示确定按钮；1、显示确定和取消按钮；2、不显示确定和取消按钮
		7.func	   : 	还原窗体的回调函数
		8.okfunc	   : 	按下OK按钮后执行的回调函数S
		9.cancelfunc: 	按下cancel按钮后执行的回调函数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
  		如果没有需要在按下ok或cancel按钮后需要处理的回调函数，可输入NULL
***/
void DialogInit(int x, int y, GUICHAR *pTitleStr, GUICHAR *pTextStr, 
				unsigned char ucIcoType, unsigned char ucButtonType,
				CALLLBACKWINDOW func, CALLLBACKHANDLE okfunc, CALLLBACKHANDLE cancelfunc)
{
	GUIWINDOW *pWnd = NULL;
	
	iDialogWinX = x;
	iDialogWinY = y;
	pDialogStrTitle = pTitleStr;
	pDialogStrInfo = pTextStr;
	DialogcallBackFunc = func;
	OkcallBackFunc = okfunc;
	CancelcallBackFunc = cancelfunc;
	GucButtonType = ucButtonType;

	switch(ucIcoType)
	{
		case 0:
			pDiologIcoType = BmpFileDirectory"bg_dialog_warring.bmp";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
			break;
		default :
			break;
	}
	
    pWnd = CreateWindow(x, y, 400, 300,
                        FrmDialogInit , FrmDialogExit, 
                        FrmDialogPaint, FrmDialogLoop, 
					    FrmDialogPause, FrmDialogResume,
                        NULL);          				//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());				//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);						//发送消息以便调用新的窗体
}

/***
  * 功能：
        设置是否跳转界面的标志位
  * 参数：
        1.jumpFlag  :   标志位，0：不跳转界面，1：跳转界面
  * 返回：
        无
  * 备注：
        如果需要点击确定或取消按钮时，界面需要跳转到其他界面，则jumpFlag设置为1，否则设置为0
***/

void SetJumpWindowFlag(unsigned char jumpFlag)
{
    GucJumpWindowFlag= jumpFlag;
}            

