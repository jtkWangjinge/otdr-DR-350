/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmselector.c  
* 摘    要：  实现选择器控件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2014-10-23
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmselector.h"


/*************************************
* 定义 wnd_frmselector.c中内部使用的宏
**************************************/
#define SEC_BTN_WIDTH	20	//选择控件高度
#define SEC_BTN_HEIGHT	15	//选择器控件宽度
#define SEC_MID_WIDTH	53	//中间按键宽度
#define SEC_MID_HEIGHT	19	//中间按键高度

/***************************************
*声明wnd_frmselector.c中内部使用的变量
****************************************/
static SELECTOR *pCurrSelector = NULL;//当前的选择器


/***************************************
*声明wnd_frmselector.c中内部使用的函数
****************************************/
//更新元素显示函数
static void UpdateItemDisp(SELECTOR * pSampleTimeSelector);

//选择器左选择按钮点击处理函数
static int SelectorBtnLeft_Down(void *pInArg,   int iInLen, 
                                void *pOutArg, int iOutLen);
static int SelectorBtnLeft_Up(void *pInArg,   int iInLen, 
                              void *pOutArg, int iOutLen);
static int SelectorBtn1Left_Up(void *pInArg,   int iInLen, 
                              void *pOutArg, int iOutLen);
//选择器右选择按钮点击处理函数
static int SelectorBtnMidd_Down(void *pInArg,   int iInLen, 
                                void *pOutArg, int iOutLen);
static int SelectorBtnMidd_Up(void *pInArg,   int iInLen, 
                              void *pOutArg, int iOutLen);
//选择器元素点击处理函数
static int SelectorBtnRight_Down(void *pInArg,   int iInLen, 
                                 void *pOutArg, int iOutLen);
static int SelectorBtnRight_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int SelectorBtn1Right_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);


/***
  * 功能：
     	创建一个选择器
  * 参数：
  		1.UINT16 uiPosiX  		:选择器放置的位置x坐标
		2.UINT16 uiPosiY  		:选择器防止位置的y坐标
		3.UINT16 uiItemCnt		:选择器中元素的数量
		4.UINT16 uiCurrSelected :选择器中当前选中的元素
		5.char   **pItemStr 	:选择器中各个元素文本
		6.SECBACKFUNC BackCall 	:选择器选择上下选择按钮点击的回调函数
		7.UINT8  uiItemTouchFlg :是否允许选择器元素点击事件
  * 返回：
        成功返回选择器有效指针，失败NULL
  * 备注：
***/ 
SELECTOR *CreateSelector(UINT16 uiPosiX, UINT16 uiPosiY, UINT16 uiItemCnt,
						 UINT16 uiCurrSelected, GUICHAR **pItemStr, 
						 SECBACKFUNC BackCall, UINT8 uiItemTouchFlg)
{
	int iErr = 0;
	SELECTOR *pSelector = NULL;
	int i = 0;
	
	//参数合法性检查
	if ((NULL == pItemStr) || (uiItemCnt < 1) || 
		(uiItemCnt > MAX_ITEM_NUM) || 
		(uiCurrSelected > (uiItemCnt - 1)))
	{
		iErr = -1;
	}

	if (!iErr)
	{
		//为选择器对象分配内存空间
		pSelector = (SELECTOR *)calloc(1, sizeof(SELECTOR));
		if (NULL == pSelector)
		{
			iErr = -2;
		}	
	}
	if (!iErr)
	{
		//初始化选择器对象
		pSelector->uiPosiX = uiPosiX;
		pSelector->uiPosiY = uiPosiY;
		pSelector->uiItemCnt = uiItemCnt;
		pSelector->uiCurrSelected = uiCurrSelected;
		pSelector->BackCallFunc = BackCall;
		pSelector->uiItemTouchFlg = 0;//目前不能点击
		pSelector->uiEnable = 1;//默认使能
		for (i = 0; i < uiItemCnt; ++i)
		{
			pSelector->pItemString[i] = pItemStr[i];
		}

        //创建相应控件
		pSelector->pSelectorBtnLeft = CreatePicture(uiPosiX, uiPosiY+2, SEC_BTN_WIDTH, SEC_BTN_HEIGHT, 
													SEC_DN_UNPRESS);
		pSelector->pSelectorBtnMidd = CreatePicture(uiPosiX + SEC_BTN_WIDTH + 1 , uiPosiY, SEC_MID_WIDTH, 
													SEC_MID_HEIGHT, SEC_MD_UNPRESS);
		pSelector->pSelectorBtnRight = CreatePicture(uiPosiX + SEC_BTN_WIDTH + 2 + SEC_MID_WIDTH, uiPosiY+2, 
													 SEC_BTN_WIDTH, SEC_BTN_HEIGHT, SEC_UP_UNPRESS);
		pSelector->pSelectorLblItem = CreateLabel(uiPosiX + SEC_BTN_WIDTH + 1, uiPosiY+1, 
												  SEC_MID_WIDTH, 16,
												  pSelector->pItemString[uiCurrSelected]);
		//设置控件参数
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pSelector->pSelectorLblItem);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSelector->pSelectorLblItem);
	}

	return pSelector;
}

