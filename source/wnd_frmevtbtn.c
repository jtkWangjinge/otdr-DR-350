/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmevtbtn.c  
* 摘    要：  实现事件表附加功能按钮控件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2015-01-05
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


/****************************
* 包含wnd_frmevtbtn需要头文件
****************************/
#include "wnd_frmevtbtn.h"
#include "wnd_frmeventtab.h"
#include "guiglobal.h"


/******************************
* 声明wnd_frmevtbtn内部使用函数
******************************/
static int EvtBtnItem_Down(void *pInArg, int iInLen, 
						   void *pOutArg, int iOutLen);
static int EvtBtnItem_Up(void *pInArg, int iInLen, 
					    void *pOutArg, int iOutLen);

WNDEVTBTN *CreateEvtBtn(int x, int y, int iPage, int iCnt, BTNFUNC *ppFunc, 
						void *pFuncArg)
{
    WNDEVTBTN *pBtnObj = NULL;
	int iTmp = 0;
	/*int iTextIndex[] = {
		1
	};*/
	char* iTextIndex[] = {
		"Switch",
		"Add",
		"Delete"
	};
	
	pBtnObj = (WNDEVTBTN*)malloc(sizeof(WNDEVTBTN));
	if (NULL == pBtnObj)
	{
		return NULL;
	}
	pBtnObj->iFocus = 0;
	pBtnObj->iEnable = 0; //默认不使能
	pBtnObj->iTouchEnable = 1;
	pBtnObj->pCallerArg = pFuncArg;

	//初始化函数等
	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		pBtnObj->pStrItem[iTmp] = TransString(iTextIndex[iTmp]);//GetCurrLanguageText(iTextIndex[iTmp]);
		pBtnObj->pItemFunc[iTmp] = ppFunc[iTmp];
		pBtnObj->iItemEn[iTmp] = 1;
		pBtnObj->iItemFcs[iTmp] = 0;
	}

	//????
	for(iTmp = 0; iTmp < 3; ++iTmp)
    {
        pBtnObj->pBtnItem[iTmp] = CreatePicture(x+50*iTmp, y, 50, 17, BmpFileDirectory"evt_btn.bmp");
        pBtnObj->pLblItem[iTmp] = CreateLabel(x+50*iTmp, y, 50, 16, NULL);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pBtnObj->pLblItem[iTmp]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBtnObj->pLblItem[iTmp]);
    }   
    
	return pBtnObj;
    
}

//销毁控件
int DestroyEvtBtn(WNDEVTBTN **pBtnObj)
{
    int iTmp;

	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		DestroyPicture(&((*pBtnObj)->pBtnItem[iTmp]));
		DestroyLabel(&((*pBtnObj)->pLblItem[iTmp]));				
	}

	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		free((*pBtnObj)->pStrItem[iTmp]);
	}

	free(*pBtnObj);
	*pBtnObj = NULL;

	return 0;
}

//绘制事件按钮控件
int PanitEvtBtn(WNDEVTBTN*pBtnObj)
{
	int iTmp = 0;

	if (!pBtnObj->iEnable)
	{
		return 0;
	}
	    
	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		SetLabelText(pBtnObj->pStrItem[iTmp], pBtnObj->pLblItem[iTmp]);
		if (pBtnObj->iItemEn[iTmp]) 
        {
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pBtnObj->pLblItem[iTmp]);
		} 
        else 
        {
			SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pBtnObj->pLblItem[iTmp]);
		}
		
		DisplayPicture(pBtnObj->pBtnItem[iTmp]);
		DisplayLabel(pBtnObj->pLblItem[iTmp]);
	}

	return 0;
    
}

//添加控件队列
int AddEvtBtnToCom(WNDEVTBTN*pBtnObj, GUIWINDOW *pWnd)
{
    int iTmp;

	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
			  	  	 pBtnObj->pBtnItem[iTmp], pWnd);
	}
	
	return 0;
}

//从控件队列摘下
int DelEvtBtnToCom(WNDEVTBTN*pBtnObj, GUIWINDOW *pWnd)
{
    int iTmp;
	
	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		DelWindowComp(pBtnObj->pBtnItem[iTmp], pWnd);
	}
	
	return 0;
}

