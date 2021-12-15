/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmvfl.c
* 摘    要：  实现右侧菜单栏处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/25
*
*******************************************************************************/

#include "wnd_frmmenubak.h"

#include "app_frminit.h"
#include "app_frmotdr.h"
#include "app_screenshots.h"

#include "guiphoto.h"

#include "wnd_frmabout.h"
#include "wnd_frmotdr.h"
#include "wnd_frmsavefile.h"
#include "wnd_frmcablerange.h"
#include "wnd_frmcablesequence.h"
//以下是工厂菜单
#include "wnd_frmconfigureinfo.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmlinearity.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmauthorization.h"
#include "wnd_frmserialnum.h"

/**************************************
* 定义wnd_frmmenu.c公有变量常用菜单文本
**************************************/

/******************************
* 声明wnd_frmmenu.c内部使用函数
******************************/
static int WndMenuBtnItem_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen);
static int WndMenuBtnItem_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen);
static int WndMenuBtnHome_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen);
static int WndMenuBtnHome_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen);
static int WndMenuKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int WndMenuKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//获取按键按下之后焦点位置
static int GetCurFocus(WNDMENU1 *pMenuObj, int option);
//获取可用的使能的按钮
static int GetEnabledMenu(WNDMENU1 *pMenuObj, int iCurrentIndex, int iOption);
//根据物理按键添加
static void FlushMenu1(WNDMENU1 *pMenuObj, int option, int iRefreshFocus);

