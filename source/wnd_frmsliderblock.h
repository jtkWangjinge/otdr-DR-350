/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsliderblock.h
* 摘    要：  实现主窗体frmsliderblock的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/25
*
*******************************************************************************/

#ifndef __WND_FRM_SLIDER_BLOCK_H_
#define __WND_FRM_SLIDER_BLOCK_H_

#include "guipicture.h"
#include "guiwindow.h"

struct Slider_Block
{
	GUIPICTURE* pBtnUp;		//向上箭头
	GUIPICTURE* pBtnDown;	//向下箭头
	GUIPICTURE* pBtnSlider;	//中间的滑块
	int pageNum;			//滑块标识列表的页数（事件列表或文件列表）
	int pageIndex;			//列表当前所在的页的索引（事件列表或文件列表）
};

/***
  * 功能：
		创建一个滑块控件
  * 参数：
		1.int iStartX:			控件的起始横坐标
		2.int iStartY:			控件的起始纵坐标
		3.int iTotalPageNum:	总页数
		4.int iPageIndex:		当前页的索引值
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
struct Slider_Block* CreateSliderBlock(int iStartX, int iStartY, int iTotalPageNum, int iPageIndex);
/***
  * 功能：
		销毁一个滑块控件
  * 参数：
		struct Slider_Block** pSliderBlock：指向滑块控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroySliderBlock(struct Slider_Block** pSliderBlock);
/***
  * 功能：
		显示一个滑块控件
  * 参数：
		struct Slider_Block* pSliderBlock：指向滑块控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplaySliderBlock(struct Slider_Block* pSliderBlock);
/***
  * 功能：
		注册一个滑块控件
  * 参数：
		1.struct Slider_Block** pSliderBlock：指向滑块控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int AddSliderBlockToWnd(struct Slider_Block* pSliderBlock, GUIWINDOW* pWnd);
/***
  * 功能：
		移除一个滑块控件
  * 参数：
		1.struct Slider_Block** pSliderBlock：指向滑块控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DelSliderBlockFromWnd(struct Slider_Block* pSliderBlock, GUIWINDOW* pWnd);

/***
  * 功能：
		刷新滑块控件
  * 参数：
		1.struct Slider_Block** pSliderBlock：指向滑块控件的指针
		2.int pageIndex:当前页所在的索引
		3.int pageNum：总页数
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int FlushSliderBlock(struct Slider_Block* pSliderBlock, int pageIndex, int pageNum);
#endif