//添加消息队列
int LoginEvtBtnToMsg(WNDEVTBTN*pBtnObj, GUIMESSAGE *pMsg)
{
    int iTmp = 0;
	
	//注册按钮消息处理
	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBtnObj->pBtnItem[iTmp],
                		EvtBtnItem_Down, pBtnObj, iTmp, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pBtnObj->pBtnItem[iTmp],
                		EvtBtnItem_Up, pBtnObj, iTmp, pMsg);
	}

	return 0;
}

//添加消息队列
int LogoutEvtBtnToMsg(WNDEVTBTN* pBtnObj, GUIMESSAGE *pMsg)
{
    int iTmp = 0;
	
	//注册按钮消息处理
	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pBtnObj->pBtnItem[iTmp],pMsg);
		LogoutMessageReg(GUIMESSAGE_TCH_UP, pBtnObj->pBtnItem[iTmp], pMsg);
	}

	return 0;
    
}

//设置控件的使能状态
int SetEvtBtnEnable(WNDEVTBTN *pBtnObj, int iEnable, GUIWINDOW *pWnd)
{
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	//判断使能状态
	if (pBtnObj->iEnable != iEnable)
	{
		if (iEnable)
		{
			AddEvtBtnToCom(pBtnObj, pWnd);
			LoginEvtBtnToMsg(pBtnObj, pMsg);
		}
		else
		{	
			DelEvtBtnToCom(pBtnObj, pWnd);
			LogoutEvtBtnToMsg(pBtnObj, pMsg);
		}

		//设置使能标志
		pBtnObj->iEnable = iEnable;
	}
	
	return 0;
}

//设置元素使能
int SetBtnItemEnable(WNDEVTBTN *pBtnObj, int *pEnable)
{
    int iTmp = 0;
	
	//注册按钮消息处理
	for (iTmp = 0; iTmp < 3; ++iTmp)
	{
		pBtnObj->iItemEn[iTmp] = pEnable[iTmp];
	}

	return 0;
    
}


//设置点击使能与否
int SetBtnTouchEnable(WNDEVTBTN *pBtnObj, int iEnable)
{
	//判断使能状态
	if (pBtnObj->iTouchEnable != iEnable)
	{
		//设置使能标志
		pBtnObj->iTouchEnable = iEnable;
	}
	return 0;
}


//获得元素使能标志
int *GetBtnItemEnable(WNDEVTBTN *pBtnObj)
{
	return pBtnObj->iItemEn;
}

//设置元素焦点
int SetEvtBtnFocus(WNDEVTBTN *pBtnObj, int iIndex, int iFocus)
{
	pBtnObj->iItemFcs[iIndex] = iFocus;
	return 0;
}

//按钮点击事件处理
static int EvtBtnItem_Down(void *pInArg, int iInLen, 
						   void *pOutArg, int iOutLen)
{
    int iIndex = iOutLen;
	int iTmp = 0;
	WNDEVTBTN *pBtnObj = (WNDEVTBTN *)pOutArg;
    iTmp += iIndex;
    if ((pBtnObj->iItemEn[iTmp]) &&
		pBtnObj->iEnable &&
		pBtnObj->iTouchEnable)
	{
		SetPictureBitmap(BmpFileDirectory"evt_btnp.bmp", 
						 pBtnObj->pBtnItem[iIndex]);
		DisplayPicture(pBtnObj->pBtnItem[iIndex]);
		DisplayLabel(pBtnObj->pLblItem[iIndex]);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
    return 0;
}

//按钮点击事件处理
static int EvtBtnItem_Up(void *pInArg, int iInLen, 
					    void *pOutArg, int iOutLen)
{
    int iIndex = iOutLen;
	int iTmp = 0;
	WNDEVTBTN *pBtnObj = (WNDEVTBTN *)pOutArg;
    iTmp += iIndex;
    if ((pBtnObj->iItemEn[iTmp]) &&
		pBtnObj->iEnable &&
		pBtnObj->iTouchEnable)
	{
		SetPictureBitmap(BmpFileDirectory"evt_btn.bmp", 
						 pBtnObj->pBtnItem[iIndex]);
		DisplayPicture(pBtnObj->pBtnItem[iIndex]);
		DisplayLabel(pBtnObj->pLblItem[iIndex]);
        if (pBtnObj->pItemFunc[iTmp])
		{
			(pBtnObj->pItemFunc[iTmp])(pBtnObj->pCallerArg);
		}
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
    return 0;
}
