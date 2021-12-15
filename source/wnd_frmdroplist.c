/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmdroplist.c
* 摘    要：  实现下拉列表控件
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014-09-01
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmdroplist.h"


/*******************************************************************************
**					 为实现窗体frmdroplist而需要引用的其他头文件 			  **
*******************************************************************************/
#include "wnd_global.h"
#include "guiglobal.h"
#include "wnd_global.h"
#include "app_global.h"
#include <string.h>


/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
static int iDropListKeyFlag = 0;
static int DROPLIST_WINDOW_X = 618;					//下拉框的左上角的点横坐标
static int DROPLIST_WINDOW_Y = 0;					//下拉框的左上角的点纵坐标
static int i = 0, j = 0, k = 0;						//临时辅助变量
static int GiActualNum = 0;							//实际的下拉菜单项数目
static int GiActualPageNum = 0;						//实际的下拉菜单项页数
static int GiActualPagePosition = 0;				//实际的下拉菜单项页位置
static unsigned char ucInactiveFlg = 0;				//是否有无效窗体标志
static unsigned int uiAreaPlaceX;					//指定区域的X坐标 
static unsigned int uiAreaPlaceY; 					//指定区域的Y坐标
static unsigned int uiAreaWidth;					//指定区域的宽度
static unsigned int uiAreaHeight;					//指定区域的高度
int iMenuItemSelected = -1;							//保存下拉列表哪项被选中了
													//初始值为-1，代表没有子项
													//被选中
static CALLLBACKHANDLE pResumeFunc = NULL;
static int SelectItem[DROPLISTMAXNUM] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};   	//记录当前选中的是第几项

/* 存贮当前位于哪一页的图片的路径 */
char *pPositionPath[] = {							
BmpFileDirectory"bg_position1.bmp", BmpFileDirectory"bg_position2.bmp",
BmpFileDirectory"bg_position3.bmp", BmpFileDirectory"bg_position4.bmp"};


/*******************************************************************************
**							窗体frmdroplist中的控件定义部分					  **
*******************************************************************************/
static GUIWINDOW *pCurWindow = NULL;


/**********************************背景控件定义********************************/ 
static GUIPICTURE *pDroplistBg = NULL;


/******************************窗体标题栏控件定义******************************/


/****************************下拉列表内的控件定义******************************/
static GUICHAR *pDroplistStrTitle = NULL; 			//标题栏的文本
static GUILABEL *pDroplistlblTitle = NULL;			//标题栏的Label
static GUIPICTURE *pDroplistBtnBack = NULL;     	//返回按钮的Picture

static GUIPICTURE *pDroplistBtnSplitline = NULL;	//分割线的Picture

GUICHAR *pStrMenuItem[DROPLISTMAXNUM];				//用于存放下拉列表每项的文本
static GUILABEL *pLblDroplist[DROPLISTMAXNUM];
static GUIPICTURE *pBtnDroplist[DROPLISTMAXNUM];

static GUIPICTURE *pDroplistBtnPageUp = NULL;   	//上一页的Picture
static GUIPICTURE *pDroplistBtnPageDown = NULL; 	//下一页的Picture
static GUIPICTURE *pDroplistBtnPosition[3]; 		//当前处于哪一页的Picture


/********************************右侧菜单控件定义******************************/


/*******************************************************************************
**	    	窗体frmdroplist中的初始化文本资源、 释放文本资源函数定义部分	  **
*******************************************************************************/
static int DroplistTextRes_Exit(void *pInArg, int iInLen, 
								void *pOutArg, int iOutLen);


/*******************************************************************************
**			    	窗体frmdroplist中的控件事件处理函数定义部分			      **
*******************************************************************************/

/***************************窗体的按键事件处理函数*****************************/
static int DroplistWndKey_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int DroplistWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/*****************************返回按钮的事件处理函数***************************/
static int DroplistBtnBack_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int DroplistBtnBack_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);


/***********************下拉列表每个条目的事件处理函数*************************/
static int BtnDroplist_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int BtnDroplist_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);


