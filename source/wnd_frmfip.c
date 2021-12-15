/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfip.c
* 摘    要：  实现主窗体FrmFip的窗体处理操作函数
*
* 当前版本：
* 作    者：
* 完成日期：
*
*******************************************************************************/

#include "wnd_frmfip.h"

/*******************************************************************************
***                  为实现窗体frmfip而需要引用的其他头文件                  ***
*******************************************************************************/
#include "app_global.h"
#include "app_frmfip.h"
#include "app_algorithm_analysis.h"
#include "app_frmfilebrowse.h"
#include "app_screenshots.h"


#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmdialog.h"
#include "wnd_frmoperatesor.h"
#include "wnd_frmfipfileopen.h"

/*******************************************************************************
***         				为实现窗体frmfip而定义的全局变量		    	***
*******************************************************************************/
extern OpticalFiberEndSur OptFiberEndSurCheck;
extern CURR_WINDOW_TYPE enCurWindow;
/*******************************************************************************
***         				为实现窗体frmfip线程而定义的全局变量		    ***
*******************************************************************************/
static GUITHREAD threadRead = -1;
static GUITHREAD threadDisplay = -1;
static int iThreadExitFlag = 1;         //线程退出标识  0表示退出线程
int iCheckErrFlag = 0;                  //退出open线程标志

//记录当前的文件路径名
static char* pFiberCurrPath = NULL;
//1表示已经保持
static int fiberholdstate = 0;

#define FIP_MENU_NUM        3

//创建右边menu菜单所需数组
static unsigned int StrFipMenuScreen[FIP_MENU_NUM] = {
    FIP_LBL_OPEN,
    FIP_LBL_SAVE,
    FIP_LBL_ALIGN
};

//各项非选中背景资源
static char *pFipUnpressBmp[FIP_MENU_NUM] =
{
    BmpFileDirectory "bg_fip_open.bmp",
    BmpFileDirectory "bg_fip_save.bmp",
    BmpFileDirectory "bg_fip_notAlign.bmp"
};

//各项选中背景资源
static char *pFipSelectBmp[FIP_MENU_NUM] =
{
    BmpFileDirectory "bg_fip_openSelect.bmp",
    BmpFileDirectory "bg_fip_saveSelect.bmp",
    BmpFileDirectory "bg_fip_notAlignSelect.bmp"
};

//各项选中背景资源
static char *pFipDisableBmp[FIP_MENU_NUM] =
{
    BmpFileDirectory "bg_fip_startDisable.bmp",
    BmpFileDirectory "bg_fip_saveDisable.bmp",
    BmpFileDirectory "bg_fip_notAlignDisable.bmp"
};

//回调函数
FIPCALLLBACK FipCallBackFunc = NULL;
/*******************************************************************************
***                                窗体中的窗体控件                          ***
*******************************************************************************/
static GUIWINDOW *pFrmFip = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pFipBg = NULL;
static GUIPICTURE *pFipBgIcon = NULL;			//标题栏图标
static GUILABEL *pFipLblTitle = NULL;
static GUICHAR *pFipStrTitle = NULL;

static WNDMENU1 *pFipMenu = NULL;

static GUIPICTURE *pFipTouchEnable = NULL;

/*******************************************************************************
***                        窗体中的文本资源处理函数                          ***
*******************************************************************************/

//初始化文本资源
static int FipTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
//释放文本资源
static int FipTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

/*******************************************************************************
***                        窗体中的线程相关处理函数                          ***
*******************************************************************************/
static void *OpenFip(void *pThreadArg);
//打开光纤端面检查的线程
static void *ShowFip(void *pThreadArg);
static void CancelShowPthread();
static void CancelReadPthread();

/*******************************************************************************
***                        窗体中的回调处理函数                          ***
*******************************************************************************/
static void FipMenuCallBack(int iOption);
// static GUIWINDOW* CreateFipWindow();
// static void FipReCreateWindow(GUIWINDOW **pWnd);

//设置当前的文件路径
static void SetCurrentPath(void);
static void setMenuEnable(int menu0, int menu1, int menu2);
//设置图像居中状态
static void changeAlignCenterState(void);
//刷新菜单栏
static void refreshMenu(void);
//刷新居中菜单状态
static void refreshAlignState(int iState);

