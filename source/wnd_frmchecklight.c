/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmchecklight.c  
* 摘    要：  实现右侧菜单栏控件
*
* 当前版本：  v1.0.0 
* 作    者：  wjg
* 完成日期：  2020/10/16

*******************************************************************************/
#include "wnd_frmchecklight.h"

//
static int iFocus = 0;										//按键光标选中标志位，默认选中确定按钮（0：确定，1：取消）
static WNDCHECKLIGHT *gCheckLight = NULL;					//窗体控件
/*******************************************************************************
***                    声明wnd_frmchecklight.c内部使用函数                   ***
*******************************************************************************/
static int WndCheckOk_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen);
static int WndCheckOk_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen);
static int WndCheckCancel_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen);
static int WndCheckCancel_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen);
static int CheckWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int CheckWndKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

//销毁有光检查
static int DestroyCheckLight(WNDCHECKLIGHT **pCheckLight);
//显示有光检查
static int DisplayCheckLight(WNDCHECKLIGHT *pCheckLight);
//添加有光检查到接收消息的控件队列
static int AddCheckLightToComp(WNDCHECKLIGHT *pCheckLight, GUIWINDOW *pWnd);
//删除控件从接受消息的控件队列中
static int DelCheckLightFromComp(WNDCHECKLIGHT *pCheckLight, GUIWINDOW *pWnd);
//刷新按键显示
static void FlushKeyDisplay(void);

	/*******************************************************************************
***                       声明wnd_frmchecklight.c函数定义                    ***
*******************************************************************************/
	/***
  * 功能：
     	创建一个提示框
  * 参数：
  		1.TextIndex		              :菜单项的数量
  		2.int button                  :提示框按钮，0:只显示取消按钮  1:只显示确认按钮  2:同时显示确认和取消
  		2.GUIWINDOW *pWnd             :当前窗体
  		3.CHECKCALLFUNC CallBack	  :回调函数
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/
	WNDCHECKLIGHT *CreateDialog(int TitleIndex, int TextIndex, int button, GUIWINDOW *pWnd, CHECKCALLFUNC CallBack)
{
	//错误标志定义
	if(TextIndex < 0 || pWnd == NULL)
		return NULL;
	
	WNDCHECKLIGHT *pCheckLight = NULL;

	//分配资源
	pCheckLight = (WNDCHECKLIGHT *)calloc(1, sizeof(WNDCHECKLIGHT));
	if (NULL == pCheckLight)
	{
		LOG(LOG_ERROR, "Menu Malloc ERR\n");
		return NULL;
	}

	//初始化结构体变量
	pCheckLight->CallBack = CallBack;
	pCheckLight->pWnd = pWnd;
	pCheckLight->buttonType = button;
	
	//背景图片
	pCheckLight->pCheckLightBg = CreatePicture(120, 88, 400, 190, BmpFileDirectory"bg_checklight.bmp");
	pCheckLight->pCheckStrOk = GetCurrLanguageText(SYSTEM_LBL_CONFIRM);
	pCheckLight->pCheckStrCancel = GetCurrLanguageText(SYSTEM_LBL_CANCEL);
	
	switch(button)
	{
        case 0://仅取消
        pCheckLight->pCheckOk = CreatePicture(224, 196, 123, 48, NULL);
	    pCheckLight->pCheckCancel = CreatePicture(120 + 101, 88 + 149, 200, 41, BmpFileDirectory"btn_dialog_press.bmp");
    	pCheckLight->pCheckLblOk = CreateLabel(230, 208, 110, 24, NULL);
    	pCheckLight->pCheckLblCancel = CreateLabel(120 + 101+50, 88+149+11, 100, 16, pCheckLight->pCheckStrCancel);
	    SetPictureEnable(0, pCheckLight->pCheckOk);
	    SetLabelEnable(0, pCheckLight->pCheckLblOk);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pCheckLight->pCheckLblCancel);
		break;
        case 1://无按钮
        pCheckLight->pCheckOk = CreatePicture(120 + 124 - 25, 50 + 60, 55, 18, BmpFileDirectory"btn_dialog_press.bmp");
	    pCheckLight->pCheckCancel = CreatePicture(424, 196, 123, 48, NULL);
    	pCheckLight->pCheckLblOk = CreateLabel(120 + 124 - 25, 50 + 60, 55, 18, pCheckLight->pCheckStrOk);
    	pCheckLight->pCheckLblCancel = CreateLabel(430, 208, 110, 24, NULL);
	    SetPictureEnable(0, pCheckLight->pCheckCancel);
	    SetLabelEnable(0, pCheckLight->pCheckLblCancel);
        break;
        case 2://确定+取消
        pCheckLight->pCheckOk = CreatePicture(120, 88 + 149, 200, 41, BmpFileDirectory"btn_dialog_press.bmp");
	    pCheckLight->pCheckCancel = CreatePicture(120 + 200, 88 + 149, 200, 41, BmpFileDirectory"btn_dialog_unpress.bmp");
    	pCheckLight->pCheckLblOk = CreateLabel(120 + 84, 88 + 149+11, 100, 16, pCheckLight->pCheckStrOk);
    	pCheckLight->pCheckLblCancel = CreateLabel(120 + 285, 88 + 149+11, 100, 16, pCheckLight->pCheckStrCancel);
        break;
        default:
        break;
	}

	pCheckLight->pCheckOk->Visible.iLayer = DIALOG_LAYER;
	pCheckLight->pCheckCancel->Visible.iLayer = DIALOG_LAYER;
	
	pCheckLight->pCheckLightBg->Visible.iFocus = 1;

	pCheckLight->pCheckStrTitle = GetCurrLanguageText(TitleIndex);
	pCheckLight->pCheckStrInfo = GetCurrLanguageText(TextIndex);
	
	pCheckLight->pCheckLblTitle = CreateLabel(120 + 8, 88 + 9, 100, 24, pCheckLight->pCheckStrTitle);
	pCheckLight->pCheckLblInfo = CreateLabel(120 + 90, 88 + 88, 220, 90, pCheckLight->pCheckStrInfo);
	
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pCheckLight->pCheckLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCheckLight->pCheckLblInfo);	
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCheckLight->pCheckLblOk);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCheckLight->pCheckLblCancel);

	// SetLabelAlign(2, pCheckLight->pCheckLblTitle);
	// SetLabelAlign(2, pCheckLight->pCheckLblOk);
	// SetLabelAlign(2, pCheckLight->pCheckLblCancel);
	SetLabelAlign(2, pCheckLight->pCheckLblInfo);
	
	pCheckLight->pCheckLblLayer = CreateLabel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	pCheckLight->pCheckLblLayer->Visible.iLayer = DIALOG_LAYER-1;
	pCheckLight->pCheckLblLayer->Visible.iCursor = 1;

	AddCheckLightToComp(pCheckLight, pWnd);
	DisplayCheckLight(pCheckLight);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	gCheckLight = pCheckLight;

	return pCheckLight;
}

/***
  * 功能：
     	销毁菜单控件
  * 参数：
  		1.WNDMENU *pCheckLight : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
static int DestroyCheckLight(WNDCHECKLIGHT **pCheckLight)
{
	//参数检测
	if (NULL == *pCheckLight)
	{
		return -1;
	}
	
	DelCheckLightFromComp(*pCheckLight, (*pCheckLight)->pWnd);
	//销毁背景
	DestroyPicture(&((*pCheckLight)->pCheckLightBg));
	DestroyPicture(&((*pCheckLight)->pCheckOk));
	DestroyPicture(&((*pCheckLight)->pCheckCancel));

	DestroyLabel(&((*pCheckLight)->pCheckLblTitle));
	DestroyLabel(&((*pCheckLight)->pCheckLblInfo));
	DestroyLabel(&((*pCheckLight)->pCheckLblOk));
	DestroyLabel(&((*pCheckLight)->pCheckLblCancel));
	DestroyLabel(&((*pCheckLight)->pCheckLblLayer));

	GuiMemFree((*pCheckLight)->pCheckStrTitle);
	GuiMemFree((*pCheckLight)->pCheckStrInfo);
	GuiMemFree((*pCheckLight)->pCheckStrOk);
	GuiMemFree((*pCheckLight)->pCheckStrCancel);

	//释放空间
	free(*pCheckLight);
	*pCheckLight = NULL;
	
	return 0;
}

/***
  * 功能：
     	显示菜单控件
  * 参数：
  		1.WNDMENU *pCheckLight : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
static int DisplayCheckLight(WNDCHECKLIGHT *pCheckLight)
{	
	if (NULL == pCheckLight)
	{
		return -1;
	}
	
	//显示背景
	DisplayPicture(pCheckLight->pCheckLightBg);
	DisplayPicture(pCheckLight->pCheckOk);
	DisplayPicture(pCheckLight->pCheckCancel);

	DisplayLabel(pCheckLight->pCheckLblTitle);
	DisplayLabel(pCheckLight->pCheckLblInfo);
	DisplayLabel(pCheckLight->pCheckLblOk);
	DisplayLabel(pCheckLight->pCheckLblCancel);

	return 0;
}

/***
  * 功能：
     	添加控件到接受消息的控件队列
  * 参数：
  		1.WNDMENU *pCheckLight : 指向menu控件
  		2.GUIWINDOW *pWnd   : 队列的所属窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有互斥锁的情况下调用
***/ 
static int AddCheckLightToComp(WNDCHECKLIGHT *pCheckLight, GUIWINDOW *pWnd)
{
	if ((NULL == pCheckLight) || (NULL == pWnd))
	{
		return -1;
	}
	
	GUIMESSAGE *pMsg;
	pMsg = GetCurrMessage();

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  	  pCheckLight->pCheckLightBg, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  	  pCheckLight->pCheckOk, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  	  pCheckLight->pCheckCancel, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL),
					  pCheckLight->pCheckLblLayer, pWnd);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCheckLight->pCheckOk,
                		WndCheckOk_Down, pCheckLight, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pCheckLight->pCheckOk,
                		WndCheckOk_Up, pCheckLight, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCheckLight->pCheckCancel,
                		WndCheckCancel_Down, pCheckLight, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pCheckLight->pCheckCancel,
                		WndCheckCancel_Up, pCheckLight, 4, pMsg);

	//注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pCheckLight->pCheckLightBg, 
                    CheckWndKey_Down, pCheckLight, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pCheckLight->pCheckLightBg, 
                    CheckWndKey_Up, pCheckLight, 0, pMsg);

	return 0;
}
                     			
