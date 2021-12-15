/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmscroll.h
* 摘    要：  
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014.10.22
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _WND_FRMSCROLL_H
#define _WND_FRMSCROLL_H

#define BTN_SCROLL_DOWNOK

typedef struct _scroll
{
	GUIPICTURE *pScrollBtn;
	GUILABEL  *pScrollLbl;
	MSGFUNC pScrollBtn_Down;
	MSGFUNC pScrollBtn_Up;
}BTN_SCROLL;

typedef int (*SCROLLCALLBACK)(BTN_SCROLL *pScroll);

BTN_SCROLL *CreateScroll(int iBtnNum, SCROLLCALLBACK InitScroll);

int DestroyScroll(BTN_SCROLL **pScroll);

int ShowScrollBtnDown(int iPrev, int *pCurr, BTN_SCROLL *pScroll);

int ShowScrollBtnUp(int iCurr, BTN_SCROLL *pScroll);

int ScrollScrollUpOrDown(int iPrev, int iCurr, BTN_SCROLL *pScroll);

int ShowScrollBtnDown1(int iPrev, int *pCurr, BTN_SCROLL *pScroll);

int ShowScrollBtnUp1(int iCurr, BTN_SCROLL *pScroll);

int ScrollScrollUpOrDown1(int iPrev, int iCurr, BTN_SCROLL *pScroll);

int ScrollOk_Down(int iCurr, BTN_SCROLL *pScroll);

int ScrollOk_Up(int iCurr, BTN_SCROLL *pScroll);

#endif