/***
  * 功能：
        窗体frmfip的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmFipInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	
	//得到当前窗体对象 
    pFrmFip = (GUIWINDOW *) pWndObj;
	enCurWindow = ENUM_PORT_WIN;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FipTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件
    ***************************************************************************/
    pFipBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_fip.bmp");
	pFipBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_fip_icon.bmp");

	pFipTouchEnable = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	pFipTouchEnable->Visible.iLayer = 10;
	pFipTouchEnable->Visible.iCursor = 1;
	pFipTouchEnable->Visible.iFocus = 1;

    pFipLblTitle = CreateLabel(40, 12, 100, 16, pFipStrTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFipLblTitle);
    
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
    pFipMenu = CreateWndMenu1(FIP_MENU_NUM, sizeof(StrFipMenuScreen), StrFipMenuScreen, 0xff00,
								 0, 1, 41, FipMenuCallBack);
    //设置菜单栏
    int i;
    for (i = 0; i < FIP_MENU_NUM; i++)
    {
        if (i == 2)
            //判断图像是否居中显示
            refreshAlignState(OptFiberEndSurCheck.isAlignCenter);
        else
        {
            SetWndMenuItemBg(i, pFipUnpressBmp[i], pFipMenu, MENU_UNPRESS);
            SetWndMenuItemBg(i, pFipSelectBmp[i], pFipMenu, MENU_SELECT);
            SetWndMenuItemBg(i, pFipDisableBmp[i], pFipMenu, MENU_DISABLE);
        }
    }
    
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFip, pFrmFip);
	
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFipMenu, pFrmFip);
	
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFipTouchEnable, pFrmFip);

    /***************************************************************************
    *                      注册右侧菜单区控件的消息处理
    ***************************************************************************/
    LoginWndMenuToMsg1(pFipMenu, pFrmFip);
    
	return iRet;
}