/***
  * 功能：
     	注册消息处理函数
  * 参数：
  		1.WNDMENU *pCheckLight : 指向menu控件
  		2.GUIMESSAGE *pMsg  : 当前的消息队列
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有消息注册队列互斥锁的情况下调用
***/ 
static int DelCheckLightFromComp(WNDCHECKLIGHT *pCheckLight, GUIWINDOW *pWnd)
{
	if ((NULL == pCheckLight) || (NULL == pWnd))
	{
		return -1;
	}

	GUIMESSAGE *pMsg;
	pMsg = GetCurrMessage();

	DelWindowComp(pCheckLight->pCheckLightBg, pWnd);
	DelWindowComp(pCheckLight->pCheckOk, pWnd);
	DelWindowComp(pCheckLight->pCheckCancel, pWnd);
	DelWindowComp(pCheckLight->pCheckLblLayer, pWnd);
	
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCheckLight->pCheckOk, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, pCheckLight->pCheckOk, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCheckLight->pCheckCancel, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, pCheckLight->pCheckCancel, pMsg);

	LogoutMessageReg(GUIMESSAGE_KEY_DOWN, pCheckLight->pCheckLightBg, pMsg);
	LogoutMessageReg(GUIMESSAGE_KEY_UP, pCheckLight->pCheckLightBg, pMsg);
	
	return 0;
}

