/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmkeycheck.h
* ժ    Ҫ��  ʵ�ֶ�������keycheck(�������)�Ĵ��崦����ز����������ô�
*             �������ṩ��鰴�������Ƿ�ʵ�֡�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/



#ifndef __WND_FRMKEYCHECK_H__
#define __WND_FRMKEYCHECK_H__

#include "guiglobal.h"

int FrmKeyCheckInit(void *pWndObj);
int FrmKeyCheckExit(void *pWndObj);
int FrmKeyCheckPaint(void *pWndObj);
int FrmKeyCheckLoop(void *pWndObj);
int FrmKeyCheckPause(void *pWndObj);
int FrmKeyCheckResume(void *pWndObj);


#endif