/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmapd.h
* ժ    Ҫ��  ʵ��������frmapd�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-30
*
*******************************************************************************/

#ifndef __WND_FRMAPD_H__
#define __WND_FRMAPD_H__

#include "common_opm.h"

int FrmApdInit(void *pWndObj);
int FrmApdExit(void *pWndObj);
int FrmApdPaint(void *pWndObj);
int FrmApdLoop(void *pWndObj);

#endif /* __WND_FRMAPD_H__ */