/***
  * 功能：
        窗体frmotdrcurve的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmFipExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    	
    CancelShowPthread();
    //得到当前窗体对象
    pFrmFip = (GUIWINDOW *) pWndObj;

    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFip);
	
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFipBg);
	DestroyPicture(&pFipBgIcon);
	DestroyPicture(&pFipTouchEnable);
	DestroyLabel(&pFipLblTitle);	
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFipMenu);
    FipCallBackFunc = FrmMainReCreateWindow;
    enCurWindow = ENUM_MAIN_WIN;
    //释放文本资源
    FipTextRes_Exit(NULL, 0, NULL, 0);
    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipPaint(void *pWndObj)
{
    ///错误标志、返回值定义
    int iRet = 0;

    //得到当前窗体对象
    pFrmFip = (GUIWINDOW *) pWndObj;

    /***************************************************************************
    *                      显示桌面上各个区域的背景控件
    ***************************************************************************/
    DisplayPicture(pFipBg);
	// DisplayPicture(pFipBgIcon);
	DisplayLabel(pFipLblTitle);
    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
    int i;
    //使能所有菜单项
    for (i = 0; i < FIP_MENU_NUM; ++i)
    {
        SetWndMenuItemEnble(i, 1, pFipMenu);
    }

    DisplayWndMenu1(pFipMenu);

    if(OptFiberEndSurCheck.isConnectDevice)
    {
        setMenuEnable(1,1,1);
    }
    else
    {
        setMenuEnable(1,0,0);
    }
	
	SetPowerEnable(1, ENUM_PORT_WIN);//设置当前界面是端面检测界面

	RefreshScreen(__FILE__, __func__, __LINE__);

	if(threadRead == -1)
	{
		iRet = ThreadCreate(&threadRead, NULL, &OpenFip, NULL);
		if(iRet == 0)
		{
            LOG(LOG_INFO, "-----create openfiber thread success!!---\n");
        }
		else
		{
            LOG(LOG_ERROR, "-----create openfiber thread failure\n");
        }
	}

	if(threadDisplay == -1)
	{
		iRet = ThreadCreate(&threadDisplay, NULL, &ShowFip, NULL);
		if(iRet == 0)
		{
            LOG(LOG_INFO, "-----create showfiber thread success!!---\n");
        }
		else
		{
            LOG(LOG_ERROR, "-----create showfiber thread failure ret!!---\n");
        }
	}
    
	SetPictureEnable(0, pFipTouchEnable);
    //设置当前界面
    enCurWindow = ENUM_FAULT_WIN;
    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;
    refreshMenu();
    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmotdrcurve的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFipResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        初始化文本资源
  * 参数：
  		...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FipTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
	
	/***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	
	pFipStrTitle = GetCurrLanguageText(MAIN_LBL_FIP);
	
    return 0;
}

/***
  * 功能：
        释放文本资源
  * 参数：
  		...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FipTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    /***************************************************************************
    *                           释放桌面上的文本
    ***************************************************************************/
	GuiMemFree(pFipStrTitle);
    
	return 0;
}

 /***
   * 功能：
         打开光纤端面检查的线程
   * 参数：
         void *pThreadArg: 线程传入的参数
   * 返回：
         空
   * 备注：   
 ***/
 static void *OpenFip(void *pThreadArg)
 {
#ifdef FIP_DEVICE
     int iRet = 0;
     while(iThreadExitFlag)
     {   
         if (OptFiberEndSurCheck.isConnectDevice)
         {
             iRet = ReadPicture();
             //返回-1，说明f252有可能被拔了，关闭，置0
             if (iRet < 0)
             {
                 OptFiberEndSurCheck.isConnectDevice = 0;
             }
         }
         else
         {
             if (!(OptFiberEndSurCheck.isConnectDevice = openDetector() ? 0 : 1))
             {
                 MsecSleep(200);
             }
         }
     }
#endif
     return NULL;
 }


 /***
   * 功能：
         打开光纤端面检查的线程
   * 参数：
         void *pThreadArg: 线程传入的参数
   * 返回：
         空
   * 备注：   
 ***/
 static void *ShowFip(void *pThreadArg)
 {
     while(iThreadExitFlag)
     {   
         ShowPicture(BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,BIG_IMG_WIDTH,BIG_IMG_HEIGTH);
     }   
     return NULL;
 }

 /***
   * 功能：
         回收线程
   * 参数：
         空
   * 返回：
         空
   * 备注：   
 ***/
 void PthreadJoin()
 {
     iThreadExitFlag = 0;
     OptFiberEndSurCheck.Sharpness = 0;
     CancelReadPthread();
     CancelShowPthread();
#ifdef FIP_DEVICE    
     StopDetector();
#endif    
     if(ReleaseFiberDevice() == 0)
     {
         LOG(LOG_INFO, "--------------------exit success-----------\n");
     }
 }

 /***
   * 功能：
         回收显示线程
   * 参数：
         空
   * 返回：
         空
   * 备注：   
 ***/
 static void CancelShowPthread()
 {
     if(threadDisplay != -1)
     {
         ThreadCancel(threadDisplay);
         ThreadJoin(threadDisplay, NULL);
         MutexTrylock(&(OptFiberEndSurCheck.mutexLock));
         MutexUnlock(&(OptFiberEndSurCheck.mutexLock));
         threadDisplay = -1;
     }
 }

 /***
   * 功能：
         回收读取图片线程
   * 参数：
         空
   * 返回：
         空
   * 备注：   
 ***/
 static void CancelReadPthread()
 {
     if(threadRead != -1)
     {
         ThreadCancel(threadRead);
         ThreadJoin(threadRead, NULL);
         threadRead = -1;
     }
 }

//回调函数
void FrmFipReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                        FrmFipInit, FrmFipExit,
                        FrmFipPaint, FrmFipLoop,
                        FrmFipPause, FrmFipResume,
                        NULL);
}

 /*  侧边菜单控件回调函数 */
