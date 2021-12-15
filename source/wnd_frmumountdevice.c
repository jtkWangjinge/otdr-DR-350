/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmumountdevice.c
* 摘    要：  实现移除移动设备
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-03-25
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmumountdevice.h"

#include "wnd_frmfilebrowse.h"

#include "app_global.h"
#include "wnd_global.h"
#include "guiglobal.h"
#include "wnd_global.h"
#include <string.h>
#include "wnd_frmdroplist.h"


/****************************
* 下拉列表窗体所需的额外变量
****************************/
#define MAXNUM 2

#define NO_DEVICE 		0
#define USB_DEVICE 		1
#define SDCARD_DEVICE	2

static int __g_iDeviceType[2] = {NO_DEVICE, NO_DEVICE};

static int iDropListKeyFlag = 0;
static int iDroplistObjNum = 0;				//记录当前droplist 所存的对象数目
static int iDroplistObject = 0;				//记录当前droplist 所指的对象
static int DROPLIST_WINDOW_X = 0;			//下拉框的左上角的点横坐标
static int DROPLIST_WINDOW_Y = 0;			//下拉框的左上角的点纵坐标
static int i = 0, j = 0;					//临时辅助变量
static int GiActualNum = 0;

static GUICHAR *pStrMenuItem1[MAXNUM];		//用于存放下拉列表每项的文本
static int __g_iMenuItemSelected = -1;		//保存下拉列表哪项被选中了,初始值为-1，代表没有子项被选中

/************************
* 窗体menu中的窗体控件
************************/
static GUIWINDOW *pFrmDroplist = NULL;

//菜单上的文本
static GUICHAR *pOtdrStrDroplist[MAXNUM];

//菜单项控件
static GUIPICTURE *pOtdrBtnDroplist[MAXNUM];

//菜单项标签
static GUILABEL *pOtdrLblDroplist[MAXNUM];

//状态栏、桌面、信息栏
static GUIPICTURE *pDroplistBg = NULL;

static int OtdrBtnDroplist_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int OtdrBtnDroplist_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);


static int OtdrMenuWndKey_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int OtdrMenuWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int OtdrMenuBtnBack_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int OtdrMenuTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int OtdrMenuTextRes_Exit(void *pInArg, int iInLen, 
							void *pOutArg, int iOutLen);

/***
  * 功能：
        窗体menu的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOtdrMenuInit2(void *pWndObj)

{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmDroplist= (GUIWINDOW *) pWndObj;

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    OtdrMenuTextRes_Init(NULL, 0, NULL, 0);

    //建立menu窗体控件
    /****************************************************************/
    pDroplistBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);	
	

    for(i=0, j=DROPLIST_WINDOW_Y; i<GiActualNum; i++,j+=34)
	{
		pOtdrBtnDroplist[i] = CreatePicture(DROPLIST_WINDOW_X-19, j, 130, 34, 
		                            	  	BmpFileDirectory"drop_list_unpress1.bmp");
		pOtdrBtnDroplist[i]->Visible.iLayer = 2;
	}
	SetPictureBitmap(BmpFileDirectory"drop_list_press1.bmp",pOtdrBtnDroplist[iDroplistObjNum]);

    for(i=0, j=DROPLIST_WINDOW_Y+1; i<GiActualNum; i++,j+=34)
	{
	    pOtdrLblDroplist[i] = CreateLabel(DROPLIST_WINDOW_X-19, j+5, 130, 34, pOtdrStrDroplist[i]);	
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrLblDroplist[i]);
	}	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrLblDroplist[iDroplistObjNum]);

    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmDroplist, 
                  pFrmDroplist);

    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pDroplistBg, 
                  pFrmDroplist);	
	
    //注册按钮区的控件
    for(i=0; i<GiActualNum; i++)
	{
	    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrBtnDroplist[i], 
	                  pFrmDroplist);
	}
	
    pMsg = GetCurrMessage();

    //注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmDroplist, 
	                OtdrMenuWndKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmDroplist, 
	                OtdrMenuWndKey_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_UP, pDroplistBg, 
                    OtdrMenuBtnBack_Up, NULL, 0, pMsg);
	
	
    //注册按钮区控件的消息处理
    for(i=0; i<GiActualNum; i++)
	{
	    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBtnDroplist[i], 
	                    OtdrBtnDroplist_Down, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrBtnDroplist[i], 
	                    OtdrBtnDroplist_Up, NULL, i, pMsg);
	}	

	return iReturn;
}


/***
  * 功能：
        窗体menu的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOtdrMenuExit2(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmDroplist= (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmDroplist);

	DestroyPicture(&pDroplistBg);
	
    //销毁menu上的控件
    for(i=0; i<GiActualNum; i++)
	{
	    DestroyPicture(&pOtdrBtnDroplist[i]);
	    DestroyLabel(&pOtdrLblDroplist[i]);
	}

    //释放文本资源
    //***************************************************************/
    OtdrMenuTextRes_Exit(NULL, 0, NULL, 0);
	
	iDroplistObjNum = 0;
	iDroplistObject = 0;
	
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
static int FrmOtdrMenuPaint2(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmDroplist= (GUIWINDOW *) pWndObj;

	/* 绘制无效区域窗体 */
	DispTransparent(80, 0x0, 0, 0, 682, 480);	

    //显示桌面上的控件
	
    for(i=0; i<GiActualNum; i++)
	{
		DisplayPicture(pOtdrBtnDroplist[i]);
		DisplayLabel(pOtdrLblDroplist[i]);
	}

	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
        窗体frmotdr的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOtdrMenuLoop2(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);   	
	
    return iReturn;
}