static int OtdrKeyCount = 0;
static WNDMENU1 *pCurMenu = NULL;
static int iCurMenuItemPush = -1;
extern CURR_WINDOW_TYPE enCurWindow;                //当前的窗口
static int iSaveFileSmallWndFlag = 0;
static int iSelectedMenuBg = 1;						//默认选中
static int iFactorySmallWndFlag[FACTORY_MENU_COUNT] = {0};
static int iFactorySmallWnd[FACTORY_MENU_COUNT] =
{
	ENUM_DEBUG_INFO_WIN,
	ENUM_LANGUAGE_CONFIG_WIN,
	ENUM_NON_LINEARITY_WIN,
	ENUM_UNCERTAINTY_WIN,
	ENUM_FACTORY_SET_WIN,
	ENUM_AUTHORIZATION_WIN
};
//按键响应回调函数
static WNDMENU1 *sWndMenu = NULL;
//标识工厂菜单中显示英文文字
static int iDsiplayLbl = 0;
//设置工厂菜单小窗口的使能标志位
static void SetFactorySmallWndFlag(int iFlag);
//声明按键响应函数
//按键down处理函数
static void KeyUpOrDownCallBack(int iOption);
//左键响应处理函数
static void KeyLeftCallBack(void);
//右键响应处理函数
static void KeyRightCallBack(void);
//enter键响应处理函数
static void KeyEnterCallBack(void);
//item中文字在每个按键中的偏移量
static int ItemWordOffset = 20;

	/***
  * 功能：
     	创建一个菜单控件（临时函数（针对于菜单中未实现的多国语翻译的文本））
  * 参数：
  		1.iNum			  :菜单项的数量
  		2.unsigned int len:文本长度
  		3.char **StrTxt	  :菜单的文本 第0个为标题
  		4.UINT16 iBackHome:按键home和back是否有作用；
								高8位为0x00为back不可用，其他为可用
								低8位为0x00为home不可用，其他为可用
  		5.int iFocus	  :菜单条目焦点 <= 0表示所有项都无焦点
  		6.UINT32 iTopAlign:表示菜单项开始的顶部边距
  		7.SECBACKFUNC  	  :回调函数
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/
	WNDMENU1 *CreateStringWndMenu(int iNum, unsigned int size, char **uiLblIdx,
								  UINT16 iBackHome, int iFocus, unsigned int iTopMargin,
								  SECBACKFUNC CallBack)
{
    //错误标志定义
	int iErr = 0;
	unsigned int y = iTopMargin;
	WNDMENU1 *pWndMenu = NULL;

	if (!iErr)
	{
		//分配资源
		pWndMenu = (WNDMENU1 *)calloc(1, sizeof(WNDMENU1));
		if (NULL == pWndMenu)
		{
			LOG(LOG_ERROR, "Menu Malloc ERR\n");
			iErr = -2;
		}
	}

	if (!iErr) 
	{
		//临时变量定义
		int i = 0;
		//初始化结构体变量
		pWndMenu->iItemCnt = iNum;
		pWndMenu->iFocus = iFocus;
		pWndMenu->CallBack = CallBack;
		pWndMenu->iBackHome = iBackHome;


		int iLblCount = size/sizeof(unsigned int);
		CODER_LOG(CoderGu, "iLblCount = %u\n", iLblCount);
		CODER_LOG(CoderGu, "GetCurrWindowType() = %d\n", GetCurrWindowType());
		
		//背景图片
		pWndMenu->pMenuBg = CreatePicture(521, 40, 119, 440, BmpFileDirectory"bg_global_menu_bg.bmp");

		for (i = 0; i < iNum; ++i) 
		{
			//按钮控件
			pWndMenu->pBtnItem[i] = CreatePicture(521, y + 55 * i, 119, 55, BmpFileDirectory"bg_menu_item_unpress.bmp");
			pWndMenu->itemEnable[i] = 1;

			if(iLblCount > i)
			{
				pWndMenu->pStrItem[i] = TransString(uiLblIdx[i]);
			}
			else
			{
				pWndMenu->pStrItem[i] = TransString("");
			}

			pWndMenu->pLblItem[i] = CreateLabel(521, y + 55 * i + ItemWordOffset, 119, 55,
												pWndMenu->pStrItem[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pWndMenu->pLblItem[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pWndMenu->pLblItem[i]);
		}

		for (i = iNum; i < MENU_MAX; i++) //无用控件
		{
			pWndMenu->pBtnItem[i] = CreatePicture(521, y + 55 * i, 119, 55, BmpFileDirectory"bg_menu_item_disable.bmp");
			pWndMenu->itemEnable[i] = 1;

			if (iLblCount > i)
			{
				pWndMenu->pStrItem[i] = TransString(uiLblIdx[i]);
			}
			else
			{
				pWndMenu->pStrItem[i] = TransString("");
			}

			pWndMenu->pLblItem[i] = CreateLabel(521, y + 55 * i + ItemWordOffset, 119, 55,
												pWndMenu->pStrItem[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pWndMenu->pLblItem[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pWndMenu->pLblItem[i]);
		}
		if (iBackHome != 0xffff)
		{
			//创建返回主菜单按钮
			pWndMenu->pBtnHome = CreatePicture(241, y + 24 * i, 72, 24, BmpFileDirectory"bg_menu_home_unpress.bmp");
		}
	}

	setCurMenu(pWndMenu);
	
	sWndMenu = pWndMenu;
	memset(iFactorySmallWndFlag, 0, sizeof(iFactorySmallWndFlag));
	//文件浏览器小窗口
	isInSmallFileBrowse = 0;
    iDsiplayLbl = 1;

	return pWndMenu;
}

/***
  * 功能：
     	创建一个菜单控件
  * 参数：
  		1.iNum			  		:菜单项的数量
  		2.unsigned int len		:文本长度
  		3.char **StrTxt	  		:菜单的文本 第0个为标题
  		4.UINT16 iBackHome		:按键home和back是否有作用；
								高8位为0x00为back不可用，其他为可用
								低8位为0x00为home不可用，其他为可用
  		5.int iFocus	  		:菜单条目焦点 <= 0表示所有项都无焦点
		6.int iDirectionEnable	:菜单条目焦点 <= 0表示所有项都无焦点
  		7.UINT32 iTopAlign		:表示菜单项开始的顶部边距
  		8.SECBACKFUNC  	  		:回调函数
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/
WNDMENU1 *CreateWndMenu1(int iNum, unsigned int size, unsigned int *uiLblIdx, 
						UINT16 iBackHome, int iFocus, int iDirectionEnable, 
						unsigned int iTopMargin, SECBACKFUNC CallBack)
{
	//错误标志定义
	int iErr = 0;
	unsigned int y = iTopMargin;
	WNDMENU1 *pWndMenu = NULL;

	if (!iErr)
	{
		//分配资源
		pWndMenu = (WNDMENU1 *)calloc(1, sizeof(WNDMENU1));
		if (NULL == pWndMenu)
		{
			LOG(LOG_ERROR, "Menu Malloc ERR\n");
			iErr = -2;
		}
	}

	if (!iErr)
	{
		//临时变量定义
		int i = 0;
		//初始化结构体变量
		pWndMenu->iItemCnt = iNum;
		pWndMenu->iFocus = iFocus;
		pWndMenu->CallBack = CallBack;
		pWndMenu->iBackHome = iBackHome;
		pWndMenu->iDirectionEnable = iDirectionEnable;

		int iLblCount = size / sizeof(unsigned int);

		//背景图片
		pWndMenu->pMenuBg = CreatePicture(521, 40, 119, 440, BmpFileDirectory"bg_global_menu_bg.bmp");
		//方向键图标
		if (pWndMenu->iDirectionEnable)
		{
			if ((GetCurrWindowType() == ENUM_OTHER_WIN) || (GetCurrWindowType() == ENUM_OPEN_OTDR_WIN)
                || (GetCurrWindowType() == ENUM_FAULT_WIN) || (GetCurrWindowType() == ENUM_FILE_WIN))
			{
				pWndMenu->pDirection = CreatePicture(521, 390, 119, 90, BmpFileDirectory "bg_saveFile_upDown_left.bmp");
			}
			else
			{
				pWndMenu->pDirection = CreatePicture(521, 390, 119, 90, BmpFileDirectory "bg_global_menu_default.bmp");
			}
		}
		for (i = 0; i < iNum; ++i)
		{
			//按钮控件
			pWndMenu->pBtnItem[i] = CreatePicture(520, y + 57 * i, 119, 55, BmpFileDirectory"bg_menu_item_unpress.bmp");
			pWndMenu->itemEnable[i] = 1;

			if (iLblCount > i)
			{
				pWndMenu->pStrItem[i] = GetCurrLanguageText(uiLblIdx[i]);
			}
			else
			{
				pWndMenu->pStrItem[i] = TransString("");
			}

			pWndMenu->pLblItem[i] = CreateLabel(521, y + 57 * i + ItemWordOffset, 119, 16,
				pWndMenu->pStrItem[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pWndMenu->pLblItem[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pWndMenu->pLblItem[i]);
		}

		for (i = iNum; i < MENU_MAX; i++) //无用控件
		{
			pWndMenu->pBtnItem[i] = CreatePicture(521, y + 55 * i, 119, 55, BmpFileDirectory"bg_menu_item_disable.bmp");
			pWndMenu->itemEnable[i] = 1;

			if (iLblCount > i)
			{
				pWndMenu->pStrItem[i] = GetCurrLanguageText(uiLblIdx[i]);
			}
			else
			{
				pWndMenu->pStrItem[i] = TransString("");
			}

			pWndMenu->pLblItem[i] = CreateLabel(521, y + 55 * i + ItemWordOffset, 119, 16,
												pWndMenu->pStrItem[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pWndMenu->pLblItem[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pWndMenu->pLblItem[i]);
		}

		if (iBackHome != 0xffff)
		{
			//创建返回主菜单按钮
			pWndMenu->pBtnHome = CreatePicture(241, y + 24 * i, 72, 24, BmpFileDirectory"bg_menu_home_unpress.bmp");
		}
	}

	setCurMenu(pWndMenu);
	
	sWndMenu = pWndMenu;
	//按键焦点在菜单栏
	isInSmallFileBrowse = 0;
	iSelectedMenuBg = 1;
    iDsiplayLbl = 0;

    return pWndMenu;
}

/***
  * 功能：
     	销毁菜单控件
  * 参数：
  		1.WNDMENU *pMenuObj : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DestroyWndMenu1(WNDMENU1 **pMenuObj)
{
	int i = 0;

	//参数检测
	if (NULL == *pMenuObj)
	{
		return -1;
	}

	//销毁背景
	DestroyPicture(&((*pMenuObj)->pMenuBg));
	//销毁方向键
	if ((*pMenuObj)->iDirectionEnable)
		DestroyPicture(&((*pMenuObj)->pDirection));
	//销毁按钮
	for (i = 0; i < MENU_MAX; ++i)
	{
		DestroyPicture(&((*pMenuObj)->pBtnItem[i]));
		DestroyLabel(&((*pMenuObj)->pLblItem[i]));
		free((*pMenuObj)->pStrItem[i]);
	}

	//销毁退出键
	if ((*pMenuObj)->pBtnHome)
	{
		DestroyPicture(&((*pMenuObj)->pBtnHome));
	}

	//释放空间
	free(*pMenuObj);
	*pMenuObj = NULL;
	
	return 0;
}

/***
  * 功能：
     	显示菜单控件
  * 参数：
  		1.WNDMENU *pMenuObj : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DisplayWndMenu1(WNDMENU1 *pMenuObj)
{
	int i = 0;
	
	if (NULL == pMenuObj)
	{
		return -1;
	}
	//设置是否选中背景图
	//iSelectedMenuBg ? SetPictureBitmap(BmpFileDirectory "bg_global_menu_bg.bmp", pMenuObj->pMenuBg)
					//: SetPictureBitmap(BmpFileDirectory "bg_global_menu_bg2.bmp", pMenuObj->pMenuBg);
	//目前不需要设置选中的背景
	SetPictureBitmap(BmpFileDirectory "bg_global_menu_bg.bmp", pMenuObj->pMenuBg);
	//显示背景
	DisplayPicture(pMenuObj->pMenuBg);
	//显示方向键
	if (pMenuObj->iDirectionEnable)
		DisplayPicture(pMenuObj->pDirection);
	//显示按钮
	//显示有效控件
	for (i = 0; i < pMenuObj->iItemCnt; ++i)
	{
        //刷新使能图标
        if (pMenuObj->itemEnable[i])
        {
            if (pMenuObj->pBtnItemUnpressBg[i] == NULL)
            {
                TouchChange("bg_menu_item_unpress.bmp", pMenuObj->pBtnItem[i],
                            NULL, NULL, 1);
            }
            else
            {
                TouchChange(pMenuObj->pBtnItemUnpressBg[i], pMenuObj->pBtnItem[i],
                            NULL, NULL, 1);
            }
            //显示选中项
            if (pMenuObj->iFocus == i)
            {
                if (pMenuObj->pBtnItemSelectBg[i] == NULL)
                {
                    TouchChange("bg_menu_item_select.bmp", pMenuObj->pBtnItem[i],
                                NULL, NULL, 1);
                }
                else
                {
                    TouchChange(pMenuObj->pBtnItemSelectBg[i], pMenuObj->pBtnItem[i],
                                NULL, NULL, 1);
                }
            }
        }

        //显示文本
        if (iDsiplayLbl)
		    DisplayLabel(pMenuObj->pLblItem[i]);
	}

	//显示退出键 Help
	if (pMenuObj->pBtnHome)
	{
		DisplayPicture(pMenuObj->pBtnHome);
	}

	return 0;
}

/***
  * 功能：
     	添加控件到接受消息的控件队列
  * 参数：
  		1.WNDMENU *pMenuObj : 指向menu控件
  		2.GUIWINDOW *pWnd   : 队列的所属窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有互斥锁的情况下调用
***/ 
int AddWndMenuToComp1(WNDMENU1 *pMenuObj, GUIWINDOW *pWnd)
{
	int i = 0;
	
	if ((NULL == pMenuObj) || (NULL == pWnd))
	{
		return -1;
	}

	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pWnd, 
	              pWnd);
	
	//按钮
	for (i = 0; i < MENU_MAX; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  	  pMenuObj->pBtnItem[i], pWnd);
	}
	
	//返回主菜单
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
		pMenuObj->pBtnHome, pWnd);

	return 0;
}
                     			