/************************上一页、下一页的事件处理函数**************************/
static int DroplistBtnPageDown_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int DroplistBtnPageUp_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int DroplistBtnPageDown_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int DroplistBtnPageUp_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);


/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static int DroplistErrProcDroplist_Func(void *pInArg, int iInLen, 
							 			void *pOutArg, int iOutLen);
static int ShowContent(int iNum);
static int	DroplistExitSubList(void);
/*static int SetWidgetEnable(unsigned char ucEnable,
					unsigned int uiPlaceX, unsigned int uiPlaceY, 
					unsigned int uiWidth, unsigned int uiHeight);*/


/***
  * 功能：
        窗体Droplist的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDroplistInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    GUIWINDOW *pCurWindow = (GUIWINDOW *) pWndObj;

	for(i=0; i<DROPLISTMAXNUM; i++)
	{
		 pStrMenuItem[i] = NULL;
	}	

	/* 初始化下拉菜单页位置为第一页 */
	GiActualPagePosition = 0;

	/* 创建右侧的背景Picture */
   	pDroplistBg = CreatePicture(DROPLIST_WINDOW_X, DROPLIST_WINDOW_Y+20, 181, 460, 
                                 BmpFileDirectory"bg_global_right.bmp");

	/* 创建返回按钮Picture */
   	pDroplistBtnBack = CreatePicture(DROPLIST_WINDOW_X, DROPLIST_WINDOW_Y+32, 36, 36, 
                                 BmpFileDirectory"btn_droplist_back_unpress.bmp");

	/* 创建标题栏Label */
	pDroplistlblTitle = CreateLabel(DROPLIST_WINDOW_X, DROPLIST_WINDOW_Y+39, 181, 24, 
									    pDroplistStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDroplistlblTitle);

	/* 创建分割线的Picture */
	pDroplistBtnSplitline = CreatePicture(DROPLIST_WINDOW_X, DROPLIST_WINDOW_Y+79, 181, 1, 
                                 			BmpFileDirectory"bg_splitline.bmp");
    
    /*创建下拉控件选项的Picture控件 */
    for(k=0; k<DROPLISTMAXNUM/PAGENUM; k++)
	{
		for(i=0, j=DROPLIST_WINDOW_Y+80; i<PAGENUM; i++,j+=51)
		{
		    pBtnDroplist[i+k*PAGENUM] = CreatePicture(DROPLIST_WINDOW_X, j, 181, 51, 
		                                	  		  BmpFileDirectory"btn_normal_unpress.bmp");		
		}
	}

    /* 创建下拉控件选项的Label控件 */
	for(k=0; k<DROPLISTMAXNUM/PAGENUM; k++)
	{
		for(i=0, j=DROPLIST_WINDOW_Y+97; i<PAGENUM; i++,j+=51)
		{
			pLblDroplist[i+k*PAGENUM] = CreateLabel(DROPLIST_WINDOW_X, j, 181, 51,     pStrMenuItem[i]);	
		}
	}
 
    /* 设置Label居中 */
    for(i=0; i<DROPLISTMAXNUM; i++)
	{
    	SetLabelAlign(GUILABEL_ALIGN_CENTER, pLblDroplist[i]);
	}

	/* 创建上一页按钮的Picture */
   	pDroplistBtnPageUp = CreatePicture(DROPLIST_WINDOW_X+24, DROPLIST_WINDOW_Y+414, 40, 40, 
                                 		BmpFileDirectory"btn_droplist_left_unpress.bmp");

	/* 创建下一页按钮的Picture */
   	pDroplistBtnPageDown = CreatePicture(DROPLIST_WINDOW_X+128, DROPLIST_WINDOW_Y+414, 40, 40, 
                                 			BmpFileDirectory"btn_droplist_right_unpress.bmp");

	/* 创建页位置的Picture */
   	pDroplistBtnPosition[0] = CreatePicture(DROPLIST_WINDOW_X+73, DROPLIST_WINDOW_Y+431, 6, 6, 
                         					BmpFileDirectory"btn_droplist_pos_unselect.bmp");
   	pDroplistBtnPosition[1] = CreatePicture(DROPLIST_WINDOW_X+93, DROPLIST_WINDOW_Y+431, 6, 6, 
                         					BmpFileDirectory"btn_droplist_pos_unselect.bmp");
   	pDroplistBtnPosition[2] = CreatePicture(DROPLIST_WINDOW_X+113, DROPLIST_WINDOW_Y+431, 6, 6, 
                         					BmpFileDirectory"btn_droplist_pos_unselect.bmp");

	/* droplist内的控件建立后不能接受触摸事件 */
	SetPictureEnable(0, pDroplistBtnBack);
	
	for(i=0; i<DROPLISTMAXNUM; i++)
	{
		SetPictureEnable(0, pBtnDroplist[i]);
	}

	SetPictureEnable(0, pDroplistBtnPageUp);
	SetPictureEnable(0, pDroplistBtnPageDown);

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件
    //...
    //设置menu的控件

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pCurWindow, 
                  pCurWindow);
    //注册按钮区的控件
    for(i=0; i<DROPLISTMAXNUM; i++)
	{
	    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pBtnDroplist[i], 
	                  pCurWindow);
	}

    //注册返回按钮、上一页和下一页按钮
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDroplistBtnBack, 
                  pCurWindow);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDroplistBtnPageUp, 
                  pCurWindow);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDroplistBtnPageDown, 
                  pCurWindow);


    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();

    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pCurWindow, 
                    DroplistWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pCurWindow, 
                    DroplistWndKey_Up, NULL, 0, pMsg);
    //注册按钮区控件的消息处理
    for(i=0; i<DROPLISTMAXNUM; i++)
	{
	    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pBtnDroplist[i], 
	                    BtnDroplist_Down, &SelectItem[i], sizeof(int), pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pBtnDroplist[i], 
	                    BtnDroplist_Up, &SelectItem[i], sizeof(int), pMsg);
	}	
    //注册返回按钮、上一页和下一页按钮的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDroplistBtnBack, 
                    DroplistBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDroplistBtnPageUp, 
                    DroplistBtnPageUp_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pDroplistBtnPageDown, 
                    DroplistBtnPageDown_Down, NULL, 0, pMsg);
	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pDroplistBtnBack, 
                    DroplistBtnBack_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pDroplistBtnPageUp, 
                    DroplistBtnPageUp_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pDroplistBtnPageDown, 
                    DroplistBtnPageDown_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pCurWindow, 
                    DroplistErrProcDroplist_Func, NULL, 0, pMsg);


	return iReturn;
}