/***
  * 功能：
        窗体frmotdr的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOtdrMenuPause2(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmotdr的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FrmOtdrMenuResume2(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int OtdrMenuTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化桌面上的文本
    for(i=0; i<GiActualNum; i++)
	{
    	pOtdrStrDroplist[i] = pStrMenuItem1[i];
	}

    return iReturn;
}


//释放文本资源
static int OtdrMenuTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    for(i=0; i<GiActualNum; i++)
	{
    	GuiMemFree(pOtdrStrDroplist[i]);
	}
		
    return iReturn;
}

static int OtdrMenuWndKey_Down(void *pInArg, int iInLen, 
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
static int OtdrMenuWndKey_Up(void *pInArg, int iInLen, 
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
				OtdrMenuBtnBack_Up(pInArg, iInLen, pOutArg, iOutLen);
				break;
			case KEYCODE_HOME:
			#ifdef SAVE_SCREEN
				ScreenShot();
			#else
				iDroplistObjNum = 0;
				iDroplistObject = 0;

			    GUIWINDOW *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
			                        FrmMainInit, FrmMainExit, 
			                        FrmMainPaint, FrmMainLoop, 
							        FrmMainPause, FrmMainResume,
			                        NULL);        
				SendWndMsg_WindowExit(pFrmDroplist);     
				SendSysMsg_ThreadCreate(pWnd); 				
			#endif	
				break;
			default:
				break;
		}		
	}
    return iReturn;
}
//菜单按下
static int OtdrBtnDroplist_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int iTemp = iOutLen;
	TouchChange("drop_list_unpress.bmp", pOtdrBtnDroplist[iDroplistObjNum], 
				NULL, pOtdrLblDroplist[iDroplistObjNum], 0);	
	TouchChange("drop_list_press.bmp", pOtdrBtnDroplist[iTemp], NULL, pOtdrLblDroplist[iTemp], 1);	
	
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

//菜单弹起
static int OtdrBtnDroplist_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	int iTemp = iOutLen;

	TouchChange("drop_list_press.bmp", pOtdrBtnDroplist[iTemp], 
				NULL, pOtdrLblDroplist[iTemp], 1);	
	
	__g_iMenuItemSelected = iTemp;

	/* 使下拉列表无效 */
    for(i=0; i<GiActualNum; i++)
	{
		SetPictureEnable(0, pOtdrBtnDroplist[i]);
	}	

	/* 绘制无效区域窗体 */
	DispTransparent(100, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	RefreshScreen(__FILE__, __func__, __LINE__);

	sync();
	
	/* 卸载移动设备 */
	switch(__g_iDeviceType[__g_iMenuItemSelected])
	{
		case USB_DEVICE:
			mysystem("/sbin/umount-usb.sh");
			break;
		case SDCARD_DEVICE:
			//mysystem("/sbin/umount-sd.sh");
			break;
		case NO_DEVICE:
		default :
			break;
	}

	__SetOpenFolder(MntUpDirectory);

	OtdrMenuBtnBack_Up(pInArg, iInLen, pOutArg, iOutLen);
	
	return iReturn;
}

static int OtdrMenuBtnBack_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIWINDOW *pWnd = NULL;

	iDroplistObjNum = 0;
	iDroplistObject = 0;

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFileBrowseInit, FrmFileBrowseExit, 
                        FrmFileBrowsePaint, FrmFileBrowseLoop, 
				        FrmFileBrowsePause, FrmFileBrowseResume,
                        NULL);        
	SendWndMsg_WindowExit(pFrmDroplist);     
	SendSysMsg_ThreadCreate(pWnd);          

	return iReturn;
}


/*********************************************************************************************************
** 函数名  :         DroplistInit
** 功  能  :         初始化设置菜单中的下拉列表框
** 输入参数:    	 int x               : 	列表框起始位置横坐标
**        			 int y               : 	列表框起始位置纵坐标
**     				 int num             :	列表项的数目，最少是1
**					 LastWindow          :	将要退出的窗体
**        			 func    			 :	重绘窗体   
**					 defaultValue		 :	选中的列表中的某一项，和数组一样从0开始
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void DroplistInit2(int x, int y, int num, GUIWINDOW *LastWindow, int defaultValue)
{
	GUIWINDOW *pWnd = NULL;
	
	int heigh = num * 28;
	if((y+num*28) > 480)
	{
		y = y-60;
	}
	DROPLIST_WINDOW_X = x;
	DROPLIST_WINDOW_Y = y;
	GiActualNum = num;
	iDroplistObjNum = defaultValue;
	pWnd = CreateWindow(x, y, 194, heigh, 
	                    FrmOtdrMenuInit2 , FrmOtdrMenuExit2, 
	                    FrmOtdrMenuPaint2, FrmOtdrMenuLoop2, 
					    FrmOtdrMenuPause2, FrmOtdrMenuResume2,
	                    NULL);          //pWnd由调度线程释放
	SendWndMsg_WindowExit(LastWindow);	    //发送消息以便退出前一个窗体
	SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
}


/***
  * 功能：
        实现移除可移动设备
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int UmountDevice(void)
{
	int iErr = 0;
	unsigned char ucCount = 0;

	if(iErr == 0)
	{
		/* 设备列表 */
		__g_iDeviceType[0] = NO_DEVICE;
		__g_iDeviceType[1] = NO_DEVICE;
	
		/* 说明挂载了USB */
		if( access("/tmp/usb", F_OK) == 0) 	
		{
			pStrMenuItem1[ucCount] = TransString("FILE_REMOVE_USB");
			__g_iDeviceType[ucCount] = USB_DEVICE;
			ucCount++;
		}

		if(ucCount == 0)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		DroplistInit2(570, 318, ucCount, GetCurrWindow(), 0);
	}

	return iErr;
}