static void FipMenuCallBack(int iOption)
{
     GUIWINDOW *pWnd = NULL;
     switch (iOption)
     {
     case 0:
	 {
		 if (pFipTouchEnable->Visible.iEnable == 1)
		 {
			 break;
		 }
		 SetPictureEnable(1, pFipTouchEnable);
		 if (pFiberCurrPath == NULL)
		 {
			 pFiberCurrPath = GuiMemAlloc(TEMP_ARRAY_SIZE);
		 }
		 strcpy(pFiberCurrPath, filefibergdmpath);//每次打开都是Fiber_Microscope目录,当该目录存在时
//          FileDialog(FILE_OPEN_MODE, MntUpDirectory, pFiberCurrPath, FipReCreateWindow, BMP);
		 GUIWINDOW* pWnd = NULL;
		 pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			 FrmFipFileOpenInit, FrmFipFileOpenExit,
			 FrmFipFileOpenPaint, FrmFipFileOpenLoop,
			 FrmFipFileOpenPause, FrmFipFileOpenResume,
			 NULL);
		 SendWndMsg_WindowExit(GetCurrWindow());
		 SendSysMsg_ThreadCreate(pWnd);
	 }

         break;
     case 1:
         SetPictureEnable(1, pFipTouchEnable);
         if (access(MntSDcardDataDirectory, F_OK))
         {
             MsgNoSdcard(pFrmFip);
         }
         else
         {
            if (!ScreenShots(BIG_IMG_XPOSITION, BIG_IMG_XPOSITION + BIG_IMG_WIDTH,
                            BIG_IMG_YPOSITION, BIG_IMG_YPOSITION + BIG_IMG_HEIGTH, NULL))
            {
                DialogInit(120, 90, GetCurrLanguageText(OTDR_LBL_SAVE_FILE),
                           GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
                           0, 0, FrmFipReCreateWindow, NULL, NULL);
            }
         }
         SetPictureEnable(0, pFipTouchEnable);
         break;
     case 2:                             //居中
         SetPictureEnable(1, pFipTouchEnable);
         changeAlignCenterState();
         SetPictureEnable(0, pFipTouchEnable);
#ifdef SAVE_IMAGE_DATA
         //测试保存数据
         LOG(LOG_INFO, "test save data!!!!\n");
         SaveImageData();
#endif
         break;
     case BACK_DOWN:
         if(FipCallBackFunc)
         {        
             if(pFipTouchEnable->Visible.iEnable == 1)
             {
                 break;
             }
             SetPictureEnable(1, pFipTouchEnable);
             PthreadJoin();
 
             (*FipCallBackFunc)(&pWnd); 
             SendWndMsg_WindowExit(pFrmFip);  
             SendSysMsg_ThreadCreate(pWnd);
 
             break;
         }
     case HOME_DOWN:
         if(pFipTouchEnable->Visible.iEnable == 1)
         {
             break;
         }
         SetPictureEnable(1, pFipTouchEnable);
         PthreadJoin();
		 ReturnMenuOK();
         break;
     default:
         break;
     }
}


 //static GUIWINDOW* CreateFipWindow()
 //{
 //    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
 //                                   FrmFipInit, FrmFipExit, 
 //                                   FrmFipPaint, FrmFipLoop, 
 //                                   FrmFipPause, FrmFipResume,
 //                                   NULL);
 //    return pWnd;
 //}
 //static void FipReCreateWindow(GUIWINDOW **pWnd)
 //{   
 //    *pWnd = CreateFipWindow();
 //}

//设置当前的文件路径
static void SetCurrentPath(void)
{
    if(pFiberCurrPath)
    {
        char buf[512] = {0};
        getcwd(buf, 512);
        if(strncmp(buf, MntUsbDirectory, strlen(MntUsbDirectory)) != 0)
        {
            strcpy(pFiberCurrPath, buf);
        }
        else
        {
            chdir(pFiberCurrPath);
        }
    }
}

//设置菜单栏按键使能
static void setMenuEnable(int menu0, int menu1, int menu2)
{
    SetWndMenuItemEnble(MENU_ITEM0, menu0, pFipMenu);
    SetWndMenuItemEnble(MENU_ITEM1, menu1, pFipMenu);
    SetWndMenuItemEnble(MENU_ITEM2, menu2, pFipMenu);
}

//设置图像居中状态
static void changeAlignCenterState(void)
{
    if(OptFiberEndSurCheck.isAlignCenter == 1)
	{
		OptFiberEndSurCheck.isAlignCenter = 0;
		SetWndMenuItemFont(MENU_ITEM3, getGlobalFnt(EN_FONT_WHITE), pFipMenu);
        refreshAlignState(0);
    }
	else
	{
		OptFiberEndSurCheck.isAlignCenter = 1;
		SetWndMenuItemFont(MENU_ITEM3, getGlobalFnt(EN_FONT_RED), pFipMenu);
        refreshAlignState(1);
    }
    // RefreshScreen(__FILE__, __func__, __LINE__);
	SetAutoCenter(OptFiberEndSurCheck.isAlignCenter);
}