SELECTOR *CreateSelector1(UINT16 uiPosiX, UINT16 uiPosiY, UINT16 uiItemCnt,
						 UINT16 uiCurrSelected, GUICHAR **pItemStr, 
						 SECBACKFUNC1 BackCall,UINT8 uiItemTouchFlg)
{
	int iErr = 0;
	SELECTOR *pSelector = NULL;
	int i = 0;
	
	//参数合法性检查
	if ((NULL == pItemStr) || (uiItemCnt < 1) || 
		(uiItemCnt > MAX_ITEM_NUM) || 
		(uiCurrSelected > (uiItemCnt - 1)))
	{
		iErr = -1;
	}

	if (!iErr)
	{
		//为选择器对象分配内存空间
		pSelector = (SELECTOR *)calloc(1, sizeof(SELECTOR));
		if (NULL == pSelector)
		{
			iErr = -2;
		}	
	}
	if (!iErr)
	{
		//初始化选择器对象
		pSelector->uiPosiX = uiPosiX;
		pSelector->uiPosiY = uiPosiY;
		pSelector->uiItemCnt = uiItemCnt;
		pSelector->uiCurrSelected = uiCurrSelected;
		pSelector->BackCallFunc1 = BackCall;
		pSelector->uiItemTouchFlg = 0;//目前不能点击
		pSelector->uiEnable = 1;//默认使能
		for (i = 0; i < uiItemCnt; ++i)
		{
			pSelector->pItemString[i] = pItemStr[i];
		}

        //创建相应控件
		pSelector->pSelectorBtnLeft = CreatePicture(uiPosiX, uiPosiY, SEC_BTN_WIDTH, SEC_BTN_HEIGHT, 
													SEC_DN_UNPRESS);
		pSelector->pSelectorBtnMidd = CreatePicture(uiPosiX + SEC_BTN_WIDTH + 1 , uiPosiY, SEC_MID_WIDTH, 
													SEC_MID_HEIGHT, SEC_MD_UNPRESS);
		pSelector->pSelectorBtnRight = CreatePicture(uiPosiX + SEC_BTN_WIDTH + 2 + SEC_MID_WIDTH, uiPosiY, 
													 SEC_BTN_WIDTH, SEC_BTN_HEIGHT, SEC_UP_UNPRESS);
		pSelector->pSelectorLblItem = CreateLabel(uiPosiX + SEC_BTN_WIDTH + 1, uiPosiY+ (SEC_MID_HEIGHT >> 1) - 8, 
												  SEC_MID_WIDTH, 24,     
												  pSelector->pItemString[uiCurrSelected]);
		//设置控件参数
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pSelector->pSelectorLblItem);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSelector->pSelectorLblItem);
	}

	return pSelector;
}

/***
  * 功能：
     	销毁一个选择器
  * 参数：
  		1.SELECTOR **pSelector:指向要销毁的选择器控件指针
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DestroySelector(SELECTOR **pSelector)
{	
	if (NULL == pSelector)
	{
		return -1;
	}

    //销毁控件
	DestroyPicture(&((*pSelector)->pSelectorBtnLeft));
	DestroyPicture(&((*pSelector)->pSelectorBtnMidd));
	DestroyPicture(&((*pSelector)->pSelectorBtnRight));
	DestroyLabel(&((*pSelector)->pSelectorLblItem));

	free(*pSelector);
	*pSelector = NULL;
	
	return 0;
}

/***
  * 功能：
     	绘制一个选择器
  * 参数：
  		1.SELECTOR **pSelector:指向要选择器控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DisplaySelector(SELECTOR *pSelector)
{
	if (NULL == pSelector)
	{
		return -1;
	}

	DisplayPicture(pSelector->pSelectorBtnLeft);
	DisplayPicture(pSelector->pSelectorBtnMidd);
	DisplayPicture(pSelector->pSelectorBtnRight);
	DisplayLabel(pSelector->pSelectorLblItem);

	return 0;
}

/***
  * 功能：
     	注册选择器到窗体的控件队列
  * 参数：
  		1.SELECTOR **pSelector:指向要选择器控件
  		2.GUIWINDOW *pWnd:调用选择器的窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
  		和注册picture一样 必须在持有互斥锁的情况下调用
***/ 
int AddSelectorToComp(SELECTOR *pSelector, GUIWINDOW *pWnd)
{
	if ((NULL == pSelector) || (NULL == pWnd))
	{
		return -1;
	}
	
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  pSelector->pSelectorBtnLeft, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  pSelector->pSelectorBtnMidd, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  pSelector->pSelectorBtnRight, pWnd);
	
	return 0;
}