//按钮按下处理
static int WndCheckOk_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen)
{
	WNDCHECKLIGHT *pCheckLight = (WNDCHECKLIGHT *)pOutArg;

	TouchChange("btn_dialog_press.bmp", pCheckLight->pCheckOk, NULL, pCheckLight->pCheckLblOk, 1);	//黄色
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

//按钮弹起处理
static int WndCheckOk_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen)
{
	WNDCHECKLIGHT *pCheckLight = (WNDCHECKLIGHT *)pOutArg;
	
	TouchChange("btn_dialog_unpress.bmp", pCheckLight->pCheckOk, NULL, pCheckLight->pCheckLblOk, 1);	//黄色

	RefreshScreen(__FILE__, __func__, __LINE__);
	if (pCheckLight->CallBack)
	{
		(*(pCheckLight->CallBack))(1);
	}
	DestroyCheckLight(&pCheckLight);
	
	return 0;
}

//Exit 按下处理
static int WndCheckCancel_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen)
{
	WNDCHECKLIGHT *pCheckLight = (WNDCHECKLIGHT *)pOutArg;

	TouchChange("btn_dialog_press.bmp", pCheckLight->pCheckCancel, NULL, pCheckLight->pCheckLblCancel, 1);	//黄色
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}

//Exit 弹起处理
static int WndCheckCancel_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen)
{
	WNDCHECKLIGHT *pCheckLight = (WNDCHECKLIGHT *)pOutArg;
	
	TouchChange("btn_dialog_unpress.bmp", pCheckLight->pCheckCancel, NULL, pCheckLight->pCheckLblCancel, 1);	//黄色
	if (pCheckLight->CallBack)
	{
		//(*(pCheckLight->CallBack))(0);
		(*(pCheckLight->CallBack))(1);
	}
	RefreshScreen(__FILE__, __func__, __LINE__);

	DestroyCheckLight(&pCheckLight);

	return 0;
}

