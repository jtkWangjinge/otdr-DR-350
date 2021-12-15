/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmtemp.h
* ժ    Ҫ��  ����һ������frmotdr(OTDR)�Ĵ��崦���̼߳���ز����������ô�������
*             �ṩOTDR������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _WND_FRMTEMP_H
#define _WND_FRMTEMP_H


/****************************
* Ϊʹ��GUI����Ҫ���õ�ͷ�ļ�
****************************/
// #include "guiglobal.h"

int FrmTempInit(void *pWndObj);
int FrmTempExit(void *pWndObj);
int FrmTempPaint(void *pWndObj);
int FrmTempLoop(void *pWndObj);
int FrmTempPause(void *pWndObj);
int FrmTempResume(void *pWndObj);



int FrmTempSetInfo1(char *pInfo);
int FrmTempSetInfo2(char *pInfo);
void initTraceArea(void);
int GetFlag();
#endif  //_WND_FRMTEMP_H

