/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmscroll.c
* 摘    要：  
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014.10.28
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guibase.h"
#include "guipicture.h"
#include "guilabel.h"
#include "wnd_global.h"
#include "wnd_frmscroll.h"

BTN_SCROLL *CreateScroll(int iBtnNum, SCROLLCALLBACK InitScroll)
{
	BTN_SCROLL *pScrollObj = NULL;
	if (iBtnNum < 1 || InitScroll == NULL)
	{
		return NULL;
	}
	pScrollObj = GuiMemAlloc(sizeof(BTN_SCROLL)*iBtnNum);
	if (!pScrollObj)
	{
		return NULL;
	}
	InitScroll(pScrollObj);

	return pScrollObj;
}

int DestroyScroll(BTN_SCROLL **pScroll)
{
	if (*pScroll != NULL)
	{
		GuiMemFree(*pScroll);
		*pScroll = NULL;
	}
	
	return 0;
}
#ifdef BTN_SCROLL_DOWNOK
static void ScrollFulshDisp(char *strBmp, GUIPICTURE *pPicObj, GUILABEL *pLblObj)
{
	SetPictureBitmap(strBmp, pPicObj);
	DisplayPicture(pPicObj);
	DisplayLabel(pLblObj);
}

int ShowScrollBtnDown(int index, int *pCurr, BTN_SCROLL *pScroll)
{
	int iPrev = *pCurr;
	GUIFONT *pFont;
    unsigned int uiColor;
	
	if (pScroll == NULL)
	{
		return -1;
	}
	*pCurr = index;
	ScrollFulshDisp(BmpFileDirectory"unclick_bg.bmp", pScroll[iPrev].pScrollBtn, 
					pScroll[iPrev].pScrollLbl);
    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00000000, 0xFFFFFFFF, pFont);
	ScrollFulshDisp(BmpFileDirectory"click_bg.bmp", pScroll[*pCurr].pScrollBtn, 
					pScroll[*pCurr].pScrollLbl);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

	return 0;
}

int ShowScrollBtnUp(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	ScrollFulshDisp(BmpFileDirectory"click_bg.bmp", pScroll[iCurr].pScrollBtn, 
					pScroll[iCurr].pScrollLbl);
	return 0;
}

int ScrollScrollUpOrDown(int iPrev, int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	ScrollFulshDisp(BmpFileDirectory"unclick_bg.bmp", pScroll[iPrev].pScrollBtn, 
					pScroll[iPrev].pScrollLbl);
	ScrollFulshDisp(BmpFileDirectory"click_bg.bmp", pScroll[iCurr].pScrollBtn, 
					pScroll[iCurr].pScrollLbl);
	return 0;
}

int ShowScrollBtnDown1(int index, int *pCurr, BTN_SCROLL *pScroll)
{
	int iPrev = *pCurr;
	GUIFONT *pFont;
    unsigned int uiColor;
	
	if (pScroll == NULL)
	{
		return -1;
	}
	*pCurr = index;
	ScrollFulshDisp(BmpFileDirectory"unclick_bg1.bmp", pScroll[iPrev].pScrollBtn, 
					pScroll[iPrev].pScrollLbl);
    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00000000, 0xFFFFFFFF, pFont);
	ScrollFulshDisp(BmpFileDirectory"click_bg1.bmp", pScroll[*pCurr].pScrollBtn, 
					pScroll[*pCurr].pScrollLbl);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

	return 0;
}

int ShowScrollBtnUp1(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	ScrollFulshDisp(BmpFileDirectory"click_bg1.bmp", pScroll[iCurr].pScrollBtn, 
					pScroll[iCurr].pScrollLbl);
	return 0;
}

int ScrollScrollUpOrDown1(int iPrev, int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	ScrollFulshDisp(BmpFileDirectory"unclick_bg1.bmp", pScroll[iPrev].pScrollBtn, 
					pScroll[iPrev].pScrollLbl);
	ScrollFulshDisp(BmpFileDirectory"click_bg1.bmp", pScroll[iCurr].pScrollBtn, 
					pScroll[iCurr].pScrollLbl);
	return 0;
}

int ScrollOk_Down(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	pScroll[iCurr].pScrollBtn_Down(NULL, 0, NULL, 0);

	return 0;
}

int ScrollOk_Up(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	pScroll[iCurr].pScrollBtn_Up(NULL, 0, NULL, 0);

	return 0;
}
#else
int ShowScrollBtnDown(int index, int *pCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	*pCurr = index;

	ShowMenuBtn_FuncDown(pScroll[*pCurr].pScrollBtn, pScroll[*pCurr].pScrollLbl);

	return 0;
}
int ShowScrollBtnUp(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	ShowMenuBtn_FuncUp(pScroll[iCurr].pScrollBtn, pScroll[iCurr].pScrollLbl);

	return 0;
}

int ShowScrollBtnDown1(int index, int *pCurr, BTN_SCROLL *pScroll)
{
	GUIFONT *pFont;
    unsigned int uiColor;
	
	if (pScroll == NULL)
	{
		return -1;
	}
	*pCurr = index;
	
    SetPictureBitmap(BmpFileDirectory"click_bg1.bmp", pScroll[*pCurr].pScrollBtn);
    DisplayPicture(pScroll[*pCurr].pScrollBtn);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00000000, 0xFFFFFFFF, pFont);
    DisplayLabel(pScroll[*pCurr].pScrollLbl);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);
	

	return 0;
}
int ShowScrollBtnUp1(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	SetPictureBitmap(BmpFileDirectory"unclick_bg1.bmp", pScroll[iCurr].pScrollBtn);
    DisplayPicture(pScroll[iCurr].pScrollBtn);
    DisplayLabel(pScroll[iCurr].pScrollLbl);

	return 0;
}
int ScrollScrollUpOrDown(int iPrev, int iCurr, BTN_SCROLL *pScroll)
{return 0;}
int ScrollScrollUpOrDown1(int iPrev, int iCurr, BTN_SCROLL *pScroll)
{return 0;}
int ScrollOk_Down(int iCurr, BTN_SCROLL *pScroll)
{return 0;}
int ScrollOk_Up(int iCurr, BTN_SCROLL *pScroll)
{return 0;}
#endif