/***
  * 功能：
     	注册消息处理函数
  * 参数：
  		1.WNDMENU *pMenuObj : 指向menu控件
  		2.GUIMESSAGE *pMsg  : 当前的消息队列
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有消息注册队列互斥锁的情况下调用
***/ 
int LoginWndMenuToMsg1(WNDMENU1 *pMenuObj, GUIWINDOW *pWnd)
{
	int i = 0;
	GUIMESSAGE *pMsg;
	pMsg = GetCurrMessage();
	
	if ((NULL == pMenuObj) || (NULL == pMsg))
	{
		return -1;
	}

	//注册按钮消息处理
	for (i = 0; i < MENU_MAX; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMenuObj->pBtnItem[i],
                		WndMenuBtnItem_Down, pMenuObj, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pMenuObj->pBtnItem[i],
                		WndMenuBtnItem_Up, pMenuObj, i, pMsg);
	}

	//注册返回主菜单消息处理
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMenuObj->pBtnHome,
		WndMenuBtnHome_Down, pMenuObj, HOME_DOWN, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMenuObj->pBtnHome,
		WndMenuBtnHome_Up, pMenuObj, HOME_DOWN, pMsg);


	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pWnd, 
	                WndMenuKey_Down, pMenuObj, 0, GetCurrMessage());
	LoginMessageReg(GUIMESSAGE_KEY_UP, pWnd, 
	                WndMenuKey_Up, pMenuObj, 0, GetCurrMessage());

	OtdrKeyCount = 0;

	return 0;
}