//刷新菜单栏
static void refreshMenu(void)
{
    static int iRecordConnectState = 0;

    if(iRecordConnectState != OptFiberEndSurCheck.isConnectDevice)
    {
        //刷新按钮背景
        RefreshMenuBackground(pFipMenu);
        int i;
        //使能所有菜单项
        for (i = 0; i < FIP_MENU_NUM; ++i)
        {
            SetWndMenuItemEnble(i, 1, pFipMenu);
        }

        if(OptFiberEndSurCheck.isConnectDevice)
        {
            if(OptFiberEndSurCheck.isImage)
            {
                setMenuEnable(1,1,1);
            }
            else
            {
                setMenuEnable(1,0,0);
    		}
        }
        else
        {
            setMenuEnable(1,0,0);
        }
        RefreshScreen(__FILE__, __func__, __LINE__);
        iRecordConnectState = OptFiberEndSurCheck.isConnectDevice;
    }
    else
    {
        MsecSleep(200);
    }
}

//刷新居中菜单状态
static void refreshAlignState(int iState)
{
    if (iState) //居中
    {
        SetWndMenuItemBg(2, BmpFileDirectory "bg_fip_align.bmp", pFipMenu, MENU_UNPRESS);
        SetWndMenuItemBg(2, BmpFileDirectory "bg_fip_alignSelect.bmp", pFipMenu, MENU_SELECT);
        SetWndMenuItemBg(2, BmpFileDirectory "bg_fip_alignDisable.bmp", pFipMenu, MENU_DISABLE);
    }
    else    //未居中
    {
        SetWndMenuItemBg(2, BmpFileDirectory "bg_fip_notAlign.bmp", pFipMenu, MENU_UNPRESS);
        SetWndMenuItemBg(2, BmpFileDirectory "bg_fip_notAlignSelect.bmp", pFipMenu, MENU_SELECT);
        SetWndMenuItemBg(2, BmpFileDirectory "bg_fip_notAlignDisable.bmp", pFipMenu, MENU_DISABLE);
    }
}

//使用Fiber Microscope打开GDM文件
int FiberViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where)
{
	int iRet = 0;
    iRet = InitializeDetector();
	if(!iRet)
	{
	    FipCallBackFunc = where;
	    iCheckErrFlag = 0;
	    iThreadExitFlag = 1;
	    fiberholdstate = 0;
        
        iRet = ReadGdmFile(fileName, &OptFiberEndSurCheck.ResultInfo);
        SetCurrentPath();
    	if (!iRet)
    	{
    	    OptFiberEndSurCheck.isScreen = 1;
    	    OptFiberEndSurCheck.Sharpness = 0;

		    char *gdmName = strrchr(fileName, '/')+1;
		    
        	if(gdmName)
        	{
                strcpy(OptFiberEndSurCheck.cName, gdmName);
            }
            
    		GUIWINDOW *pWnd = NULL;
    		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    		                    FrmFipInit, FrmFipExit, 
    		                    FrmFipPaint, FrmFipLoop, 
    					        FrmFipPause, FrmFipResume,
    		                    NULL);          //pWnd由调度线程释放
    	    SendWndMsg_WindowExit(from);	    //发送消息以便退出前一个窗体
        	SendSysMsg_ThreadCreate(pWnd);
        }

	}
	else
	{
		DialogInit(120, 90, TransString("Warning"), 
				   GetCurrLanguageText(DIALOG_LBL_INSERT_DETECTOR), 
				   0, 0, where, NULL, NULL);
	}	
	return iRet;
}

/***
  * 功能：
		调用fip窗体
  * 参数：
		GUIWINDOW *LastWindow   : 上个窗体
		FIPCALLLBACK func : 回调函数
  * 返回：
		空
  * 备注：	
***/
void CallFipWnd(GUIWINDOW *LastWindow, FIPCALLLBACK func)
{
	int iRet = 0;
	iRet = InitializeDetector();
	if(!iRet)
	{
	    FipCallBackFunc = func;
	    iCheckErrFlag = 0;
	    iThreadExitFlag = 1;
	    fiberholdstate = 0;
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		                    FrmFipInit, FrmFipExit, 
		                    FrmFipPaint, FrmFipLoop, 
					        FrmFipPause, FrmFipResume,
		                    NULL);          //pWnd由调度线程释放
	    SendWndMsg_WindowExit(LastWindow);	    //发送消息以便退出前一个窗体
    	SendSysMsg_ThreadCreate(pWnd);
	}
	else
	{
		DialogInit(120, 90, TransString("Warning"), 
				   GetCurrLanguageText(DIALOG_LBL_INSERT_DETECTOR), 
				   0, 0, func, NULL, NULL);
	}
}