/***
  * 功能：
        窗体Droplist的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDroplistExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //销毁窗体控件
    //***************************************************************/
	DestroyPicture(&pDroplistBg);

	//销毁下拉列表的标题栏的Label控件
	DestroyLabel(&pDroplistlblTitle);
	
    //销毁menu上的控件
    for(i=0; i<DROPLISTMAXNUM; i++)
	{
	    DestroyPicture(&pBtnDroplist[i]);
	    DestroyLabel(&pLblDroplist[i]);
	}
	
	//销毁返回、上一页、下一页的Picture
    DestroyPicture(&pDroplistBtnBack);
	DestroyPicture(&pDroplistBtnSplitline);
    DestroyPicture(&pDroplistBtnPageUp);
    DestroyPicture(&pDroplistBtnPageDown);

    for(i=0; i<3; i++)
	{
		DestroyPicture(&pDroplistBtnPosition[i]);
	}	
    

    //释放文本资源
    //***************************************************************/
    DroplistTextRes_Exit(NULL, 0, NULL, 0);

    //释放字体资源
    //***************************************************************/

    return iReturn;
}


/***
  * 功能：
        窗体menu的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmDroplistPaint(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* droplist内的控件可见 */
	SetPictureEnable(1, pDroplistBtnBack);
	SetPictureEnable(1, pDroplistBtnSplitline);

	/* 显示背景 */
	DisplayPicture(pDroplistBg);

	//显示下拉列表的标题栏的Label控件
	DisplayLabel(pDroplistlblTitle);

	//显示桌面上的控件
    ShowContent(0);
	
	//显示返回、分割线的Picture
    DisplayPicture(pDroplistBtnBack);
    DisplayPicture(pDroplistBtnSplitline);

	/* 下拉菜单的内容多余一页时，才显示上一页和下一页按钮 */
	if(GiActualPageNum>=1)	
	{
		DisplayPicture(pDroplistBtnPageUp);
    	DisplayPicture(pDroplistBtnPageDown);
	}	

	SetPowerEnable(1, 1);

	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);
	
    return iReturn;
}


