/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmfiber.h
* ժ    Ҫ��  ����������frmfiber�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-30
*
*******************************************************************************/

#ifndef __WND_FRMFIBER_H__
#define __WND_FRMFIBER_H__

// #include "guiglobal.h"

int FrmFiberInit(void *pWndObj);
int FrmFiberExit(void *pWndObj);
int FrmFiberPaint(void *pWndObj);
int FrmFiberLoop(void *pWndObj);
int FrmFiberPause(void *pWndObj);
int FrmFiberResume(void *pWndObj);
	
#endif /* __WND_FRMFIBER_H__ */


