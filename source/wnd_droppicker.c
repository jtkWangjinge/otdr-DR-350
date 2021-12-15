/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_droppicker.c
* 摘    要：  otdr中下拉选项菜单
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*******************************************************************************/
#include "wnd_droppicker.h"

#include "app_curve.h"

#include "wnd_global.h"

static DROPPICKER * pLocalOtdrList;
static int MarkCallWnd = 0;
static int iOldFocus = 0;

/*******************************************************************************
***                      wnd_droppicker.c内部函数声明                        ***
*******************************************************************************/
static int DropLayer_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int DropLayer_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int DropLayerKey_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int DropLayerKey_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int DropPicker_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int DropPicker_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

//销毁上拉列表控件
static int DestroyDropPicker(DROPPICKER **pDropPicker);
//获取当前下拉框所在的位置
static int GetCurFocus(DROPPICKER *pDropPicker, int option);
//刷新下拉列表
static int FlushDropList(DROPPICKER *pDropPicker);

/***
  * 功能：
     	创建上啦列表控件
  * 参数：
  		1.int x: 起始点横坐标
  		2.int y: 起始点纵坐标
  		3.int iNum: 列表数量
  		4.char **str: 列表内容
  		5.int *pEnable: 使能数组
  		6.int iSelect:  焦点
  		7.DROPACKFUNC BackCall: 回调函数
  		8.GUIWINDOW *pWnd: 当前窗体
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
DROPPICKER * CreateDropPicker(int x, int y, int iNum, char **str,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd)
{
	int iErr = 0;
	DROPPICKER *pDropPicker = NULL;
	GUIMESSAGE *pMsg;
	
	if(str == NULL || pEnable == NULL || pWnd == NULL)
		return NULL;
	
	if (!iErr)
	{	//分配资源
		pDropPicker = (DROPPICKER *)calloc(1, sizeof(DROPPICKER));
		if (NULL == pDropPicker)
		{
			iErr = -1;
		}
	}

	if(!iErr)
	{
		int i = 0;
		pDropPicker->iItemCnt = iNum;
		pDropPicker->iFocus = iSelect;
		pDropPicker->BackCallFunc = BackCall;
		pDropPicker->pWnd = pWnd;
		
		pDropPicker->pDropLayerLbl = CreateLabel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		pDropPicker->pDropLayerLbl->Visible.iLayer = DROPLIST_LAYER-1;
		pDropPicker->pDropLayerLbl->Visible.iCursor = 1;
		pDropPicker->pDropLayerLbl->Visible.iFocus = 1;
		for(i = 0;i < iNum; i++)
		{
			if(i != iSelect)
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y-(i+1)*24, 110, 24, BmpFileDirectory"drop_list_unpress.bmp");
			else
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y-(i+1)*24, 110, 24, BmpFileDirectory"drop_list_press.bmp");
			pDropPicker->pDropItemStr[i] = TransString(str[i]);
			pDropPicker->pDropItemLbl[i] = CreateLabel(x, y-(i+1)*24+5, 110, 24, pDropPicker->pDropItemStr[i]);
			pDropPicker->pDropItemLbl[i]->Visible.iLayer = DROPLIST_LAYER;
			pDropPicker->pDropItemLbl[i]->Visible.iCursor = 1;

			SetLabelAlign(2, pDropPicker->pDropItemLbl[i]);
			if(pEnable[i] == 0)
			{
				pDropPicker->iEnable[i] = 0;
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pDropPicker->pDropItemLbl[i]);
			}
			else
			{
				pDropPicker->iEnable[i] = 1;
			}
		}

		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pDropPicker->pDropLayerLbl, pWnd);
		for(i = 0;i < iNum; i++)
		{
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pDropPicker->pDropItemLbl[i], pWnd);
		}

		pMsg = GetCurrMessage();
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayer_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropLayerLbl,
	               		DropLayer_Up, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_UP, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Up, pDropPicker, 4, pMsg);
		for(i = 0; i < iNum; i++)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Down, pDropPicker, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Up, pDropPicker, i, pMsg);
		}
		//显示
		memcpy(pFrameBufferBak,GetCurrFbmap()->pMapBuff, GetCurrFbmap()->uiBufSize);
		
		for(i = 0; i < pDropPicker->iItemCnt; i++)
		{
			DisplayPicture(pDropPicker->pDropItemBg[i]);
			DisplayLabel(pDropPicker->pDropItemLbl[i]);
		}
		TouchChange(NULL, NULL, NULL, pDropPicker->pDropItemLbl[iSelect], 1);

		RefreshScreen(__FILE__, __func__, __LINE__);
	}

	pLocalOtdrList = pDropPicker;

	MarkCallWnd = 1;

	return pDropPicker;	
}

/***
  * 功能：
     	创建下拉列表控件
  * 参数：
  		1.int x: 起始点横坐标
  		2.int y: 起始点纵坐标
  		3.int size: 列表数量
  		4.unsigned int *ItemStr: 列表内容
  		5.int *pEnable: 使能数组
  		6.int iSelect:  焦点
  		7.DROPACKFUNC BackCall: 回调函数
  		8.GUIWINDOW *pWnd: 当前窗体
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
DROPPICKER * CreateDropList(int x, int y, int size, unsigned int *ItemStr,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd)
{
	int iErr = 0;
	DROPPICKER *pDropPicker = NULL;
	GUIMESSAGE *pMsg;
	
	if(pEnable == NULL || pWnd == NULL)
	{
		return NULL;
	}
	
	if (!iErr)
	{	//分配资源
		pDropPicker = (DROPPICKER *)calloc(1, sizeof(DROPPICKER));
		if (NULL == pDropPicker)
		{
			iErr = -1;
		}
	}

	if(!iErr)
	{
		int i = 0;
		pDropPicker->iItemCnt = size/sizeof(unsigned int);
		pDropPicker->iFocus = iSelect;
		pDropPicker->BackCallFunc = BackCall;
		pDropPicker->pWnd = pWnd;
		
		pDropPicker->pDropLayerLbl = CreateLabel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		pDropPicker->pDropLayerLbl->Visible.iLayer = DROPLIST_LAYER-1;
		pDropPicker->pDropLayerLbl->Visible.iCursor = 1;
		pDropPicker->pDropLayerLbl->Visible.iFocus = 1;
		for(i = 0;i < pDropPicker->iItemCnt; i++)
		{
			if(i != iSelect)
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y-(i+1)*28, 109, 28, BmpFileDirectory"drop_list_unpress.bmp");
			else
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y-(i+1)*28, 109, 28, BmpFileDirectory"drop_list_press.bmp");
			pDropPicker->pDropItemStr[i] = GetCurrLanguageText(ItemStr[i]);
			pDropPicker->pDropItemLbl[i] = CreateLabel(x, y-(i+1)*28+5, 109, 24, pDropPicker->pDropItemStr[i]);
			pDropPicker->pDropItemLbl[i]->Visible.iLayer = DROPLIST_LAYER;
			pDropPicker->pDropItemLbl[i]->Visible.iCursor = 1;

			SetLabelAlign(2, pDropPicker->pDropItemLbl[i]);
			if(pEnable[i] == 0)
			{
				pDropPicker->iEnable[i] = 0;
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pDropPicker->pDropItemLbl[i]);
			}
			else
			{
				pDropPicker->iEnable[i] = 1;
			}
		}

		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pDropPicker->pDropLayerLbl, pWnd);
		for(i = 0;i < pDropPicker->iItemCnt; i++)
		{
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pDropPicker->pDropItemLbl[i], pWnd);
		}

		pMsg = GetCurrMessage();
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayer_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropLayerLbl,
	               		DropLayer_Up, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_UP, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Up, pDropPicker, 4, pMsg);
		for(i = 0; i < pDropPicker->iItemCnt; i++)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Down, pDropPicker, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Up, pDropPicker, i, pMsg);
		}
		//显示
		memcpy(pFrameBufferBak,GetCurrFbmap()->pMapBuff, GetCurrFbmap()->uiBufSize);
		
		for(i = 0; i < pDropPicker->iItemCnt; i++)
		{
			DisplayPicture(pDropPicker->pDropItemBg[i]);
			DisplayLabel(pDropPicker->pDropItemLbl[i]);
		}
		TouchChange(NULL, NULL, NULL, pDropPicker->pDropItemLbl[iSelect], 1);

		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	
	pLocalOtdrList = pDropPicker;
	MarkCallWnd = 1;

	return pDropPicker;	
}



/***
  * 功能：
     	创建下拉列表控件
  * 参数：
  		1.int x: 起始点横坐标
  		2.int y: 起始点纵坐标
  		3.int iNum: 列表数量
  		4.char **str1: 列表内容(英文)
		5.int *str2: 列表内容（多国语）
  		6.int *pEnable: 使能数组
  		7.int iSelect:  焦点
  		8.DROPACKFUNC BackCall: 回调函数
  		9.GUIWINDOW *pWnd: 当前窗体
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
DROPPICKER * CreateDropPicker1(int x, int y, int iNum, char **str1, int *str2,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd)
{
	int iErr = 0;
	DROPPICKER *pDropPicker = NULL;
	GUIMESSAGE *pMsg;
	
	if(((str1 == NULL) && (str2 == NULL))|| pEnable == NULL || pWnd == NULL)
		return NULL;
	
	if (!iErr)
	{	//分配资源
		pDropPicker = (DROPPICKER *)calloc(1, sizeof(DROPPICKER));
		if (NULL == pDropPicker)
		{
			iErr = -1;
		}
	}

	if(!iErr)
	{
		int i = 0;
		pDropPicker->iItemCnt = iNum;
		pDropPicker->iFocus = iSelect;
		iOldFocus = iSelect;
		pDropPicker->BackCallFunc = BackCall;
		pDropPicker->pWnd = pWnd;
		
		pDropPicker->pDropLayerLbl = CreateLabel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		pDropPicker->pDropLayerLbl->Visible.iLayer = DROPLIST_LAYER-1;
		pDropPicker->pDropLayerLbl->Visible.iCursor = 1;
		pDropPicker->pDropLayerLbl->Visible.iFocus = 1;
		for(i = 0;i < iNum; i++)
		{
			if(i != iSelect)
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y+(i+1)*25, 158, 25,
												BmpFileDirectory"drop_list_unpress1.bmp");
			else
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y+(i+1)*25, 158, 25, 
												BmpFileDirectory"drop_list_press1.bmp");
			if (!str2)
			{
				pDropPicker->pDropItemStr[i] = TransString(str1[i]);
			}
			else
			{
				//针对下拉框中出现多国语的情况
				pDropPicker->pDropItemStr[i] = GetCurrLanguageText(str2[i]);
			}
			
			pDropPicker->pDropItemLbl[i] = CreateLabel(x, y+(i+1)*25+4, 158, 16,
												pDropPicker->pDropItemStr[i]);
			pDropPicker->pDropItemLbl[i]->Visible.iLayer = DROPLIST_LAYER;
			pDropPicker->pDropItemLbl[i]->Visible.iCursor = 1;

			SetLabelAlign(2, pDropPicker->pDropItemLbl[i]);
			if(pEnable[i] == 0)
			{
				pDropPicker->iEnable[i] = 0;
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pDropPicker->pDropItemLbl[i]);
			}
			else
			{
				pDropPicker->iEnable[i] = 1;
				SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDropPicker->pDropItemLbl[i]);
			}
		}

		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
							pDropPicker->pDropLayerLbl, pWnd);
		for(i = 0;i < iNum; i++)
		{
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
							pDropPicker->pDropItemLbl[i], pWnd);
		}

		pMsg = GetCurrMessage();
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayer_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropLayerLbl,
	               		DropLayer_Up, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_UP, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Up, pDropPicker, 4, pMsg);
		for(i = 0; i < iNum; i++)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Down, pDropPicker, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Up, pDropPicker, i, pMsg);
		}
		//显示
		memcpy(pFrameBufferBak,GetCurrFbmap()->pMapBuff, GetCurrFbmap()->uiBufSize);
		
		for(i = 0; i < pDropPicker->iItemCnt; i++)
		{
			DisplayPicture(pDropPicker->pDropItemBg[i]);
			DisplayLabel(pDropPicker->pDropItemLbl[i]);
		}
		TouchChange(NULL, NULL, NULL, pDropPicker->pDropItemLbl[iSelect], 1);
		//设置标题栏的电量、日期的刷新
		SetPowerEnable(0, 1);

		RefreshScreen(__FILE__, __func__, __LINE__);
	}

	pLocalOtdrList = pDropPicker;

	MarkCallWnd = 2;

	return pDropPicker;	
}


/***
  * 功能：
     	创建上拉列表控件
  * 参数：
  		1.int x: 起始点横坐标
  		2.int y: 起始点纵坐标
  		3.int size: 列表数量
  		4.unsigned int *ItemStr: 列表内容
  		5.int *pEnable: 使能数组
  		6.int iSelect:  焦点
  		7.DROPACKFUNC BackCall: 回调函数
  		8.GUIWINDOW *pWnd: 当前窗体
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
DROPPICKER * CreateDropList1(int x, int y, int size, unsigned int *ItemStr,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd)
{
	int iErr = 0;
	DROPPICKER *pDropPicker = NULL;
	GUIMESSAGE *pMsg;
	
	if(pEnable == NULL || pWnd == NULL)
	{
		return NULL;
	}
	
	if (!iErr)
	{	//分配资源
		pDropPicker = (DROPPICKER *)calloc(1, sizeof(DROPPICKER));
		if (NULL == pDropPicker)
		{
			iErr = -1;
		}
	}

	if(!iErr)
	{
		int i = 0;
		pDropPicker->iItemCnt = size/sizeof(unsigned int);
		pDropPicker->iFocus = iSelect;
		pDropPicker->BackCallFunc = BackCall;
		pDropPicker->pWnd = pWnd;
		
		pDropPicker->pDropLayerLbl = CreateLabel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		pDropPicker->pDropLayerLbl->Visible.iLayer = DROPLIST_LAYER-1;
		pDropPicker->pDropLayerLbl->Visible.iCursor = 1;
		pDropPicker->pDropLayerLbl->Visible.iFocus = 1;
		for(i = 0;i < pDropPicker->iItemCnt; i++)
		{
			if(i != iSelect)
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y+(i+1)*34, 130, 34, BmpFileDirectory"drop_list_unpress1.bmp");
			else
				pDropPicker->pDropItemBg[i] = CreatePicture(x, y+(i+1)*34, 130, 34, BmpFileDirectory"drop_list_press1.bmp");
			pDropPicker->pDropItemStr[i] = GetCurrLanguageText(ItemStr[i]);
			pDropPicker->pDropItemLbl[i] = CreateLabel(x, y+(i+1)*34+9, 130, 24, pDropPicker->pDropItemStr[i]);
			pDropPicker->pDropItemLbl[i]->Visible.iLayer = DROPLIST_LAYER;
			pDropPicker->pDropItemLbl[i]->Visible.iCursor = 1;

			SetLabelAlign(2, pDropPicker->pDropItemLbl[i]);
			if(pEnable[i] == 0)
			{
				pDropPicker->iEnable[i] = 0;
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pDropPicker->pDropItemLbl[i]);
			}
			else
			{
				pDropPicker->iEnable[i] = 1;
			}
		}

		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pDropPicker->pDropLayerLbl, pWnd);
		for(i = 0;i < pDropPicker->iItemCnt; i++)
		{
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pDropPicker->pDropItemLbl[i], pWnd);
		}

		pMsg = GetCurrMessage();
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayer_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropLayerLbl,
	               		DropLayer_Up, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_DOWN, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Down, pDropPicker, 4, pMsg);
		LoginMessageReg(GUIMESSAGE_KEY_UP, pDropPicker->pDropLayerLbl,
	               		DropLayerKey_Up, pDropPicker, 4, pMsg);
		for(i = 0; i < pDropPicker->iItemCnt; i++)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Down, pDropPicker, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pDropPicker->pDropItemLbl[i],
	               		DropPicker_Up, pDropPicker, i, pMsg);
		}
		//显示
		memcpy(pFrameBufferBak,GetCurrFbmap()->pMapBuff, GetCurrFbmap()->uiBufSize);
		
		for(i = 0; i < pDropPicker->iItemCnt; i++)
		{
			DisplayPicture(pDropPicker->pDropItemBg[i]);
			DisplayLabel(pDropPicker->pDropItemLbl[i]);
		}
		TouchChange(NULL, NULL, NULL, pDropPicker->pDropItemLbl[iSelect], 1);

		RefreshScreen(__FILE__, __func__, __LINE__);
	}

	pLocalOtdrList = pDropPicker;

	MarkCallWnd = 2;

	return pDropPicker;	
}


//销毁上拉列表控件
static int DestroyDropPicker(DROPPICKER **pDropPicker)
{
	int i = 0;
	GUIMESSAGE *pMsg;

	if(pDropPicker == NULL)
		return -1;

	pMsg = GetCurrMessage();

	DelWindowComp((*pDropPicker)->pDropLayerLbl, (*pDropPicker)->pWnd);
	for (i = 0; i < (*pDropPicker)->iItemCnt; i++)
	{
		DelWindowComp((*pDropPicker)->pDropItemLbl[i], (*pDropPicker)->pWnd);
	}

	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, (*pDropPicker)->pDropLayerLbl, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, (*pDropPicker)->pDropLayerLbl, pMsg);
	LogoutMessageReg(GUIMESSAGE_KEY_DOWN, (*pDropPicker)->pDropLayerLbl, pMsg);
	LogoutMessageReg(GUIMESSAGE_KEY_UP, (*pDropPicker)->pDropLayerLbl, pMsg);
	for (i = 0; i < (*pDropPicker)->iItemCnt; i++)
	{
		LogoutMessageReg(GUIMESSAGE_TCH_DOWN, (*pDropPicker)->pDropItemLbl[i], pMsg);
		LogoutMessageReg(GUIMESSAGE_TCH_UP, (*pDropPicker)->pDropItemLbl[i], pMsg);
	}

	DestroyLabel(&((*pDropPicker)->pDropLayerLbl));
	for(i = 0; i < (*pDropPicker)->iItemCnt; i++)
	{
		DestroyPicture(&((*pDropPicker)->pDropItemBg[i]));
		DestroyLabel(&((*pDropPicker)->pDropItemLbl[i]));
		GuiMemFree((*pDropPicker)->pDropItemStr[i]);
	}

	free((*pDropPicker));
	(*pDropPicker) = NULL;
	pLocalOtdrList = NULL;

	MarkCallWnd = 0;

	return 0;
}

//按下函数
static int DropPicker_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	DROPPICKER *pDropPicker = (DROPPICKER *)pOutArg;
	int iTouch = iOutLen;

    if (pDropPicker->iFocus == -1)
    {
        if(pDropPicker->iEnable[iTouch])
    	{
            TouchChange("drop_list_press1.bmp", pDropPicker->pDropItemBg[iTouch], NULL, pDropPicker->pDropItemLbl[iTouch], 1);
            RefreshScreen(__FILE__, __func__, __LINE__);
        }
    }
    else 
    {
    	int OldFocus = pDropPicker->iFocus;
        //改变按钮状态
    	if(pDropPicker->iEnable[iTouch])
    	{
    		TouchChange("drop_list_unpress1.bmp", pDropPicker->pDropItemBg[OldFocus], NULL, pDropPicker->pDropItemLbl[OldFocus], 1);
    		TouchChange("drop_list_press1.bmp", pDropPicker->pDropItemBg[iTouch], NULL, pDropPicker->pDropItemLbl[iTouch], 1);
    		pDropPicker->iFocus = iTouch;
    		RefreshScreen(__FILE__, __func__, __LINE__);
    	}    
    }

	return 0;
}

//弹起函数
static int DropPicker_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	DROPPICKER *pDropPicker = (DROPPICKER *)pOutArg;
	int iTouch = iOutLen;
	if(pDropPicker->iEnable[iTouch])
	{
		memcpy(GetCurrFbmap()->pMapBuff, pFrameBufferBak, GetCurrFbmap()->uiBufSize);
		pDropPicker->BackCallFunc(iTouch);
		DestroyDropPicker(&pDropPicker);
	}
	return 0;
}

//中间layer层按下函数
static int DropLayer_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}
//中间layer层弹起函数
static int DropLayer_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	DROPPICKER *pDropPicker = (DROPPICKER *)pOutArg;
	
	memcpy(GetCurrFbmap()->pMapBuff, pFrameBufferBak, GetCurrFbmap()->uiBufSize);

	pDropPicker->BackCallFunc(iOldFocus);
	DestroyDropPicker(&pDropPicker);

	return 0;
}

static int DropLayerKey_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
	
    uiValue = (unsigned int)pInArg;
	
	switch (uiValue)
    {
		case KEYCODE_ESC:
			DropLayer_Down(pInArg, iInLen, pOutArg, iOutLen);
			break;
		case KEYCODE_HOME:
			break;
		default:
			break;
	}

    return iReturn;
}

static int DropLayerKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	DROPPICKER *pDropPicker = (DROPPICKER *)pOutArg;

	switch (uiValue)
	{
	case KEYCODE_UP:
		if (!GetCurFocus(pDropPicker, 0))
		{
			FlushDropList(pDropPicker);
		}
		break;
	case KEYCODE_DOWN:
		if (!GetCurFocus(pDropPicker, 1))
		{
			FlushDropList(pDropPicker);
		}
		break;
	case KEYCODE_ENTER:
		DropPicker_Up(NULL, iInLen, pOutArg, pDropPicker->iFocus);
		break;
	case KEYCODE_ESC:
		DropLayer_Up(pInArg, iInLen, pOutArg, iOutLen);
		break;
	default:
		break;
	}

	return iReturn;
}

int DestroyDropList()
{
	if(pLocalOtdrList != NULL)
	{
		memcpy(GetCurrFbmap()->pMapBuff, pFrameBufferBak, GetCurrFbmap()->uiBufSize);
		SetCurveThreadMode(EN_NORMAL_MODE);
		DestroyDropPicker(&pLocalOtdrList);
	}
	
	return 0;
}

//获取当前下拉框所在的位置
int GetCurFocus(DROPPICKER *pDropPicker, int option)
{
	if (pDropPicker == NULL)
	{
		return -1;
	}

	switch (option)
	{
	case 0://up
		if (pDropPicker->iFocus != 0)
		{
			pDropPicker->iFocus -= 1;
		}
		break;
	case 1://down
		if (pDropPicker->iFocus != (pDropPicker->iItemCnt - 1))
		{
			pDropPicker->iFocus += 1;
		}
		break;
	default:
		break;
	}

	return 0;
}

//刷新下拉列表
static int FlushDropList(DROPPICKER *pDropPicker)
{
	if (pDropPicker == NULL)
	{
		return -1;
	}

	int i;
	int iFocus = pDropPicker->iFocus;

	for (i = 0; i < pDropPicker->iItemCnt; ++i)
	{
		TouchChange("drop_list_unpress1.bmp", pDropPicker->pDropItemBg[i], NULL, pDropPicker->pDropItemLbl[i], 1);
	}

	if (iFocus != -1)
	{
		TouchChange("drop_list_press1.bmp", pDropPicker->pDropItemBg[iFocus], NULL, pDropPicker->pDropItemLbl[iFocus], 1);
	}
	
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}
