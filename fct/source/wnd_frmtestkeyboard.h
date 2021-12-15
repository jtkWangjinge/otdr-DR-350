/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmtestkeyboard.c
* ժ    Ҫ��  ����������frmtestkeyboard�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�wjg
* ������ڣ�2020/10/9
*
*******************************************************************************/

#ifndef _WND_FRM_TEST_KEYBOARD_H_
#define _WND_FRM_TEST_KEYBOARD_H_

#include "guipicture.h"
#include "guiwindow.h"

// #include "wnd_frmselector.h"
//����ص���������
typedef void(*SECBACKFUNC)(int selected);

#define MAX_KEY_NUM		12

struct KeyBoard_Control
{
	GUIPICTURE* pBg;
	GUIPICTURE* pBtn[MAX_KEY_NUM];
	int iFocus;
	int iEnabled;
	SECBACKFUNC pCallBack;
};

enum KEYBOARD_CODE
{
	KEYBOARD_SHIFT = 0,
	KEYBOARD_OTDR_LEFT,
	KEYBOARD_OTDR_RIGHT,
	KEYBOARD_OTDR_TEST,
	KEYBOARD_UP,
	KEYBOARD_DOWN,
	KEYBOARD_LEFT,
	KEYBOARD_RIGHT,
	KEYBOARD_ENTER,
	KEYBOARD_FILE,
	KEYBOARD_VFL,
	KEYBOARD_ESC
};

/***
  * ���ܣ�
		����һ�������ؼ�
  * ������
		1.int iStartX:			�ؼ�����ʼ������
		2.int iStartY:			�ؼ�����ʼ������
  * ���أ�
		�ɹ�������Чָ�룬ʧ��NULL
  * ��ע��
***/
struct KeyBoard_Control* CreateKeyBoard(int iStartX, int iStartY, SECBACKFUNC CallBack);
/***
  * ���ܣ�
		����һ�������ؼ�
  * ������
		struct KeyBoard_Control** pKeyBoardControl��ָ�򰴼��ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DestroyKeyBoardControl(struct KeyBoard_Control** pKeyBoardControl);
/***
  * ���ܣ�
		��ʾһ�������ؼ�
  * ������
		struct KeyBoard_Control* pKeyBoardControl��ָ�򰴼��ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DisplayKeyBoardControl(struct KeyBoard_Control* pKeyBoardControl);
/***
  * ���ܣ�
		ע��һ�������ؼ�
  * ������
		1.struct KeyBoard_Control* pKeyBoardControl��ָ�򰴼��ؼ���ָ��
		2.GUIWINDOW* pWnd��ָ�����ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int AddKeyBoardControlToWnd(struct KeyBoard_Control* pKeyBoardControl, GUIWINDOW* pWnd);
/***
  * ���ܣ�
		�Ƴ�һ�������ؼ�
  * ������
		1.struct KeyBoard_Control* pKeyBoardControl��ָ�򰴼��ؼ���ָ��
		2.GUIWINDOW* pWnd��ָ�����ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DelKeyBoardControlFromWnd(struct KeyBoard_Control* pKeyBoardControl, GUIWINDOW* pWnd);

#endif