/***
  * 功能：
        显示下拉列表中指定页的内容
  * 参数：
        1、int iNum:	页位置
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int ShowContent(int iNum)
{
	int iReturn = 0;

	/* 显示背景 */
	DisplayPicture(pDroplistBg);

	//显示下拉列表的标题栏的Label控件
	DisplayLabel(pDroplistlblTitle);
	
	//显示返回、分割线的Picture
    DisplayPicture(pDroplistBtnBack);
    DisplayPicture(pDroplistBtnSplitline);

	/* 下拉菜单的内容多余一页时，才显示上一页和下一页按钮 */
	if(GiActualPageNum>=1)	
	{
		DisplayPicture(pDroplistBtnPageUp);
    	DisplayPicture(pDroplistBtnPageDown);
	}	

    for(k=0; k<DROPLISTMAXNUM/PAGENUM; k++)
	{
		/* 设置该页的控件可见 */
		if(k == iNum)
		{
			/* 该页为最后一页 */
			if(k == GiActualPageNum)
			{
				/* 最后一页可能没有达到PAGENUM，只显示实际的项数 */
				for(i=0; i<(GiActualNum-GiActualPageNum*PAGENUM); i++)
				{	
					LOG(LOG_INFO, "GiActualNum = %d\n", GiActualNum);
					LOG(LOG_INFO, "GiActualPageNum = %d\n", GiActualPageNum);
					LOG(LOG_INFO, "i = %d\n", i);
					SetPictureEnable(1, pBtnDroplist[i+k*PAGENUM]);
		    		SetLabelEnable(1, pLblDroplist[i+k*PAGENUM]);
				}
				/*  其他项不显示 */
				for(; i<PAGENUM; i++)
				{	
					SetPictureEnable(0, pBtnDroplist[i+k*PAGENUM]);
		    		SetLabelEnable(0, pLblDroplist[i+k*PAGENUM]);
				}
			}
			/* 该页不是最后一页 */
			else
			{	
				for(i=0; i<PAGENUM; i++)
				{	
					SetPictureEnable(1, pBtnDroplist[i+k*PAGENUM]);
		    		SetLabelEnable(1, pLblDroplist[i+k*PAGENUM]);
				}
			}
	
		}
		/* 其他页的控件不可见 */
		else
		{
			for(i=0; i<PAGENUM; i++)
			{	
				SetPictureEnable(0, pBtnDroplist[i+k*PAGENUM]);
	    		SetLabelEnable(0, pLblDroplist[i+k*PAGENUM]);
			}	
		}
	}

	//显示下拉列表内容
	for(i=0; i<DROPLISTMAXNUM; i++)
	{
		DisplayPicture(pBtnDroplist[i]);
		DisplayLabel(pLblDroplist[i]);
	}

	/* 显示页位置缩略图 */
	for(i=0; i<3; i++)
	{
		if(i == iNum)
		{
			TouchChange("btn_droplist_pos_select.bmp",
						pDroplistBtnPosition[i], NULL, NULL, 0);	
		}
		else
		{
			TouchChange("btn_droplist_pos_unselect.bmp", 
						pDroplistBtnPosition[i], NULL, NULL, 0);				
		}
	}
	
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	 

	return iReturn;
}


