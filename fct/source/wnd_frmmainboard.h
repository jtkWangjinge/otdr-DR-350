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

#ifndef __WND_FRMMAINBOARD_H__
#define __WND_FRMMAINBOARD_H__

#include "guiglobal.h"

int FrmMainBoardInit(void *pWndObj);
int FrmMainBoardExit(void *pWndObj);
int FrmMainBoardPaint(void *pWndObj);
int FrmMainBoardLoop(void *pWndObj);
int FrmMainBoardPause(void *pWndObj);
int FrmMainBoardResume(void *pWndObj);

#endif /* __WND_FRMMAINBOARD_H__ */