/***
  * 功能：
     	注册选择器到消息队列
  * 参数：
  		1.SELECTOR **pSelector:指向要选择器控件
  		2.GUIMESSAGE *pMsg:当前的消息队列
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有消息注册队列互斥锁的情况下调用
***/ 
int LoginSelectorToMsg(SELECTOR *pSelector, GUIMESSAGE *pMsg)
{
	if ((NULL == pSelector) || (NULL == pMsg))
	{
		return -1;
	}

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSelector->pSelectorBtnLeft, 
                	SelectorBtnLeft_Down, pSelector, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSelector->pSelectorBtnLeft, 
                	SelectorBtnLeft_Up, pSelector, sizeof(int), pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSelector->pSelectorBtnMidd, 
                	SelectorBtnMidd_Down, pSelector, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSelector->pSelectorBtnMidd, 
                	SelectorBtnMidd_Up, pSelector, sizeof(int), pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSelector->pSelectorBtnRight, 
                	SelectorBtnRight_Down, pSelector, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSelector->pSelectorBtnRight, 
                	SelectorBtnRight_Up, pSelector, sizeof(int), pMsg);
	
	return 0;
}

int LoginSelectorToMsg1(SELECTOR *pSelector, int selectID,GUIMESSAGE *pMsg)
{
	if ((NULL == pSelector) || (NULL == pMsg))
	{
		return -1;
	}

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSelector->pSelectorBtnLeft, 
                	SelectorBtnLeft_Down, pSelector, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSelector->pSelectorBtnLeft, 
                	SelectorBtn1Left_Up, pSelector, selectID, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSelector->pSelectorBtnMidd, 
                	SelectorBtnMidd_Down, pSelector, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSelector->pSelectorBtnMidd, 
                	SelectorBtnMidd_Up, pSelector, sizeof(int), pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSelector->pSelectorBtnRight, 
                	SelectorBtnRight_Down, pSelector, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSelector->pSelectorBtnRight, 
                	SelectorBtn1Right_Up, pSelector, selectID, pMsg);
	
	return 0;
}
/***
  * 功能：
     	设置控件是否使能，也就是是否接受点击事件
  * 参数：
  		1.SELECTOR **pSelector:指向要选择器控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetSelectorEnable(UINT8 uiEnable, SELECTOR *pSelector)
{

	if (NULL == pSelector)
	{
		return -1;
	}

	if (pSelector->uiEnable != uiEnable)
	{
		pSelector->uiEnable = uiEnable;
		if (pSelector->uiEnable)
		{
			SetPictureBitmap(SEC_UP_UNPRESS, pSelector->pSelectorBtnRight);
			SetPictureBitmap(SEC_DN_UNPRESS, pSelector->pSelectorBtnLeft);
		}
		else
		{
            SetPictureBitmap(SEC_UP_UNPRESS, pSelector->pSelectorBtnRight);
            SetPictureBitmap(SEC_DN_UNPRESS, pSelector->pSelectorBtnLeft);
		}
	}
	
	return 0;
}

/***
  * 功能：
     	设置选择第几个
  * 参数：
  		1.SELECTOR *pSelector:指向要选择器控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 

int SetSelectorSelected(UINT32 uiSelect, SELECTOR *pSelector)
{
	if (uiSelect >= pSelector->uiItemCnt)
	{
		return -1;
	}
	pSelector->uiCurrSelected = uiSelect;
	UpdateItemDisp(pSelector);
	return 0;
}

/***
  * 功能：
     	更新选择器内容显示
  * 参数：
  		1.SELECTOR **pSelector:指向要选择器控件
  * 返回：
        无
  * 备注：
***/ 
static void UpdateItemDisp(SELECTOR * pSelector)
{
	SetLabelText(pSelector->pItemString[pSelector->uiCurrSelected], 
				 pSelector->pSelectorLblItem);
	DisplayPicture(pSelector->pSelectorBtnMidd);
	DisplayLabel(pSelector->pSelectorLblItem);
}

