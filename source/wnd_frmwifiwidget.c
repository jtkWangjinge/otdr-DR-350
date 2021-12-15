/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifiwidget.c
* 摘    要：  wifi信息控件整合
*             
*
* 当前版本：  v1.0.0
* 作    者：  xiazhizhu
* 完成日期：  2016-4-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmwifiwidget.h"
#include "wnd_global.h"

/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/	
/* 未选中状态时wifi加密图标 */
static char *pLockBmp[] = {"", BmpFileDirectory"ico_wifi_encrypt_unpress.bmp"};
/* 选中状态时wifi加密图标 */
static char *pLockPressBmp[] = {"", BmpFileDirectory"ico_wifi_encrypt_press.bmp"};

/* 未选中状态时wifi强度图标 */
static char *pLevelBmp[] = 
{
	BmpFileDirectory"ico_wifi_empty_unpress.bmp", 
	BmpFileDirectory"ico_wifi_quar_unpress.bmp", 
	BmpFileDirectory"ico_wifi_half_unpress.bmp", 
	BmpFileDirectory"ico_wifi_three_quars_unpress.bmp", 
	BmpFileDirectory"ico_wifi_full_unpress.bmp"
};

/* 选中状态时wifi强度图标 */
static char *pLevelPressBmp[] = 
{
	BmpFileDirectory"ico_wifi_empty_press.bmp", 
	BmpFileDirectory"ico_wifi_quar_press.bmp", 
	BmpFileDirectory"ico_wifi_half_press.bmp", 
	BmpFileDirectory"ico_wifi_three_quars_press.bmp", 
	BmpFileDirectory"ico_wifi_full_press.bmp"
};


/*******************************************************************************
**								其他函数声明				 					  **
*******************************************************************************/
//wifi控件的资源销毁函数
int DestroyWifiWidget(WIFIWIDGET **pWifiObj);
//设置wifi控件
static int SetWifiWidget(WIFIWIDGET *pWifiObj, GUICHAR *pStrName, char *pLockBmp, 
				  char *pLvlBmp
				  );





/***
  * 功能：
        初始化wifi显示控件(wifi名称、强度值、强度图标、加密锁)
  * 参数：
        1.int iStartX :		起始点横坐标
        2.int iStartY :		起始点纵坐标
  * 返回：
        成功返回wifi控件对象信息，失败返回NULL
  * 备注：
***/
WIFIWIDGET* CreateWifiWidget(int iStartX, int iStartY)
{
	int iOffset = 0;
	int iWdith = 308;
	WIFIWIDGET *pWifiObj = NULL;

	pWifiObj = (WIFIWIDGET *)malloc(sizeof(WIFIWIDGET));
	
	//标签对象可视信息赋值
	pWifiObj->Visible.Area.Start.x = iStartX;
	pWifiObj->Visible.Area.Start.y = iStartY + 4;
	pWifiObj->Visible.Area.End.x = iStartX + iWdith - 1;
	pWifiObj->Visible.Area.End.y = iStartY + 24 - 1;
	pWifiObj->Visible.iEnable = 1;	//默认可见
	pWifiObj->Visible.iLayer = 0;	//默认图层为0(最底层)
	pWifiObj->Visible.iFocus = 0;	//默认无焦点
	pWifiObj->Visible.iCursor = 0;	//默认无光标
	memset(&(pWifiObj->Visible.Hide), 0, sizeof(GUIRECTANGLE));
	pWifiObj->Visible.pResume = NULL;
	pWifiObj->Visible.fnDestroy = (DESTFUNC) DestroyWifiWidget;
	pWifiObj->Visible.fnDisplay = (DISPFUNC) DisplayWifiWidget;


	pWifiObj->pBg = CreatePicture(iStartX, iStartY, iWdith, 25, BmpFileDirectory"btn_wifi_ssid_unpress.bmp");

	iOffset = iWdith - 25;
	pWifiObj->pLevel = CreatePicture(iStartX + iOffset, (iStartY + 5), 19, 15, NULL);//wifi信号图标
	iOffset -= 20;
	pWifiObj->pLock = CreatePicture(iStartX + iOffset, (iStartY + 6), 11, 14, NULL);//wifi加密锁

	pWifiObj->pName = CreateLabel(iStartX+10, iStartY+4, iOffset, 25, NULL);//wifi SSID名称

	pWifiObj->iLock = 0;
	pWifiObj->iLevel = 0;
	pWifiObj->pLockBmp = pLockBmp;
	pWifiObj->pLvlBmp = pLevelBmp;
	
	return pWifiObj;
}

/***
  * 功能：
        wifi控件资源销毁
  * 参数：
        1.WIFIWIDGET **pWifiObj :		wifi控件信息结构
  * 返回：
        成功返回0,失败返回非0
  * 备注：
***/
int DestroyWifiWidget(WIFIWIDGET **pWifiObj)
{
	int iReturn = 0;
	if(NULL == pWifiObj)
	{
		iReturn = -1;
	}
	if( NULL == *pWifiObj )
	{
		iReturn = -2;
	}
	if( 0 == iReturn )
	{
		if(NULL != (*pWifiObj)->pBg)
		{
			DestroyPicture(&((*pWifiObj)->pBg));
		}
		if(NULL != (*pWifiObj)->pName)
		{
			DestroyLabel(&((*pWifiObj)->pName));
		}
	
		if(NULL != (*pWifiObj)->pLock)
		{
			DestroyPicture(&((*pWifiObj)->pLock));
		}
		if(NULL != (*pWifiObj)->pLevel)
		{
			DestroyPicture(&((*pWifiObj)->pLevel));
		}

		GuiMemFree(*pWifiObj);
		*pWifiObj = NULL;
	}

	return iReturn;
}

