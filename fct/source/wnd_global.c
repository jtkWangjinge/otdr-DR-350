/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  wnd_global.c
* ժ    Ҫ��  ʵ�ָ�����Ĺ������ݡ���������������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2012-12-31
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "wnd_global.h"


/***************************************
* Ϊʵ��wnd_global����Ҫ���õ�����ͷ�ļ�
***************************************/
//#include "app_global.h"
#include <sys/types.h>
#include <sys/wait.h>

/***
  * ���ܣ�
        ������Ϣ�Ա��˳���ǰ����
  * ������
        1.GUIWINDOW *pWndObj:   ��Ҫ�˳��ĵ�ǰ�������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SendMsg_ExitWindow(GUIWINDOW *pWndObj)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //���ʹ�����ϢGUIMESSAGE_WND_EXIT������
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_WND_EXIT;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, pMsg); //pItem�ɱ��������Ϣ�����ͷ�
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * ���ܣ�
        ������Ϣ�Ա�����µĴ���
  * ������
        1.GUIWINDOW *pWndObj:   ��Ҫ���õ��µĴ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SendMsg_CallWindow(GUIWINDOW *pWndObj)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //����ϵͳ��ϢGUIMESSAGE_THD_CREAT�������߳��Ա�����´���
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_SYS;
    pItem->iMsgCode = GUIMESSAGE_THD_CREAT;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, GetCurrMessage()); //pItem�ɵ����߳��ͷ�
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * ���ܣ�
        ������Ϣ�Ա�������������ѭ��
  * ������
        1.GUIWINDOW *pWndObj:   ��Ҫ������������ѭ���ĵ�ǰ�������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SendMsg_EnableLoop(GUIWINDOW *pWndObj)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //����ϵͳ��ϢGUIMESSAGE_THD_CREAT�������߳��Ա�����´���
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_LOP_ENABLE;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, GetCurrMessage()); //pItem�ɵ����߳��ͷ�
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * ���ܣ�
        ������Ϣ�Ա��ֹ��ֹͣ����ѭ��
  * ������
        1.GUIWINDOW *pWndObj:   ��Ҫ��ֹ��ֹͣ����ѭ���ĵ�ǰ�������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SendMsg_DisableLoop(GUIWINDOW *pWndObj)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //����ϵͳ��ϢGUIMESSAGE_THD_CREAT�������߳��Ա�����´���
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_LOP_DISABLE;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, GetCurrMessage()); //pItem�ɵ����߳��ͷ�
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * ���ܣ�
        ��ʾ���ܰ�ť����
  * ������
        1.GUIPICTURE *pBtnFx:   ��Ҫ��ʾ�Ĺ��ܰ�ťͼ�ο�
        2.GUILABEL *pLblFx:     ��Ҫ��ʾ�Ĺ��ܰ�ť��ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ShowBtn_FuncDown(GUIPICTURE *pBtnFx, GUILABEL *pLblFx)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"btn_active.bmp", pBtnFx);
    DisplayPicture(pBtnFx);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x003C3028, 0xFFFFFFFF, pFont);
    DisplayLabel(pLblFx);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


/***
  * ���ܣ�
        ��ʾ���ܰ�ť����
  * ������
        1.GUIPICTURE *pBtnFx:   ��Ҫ��ʾ�Ĺ��ܰ�ťͼ�ο�
        2.GUILABEL *pLblFx:     ��Ҫ��ʾ�Ĺ��ܰ�ť��ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ShowBtn_FuncUp(GUIPICTURE *pBtnFx, GUILABEL *pLblFx)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������

    SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pBtnFx);
    DisplayPicture(pBtnFx);

    DisplayLabel(pLblFx);

    return iReturn;
}


/***
  * ���ܣ�
        ��ʾ��ǰ����
  * ������
        1.GUILABEL *pLblDate:   ��Ҫ��ʾ�ĵ�ǰ���ڱ�ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ShowSta_CurrDate(GUILABEL *pLblDate)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������

    return iReturn;
}


/***
  * ���ܣ�
        ��ʾ��ǰʱ��
  * ������
        1.GUILABEL *pLblTime:   ��Ҫ��ʾ�ĵ�ǰʱ���ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ShowSta_CurrTime(GUILABEL *pLblTime)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������

    return iReturn;
}


/***
  * ���ܣ�
        ��ʾLCD��������
  * ������
        1.GUILABEL *pLblLight:  ��Ҫ��ʾ�ĵ�ǰ���ȱ�ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ShowSta_LcdLight(GUILABEL *pLblLight)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������

    return iReturn;
}


/***
  * ���ܣ�
        ��ʾ��Դ��Ϣ
  * ������
        1.GUIPICTURE *pIcoPower:    ��Ҫ��ʾ�ĵ�Դ״̬ͼ�ο�
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ShowSta_PowerInfo(GUIPICTURE *pIcoPower)
{
    //�����־������ֵ����
    int iReturn = 0;
    //��ʱ��������

    return iReturn;
}

/***
  * ���ܣ�
        ����LABEL���ı���Դ
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int UpdateLabelRes(GUICHAR *pTargetStr, GUILABEL *pTargetLbl, GUIPICTURE *pBackGround)
{
    //�����־������ֵ����
	int iReturn = 0;
	GUIPEN *pPen = NULL;
	GUIRECTANGLE *pRectangle = NULL;
	unsigned int uiColorBack = 0;
	
	if (iReturn == 0)
	{
		//�������
		if ((NULL == pTargetLbl) || (NULL == pTargetStr))
		{
			iReturn = -1;
		}
	}
	
	if (iReturn == 0)
	{
		//����ͼƬ��ΪNULL������Ҫ�ػ汳��
		if (pBackGround != NULL)
		{
			DisplayPicture(pBackGround);
		}
		//����ΪNULL�����ػ��ɫ����Ϊ����
		else
		{
			pPen = GetCurrPen();
			uiColorBack = pPen->uiPenColor;
			pPen->uiPenColor = 0;
			pRectangle = &(pTargetLbl->Visible.Area);
			DrawBlock(pRectangle->Start.x, pRectangle->Start.y, pRectangle->End.x, pRectangle->End.y);
			pPen->uiPenColor = uiColorBack;
		}
		
		//����ǩ�Ƿ�ɼ�
		if (pTargetLbl->Visible.iEnable == 0)
		{
			iReturn = -2;
		}
	}
	
	if (iReturn == 0)
	{
		SetLabelText(pTargetStr, pTargetLbl);
		DisplayLabel(pTargetLbl);
	}

	return iReturn;
}


/***
  * ���ܣ�
		�Լ�ʵ��systemϵͳ����
  * ������
		1.const char * cmdstring:	 ��Ҫִ�е�����
  * ���أ�
		�ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int my_system(const char * cmdstring) 
{ 
	pid_t pid; 
	int status; 
	
	if(cmdstring == NULL) 
	{ 
		return (1); //���cmdstringΪ�գ����ط���ֵ��һ��Ϊ1 
	} 

	if((pid = vfork())<0) 
	{ 
		status = -1; //forkʧ�ܣ�����-1 
	} 
	else if(pid == 0) 
	{ 
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0); 
		_exit(127); // execִ��ʧ�ܷ���127��ע��execֻ��ʧ��ʱ�ŷ������ڵĽ��̣��ɹ��Ļ����ڵĽ��̾Ͳ�������~~ 
	} 
	else //������ 
	{ 
		while(waitpid(pid, &status, 0) < 0) 
		{ 
			if(errno != EINTR) 
			{ 
				status = -1; //���waitpid���ź��жϣ��򷵻�-1 
				break; 
			} 
		} 

	} 

	return status; //���waitpid�ɹ����򷵻��ӽ��̵ķ���״̬ 
} 

