/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmmain.h
* ժ    Ҫ��  ����������frmmain�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/
#ifndef __WND_FRMPULSE_H__
#define __WND_FRMPULSE_H__

// #include "guiglobal.h"

int FrmPulseWidInit(void *pWndObj);
int FrmPulseWidExit(void *pWndObj);
int FrmPulseWidPaint(void *pWndObj);
int FrmPulseWidLoop(void *pWndObj);
int FrmPulseWidPause(void *pWndObj);
int FrmPulseWidResume(void *pWndObj);
int PulseWidFCTShow(void);
	
#endif /* __WND_FRMPULSE_H__ */