/***
  * 功能：
        wifi控件显示
  * 参数：
        1.WIFIWIDGET **pWifiObj :		wifi控件信息结构
  * 返回：
        成功返回0,失败返回非0
  * 备注：
***/
int DisplayWifiWidget(WIFIWIDGET *pWifiObj)
{
	DisplayPicture(pWifiObj->pBg);
	DisplayLabel(pWifiObj->pName);
	DisplayPicture(pWifiObj->pLock);
	DisplayPicture(pWifiObj->pLevel);

	return 0;
}

/***
  * 功能：
        wifi控件设置
  * 参数：
        1.WIFIWIDGET **pWifiObj :	wifi控件信息结构
		2.GUICHAR *pStrName :		wifi控件中ssid项
		3.char *pLockBmp :			wifi控件中encrypt加密锁项
		4.char *pLvlBmp:			wifi控件中信号强度图标项
		5.GUICHAR *pLvlNum:			wifi控件中Level值项 
  * 返回：
        成功返回0,失败返回非0
  * 备注：
***/
static int SetWifiWidget(WIFIWIDGET *pWifiObj, GUICHAR *pStrName, char *pLockBmp, 
				  char *pLvlBmp)
{
	int iReturn = 0;
	SetWifiwidgeEnable(1,pWifiObj);
	
	SetLabelText(pStrName, pWifiObj->pName);
	SetPictureBitmap(pLockBmp, pWifiObj->pLock);
	SetPictureBitmap(pLvlBmp, pWifiObj->pLevel);

	return iReturn;
}


/***
  * 功能：
        根据data设置具体显示的wifi控件信息
  * 参数：
        1.WIFIWIDGET **pWifiObj :	wifi控件信息结构
		2.int iLvlNum :				wifi控件中信号强度值
		3.int iLock :				wifi控件中是否显示加密锁
		4.int iLevel:				wifi控件中信号强度图
  * 返回：
        成功返回0,失败返回非0
  * 备注：
***/
int SetWifiWidgetData(WIFIWIDGET *pWifiObj, char *pStrName,int iLock, int iLevel)
{
	int iReturn = 0;
	GUICHAR *pName = TransString(pStrName);
	SetWifiWidget(pWifiObj, pName, pWifiObj->pLockBmp[iLock], pWifiObj->pLvlBmp[iLevel]);
	pWifiObj->iLock = iLock;
	pWifiObj->iLevel = iLevel;
	GuiMemFree(pName);

	return iReturn;
}


/***
  * 功能：
        设置wifi控件的字体信息
  * 参数：
        1.WIFIWIDGET **pWifiObj :	wifi控件信息结构
		2.GUIFONT *pFont :			GUI字体
  * 返回：
        成功返回0,失败返回非0
  * 备注：
***/
int SetWifiWidgetFont(WIFIWIDGET *pWifiObj, GUIFONT *pFont)
{
	int iReturn = 0;
	SetLabelFont(pFont, pWifiObj->pName);	

	return iReturn;
}

/***
  * 功能：
        wifi控件 down事件(更改显示)
  * 参数：
        1.WIFIWIDGET **pWifiObj :	wifi控件信息结构
  * 返回：
  * 备注：
***/
void WifiWidgetDown(WIFIWIDGET *pWifiObj)
{
	pWifiObj->pLockBmp = pLockPressBmp;
	pWifiObj->pLvlBmp = pLevelPressBmp;
	SetPictureBitmap(BmpFileDirectory"btn_wifi_ssid_press.bmp", pWifiObj->pBg);
	SetPictureBitmap(pWifiObj->pLockBmp[pWifiObj->iLock], pWifiObj->pLock);
	SetPictureBitmap(pWifiObj->pLvlBmp[pWifiObj->iLevel], pWifiObj->pLevel);
}

/***
  * 功能：
        wifi控件 up事件(更改显示)
  * 参数：
        1.WIFIWIDGET **pWifiObj :	wifi控件信息结构
  * 返回：
  * 备注：
***/
void WifiWidgetUp(WIFIWIDGET *pWifiObj, int index)
{
	pWifiObj->pLockBmp = pLockBmp;
	pWifiObj->pLvlBmp = pLevelBmp;
    if(0 == index%2)
    {
        SetPictureBitmap(BmpFileDirectory"btn_wifi_ssid_unpress.bmp", pWifiObj->pBg);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory"btn_wifi_ssid_unpress1.bmp", pWifiObj->pBg);
    }	
	SetPictureBitmap(pWifiObj->pLockBmp[pWifiObj->iLock], pWifiObj->pLock);
	SetPictureBitmap(pWifiObj->pLvlBmp[pWifiObj->iLevel], pWifiObj->pLevel);
}


/***
  * 功能：
		设置图形框的可见性
  * 参数：
		1.int iEnable:			图形框是否可见，0不可见，1可见
		5.WIFIWIDGET *pPicObj:	需要设置可见性的图形框对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int SetWifiwidgeEnable(int iEnable, WIFIWIDGET *pPicObj)
{
	//错误标志、返回值定义
	int iErr = 0;

	if (!iErr)
	{
		//判断pPicObj是否有效
		if (NULL == pPicObj)
		{
			iErr = -1;
		}
	}

	pPicObj->Visible.iEnable = iEnable;

	return 0;
}

