/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_droppicker.h
* 摘    要：  上拉列表
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_DROPPICKER_H
#define _WND_DROPPICKER_H

#include "guiglobal.h"

/*******************************************************************************
***                定义wnd_droppicker.h公有的宏定义                       ***
*******************************************************************************/
#define DROP_MAX  15

//上啦列表的回调函数
typedef void (*DROPACKFUNC)(int selected);

/*******************************************************************************
***                    定义wnd_droppicker.h结构定义                          ***
*******************************************************************************/
typedef struct _wnd_drop_picker{
	GUIPICTURE *pDropItemBg[DROP_MAX];				//背景
	GUILABEL *pDropItemLbl[DROP_MAX];				//中心
	GUICHAR    *pDropItemStr[DROP_MAX]; 			//选项控件的图片

	GUILABEL *pDropLayerLbl;						//图层

	DROPACKFUNC BackCallFunc;						//回调函数
	GUIWINDOW *pWnd;
	
	int 		iEnable[DROP_MAX];  				//每个标签的使能状态
	int 		iItemCnt;							//标签的个数
	int 		iFocus;								//焦点的位置,传入-1 则不处理焦点
}DROPPICKER;


/*******************************************************************************
***               定义wnd_droppicker.h对外提供的函数声明                     ***
*******************************************************************************/
//生成上拉列表
DROPPICKER * CreateDropPicker(int x, int y, int iNum, char **str,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd);

DROPPICKER * CreateDropList(int x, int y, int size, unsigned int *ItemStr,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd);

//生成下拉列表
DROPPICKER * CreateDropPicker1(int x, int y, int iNum, char **str1, int *str2,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd);

DROPPICKER * CreateDropList1(int x, int y, int size, unsigned int *ItemStr,
							  int *pEnable,int iSelect, DROPACKFUNC BackCall,
							  GUIWINDOW *pWnd);

//销毁
int DestroyDropList();
#endif