/***
  * 功能：
     	设置控件使能状态
  * 参数：
  		1.int iEnable		: 使能标志
  		2.WNDMENU *pMenuObj : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetWndMenuEnable1(int iEnable, WNDMENU1 *pMenuObj)
{
	int i = 0;
	
	if (NULL == pMenuObj)
	{
		return -1;
	}
	
	//背景	
	SetPictureEnable(iEnable, pMenuObj->pMenuBg);

	//按钮
	for (i = 0; i < MENU_MAX; ++i)
	{
		SetPictureEnable(iEnable, pMenuObj->pBtnItem[i]);
		SetLabelEnable(iEnable, pMenuObj->pLblItem[i]);
	}

	//退出键
	if (pMenuObj->pBtnHome)
	{
		SetPictureEnable(iEnable, pMenuObj->pBtnHome);
	}

	return 0;
}
/***
  * 功能：
     	设置控件某一项使能状态
  * 参数：
  		1.int item			: 指定项(0-6)
  		2.int iEnable		: 使能标志
  		3.WNDMENU *pMenuObj : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetWndMenuItemEnble(int item, int enable, WNDMENU1 *pMenuObj)
{
	if(pMenuObj == NULL)
	{
		return -1;	
	}
	
	if(item >= pMenuObj->iItemCnt)
	{
		return -1;
	}

	if(enable)
	{
		if (item == pMenuObj->iFocus && iSelectedMenuBg)
		{
			if (pMenuObj->pBtnItemSelectBg[item] == NULL)
			{
				SetPictureBitmap(BmpFileDirectory"bg_menu_item_select.bmp", pMenuObj->pBtnItem[item]);
			}
			else
			{
				SetPictureBitmap(pMenuObj->pBtnItemSelectBg[item], pMenuObj->pBtnItem[item]);
			}	
		}
		else
		{
			if (pMenuObj->pBtnItemUnpressBg[item] == NULL)
			{
				SetPictureBitmap(BmpFileDirectory"bg_menu_item_unpress.bmp", pMenuObj->pBtnItem[item]);
			}
			else
			{
				SetPictureBitmap(pMenuObj->pBtnItemUnpressBg[item], pMenuObj->pBtnItem[item]);
			}
		}
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pMenuObj->pLblItem[item]);
	}
	else
	{
		if (pMenuObj->pBtnItemDisableBg[item] == NULL)
		{
			SetPictureBitmap(BmpFileDirectory"bg_menu_item_disable.bmp", pMenuObj->pBtnItem[item]);
		}
		else
		{
			SetPictureBitmap(pMenuObj->pBtnItemDisableBg[item], pMenuObj->pBtnItem[item]);
		}
		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pMenuObj->pLblItem[item]);
	}
	//使能不同时才设置
	if (enable)
	{
		SetPictureEnable(enable, pMenuObj->pBtnItem[item]);
		DisplayPicture(pMenuObj->pBtnItem[item]);
        //显示文本
        if (iDsiplayLbl)
		    DisplayLabel(pMenuObj->pLblItem[item]);
	}
	else
	{
		DisplayPicture(pMenuObj->pBtnItem[item]);
        //显示文本
        if (iDsiplayLbl)
            DisplayLabel(pMenuObj->pLblItem[item]);
        SetPictureEnable(enable, pMenuObj->pBtnItem[item]);
	}

	pMenuObj->itemEnable[item] = enable;

	return 0;
}
/***
  * 功能：
     	//设置控件某一项背景
  * 参数：
  		1.int item			: 指定项(0-6)
  		2.char *backGroundBmp: 背景资源
  		3.WNDMENU *pMenuObj : 指向menu控件
		4：int state :指定项不同状态 0：设置非选中的背景1 ：设置选中的背景
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetWndMenuItemBg(int item, char *backGroundBmp, WNDMENU1 *pMenuObj,int state)  
{
	if(pMenuObj == NULL)
	{
		return -1;	
	}
	
	if(item >= pMenuObj->iItemCnt)
	{
		return -1;
	}

    if (NULL == backGroundBmp)
    {
            return -1;
    }

	if (state == MENU_UNPRESS)//加载非选中bg资源
	{
		pMenuObj->pBtnItemUnpressBg[item] = backGroundBmp;
	}

	if(state == MENU_SELECT)//加载选中bg资源
	{
		pMenuObj->pBtnItemSelectBg[item] = backGroundBmp;
	}

	if (state == MENU_DISABLE) //加载不使能bg资源
	{
		pMenuObj->pBtnItemDisableBg[item] = backGroundBmp;
	}

	return 0;

}
/***
  * 功能：
     	//设置每一项的文本的偏移
  * 参数：
  		1.int offse:项文本偏移量
  * 返回：
        成功返回无
  * 备注：
***/ 
void SetItemWordOffset(int offset)
{
	ItemWordOffset = offset;
}
/***
  * 功能：
     	//得到每一项的文本的偏移
  * 参数：无
  * 返回：
        成功返回int offse:项文本偏移量
  * 备注：
***/ 
int GetItemWordOffset(void)
{
	return ItemWordOffset;
}