/*******************************************************************************
**	    	窗体frmdroplist中的初始化文本资源、 释放文本资源函数定义部分	  **
*******************************************************************************/
static int DroplistTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放下拉菜单上的文本

	for(i=0; i < DROPLISTMAXNUM; i++)
	{
		if(NULL != pStrMenuItem[i])
		{
			GuiMemFree(pStrMenuItem[i]); 
		}
	}	
	
	//释放下拉列表的标题栏的文本
	if(NULL != pDroplistStrTitle)
	{
		GuiMemFree(pDroplistStrTitle);
	} 
	
    return iReturn;
}


/*******************************************************************************
**	    					窗体的按键事件处理函数		 					  **
*******************************************************************************/
static int DroplistWndKey_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	if(iDropListKeyFlag == 0)
	{
		iDropListKeyFlag = 1;
   		switch (uiValue)
	    {
			case KEYCODE_ESC:
				break;
			case KEYCODE_HOME:
				break;
			default:
				break;
	    }
	}

    return iReturn;
}


static int DroplistWndKey_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	if(iDropListKeyFlag == 1)
	{
		iDropListKeyFlag = 0;
		
    	switch (uiValue)
	    {
			case KEYCODE_ESC:
				break;
			case KEYCODE_HOME:
			#ifdef SAVE_SCREEN
				ScreenShot();
			#endif
				break;
			default:
				break;
	    }
	}
    return iReturn;
}


/*******************************************************************************
**			    				返回按钮的事件处理函数				 	      **
*******************************************************************************/
/***
  * 功能：
        返回按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistBtnBack_Down(void *pInArg, int iInLen, 
							 void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	/* 显示按钮按下效果 */
	TouchChange("btn_droplist_back_press.bmp", pDroplistBtnBack, 
				NULL, NULL, 0); 
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

	return iReturn;
}


/***
  * 功能：
        返回按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistBtnBack_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 显示按钮弹起效果 */
	TouchChange("btn_droplist_back_unpress.bmp", 
				pDroplistBtnBack, NULL, NULL, 0);	
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

	iMenuItemSelected = DROPLISTMAXNUM;
	DroplistExitSubList();

    return iReturn;
}


/*******************************************************************************
**			    			下拉列表每个条目的事件处理函数		 			  **
*******************************************************************************/
/***
  * 功能：
        下拉列表每个条目的down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int BtnDroplist_Down(void *pInArg, int iInLen, 
						   void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int iTemp = *((int *)pOutArg);

	if(iTemp >= GiActualNum)
	{
		return iReturn;
	}
	else
	{
		/* 显示按钮按下效果 */
		TouchChange("btn_normal_press.bmp", pBtnDroplist[iTemp], 
					NULL, pLblDroplist[iTemp], 1);
		//刷新屏幕缓存
		RefreshScreen(__FILE__, __func__, __LINE__);
	
		return iReturn;
	}
}


/***
  * 功能：
        下拉列表每个条目的up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int BtnDroplist_Up(void *pInArg, int iInLen, 
						 void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int iTemp = *((int *)pOutArg);

	if(iTemp >= GiActualNum)
	{
		return iReturn;
	}
	else
	{
		/* 显示按钮弹起效果 */
		TouchChange("btn_normal_unpress.bmp",
					pBtnDroplist[iTemp], NULL, pLblDroplist[iTemp], 0);
		//刷新屏幕缓存
		RefreshScreen(__FILE__, __func__, __LINE__);

		iMenuItemSelected = iTemp;
		DroplistExitSubList();
		return iReturn;
	}
}


/*******************************************************************************
**			    				上一页、下一页的事件处理函数 			 	  **
*******************************************************************************/
/***
  * 功能：
		向上一页down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistBtnPageUp_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 显示按钮按下效果 */
	TouchChange("btn_droplist_left_press.bmp", 
				pDroplistBtnPageUp, NULL, NULL, 0);	
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

    return iReturn;
}


