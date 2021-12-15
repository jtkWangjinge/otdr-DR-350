/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmcurveresult.h
* ժ    Ҫ��  ʵ��������frmcurveresult�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�wjg
* ������ڣ�2020/9/26
*
*******************************************************************************/

#ifndef _WND_FRM_CURVE_RESULT_H_
#define _WND_FRM_CURVE_RESULT_H_

#include "guipicture.h"
#include "guilabel.h"
#include "guibase.h"
#include "guiwindow.h"

#define CURVE_RESULT_TITLE_NUM		4

struct Curve_Result
{
	GUIPICTURE* pBg;
	GUILABEL* pLblFiberLength;
	GUILABEL* pLblFiberLengthValue;		//����λ
	GUILABEL* pLblTableTitle[CURVE_RESULT_TITLE_NUM];
	GUILABEL* pLblWaveInfo[2][CURVE_RESULT_TITLE_NUM];
	GUICHAR* pStrFiberLength;
	GUICHAR* pStrFiberLengthValue;
	GUICHAR* pStrTableTitle[CURVE_RESULT_TITLE_NUM];
	GUICHAR* pStrWaveInfo[2][CURVE_RESULT_TITLE_NUM];
	int iEnable;
};

/***
  * ���ܣ�
		����һ�����߽���ؼ�
  * ������
		1.int iStartX:			�ؼ�����ʼ������
		2.int iStartY:			�ؼ�����ʼ������
  * ���أ�
		�ɹ�������Чָ�룬ʧ��NULL
  * ��ע��
***/
struct Curve_Result* CreateCurveResult(int iStartX, int iStartY);
/***
  * ���ܣ�
		����һ�����߽���ؼ�
  * ������
		struct Curve_Result** pCurveResult��ָ�����߽���ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DestroyCurveResult(struct Curve_Result** pCurveResult);
/***
  * ���ܣ�
		��ʾһ�����߽���ؼ�
  * ������
		struct Curve_Result* pCurveResult��ָ�����߽���ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DisplayCurveResult(struct Curve_Result* pCurveResult);
/***
  * ���ܣ�
		ע��һ�����߽���ؼ�
  * ������
		1.struct Curve_Result* pCurveREsult��ָ�����߽���ؼ���ָ��
		2.GUIWINDOW* pWnd��ָ�����ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int AddCurveResultToWnd(struct Curve_Result* pCurveResult, GUIWINDOW* pWnd);
/***
  * ���ܣ�
		�Ƴ�һ�����߽���ؼ�
  * ������
		1.struct Curve_Result* pCurveResult��ָ�����߽���ؼ���ָ��
		2.GUIWINDOW* pWnd��ָ�����ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DelCurveResultFromWnd(struct Curve_Result* pCurveResult, GUIWINDOW* pWnd);

/***
  * ���ܣ�
		ˢ�����߽���ؼ�
  * ������
		1.struct Curve_Result* pCurveResult��ָ�����߽���ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int FlushCurveResult(struct Curve_Result* pCurveResult);

//���ÿؼ���ʹ��״̬
int SetCurveResultEnable(struct Curve_Result* pCurveResult, int iEnable, GUIWINDOW *pWnd);

#endif
