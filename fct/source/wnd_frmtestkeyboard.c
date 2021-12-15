/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_frmtestkeyboard.c
* ժ    Ҫ��  ʵ��������frmtestkeyboard�Ĵ��崦���̼߳���ز�������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020/10/9
*
*******************************************************************************/

#include "wnd_frmtestkeyboard.h"
#include "wnd_global.h"

#include "guimessage.h"

static char* bg_key[MAX_KEY_NUM] = 
{
	BmpFileDirectory"bg_key_shift.bmp",
	BmpFileDirectory"bg_key_otdr_left.bmp",
	BmpFileDirectory"bg_key_otdr_right.bmp",
	BmpFileDirectory"bg_key_test.bmp",
	BmpFileDirectory"bg_key_up.bmp",
	BmpFileDirectory"bg_key_down.bmp",
	BmpFileDirectory"bg_key_left.bmp",
	BmpFileDirectory"bg_key_right.bmp",
	BmpFileDirectory"bg_key_enter.bmp",
	BmpFileDirectory"bg_key_file.bmp",
	BmpFileDirectory"bg_key_vfl.bmp",
	BmpFileDirectory"bg_key_esc.bmp"
};

//��ť��Ӧ����
static int WndKeyBoardBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndKeyBoardBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

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
struct KeyBoard_Control* CreateKeyBoard(int iStartX, int iStartY, SECBACKFUNC CallBack)
{
	//�����־����
	int iErr = 0;
	struct KeyBoard_Control* pKeyBoardControl = NULL;

	if (!iErr)
	{
		//������Դ
		pKeyBoardControl = (struct KeyBoard_Control*)calloc(1, sizeof(struct KeyBoard_Control));
		if (NULL == pKeyBoardControl)
		{
			printf("----%s----%d-----pKeyBoardControl Malloc ERR------\n", __func__, __LINE__);
			iErr = -2;
		}
	}

	if (!iErr)
	{
		pKeyBoardControl->pBg = CreatePicture(iStartX, iStartY, 480, 480, BmpFileDirectory"bg_testKeyBoard.bmp");
		pKeyBoardControl->pBtn[KEYBOARD_SHIFT] = CreatePicture(iStartX+5, iStartY+30, 100, 48, bg_key[KEYBOARD_SHIFT]);
		pKeyBoardControl->pBtn[KEYBOARD_OTDR_LEFT] = CreatePicture(iStartX + 5+120, iStartY + 30, 100, 50, bg_key[KEYBOARD_OTDR_LEFT]);
		pKeyBoardControl->pBtn[KEYBOARD_OTDR_RIGHT] = CreatePicture(iStartX + 5 + 120*2, iStartY + 30, 100, 50, bg_key[KEYBOARD_OTDR_RIGHT]);
		pKeyBoardControl->pBtn[KEYBOARD_OTDR_TEST] = CreatePicture(iStartX + 5 + 120 * 3, iStartY + 30, 100, 47, bg_key[KEYBOARD_OTDR_TEST]);

		pKeyBoardControl->pBtn[KEYBOARD_UP] = CreatePicture(iStartX + 5+190, iStartY + 30+100, 100, 55, bg_key[KEYBOARD_UP]);
		pKeyBoardControl->pBtn[KEYBOARD_DOWN] = CreatePicture(iStartX + 5+190, iStartY + 30 + 100+120, 100, 55, bg_key[KEYBOARD_DOWN]);
		pKeyBoardControl->pBtn[KEYBOARD_LEFT] = CreatePicture(iStartX + 5+90, iStartY + 30 + 100+60, 54, 77, bg_key[KEYBOARD_LEFT]);
		pKeyBoardControl->pBtn[KEYBOARD_RIGHT] = CreatePicture(iStartX + 5+190+100+30, iStartY + 30 + 100+60, 54, 77, bg_key[KEYBOARD_RIGHT]);
		pKeyBoardControl->pBtn[KEYBOARD_ENTER] = CreatePicture(iStartX + 5+190, iStartY + 30 + 100 + 60, 100, 52, bg_key[KEYBOARD_ENTER]);

		pKeyBoardControl->pBtn[KEYBOARD_FILE] = CreatePicture(iStartX + 5+50, iStartY + 30+100+100*2, 100, 90, bg_key[KEYBOARD_FILE]);
		pKeyBoardControl->pBtn[KEYBOARD_VFL] = CreatePicture(iStartX + 5+50+140, iStartY + 30 + 100 + 100 * 2, 100, 90, bg_key[KEYBOARD_VFL]);
		pKeyBoardControl->pBtn[KEYBOARD_ESC] = CreatePicture(iStartX + 5 + 50 + 140*2, iStartY + 30 + 100 + 100 * 2, 100, 100, bg_key[KEYBOARD_ESC]);

		pKeyBoardControl->pCallBack = CallBack;
	}

