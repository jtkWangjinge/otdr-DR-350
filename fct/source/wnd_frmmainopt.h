/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmmainopt.h
* ժ    Ҫ��  ����������frmmainopt�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
*******************************************************************************/

#ifndef _WND_FRMMAINOPT_H
#define _WND_FRMMAINOPT_H


/****************************
* Ϊʹ��GUI����Ҫ���õ�ͷ�ļ�
****************************/
// #include "guiglobal.h"


/*********************
* �������崦����غ���
*********************/
//����frmmain�ĳ�ʼ����������������ؼ���ע����Ϣ����
int FrmMainOptInit(void *pWndObj);
//����frmmain���˳��������ͷ�������Դ
int FrmMainOptExit(void *pWndObj);
//����frmmain�Ļ��ƺ�����������������
int FrmMainOptPaint(void *pWndObj);
//����frmmain��ѭ�����������д���ѭ��
int FrmMainOptLoop(void *pWndObj);
//����frmmain�Ĺ����������д������ǰԤ����
int FrmMainOptPause(void *pWndObj);
//����frmmain�Ļָ����������д���ָ�ǰԤ����
int FrmMainOptResume(void *pWndObj);

#endif  //_WND_FRMMAINOPT_H