/***
  * 功能：
		向下一页down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistBtnPageDown_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 显示按钮按下效果 */
	TouchChange("btn_droplist_right_press.bmp", 
				pDroplistBtnPageDown, NULL, NULL, 0);	
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

    return iReturn;
}


/***
  * 功能：
		向上一页up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistBtnPageUp_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 显示按钮弹起效果 */
	TouchChange("btn_droplist_left_unpress.bmp", 
				pDroplistBtnPageUp, NULL, NULL, 0);	
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

	GiActualPagePosition--;
	if(GiActualPagePosition < 0)
	{
		GiActualPagePosition = 0;
		LOG(LOG_INFO, "GiActualPagePosition = %d\n", GiActualPagePosition);
		return iReturn;
	}
	else
	{
		LOG(LOG_INFO, "GiActualPagePosition = %d\n", GiActualPagePosition);
		ShowContent(GiActualPagePosition);
	}

    return iReturn;
}


/***
  * 功能：
		向下一页up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistBtnPageDown_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 显示按钮弹起效果 */
	TouchChange("btn_droplist_right_unpress.bmp", 
				pDroplistBtnPageDown, NULL, NULL, 0);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

	GiActualPagePosition++;
	if(GiActualPagePosition > GiActualPageNum)
	{
		GiActualPagePosition = GiActualPageNum;
		LOG(LOG_INFO, "GiActualPagePosition = %d\n", GiActualPagePosition);
		return iReturn;
	}
	else
	{
		LOG(LOG_INFO, "GiActualPagePosition = %d\n", GiActualPagePosition);
		ShowContent(GiActualPagePosition);
	}

    return iReturn;
}


/*******************************************************************************
**			    			窗体内的其他函数定义					 		  **
*******************************************************************************/
/***
  * 功能：
		错误处理
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DroplistErrProcDroplist_Func(void *pInArg, int iInLen, 
                             			void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}


/***
  * 功能：
		退出当前窗体
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int  DroplistExitSubList(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    /* 释放文本资源 */
    DroplistTextRes_Exit(NULL, 0, NULL, 0);	

	/* 恢复设置指定区域内的控件可见 */
	if(ucInactiveFlg)
	{
		ChangeAbleInArea(1, OBJTYP_GUIPICTURE, 
						 uiAreaPlaceX, uiAreaPlaceY, uiAreaPlaceX+uiAreaWidth-1,
						 uiAreaPlaceY+uiAreaHeight-1);
	}
	
	/* 恢复原窗体中被droplist控件占用的区域控件可见 */
	ChangeAbleInArea(1, OBJTYP_GUIPICTURE, 610, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	/* 
	 * droplist内的控件不可见 
	 */
	SetPictureEnable(0, pDroplistBtnBack);
	
	for(i=0; i<DROPLISTMAXNUM; i++)
	{
		SetPictureEnable(0, pBtnDroplist[i]);
	}

	SetPictureEnable(0, pDroplistBtnPageUp);
	SetPictureEnable(0, pDroplistBtnPageDown);	

	/* 重绘右侧的控件 */
	(*pResumeFunc)();

    return iReturn;
}