/***
  * 功能：
     	设置控件某一项Label颜色
  * 参数：
  		1.int item			: 指定项(0-5)
  		2.GUIFONT *pLblFont	: 字体颜色
  		3.WNDMENU *pMenuObj : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetWndMenuItemFont(int item, GUIFONT *pLblFont, WNDMENU1 *pMenuObj)
{
	if(pMenuObj == NULL && pLblFont == NULL)
	{
		return -1;	
	}
	
	if(item >= pMenuObj->iItemCnt || !pMenuObj->pBtnItem[item]->Visible.iEnable)
	{
		return -1;
	}

	SetLabelFont(pLblFont, pMenuObj->pLblItem[item]);
	DisplayPicture(pMenuObj->pBtnItem[item]);
    //显示文本
    if (iDsiplayLbl)
        DisplayLabel(pMenuObj->pLblItem[item]);

    return 0;
}
/***
  * 功能：
     	设置控件的图层
  * 参数：
  		1.int iLayer		: 图层 (必须 > 0)
  		2.WNDMENU *pMenuObj : 指向menu控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetWndMenuLayer1(int iLayer, WNDMENU1 *pMenuObj)
{
	int i = 0;
	
	if (NULL == pMenuObj)
	{
		return -1;
	}
	
	iLayer *= 2;
	//背景	
	pMenuObj->pMenuBg->Visible.iLayer = iLayer;

	//按钮
	for (i = 0; i < MENU_MAX; ++i)
	{
		pMenuObj->pBtnItem[i]->Visible.iLayer = iLayer + 1;
	}

	//退出键
	if (pMenuObj->pBtnHome)
	{
		pMenuObj->pBtnHome->Visible.iLayer = iLayer + 1;
	}
	
	return 0;
}

//按钮按下处理
static int WndMenuBtnItem_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen)
{
	int iSelected = iOutLen;
	WNDMENU1 *pMenuObj = (WNDMENU1 *)pOutArg;
	setCurItemPush(iSelected);

	if(iSelected < pMenuObj->iItemCnt)
	{
		if(iSelected != pMenuObj->iFocus && pMenuObj->itemEnable[iSelected])
		{	
			TouchChange("bg_menu_item_press.bmp", pMenuObj->pBtnItem[iSelected],
                NULL, NULL, 1);
		}

		RefreshScreen(__FILE__, __func__, __LINE__);
	}

	if(pMenuObj->itemEnable[iSelected])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//按钮弹起处理
static int WndMenuBtnItem_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen)
{
	int iSelected = iOutLen;
	WNDMENU1 *pMenuObj = (WNDMENU1 *)pOutArg;
	setCurItemPush(-1);
	DisableAllWidgets(GetCurrWindow());

	if (iSelected < pMenuObj->iItemCnt)
	{
		if (pMenuObj->CallBack && pMenuObj->itemEnable[iSelected])
		{
			TouchChange("bg_menu_item_press.bmp", pMenuObj->pBtnItem[iSelected],
            NULL, NULL, 1);

            (*(pMenuObj->CallBack))(iSelected);
            //针对fip界面刷新
            if (enCurWindow == ENUM_FAULT_WIN)
                FlushMenu1(pMenuObj, 2, 1);
            RefreshScreen(__FILE__, __func__, __LINE__);
        }
	}

	EnableAllWidgets(GetCurrWindow());

	return 0;
}

//Home 按下处理
static int WndMenuBtnHome_Down(void *pInArg, int iInLen, 
                     		   void *pOutArg, int iOutLen)
{
	WNDMENU1 *pMenuObj = (WNDMENU1 *)pOutArg;
	setCurItemPush(iOutLen);
	TouchChange("bg_menu_home_press.bmp", pMenuObj->pBtnHome, NULL, NULL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

//Home 弹起处理
static int WndMenuBtnHome_Up(void *pInArg, int iInLen, 
                     		 void *pOutArg, int iOutLen)
{
	WNDMENU1 *pMenuObj = (WNDMENU1 *)pOutArg;
	setCurItemPush(-1);
	DisableAllWidgets(GetCurrWindow());
	TouchChange("bg_menu_home_unpress.bmp", pMenuObj->pBtnHome, NULL, NULL, 0);

	if (pMenuObj->CallBack)
	{
		(*(pMenuObj->CallBack))(HOME_DOWN);
	}

    //针对文件管理器界面回退到顶层目录
    FlushMenu1(pMenuObj, 2, 1);
    // RefreshScreen(__FILE__, __func__, __LINE__);

	EnableAllWidgets(GetCurrWindow());

	return 0;
}

//按键相应函数
static int WndMenuKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	//LOG(LOG_ERROR, "WndMenuKey_Down pInArg = %d\n", (unsigned int)pInArg);
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	{
    	switch (uiValue)
	    {
		case KEYCODE_UP:
			LOG(LOG_INFO, "---up------\n");
			break;
		case KEYCODE_DOWN:
			LOG(LOG_INFO, "---down------\n");
			break;
		case KEYCODE_ESC:
			//WndMenuBtnHome_Down(NULL, 0, NULL, 0);
			break;
		default:
			break;
    	}
    }

	return iReturn;
}
static int WndMenuKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
	int iReturn = 0;
	
	unsigned int uiValue;
	WNDMENU1 *pMenuObj = (WNDMENU1 *)pOutArg;

    uiValue = (unsigned int)pInArg;
	//退出按键是公共按键
	if (uiValue == KEYCODE_ESC)
		WndMenuBtnHome_Up(NULL, 0, pMenuObj, pMenuObj->iFocus);
	//快捷键vfl
	if ((uiValue == KEYCODE_VFL) && (enCurWindow != ENUM_OTDR_WIN))
		JumpVflWindow();

	if (isInSmallFileBrowse)
	{
		KeyBoardCallBack1(uiValue);
		if (!isInSmallFileBrowse)
			KeyRightCallBack();
	}
	else
	{
		switch (uiValue)
		{
			case KEYCODE_OTDR_LEFT:
			{
				if (enCurWindow == ENUM_SERIALNUM_WIN)
				{
					//授权界面
					CreateAuthorizationWindow();
				}
				else if (enCurWindow == ENUM_SAVE_OTDR_WIN)
				{
					//打开文件
					CreateOpenFileDialog();
				}
			}
			break;
			case KEYCODE_OTDR_RIGHT:
			{
				if (enCurWindow == ENUM_AUTHORIZATION_WIN)
				{
					//序列号界面
					CreateSerialNumWindow();
				}
				else if (enCurWindow == ENUM_OPEN_OTDR_WIN)
				{
					//保存文件
					CreateSaveFileDialog(NULL);
				}
			}
			break;
			case KEYCODE_UP:
				KeyUpOrDownCallBack(0);
			break;
			case KEYCODE_DOWN:
				KeyUpOrDownCallBack(1);
			break;
			case KEYCODE_LEFT:
				KeyLeftCallBack();
			break;
			case KEYCODE_RIGHT:
				KeyRightCallBack();
			break;
			case KEYCODE_ENTER:
				KeyEnterCallBack();
			break;
			default:
			break;
		}
	}

	return iReturn;
}

/***
  * 功能：
     	刷新menu
  * 参数：
  		1.WNDMENU1 *pMenuObj: MENU控件
  		2.int iFocus        : 设置当前的选中项
  		3.int iNum			: 控件中Item数，包含标题栏
  		4.UINT16 iBackHome  : back和home键是否可用
								高8位为0x00为back不可用，其他为可用
								低8位为0x00为home不可用，其他为可用
  		5.unsigned int len:文本长度
  		6.unsigned *uiLblIdx: 改变标签的内容
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
  		当iNum = 0时: 则表明只是刷新menu控件
  		当iNum > 0时: 则表明有可能更改控件可用Item的数量
  				当uiLblIdx = NULL 时: 则表明不需更改Item的文本内容
  				当uiLblIdx != NULL时: 则表明可能更改Item的文本内容
***/ 

