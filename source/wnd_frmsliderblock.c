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

#include "wnd_frmsliderblock.h"

#include "app_global.h"

#include "guimessage.h"
#include "guidevice.h"

//向上箭头按下响应函数
static int WndSliderBlockBtnUp_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//向下箭头按下响应函数
static int WndSliderBlockBtnDown_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//设置滑块背景
static void SetSliderBackground(struct Slider_Block* pSliderBlock);
//设置当前滑块所在位置
static void SetCurrentSliderIndex(struct Slider_Block* pSliderBlock, int option);

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
struct Slider_Block* CreateSliderBlock(int iStartX, int iStartY, int iTotalPageNum, int iPageIndex)
{
	//错误标志定义
	int iErr = 0;
	struct Slider_Block* pSliderBlock = NULL;

	if (!iErr)
	{
		//分配资源
		pSliderBlock = (struct Slider_Block*)calloc(1, sizeof(struct Slider_Block));
		if (NULL == pSliderBlock)
		{
			LOG(LOG_ERROR, "pSliderBlock Malloc ERR\n");
			iErr = -2;
		}
	}

	if (!iErr)
	{
		pSliderBlock->pBtnUp = CreatePicture(iStartX, iStartY, 20, 20, BmpFileDirectory"btn_slider_up_unpress.bmp");
		pSliderBlock->pBtnSlider = CreatePicture(iStartX, iStartY+21, 20, 77, BmpFileDirectory"bg_slider_full.bmp");
		pSliderBlock->pBtnDown = CreatePicture(iStartX, iStartY+21+77+1, 20, 20, BmpFileDirectory"btn_slider_down_unpress.bmp");
		pSliderBlock->pageNum = iTotalPageNum;
		pSliderBlock->pageIndex = iPageIndex;
	}

	return pSliderBlock;
}

/***
  * 功能：
		销毁一个滑块控件
  * 参数：
		struct Slider_Block** pSliderBlock：指向滑块控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroySliderBlock(struct Slider_Block** pSliderBlock)
{
	//错误标志定义
	int iErr = 0;

	if (*pSliderBlock == NULL)
	{
		LOG(LOG_ERROR, "pSliderBlock is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		DestroyPicture(&((*pSliderBlock)->pBtnUp));
		DestroyPicture(&((*pSliderBlock)->pBtnSlider));
		DestroyPicture(&((*pSliderBlock)->pBtnDown));
	}

	free(*pSliderBlock);
	(*pSliderBlock) = NULL;

	return iErr;
}

/***
  * 功能：
		显示一个滑块控件
  * 参数：
		struct Slider_Block* pSliderBlock：指向滑块控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplaySliderBlock(struct Slider_Block* pSliderBlock)
{
	//错误标志定义
	int iErr = 0;

	if (pSliderBlock == NULL)
	{
		LOG(LOG_ERROR, "pSliderBlock is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		DisplayPicture(pSliderBlock->pBtnUp);
		SetSliderBackground(pSliderBlock);
		DisplayPicture(pSliderBlock->pBtnSlider);
		DisplayPicture(pSliderBlock->pBtnDown);
	}

	return iErr;
}

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
int AddSliderBlockToWnd(struct Slider_Block* pSliderBlock, GUIWINDOW* pWnd)
{
	//错误标志定义
	int iErr = 0;
	GUIMESSAGE *pMsg = GetCurrMessage();;

	if ((NULL == pSliderBlock) || (NULL == pWnd))
	{
		LOG(LOG_ERROR, "pSliderBlock is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
			pSliderBlock->pBtnUp, pWnd);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
			pSliderBlock->pBtnDown, pWnd);

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSliderBlock->pBtnUp,
			WndSliderBlockBtnUp_Down, pSliderBlock, 0, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSliderBlock->pBtnDown,
			WndSliderBlockBtnDown_Down, pSliderBlock, 0, pMsg);
	}

	return iErr;
}
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
int DelSliderBlockFromWnd(struct Slider_Block* pSliderBlock, GUIWINDOW* pWnd)
{
	//错误标志定义
	int iErr = 0;
	GUIMESSAGE *pMsg = GetCurrMessage();;

	if ((NULL == pSliderBlock) || (NULL == pWnd))
	{
		LOG(LOG_ERROR, "pSliderBlock is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		DelWindowComp(pSliderBlock->pBtnUp, pWnd);
		DelWindowComp(pSliderBlock->pBtnDown, pWnd);

		LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pSliderBlock->pBtnUp, pMsg);
		LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pSliderBlock->pBtnDown, pMsg);
	}

	return iErr;
}

/************************************************************************************
****************************        内部函数实现     ********************************
************************************************************************************/

//向上箭头按下响应函数
static int WndSliderBlockBtnUp_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	struct Slider_Block *pSliderBlock = (struct Slider_Block *)pOutArg;
	SetCurrentSliderIndex(pSliderBlock, 0);
	DisplayPicture(pSliderBlock->pBtnSlider);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return 0;
}

//向下箭头按下响应函数
static int WndSliderBlockBtnDown_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	struct Slider_Block *pSliderBlock = (struct Slider_Block *)pOutArg;
	SetCurrentSliderIndex(pSliderBlock, 1);
	DisplayPicture(pSliderBlock->pBtnSlider);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

//设置滑块背景
static void SetSliderBackground(struct Slider_Block* pSliderBlock)
{
	int pageIndex = pSliderBlock->pageIndex;
	int pageNum = pSliderBlock->pageNum;

	if (pageIndex == 0)
	{
		if (pageNum <= 1)
		{
			SetPictureBitmap(BmpFileDirectory"bg_slider_full.bmp", pSliderBlock->pBtnSlider);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"bg_slider_quarter.bmp", pSliderBlock->pBtnSlider);
		}
	}
	else if (pageIndex < (pageNum-1))
	{
		SetPictureBitmap(BmpFileDirectory"bg_slider_half.bmp", pSliderBlock->pBtnSlider);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"bg_slider_three_quarter.bmp", pSliderBlock->pBtnSlider);
	}
}

//设置当前滑块所在位置
static void SetCurrentSliderIndex(struct Slider_Block* pSliderBlock, int option)
{
	int* index = &(pSliderBlock->pageIndex);

	switch (option)
	{
	case 0://向上箭头
		if (*index != 0)
		{
			(*index)--;
		}
		break;
	case 1://向下箭头
		if (*index != (pSliderBlock->pageNum -1))
		{
			(*index)++;
		}
	default:
		break;
	}
}

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
int FlushSliderBlock(struct Slider_Block* pSliderBlock, int pageIndex, int pageNum)
{
	pSliderBlock->pageIndex = pageIndex;
	pSliderBlock->pageNum = pageNum;
	DisplaySliderBlock(pSliderBlock);

	return 0;
}