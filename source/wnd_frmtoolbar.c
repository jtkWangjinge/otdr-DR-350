/*******************************************************************************
* Copyright(c)2015，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmtoolbar.c
* 摘    要：  提供otdr窗体左侧工具栏
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015.04.17
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmtoolbar.h"
#include "app_frmfilebrowse.h"
#include "wnd_frmcoor.h"
#include "wnd_frmotdr.h"
/*************************************
* 定义frmtoolbar控件事件表所需要的变量
*************************************/
static char *ppLeftToolsPicUnpress[] = //定义左侧工具栏按钮图片
{
    BmpFileDirectory"btn_otdr_hand_unpress.bmp",
    BmpFileDirectory"btn_otdr_ezoom_unpress.bmp",
    BmpFileDirectory"btn_otdr_11_unpress.bmp",
    BmpFileDirectory"btn_otdr_zoomin_unpress.bmp",
    BmpFileDirectory"btn_otdr_zoomout_unpress.bmp",
    BmpFileDirectory"btn_otdr_full_unpress.bmp",
    BmpFileDirectory"btn_otdr_half_unpress.bmp",
};
static char *ppLeftToolsPicPress[] = //定义左侧工具栏按钮图片
{
    BmpFileDirectory"btn_otdr_hand_press.bmp",
    BmpFileDirectory"btn_otdr_ezoom_press.bmp",
    BmpFileDirectory"btn_otdr_11_press.bmp",
    BmpFileDirectory"btn_otdr_zoomin_press.bmp",
    BmpFileDirectory"btn_otdr_zoomout_press.bmp",
    BmpFileDirectory"btn_otdr_full_press.bmp",
    BmpFileDirectory"btn_otdr_half_press.bmp",
};

/*****************************************
* 声明frmtoolbar控件内部所使用事件处理函数
*****************************************/
//static int WndToolBar_Down(void *pInArg, int iInLen,
//                           void *pOutArg, int iOutLen);
//static int WndToolBar_Up(void *pInArg, int iInLen,
//                         void *pOutArg, int iOutLen);                      


//创建一个工具栏
WNDTOOLBAR *CreateToolBar(TBACTION pDnAction, TBACTION pUpAction,
						  GUIWINDOW *pWnd)
{
    //临时变量定义
    int iTmp = 0;
    WNDTOOLBAR *pTBObj = NULL;

	//检查参数
	if (NULL == pWnd)
	{
		LOG(LOG_ERROR, "Invaild args --\n");
		return NULL;
	}

	//分配资源
	pTBObj = (WNDTOOLBAR *)calloc(1, sizeof(WNDTOOLBAR));
	if (NULL == pTBObj)
	{
		LOG(LOG_ERROR, "malloc err --\n");
		return NULL;
	}

	//初始化结构体
	pTBObj->pDnAction = pDnAction;
	pTBObj->pUpAction = pUpAction;

    //创建窗体中左侧工具按键控件6个
	for (iTmp = 0; iTmp < TOOLS_NUM; ++iTmp) 
	{
		pTBObj->pEnable[iTmp] = 1;	//默认使能
		if(iTmp == 0)
		{
            pTBObj->pToolBtn[iTmp] = CreatePicture(0, 40 + iTmp * 47, 40, 48,
                                               ppLeftToolsPicUnpress[iTmp]);
        }
        else if(iTmp == 5)
        {
            pTBObj->pToolBtn[iTmp] = CreatePicture(0, 40 + iTmp * 47 + 1, 40, 48,
                                               ppLeftToolsPicUnpress[iTmp]);
        }
        else
        {
            pTBObj->pToolBtn[iTmp] = CreatePicture(0, 40 + iTmp * 47 + 1, 40, 47,
                                               ppLeftToolsPicUnpress[iTmp]);
        }
	}

    if(COOR_LARGE == GetOtdrCurrWnd())
    {
        SetPictureBitmap(ppLeftToolsPicUnpress[6], pTBObj->pToolBtn[5]);
    }
    
	//添加控件到接受消息队列
	for (iTmp = 0; iTmp < TOOLS_NUM; ++iTmp)
	{
// 		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
// 					  pTBObj->pToolBtn[iTmp], pWnd);
	}
	
	//注册消息
// 	GUIMESSAGE *pMsg = GetCurrMessage();
	for (iTmp = 0; iTmp < TOOLS_NUM; ++iTmp) 
	{
// 		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTBObj->pToolBtn[iTmp], 
//                     	WndToolBar_Down, (void *)pTBObj, iTmp, pMsg);
// 		LoginMessageReg(GUIMESSAGE_TCH_UP, pTBObj->pToolBtn[iTmp], 
//                     	WndToolBar_Up, (void *)pTBObj, iTmp, pMsg);
	}

    return pTBObj;
}