	return pKeyBoardControl;
}

/***
  * ���ܣ�
		����һ�������ؼ�
  * ������
		struct KeyBoard_Control** pKeyBoardControl��ָ�򰴼��ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DestroyKeyBoardControl(struct KeyBoard_Control** pKeyBoardControl)
{
	//�����־����
	int iErr = 0;

	if (*pKeyBoardControl == NULL)
	{
		printf("----%s----%d-----pKeyBoardControl is NULL------\n", __func__, __LINE__);
		iErr = -2;
	}

	if (!iErr)
	{
		DestroyPicture(&((*pKeyBoardControl)->pBg));
		int i;
		for (i = 0; i < MAX_KEY_NUM; ++i)
		{
			DestroyPicture(&((*pKeyBoardControl)->pBtn[i]));
		}
	}

	free(*pKeyBoardControl);
	(*pKeyBoardControl) = NULL;

	return iErr;
}

/***
  * ���ܣ�
		��ʾһ�������ؼ�
  * ������
		struct KeyBoard_Control* pKeyBoardControl��ָ�򰴼��ؼ���ָ��
  * ���أ�
		�ɹ�����0��ʧ�ܷ�0
  * ��ע��
***/
int DisplayKeyBoardControl(struct KeyBoard_Control* pKeyBoardControl)
{
	//�����־����
	int iErr = 0;

	if (pKeyBoardControl == NULL)
	{
		printf("----%s----%d-----pKeyBoardControl is NULL------\n", __func__, __LINE__);
		iErr = -2;
	}

	if (!iErr)
	{
		DisplayPicture(pKeyBoardControl->pBg);
		int i = 0;
		for (i = 0; i < MAX_KEY_NUM; ++i)
		{
			DisplayPicture(pKeyBoardControl->pBtn[i]);
		}
	}

	return iErr;
}

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
int AddKeyBoardControlToWnd(struct KeyBoard_Control* pKeyBoardControl, GUIWINDOW* pWnd)
{
	//�����־����
	int iErr = 0;

	if ((NULL == pKeyBoardControl) || (NULL == pWnd))
	{
		printf("----%s----%d-----pKeyBoardControl is NULL------\n", __func__, __LINE__);
		iErr = -2;
	}

	if (!iErr)
	{
		GUIMESSAGE *pMsg = GetCurrMessage();;
		int i;
		for (i = 0; i < MAX_KEY_NUM; ++i)
		{		
			//addwnd
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				pKeyBoardControl->pBtn[i], pWnd);
			//logreg
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pKeyBoardControl->pBtn[i],
				WndKeyBoardBtn_Down, pKeyBoardControl, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pKeyBoardControl->pBtn[i],
				WndKeyBoardBtn_Up, pKeyBoardControl, i, pMsg);
		}

	}

	return iErr;
}

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
int DelKeyBoardControlFromWnd(struct KeyBoard_Control* pKeyBoardControl, GUIWINDOW* pWnd)
{
	//�����־����
	int iErr = 0;

	if ((pKeyBoardControl == NULL) || (pWnd == NULL))
	{
		printf("----%s----%d-----pKeyBoardControl is NULL------\n", __func__, __LINE__);
		return -1;
	}
	
	if (!iErr)
	{
		int i;
		for (i = 0; i < MAX_KEY_NUM; ++i)
		{		
			//�ӿؼ�����ժ�¿ؼ�
			DelWindowComp(pKeyBoardControl->pBtn[i], pWnd);
			//ע���Ͽؼ�����Ϣ���� 
			GUIMESSAGE *pMsg = GetCurrMessage();
			//ѡ���Ϣע��
			LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pKeyBoardControl->pBtn[i], pMsg);
			LogoutMessageReg(GUIMESSAGE_TCH_UP, pKeyBoardControl->pBtn[i], pMsg);
		}
	}

	return iErr;
}

//������Ӧ����
int WndKeyBoardBtn_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	return 0;
}

int WndKeyBoardBtn_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	struct KeyBoard_Control* pKeyBoardControl = (struct KeyBoard_Control*)pOutArg;

	if (pKeyBoardControl->pCallBack)
	{
		(*(pKeyBoardControl->pCallBack))(iOutLen);
	}

	return 0;
}