/*选择器左按键按下函数*/
static int SelectorBtnLeft_Down (void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{
		SetPictureBitmap(SEC_DN_PRESS, pSelector->pSelectorBtnLeft);
		DisplayPicture(pSelector->pSelectorBtnLeft);
		if (pSelector->uiCurrSelected > 0)
		{
			pSelector->uiCurrSelected --;
			UpdateItemDisp(pSelector);
		}
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}
static int SelectorBtnLeft_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{	
		//更新显示
		SetPictureBitmap(SEC_DN_UNPRESS, pSelector->pSelectorBtnLeft);
		DisplayPicture(pSelector->pSelectorBtnLeft);
		//执行回调函数
		pSelector->BackCallFunc(pSelector->uiCurrSelected);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}

static int SelectorBtn1Left_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{	
		//更新显示
		SetPictureBitmap(SEC_DN_UNPRESS, pSelector->pSelectorBtnLeft);
		DisplayPicture(pSelector->pSelectorBtnLeft);
		//执行回调函数
		pSelector->BackCallFunc1(pSelector->uiCurrSelected,iOutLen);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}

/*选择器右按键按下函数*/
static int SelectorBtnRight_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{
		SetPictureBitmap(SEC_UP_PRESS, pSelector->pSelectorBtnRight);
		DisplayPicture(pSelector->pSelectorBtnRight);
		if (pSelector->uiCurrSelected < (pSelector->uiItemCnt-1))
		{
			pSelector->uiCurrSelected ++;
			UpdateItemDisp(pSelector);
		}
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}
static int SelectorBtnRight_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;

	if (NULL == pSelector)
	{
		return -1;
	}

	if (pSelector->uiEnable)
	{
		//更新显示
		SetPictureBitmap(SEC_UP_UNPRESS, pSelector->pSelectorBtnRight);
		DisplayPicture(pSelector->pSelectorBtnRight);		 
		//执行回调函数 
		pSelector->BackCallFunc(pSelector->uiCurrSelected);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}

static int SelectorBtn1Right_Up(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;

	if (NULL == pSelector)
	{
		return -1;
	}

	if (pSelector->uiEnable)
	{
		//更新显示
		SetPictureBitmap(SEC_UP_UNPRESS, pSelector->pSelectorBtnRight);
		DisplayPicture(pSelector->pSelectorBtnRight);		 
		//执行回调函数 
		pSelector->BackCallFunc1(pSelector->uiCurrSelected,iOutLen);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}

/*选择器中间元素点击处理函数，使能之后有效*/
static int SelectorBtnMidd_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{

	SELECTOR *pSelector = (SELECTOR *)pOutArg;

	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{
		if (pSelector->uiItemTouchFlg)
		{
			pCurrSelector = pSelector;
			TouchChange("sysset_btn_time_press.bmp", pSelector->pSelectorBtnMidd, 
						NULL, pSelector->pSelectorLblItem, 1);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}	
	return 0;
}
static int SelectorBtnMidd_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
	SELECTOR *pSelector = (SELECTOR *)pOutArg;

	if (NULL == pSelector)
	{
		return -1;
	}

	if (pSelector->uiEnable)
	{
		if (pSelector->uiItemTouchFlg)
		{
			//更新显示
			SetPictureBitmap(SEC_MD_UNPRESS, pSelector->pSelectorBtnMidd);
			DisplayPicture(pSelector->pSelectorBtnMidd);
			DisplayLabel(pSelector->pSelectorLblItem);
			RefreshScreen(__FILE__, __func__, __LINE__);
			//...
		}
	}
	return 0;
}

//处理选择器递增或递减
void HandleSelectorAddOrDec(SELECTOR *pSelector, int isAddOrDec)
{
	if (isAddOrDec)//add
	{
		SelectorBtnRight_Down(NULL, 0, pSelector, 0);
		SelectorBtnRight_Up(NULL, 0, pSelector, 0);
	}
	else//dec
	{
		SelectorBtnLeft_Down(NULL, 0, pSelector, 0);
		SelectorBtnLeft_Up(NULL, 0, pSelector, 0);
	}
}
