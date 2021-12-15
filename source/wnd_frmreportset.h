/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmreportset.h
* ժ    Ҫ��  ʵ��������FrmReportSet�Ĵ��崦�������������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�wjg
* ������ڣ�2020/9/7
*
*******************************************************************************/

#ifndef _WND_FRM_REPORTSET_H_
#define _WND_FRM_REPORTSET_H_

/*******************************************************************************
**							�������崦����غ���							  **
*******************************************************************************/

/***
  * ���ܣ�
		����frmreportset�ĳ�ʼ������
  * ������
		void *pWndObj:	ָ��ǰ�������
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
		��������ؼ���ע����Ϣ����
***/
int FrmReportSetInit(void *pWndObj);

/***
  * ���ܣ�
		����frmreportset���˳�����
  * ������
		void *pWndObj:	ָ��ǰ�������
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
		�ͷ�������Դ
***/
int FrmReportSetExit(void *pWndObj);

/***
  * ���ܣ�
		����frmreportset�Ļ��ƺ���
  * ������
		void *pWndObj:	ָ��ǰ�������
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int FrmReportSetPaint(void *pWndObj);

/***
  * ���ܣ�
		����frmreportset��ѭ������
  * ������
		void *pWndObj:	ָ��ǰ�������
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int FrmReportSetLoop(void *pWndObj);

/***
  * ���ܣ�
		����frmreportset�Ĺ�����
  * ������
		void *pWndObj:	ָ��ǰ�������
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int FrmReportSetPause(void *pWndObj);

/***
  * ���ܣ�
		����frmreportset�Ļָ�����
  * ������
		void *pWndObj:	ָ��ǰ�������
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int FrmReportSetResume(void *pWndObj);


#endif