/***
  * 功能：
        设置指定区域内控件的可见性
  * 参数：
        1.unsigned char ucEnable:	0:控件可见;	1:控件不可见
        2.unsigned int uiPlaceX:    指定区域的X坐标
        3.unsigned int uiPlaceY:    指定区域的Y坐标
        4.unsigned int uiWidth:  	指定区域的宽度
        5.unsigned int uiHeight: 	指定区域的高度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
#if 0
static int SetWidgetEnable(unsigned char ucEnable,
						   unsigned int uiPlaceX, unsigned int uiPlaceY, 
                    	   unsigned int uiWidth, unsigned int uiHeight)
{
	//错误标志、返回值定义
    int iReturn = 0;
	int iPos;
    GUILIST_ITEM *pItem = NULL;
	GUIOBJ *pPkt = NULL;
	GUICOMPONENT *pComp = NULL;

	/* 设置指定区域内的控件的有效性 */
	

	/* 遍历控件队列，查看是否有控件在指定区域内 */
    list_next_each(pItem, pCurWindow->pWndComps, iPos)
    {
        pPkt = container_of(pItem, GUIOBJ, List);
        pComp = (GUICOMPONENT *) (pPkt->pObj);

		/* 控件在指定区域内 */
      if((pComp->Visible.Area.Start.x >= uiPlaceX) && 
        (pComp->Visible.Area.End.x   <= uiPlaceX+uiWidth-1) &&
        (pComp->Visible.Area.Start.y >= uiPlaceY) &&
        (pComp->Visible.Area.End.y   <= uiPlaceY+uiHeight-1))
        {
        	pComp->Visible.iEnable = ucEnable;
        }
	}

	return iReturn;
}
#endif

