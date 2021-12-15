/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmcurvepara.h
* ժ    Ҫ��  ʵ��������FrmCurvePara�Ĵ��崦���������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�wjg
* ������ڣ�2020/9/19
*
*******************************************************************************/

#ifndef _WND_FRM_CURVE_PARA_H_
#define _WND_FRM_CURVE_PARA_H_

#include "guipicture.h"
#include "guilabel.h"
#include "guibase.h"
#include "guiwindow.h"

#define CURVE_TITLE_NUM		4

typedef struct Otdr_Curve_Para
{
	GUIPICTURE* pBg;
	GUILABEL* pLblTitle[CURVE_TITLE_NUM];
	GUICHAR* pStrTitle[CURVE_TITLE_NUM];
	GUIPICTURE* pBtn[2][3];
	GUILABEL* pLblDataTitle[2][2];
	GUICHAR* pStrDataTitle[2][2];
	GUILABEL* pLblDataContent[2][3];
	GUICHAR* pStrDataContent[2][3];
	int iEnabled;
	int iFocus;
}OTDR_CURVE_PARA;

/***
  * ���ܣ�
		����һ��otdr����ײ��������ý���
  * ������
		��
  * ���أ�
		�ɹ�������Чָ�룬ʧ��NULL
  * ��ע��
***/
OTDR_CURVE_PARA *CreateOtdrCurvePara(void);

/***
  * ���ܣ�
		����OtdrCurvePara�ؼ�
  * ������
		1.OTDR_CURVE_PARA **pOtdrCurveParaObj : ָ��OtdrCurvePara�ؼ�
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DestroyOtdrCurvePara(OTDR_CURVE_PARA **pOtdrCurveParaObj);

/***
  * ���ܣ�
		��ʾOtdrCurvePara�ؼ�
  * ������
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : ָ��pOtdrCurveParaObj�ؼ�
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DisplayOtdrCurvePara(OTDR_CURVE_PARA *pOtdrCurveParaObj);

/***
  * ���ܣ�
		����OtdrCurvePara�ؼ���Ϣ��ʾ
  * ������
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : ָ��pOtdrCurveParaObj�ؼ�
		2.int iEnabled:0,��ʹ�ܣ�1��ʹ�ܣ���ʾ������Ϣ��
		3., GUIWINDOW *pWnd:ע�ᵽ������
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int SetOtdrCurveParaEnable(OTDR_CURVE_PARA *pOtdrCurveParaObj, int iEnabled, GUIWINDOW *pWnd);

/***
  * ���ܣ�
		ˢ��OtdrCurvePara�ؼ�
  * ������
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : ָ��pOtdrCurveParaObj�ؼ�
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int RefreshOtdrCurvePara(OTDR_CURVE_PARA *pOtdrCurveParaObj);

/***
  * ���ܣ�
		��ȡ�Ƿ��ڵ�ǰ����ı�־λ
  * ������
		��
  * ���أ�
		0:���ڵ�ǰ���壬1���ڵ�ǰ���壬����ڰ����Ŀ���
  * ��ע��
***/
int GetCurrentWindow(void);

/***
  * ���ܣ�
		�����ؼ���Ӧ�ص�����
  * ������
		int iSelected: ����ֵ��keyboard_up�ȣ�
  * ���أ�
		��
  * ��ע��
***/
void CurveParaKeyBoardCallBack(int iSelected);

#endif