//销毁工具栏
int DestroyToolBar(WNDTOOLBAR **pTBObj)
{
	int iTmp = 0;
	
	if (NULL == *pTBObj)
	{
		return -1;
	}

	for (iTmp = 0; iTmp < TOOLS_NUM; ++iTmp)
	{
		DestroyPicture(&((*pTBObj)->pToolBtn[iTmp]));
	}

	GuiMemFree(*pTBObj);
	*pTBObj = NULL;

	return 0;
}

//绘制工具栏
int DisplayToolBar(WNDTOOLBAR *pTBObj)
{
	int iTmp = 0;

	if (NULL == pTBObj)
	{
		return -1;
	}

	for (iTmp = 0; iTmp < TOOLS_NUM; ++iTmp)
	{
		DisplayPicture(pTBObj->pToolBtn[iTmp]);
	}

	return 0;
}


/***
  * 功能：
     	设置工具栏的图标状态
  * 参数：
  		int 
  		int iNum:       对应的第几个图标  0~5
  		int iStatus:    0----unpress状态     1----press状态
  * 返回：
        成功返0，失败返回-1
  * 备注：
***/ 
int SetPicStatus(int iNum, int iStatus, WNDTOOLBAR *pTBObj)
{
	if(iNum < 0 || iNum > 6)
		return -1;

	if(!pTBObj)
	{
		return -1;
	}
	
	if(iStatus == 0)
	{
		SetPictureBitmap(ppLeftToolsPicUnpress[iNum], pTBObj->pToolBtn[iNum]);
	}
	else if(iStatus == 1)
	{
		SetPictureBitmap(ppLeftToolsPicPress[iNum], pTBObj->pToolBtn[iNum]);
	}
	else
		return -1;
	
	//RefreshScreen(__FILE__, __func__, __LINE__);
	
	return 0;
}

//工具栏按下处理
//static int wndtoolbar_down(void *pinarg, int iinlen, 
//                           void *poutarg, int ioutlen)
//{
//	wndtoolbar *ptbobj = (wndtoolbar *)poutarg;
//	int iindex = ioutlen;
//
//	//如果不使能 则返回
//	if (0 == ptbobj->penable[iindex])
//	{
//		return 0;
//	}
//	
//	//改变状态
//	if(iindex == 5)
//	{
//	    setpicturebitmap(pplefttoolspicpress[iindex+getotdrcurrwnd()], ptbobj->ptoolbtn[iindex]);
//    }
//    else
//    {
//	    setpicturebitmap(pplefttoolspicpress[iindex], ptbobj->ptoolbtn[iindex]);
//    }
//	displaypicture(ptbobj->ptoolbtn[iindex]);
//	refreshscreen(__file__, __func__, __line__);
//
//	//调用回调
//	if (ptbobj->pdnaction)
//	{
//		ptbobj->pdnaction(iindex);
//	}
//
//	return 0;
//}
//
//
////工具栏弹起事件处理
//static int wndtoolbar_up(void *pinarg, int iinlen, 
//                         void *poutarg, int ioutlen)
//{
//	wndtoolbar *ptbobj = (wndtoolbar *)poutarg;
//	int iindex = ioutlen;
//
//	//如果不使能 则返回
//	if (0 == ptbobj->penable[iindex])
//	{
//		return 0;
//	}
//	
//	//改变状态
//	if(iindex == 5)
//	{
//	    setpicturebitmap(pplefttoolspicunpress[iindex+1-getotdrcurrwnd()], ptbobj->ptoolbtn[iindex]);
//    }
//    else
//    {
//	    setpicturebitmap(pplefttoolspicunpress[iindex], ptbobj->ptoolbtn[iindex]);
//    }
//
//	displaypicture(ptbobj->ptoolbtn[iindex]);
//	refreshscreen(__file__, __func__, __line__);
//
//	//调用回调
//	if (ptbobj->pupaction)
//	{
//		ptbobj->pupaction(iindex);
//	}
//
//	return 0;
//}