void RefreshMenu(WNDMENU1 *pMenuObj, int iFocus, int iNum, 
				UINT16 iBackHome, unsigned int size,unsigned int *uiLblIdx)
{
	int i;
	int iLblCount = size/sizeof(unsigned int);
	
	pMenuObj->iFocus = iFocus;
	pMenuObj->iBackHome = iBackHome;

	if(iNum >= 0)
	{
		pMenuObj->iItemCnt = iNum;
		
		for(i = 0; i < pMenuObj->iItemCnt; i++)
		{
			if(pMenuObj->itemEnable[i])
			{
				SetPictureBitmap(BmpFileDirectory"bg_menu_item_unpress.bmp", pMenuObj->pBtnItem[i]);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"bg_menu_item_disable.bmp", pMenuObj->pBtnItem[i]);
			}

            SetPictureEnable(1, pMenuObj->pBtnItem[i]);

			if(uiLblIdx != NULL)
			{
				GuiMemFree(pMenuObj->pStrItem[i]);
				if(iLblCount > i)
				{
					pMenuObj->pStrItem[i] = GetCurrLanguageText(uiLblIdx[i]);
				}
				else
				{
					pMenuObj->pStrItem[i] = TransString("");
				}
				SetLabelText(pMenuObj->pStrItem[i], pMenuObj->pLblItem[i]);
                SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pMenuObj->pLblItem[i]);
			}
		}
		for (i = pMenuObj->iItemCnt; i < MENU_MAX; i++)
		{
			SetPictureBitmap(BmpFileDirectory "bg_menu_item_disable.bmp", pMenuObj->pBtnItem[i]);
			SetPictureEnable(1, pMenuObj->pBtnItem[i]);
			if (uiLblIdx != NULL)
			{
				GuiMemFree(pMenuObj->pStrItem[i]);
				if (iLblCount > i)
				{
					pMenuObj->pStrItem[i] = GetCurrLanguageText(uiLblIdx[i]);
				}
				else
				{
					pMenuObj->pStrItem[i] = TransString("");
				}
				SetLabelText(pMenuObj->pStrItem[i], pMenuObj->pLblItem[i]);
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pMenuObj->pLblItem[i]);
			}
		}
	}
	
	if (pMenuObj->iBackHome & 0x00ff) 
	{
		SetPictureBitmap(BmpFileDirectory"bg_menu_home_unpress.bmp", 
							pMenuObj->pBtnHome);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"bg_menu_home_disable.bmp", 
							pMenuObj->pBtnHome);
	}
	  
    SetPictureEnable(1, pMenuObj->pBtnHome);

	DisplayWndMenu1(pMenuObj);
    
}

/***
  * 功能：
     	刷新menu
  * 参数：
  		1.WNDMENU1 *pMenuObj: MENU控件
  		2.int iFocus        : 设置当前的选中项
  		3.int iNum			: 控件中Item数，包含标题栏
  		4.UINT16 iBackHome  : back和home键是否可用
								高8位为0x00为back不可用，其他为可用
								低8位为0x00为home不可用，其他为可用
  		5.unsigned int len:文本长度
  		6.unsigned *uiLblIdx: 改变标签的内容
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
  		当iNum = 0时: 则表明只是刷新menu控件
  		当iNum > 0时: 则表明有可能更改控件可用Item的数量
  				当uiLblIdx = NULL 时: 则表明不需更改Item的文本内容
  				当uiLblIdx != NULL时: 则表明可能更改Item的文本内容
  				
  	    使能不可用的键不显示
***/ 

void RefreshMenu1(WNDMENU1 *pMenuObj, int iFocus, int iNum, 
				UINT16 iBackHome, unsigned int size,unsigned int *uiLblIdx, int iEnabled)
{
	int i;
	int iLblCount = size/sizeof(unsigned int);
	
	pMenuObj->iFocus = iFocus;
	pMenuObj->iBackHome = iBackHome;

	if (pMenuObj == NULL)
	{
		LOG(LOG_ERROR, "pMenuObj == NULL");
	}

	if(iNum >= 0)
	{
		pMenuObj->iItemCnt = iNum;

		for(i = 0; i < pMenuObj->iItemCnt; i++)
		{
			if(pMenuObj->itemEnable[i])
			{
				SetPictureBitmap(BmpFileDirectory"bg_menu_item_unpress.bmp", pMenuObj->pBtnItem[i]);
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"bg_menu_item_disable.bmp", pMenuObj->pBtnItem[i]);
			}

			if(uiLblIdx != NULL)
			{
				GuiMemFree(pMenuObj->pStrItem[i]);

				if(iLblCount > i)
				{
					pMenuObj->pStrItem[i] = GetCurrLanguageText(uiLblIdx[i]);
				}
				else
				{
					pMenuObj->pStrItem[i] = TransString("");
				}

				SetLabelText(pMenuObj->pStrItem[i], pMenuObj->pLblItem[i]);
                SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pMenuObj->pLblItem[i]);
			}
		}

		for(i = pMenuObj->iItemCnt; i < MENU_MAX; i++)
		{
			SetPictureEnable(0, pMenuObj->pBtnItem[i]);
            GuiMemFree(pMenuObj->pStrItem[i]);
			pMenuObj->pStrItem[i] = TransString(" ");
            SetLabelText(pMenuObj->pStrItem[i], pMenuObj->pLblItem[i]);
		}
	}

	if (pMenuObj->iBackHome & 0x00ff) 
	{
		SetPictureBitmap(BmpFileDirectory"bg_menu_home_unpress.bmp", 
							pMenuObj->pBtnHome);
	}
	else
	{
	    if(iEnabled)
        {
            SetPictureBitmap(BmpFileDirectory"bg_menu_home_disable.bmp", 
							pMenuObj->pBtnHome);
        } 
        else
        {
            SetPictureEnable(0,pMenuObj->pBtnHome);
        }
	}

	DisplayWndMenu1(pMenuObj);
}