/***
  * 功能：
        创建下拉列表框
  * 参数：
        1.GUICHAR *pTitleStr   :			下拉列表标题栏的文本
        2.int iMenuItemNum	   :			下拉列表的数目
        3.unsigned char ucFlg  :			是否有无效区域的标志；
        									0:没有无效区域;	1:有无效区域
        4.unsigned int uiPlaceX:    		指定区域的X坐标
        5.unsigned int uiPlaceY:    		指定区域的Y坐标
        6.unsigned int uiWidth :  			指定区域的宽度
        7.unsigned int uiHeight: 			指定区域的高度
		8.CALLLBACKHANDLE pPauseFunc: 		挂起函数指针，
											使用droplist前进行调用，可以为空
		9.CALLLBACKHANDLE pResumeFunction: 	恢复函数指针，
											使用droplist后进行调用，不可为空，
											主要进行重绘被覆盖的控件，
											对用户做出的选择进行相应的处理
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		调用此控件时，在当前窗体的初始化函数中调用
		int FrmDroplistInit(void *pWndObj)函数，
		以便将droplist控件中的控件加入当前窗体的控件队列中；
		同时在当前窗体的退出函数中调用int FrmDroplistExit(void *pWndObj)函数，
		最好放在退出函数的最后调用以便销毁droplist控件中占用的资源
***/
int CreateDroplist(GUICHAR *pTitleStr, int iMenuItemNum, unsigned char ucFlg, 
					unsigned int uiPlaceX, unsigned int uiPlaceY, 
                    unsigned int uiWidth, unsigned int uiHeight, 
					CALLLBACKHANDLE pPauseFunc, CALLLBACKHANDLE pResumeFunction)
{
	int iReturn = 0;

	if(pTitleStr==NULL || iMenuItemNum>DROPLISTMAXNUM ||
	   pResumeFunction==NULL)
	{
		iReturn = -1;
	}

	/* 获得当前窗体对象 */
	if(iReturn == 0)
	{
		//保持当前窗体对象
    	//pWnd = HoldCurrWindow();
		
		pCurWindow = GetCurrWindow();

		/* 判断pCurWindow是否为有效指针 */
        if (NULL == pCurWindow)
        {
            iReturn = -2;
        }
	}

    if (iReturn == 0)
    {
        /* 判断控件队列pWndComps是否为有效指针 */
        if (NULL == pCurWindow->pWndComps)
        {
            iReturn = -3;
        }
    }

	if(iReturn == 0)
	{
		ucInactiveFlg = ucFlg;
		pDroplistStrTitle = pTitleStr;
		pResumeFunc = pResumeFunction;
		uiAreaPlaceX = uiPlaceX;
		uiAreaPlaceY = uiPlaceX;
		uiAreaWidth = uiWidth;
		uiAreaHeight = uiHeight;

		//iMenuItemNum = iMenuItemNum-1;
		GiActualNum = iMenuItemNum;
		/* 计算实际的下拉菜单页数 */
		GiActualPageNum = (iMenuItemNum-1)/PAGENUM;
		/* 初始化下拉菜单页位置为第一页 */
		GiActualPagePosition = 0;

		/* 设置指定区域内的控件不可见 */
		if(ucInactiveFlg)
		{
			ChangeAbleInArea(0, OBJTYP_GUIPICTURE, 
							uiAreaPlaceX, uiAreaPlaceY, 
						    uiAreaPlaceX+uiAreaWidth-1, 
						    uiAreaPlaceY+uiAreaHeight-1);
		}
		
		/* 原窗体中被droplist控件占用的区域默认控件不可见 */
		ChangeAbleInArea(0, OBJTYP_GUIPICTURE, 618, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		/* 根据下拉菜单的内容数目，重新设定页缩略图的位置 */
		switch(GiActualPageNum)
		{
			case 1:
				SetPictureArea(DROPLIST_WINDOW_X+80, DROPLIST_WINDOW_Y+431, 
							   DROPLIST_WINDOW_X+85, DROPLIST_WINDOW_Y+436, 
							   pDroplistBtnPosition[0]);
				SetPictureArea(DROPLIST_WINDOW_X+106, DROPLIST_WINDOW_Y+431, 
							   DROPLIST_WINDOW_X+111, DROPLIST_WINDOW_Y+436, 
							   pDroplistBtnPosition[1]);
				SetPictureEnable(1, pDroplistBtnPosition[0]);
				SetPictureEnable(1, pDroplistBtnPosition[1]);
				SetPictureEnable(0, pDroplistBtnPosition[2]);

				SetPictureEnable(1, pDroplistBtnPageUp);
				SetPictureEnable(1, pDroplistBtnPageDown);
				break;
			case 2:
				SetPictureArea(DROPLIST_WINDOW_X+73, DROPLIST_WINDOW_Y+431, 
							   DROPLIST_WINDOW_X+78, DROPLIST_WINDOW_Y+436, 
							   pDroplistBtnPosition[0]);
				SetPictureArea(DROPLIST_WINDOW_X+93, DROPLIST_WINDOW_Y+431, 
							   DROPLIST_WINDOW_X+98, DROPLIST_WINDOW_Y+436, 
							   pDroplistBtnPosition[1]);
				SetPictureArea(DROPLIST_WINDOW_X+113, DROPLIST_WINDOW_Y+431, 
							   DROPLIST_WINDOW_X+118, DROPLIST_WINDOW_Y+436, 
							   pDroplistBtnPosition[2]);
				SetPictureEnable(1, pDroplistBtnPosition[0]);
				SetPictureEnable(1, pDroplistBtnPosition[1]);
				SetPictureEnable(1, pDroplistBtnPosition[2]);
				
				SetPictureEnable(1, pDroplistBtnPageUp);
				SetPictureEnable(1, pDroplistBtnPageDown);
				break;
			default :
				SetPictureEnable(0, pDroplistBtnPageUp);
				SetPictureEnable(0, pDroplistBtnPageDown);
				
				SetPictureEnable(0, pDroplistBtnPosition[0]);
				SetPictureEnable(0, pDroplistBtnPosition[1]);
				SetPictureEnable(0, pDroplistBtnPosition[2]);
				break;
		}

		/* 调用挂起函数 */
		if(NULL != pPauseFunc)
		{
			(*pPauseFunc)();
		}
		iReturn = SetLabelText(pDroplistStrTitle, pDroplistlblTitle);
		LOG(LOG_INFO, "iReturn = %d\n", iReturn);
		for(i=0; i<GiActualNum; i++)
		{
			iReturn = SetLabelText(pStrMenuItem[i], pLblDroplist[i]);
			LOG(LOG_INFO, "iReturn = %d\n", iReturn);
		}		
		
		/* 绘制无效区域窗体 */
		DispTransparent(100, 0x0, uiAreaPlaceX, uiAreaPlaceY, 
						uiAreaWidth, uiAreaHeight);
		/* 绘制droplist控件 */
		FrmDroplistPaint();
	}

	return iReturn;
}