//按键按下处理函数
static int CheckWndKey_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

	switch (uiValue)
    {
		case KEYCODE_ESC:
			WndCheckCancel_Down(pInArg, iInLen, pOutArg, iOutLen);
			break;
		case KEYCODE_HOME:
			break;
		default:
			break;
	}
    
	return iReturn;
}

//按键弹起处理函数
static int CheckWndKey_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

	switch (uiValue)
    {
	case KEYCODE_LEFT:
		if (gCheckLight->buttonType == 2)
		{
			iFocus = (iFocus == 1) ? 0 : 1;
			FlushKeyDisplay();
		}
		break;
	case KEYCODE_RIGHT:
		if (gCheckLight->buttonType == 2)
		{
			iFocus = (iFocus == 1) ? 0 : 1;
			FlushKeyDisplay();
		}
		break;
	case KEYCODE_ENTER:
		if ((gCheckLight->buttonType == 0) ||
			((gCheckLight->buttonType == 2) && (iFocus == 1)))
		{
			WndCheckCancel_Up(NULL, 0, gCheckLight, 0);
		}
		else
		{
			WndCheckOk_Up(NULL, 0, gCheckLight, 0);
		}
		break;
	case KEYCODE_ESC:
		if ((gCheckLight->buttonType == 0) || (gCheckLight->buttonType == 2))
		{
			WndCheckCancel_Up(NULL, 0, gCheckLight, 0);
		}
		break;
	default:
		break;
	}

	return iReturn;
}

//刷新按键显示
static void FlushKeyDisplay(void)
{
	if (iFocus == 0)
	{
		TouchChange("btn_dialog_press.bmp", gCheckLight->pCheckOk,
			gCheckLight->pCheckStrOk, gCheckLight->pCheckLblOk, 1);
		TouchChange("btn_dialog_unpress.bmp", gCheckLight->pCheckCancel,
			gCheckLight->pCheckStrCancel, gCheckLight->pCheckLblCancel, 1);
	}
	else
	{
		TouchChange("btn_dialog_unpress.bmp", gCheckLight->pCheckOk,
			gCheckLight->pCheckStrOk, gCheckLight->pCheckLblOk, 1);
		TouchChange("btn_dialog_press.bmp", gCheckLight->pCheckCancel,
			gCheckLight->pCheckStrCancel, gCheckLight->pCheckLblCancel, 1);
	}

	RefreshScreen(__FILE__, __func__, __LINE__);
}