//刷新菜单栏背景
int RefreshMenuBackground(WNDMENU1 *pMenuObj)
{
	if (NULL == pMenuObj)
	{
		return -1;
	}
	//设置是否选中背景图
	//iSelectedMenuBg ? SetPictureBitmap(BmpFileDirectory "bg_global_menu_bg.bmp", pMenuObj->pMenuBg)
					//: SetPictureBitmap(BmpFileDirectory "bg_global_menu_bg2.bmp", pMenuObj->pMenuBg);
	//目前不需要设置选中的背景
	SetPictureBitmap(BmpFileDirectory "bg_global_menu_bg.bmp", pMenuObj->pMenuBg);
	//显示背景
	DisplayPicture(pMenuObj->pMenuBg);
	//显示方向键
	if (pMenuObj->iDirectionEnable)
		DisplayPicture(pMenuObj->pDirection);

	return 0;
}

//获得当前的menu
WNDMENU1 *getCurMenu(void)
{
	return pCurMenu;
}

//设置当前的menu
void setCurMenu(WNDMENU1 *pOpt)
{
	pCurMenu = pOpt;
}

int getCurItemPush(void)
{
	return iCurMenuItemPush;
}

void setCurItemPush(int iOpt)
{
	iCurMenuItemPush = iOpt;
}

//获取是否选中菜单栏
int getSelectedMenu(void)
{
	return iSelectedMenuBg;
}

//获取按键按下之后焦点位置(上下循环可按下)
static int GetCurFocus(WNDMENU1 *pMenuObj, int option)
{
    int iFocus = pMenuObj->iFocus;

    switch (option)
	{
		case 0://UP
			if (iFocus != 0)
			{
				iFocus -= 1;
			}
            else
            {
                iFocus = pMenuObj->iItemCnt - 1;
            }
			break;
		case 1://DOWN
			if (iFocus != (pMenuObj->iItemCnt-1))
			{
				iFocus += 1;
			}
            else
            {
                iFocus = 0;
            }
		default:
			break;
	}

    return iFocus;
}

//获取可用的使能的按钮
static int GetEnabledMenu(WNDMENU1 *pMenuObj, int iCurrentIndex, int iOption)
{
    //检查参数
    if (pMenuObj == NULL)
        return -1;
    int item = 0;
    int i;
    //确定循环起始/末尾位置
    if (iOption == 0 || (iOption == 2))//up/esc
    {
        for (i = iCurrentIndex; i >= 0; i--)
        {
            if (pMenuObj->itemEnable[i])
            {
                item = i;
                break;
            }
        }
    }
    else//down
    {
        for (i = iCurrentIndex; i < pMenuObj->iItemCnt; i++)
        {
            if (pMenuObj->itemEnable[i])
            {
                item = i;
                break;
            }
        }
    }

    return item;
}

//根据物理按键添加
static void FlushMenu1(WNDMENU1 *pMenuObj, int option, int iRefreshFocus)
{
	int i;
	for (i = 0; i < pMenuObj->iItemCnt; ++i)
	{
		if (pMenuObj->itemEnable[i])
		{
			SetPictureEnable(1, pMenuObj->pBtnItem[i]);
			if (pMenuObj->pBtnItemUnpressBg[i] == NULL)
			{
				TouchChange("bg_menu_item_unpress.bmp", pMenuObj->pBtnItem[i],
					NULL, NULL, 1);
			}
			else
			{
				TouchChange(pMenuObj->pBtnItemUnpressBg[i], pMenuObj->pBtnItem[i],
							NULL, NULL, 1);
			}
			pMenuObj->itemEnable[i] = 1;
		}
		else
		{
			if (pMenuObj->pBtnItemDisableBg[i] == NULL)
			{
				TouchChange("bg_menu_item_disable.bmp", pMenuObj->pBtnItem[i],
							NULL, NULL, 4);
			}
			else
			{
				TouchChange(pMenuObj->pBtnItemDisableBg[i], pMenuObj->pBtnItem[i],
							NULL, NULL, 4);
			}
			SetPictureEnable(0, pMenuObj->pBtnItem[i]);
			pMenuObj->itemEnable[i] = 0;
		}
        //显示文本
        if (iDsiplayLbl)
            DisplayLabel(pMenuObj->pLblItem[i]);
    }

	int focus = GetCurFocus(pMenuObj, option);
    int item = 0;   //记录选中项

	if (iRefreshFocus)//刷新选中项
	{
		if (pMenuObj->itemEnable[focus])
		{
			pMenuObj->itemEnable[focus] = 1;
            pMenuObj->iFocus = focus;
            item = focus;
		}
        else
        {
            item = GetEnabledMenu(pMenuObj, focus, option);
            if (item >= 0)
                pMenuObj->iFocus = item;
        }

        //判断范围内
        if (item >= 0 && item < pMenuObj->iItemCnt)
        {
            SetPictureEnable(1, pMenuObj->pBtnItem[item]);
            if (pMenuObj->pBtnItemSelectBg[item] == NULL)
            {
                TouchChange("bg_menu_item_select.bmp", pMenuObj->pBtnItem[item],
                            NULL, NULL, 1);
            }
            else
            {
                TouchChange(pMenuObj->pBtnItemSelectBg[item], pMenuObj->pBtnItem[item],
                            NULL, NULL, 1);
            }
        }
        //显示文本
        if (iDsiplayLbl)
            DisplayLabel(pMenuObj->pLblItem[focus]);
    }

    //判断是否在左侧文件列表
    if (!isInSmallFileBrowse)
	    RefreshScreen(__FILE__, __func__, __LINE__);
}

//工厂菜单的处理函数
static void HandleFactoryMenu(int iSelected, int iOption)
{
	switch (iSelected)
	{
	case DEBUG_INFO://debug info
		DebugInfoKeyCallBack(iOption);
		break;
	case LANGUAGE_SET://language configure
		LanguageKeyCallBack(iOption);
		break;
	case NON_LINEARITY://non-linearity
		LinearityKeyBackCall(iOption);
		break;
	case UNCERTAINTY://uncertainty
		UncertaintyKeyCallBack(iOption);
		break;
	case FACTORY_SET://factory set
		FactorySetKeyBackCall(iOption);
		break;
	case AUTHORIZATION://Authorization
		AuthorizationKeyBackCall(iOption);
		break;
	default:
		break;
	}
}

