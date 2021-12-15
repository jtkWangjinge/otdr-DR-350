/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guiglobal.h
* ժ    Ҫ��  ����GUI�Ĺ������ݼ�������ز��������ṩ����GUI�����ĳ�ʼ�����á�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  yexin.zhu
* ������ڣ�  2012-8-1
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_GLOBAL_H
#define _GUI_GLOBAL_H


/**************************************
* Ϊ����GUIGLOBAL����Ҫ���õ�����ͷ�ļ�
**************************************/
#include "guibase.h"
#include "guifbmap.h"
#include "guikeypad.h"
#include "guitouch.h"
#include "guievent.h"
#include "guimessage.h"
#include "guischeduler.h"
#include "guibrush.h"
#include "guifont.h"
#include "guipalette.h"
#include "guipen.h"
#include "guidraw.h"
#include "guiimage.h"
#include "guibitmap.h"
#include "guitext.h"
#include "guilabel.h"
#include "guipicture.h"
#include "guiwindow.h"


/********************************
* ����GUI�е�ǰʹ�õİ���ӳ����ֵ
********************************/
#define KEYCODE_File    	144
#define KEYCODE_HOME    	139
#define KEYCODE_Setup   	141
#define KEYCODE_BACK    	1 

#define KEYCODE_up      	103
#define KEYCODE_down    	108
#define KEYCODE_left    	105
#define KEYCODE_right   	106
#define KEYCODE_OTDR   	    28

#define KEYCODE_Start   	315
/**********************************
* ������GUI������ʼ����صĲ�������
**********************************/
//��ʼ��GUI����������GUI��Դ
int InitializeGUI(void);
//�ͷ�GUI������������Դ
int ReleaseGUI(void);


#endif  //_GUI_GLOBAL_H