//按键down处理函数
static void KeyUpOrDownCallBack(int iOption)
{
	if ((enCurWindow == ENUM_SAVE_OTDR_WIN)
		&& iSaveFileSmallWndFlag)
	{
		SaveFileSetKeyFocusPos(iOption);
	}
	else
	{
		int i;
		int count = 0;
		for (i = 0; i < FACTORY_MENU_COUNT; ++i)
		{
			if ((enCurWindow == iFactorySmallWnd[i])
				&& iFactorySmallWndFlag[i])
			{
				count++;
				//处理工厂菜单的按键
				HandleFactoryMenu(i, iOption);
				break;
			}
		}

		if (!count)
		{
			FlushMenu1(sWndMenu, iOption, 1);
		}
	}
}
//左键响应处理函数
static void KeyLeftCallBack(void)
{
	if (enCurWindow == ENUM_SAVE_OTDR_WIN)
	{
		iSelectedMenuBg = 0;
		iSaveFileSmallWndFlag = 1;
		//设置方向键
		if (sWndMenu->iDirectionEnable)
			SetPictureBitmap(BmpFileDirectory "bg_saveFile_upDown_right.bmp", sWndMenu->pDirection);
        // SaveFileSetLeftBackground(1);
        //显示背景
        RefreshMenuBackground(sWndMenu);
        RefreshSelectedFile();
		FlushMenu1(sWndMenu, 2, 0);
	}
	else
	{
		if ((enCurWindow >= ENUM_DEBUG_INFO_WIN
			&& enCurWindow <= ENUM_SERIALNUM_WIN))
		{
			isInSmallFileBrowse = 1;
			iSelectedMenuBg = 1;
			SetFactorySmallWndFlag(1);
		}
		//文件管理操作
		if ((enCurWindow == ENUM_OPEN_OTDR_WIN) || (enCurWindow == ENUM_FAULT_WIN) 
			|| (enCurWindow == ENUM_FILE_WIN))
		{
			isInSmallFileBrowse = 1;
			iSelectedMenuBg = 0;
			//设置方向键
			if (sWndMenu->iDirectionEnable)
				SetPictureBitmap(BmpFileDirectory "bg_saveFile_upDown_right.bmp", sWndMenu->pDirection);
            //显示背景(文件管理器中循环刷新背景，故不需要再次刷新)
            if (enCurWindow != ENUM_FILE_WIN)
                RefreshMenuBackground(sWndMenu);
            RefreshFileBrowseSelectedFile();
			FlushMenu1(sWndMenu, 2, 0);
		}
		else
		{
			iSelectedMenuBg = 1;
		}
	}

	//文件管理器界面有不使能按钮，会导致界面按钮消失
	if ((enCurWindow != ENUM_FILE_WIN) && (enCurWindow != ENUM_SAVE_OTDR_WIN) 
			&& (enCurWindow != ENUM_OPEN_OTDR_WIN) && (enCurWindow != ENUM_FAULT_WIN))
		DisplayWndMenu1(sWndMenu);

	SyncCurrFbmap();
}

//右键响应处理函数
static void KeyRightCallBack(void)
{
	if (enCurWindow == ENUM_SAVE_OTDR_WIN)
	{
		iSaveFileSmallWndFlag = 0;
		iSelectedMenuBg = 1;
		//设置方向键
		if (sWndMenu->iDirectionEnable)
			SetPictureBitmap(BmpFileDirectory "bg_saveFile_upDown_left.bmp", sWndMenu->pDirection);
        // SaveFileSetLeftBackground(0);
        //显示背景
        RefreshMenuBackground(sWndMenu);
        RefreshSelectedFile();
		FlushMenu1(sWndMenu, 2, 1);
	}
	else
	{
		isInSmallFileBrowse = 0;
		iSelectedMenuBg = 1;
		SetFactorySmallWndFlag(0);
		//文件管理操作
		if ((enCurWindow == ENUM_OPEN_OTDR_WIN) || (enCurWindow == ENUM_FAULT_WIN) 
		|| (enCurWindow == ENUM_FILE_WIN))
		{
			//设置方向键
			if (sWndMenu->iDirectionEnable)
				SetPictureBitmap(BmpFileDirectory "bg_saveFile_upDown_left.bmp", sWndMenu->pDirection);
            //显示背景(文件管理器中循环刷新背景，故不需要再次刷新)
            if (enCurWindow != ENUM_FILE_WIN) 
                RefreshMenuBackground(sWndMenu);
            RefreshFileBrowseSelectedFile();
			FlushMenu1(sWndMenu, 2, 1);
		}
	}

	//文件管理器界面有不使能按钮，会导致界面按钮消失
	if ((enCurWindow != ENUM_FILE_WIN) && (enCurWindow != ENUM_SAVE_OTDR_WIN) 
		&& (enCurWindow != ENUM_OPEN_OTDR_WIN) && (enCurWindow != ENUM_FAULT_WIN))
		DisplayWndMenu1(sWndMenu);

	SyncCurrFbmap();
}

//设置工厂菜单小窗口的使能标志位
static void SetFactorySmallWndFlag(int iFlag)
{
	int i;
	for (i = 0; i < FACTORY_MENU_COUNT; ++i)
	{
		if (enCurWindow == iFactorySmallWnd[i])
		{
			iFactorySmallWndFlag[i] = iFlag;
			isInSmallFileBrowse = 0;
			break;
		}
	}
}

//enter键响应处理函数
static void KeyEnterCallBack(void)
{
	int i;
	int count = 0;

	for (i = 0; i < FACTORY_MENU_COUNT; ++i)
	{
		if (iFactorySmallWnd[i] == enCurWindow
			&& iFactorySmallWndFlag[i])
		{
			count++;
			HandleFactoryMenu(i, 2);
			break;
		}
	}

	if (!count)
	{
		if (iSelectedMenuBg)//只有光标在右侧时才能执行响应
			WndMenuBtnItem_Up(NULL, 0, sWndMenu, sWndMenu->iFocus);
	}
}