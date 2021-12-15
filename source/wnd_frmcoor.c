/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcoor.c
* 摘    要：  实现otdr坐标系控件
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本： 
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmcoor.h"

/*******************************
*定义wnd_frmcoor.c所需要的头文件
*******************************/
#include "app_global.h"
#include "app_frminit.h"
#include "app_curve.h"

/*****************************
*定义wnd_frmcoor.c使用内部函数
*****************************/
// static int CoorBtnHome_Down(void *pInArg, int iInLen,
// 						void *pOutArg, int iOutLen);
static int CoorBtnSet_Down(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);
static int CoorBtnFile_Down(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);

static int CoorBtnWave_Down(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);
static int CoorBtn_Down(void *pInArg, int iInLen, 
						void *pOutArg, int iOutLen);

static char *pBtnBmpResources[OPTION_NUM] = {
	BmpFileDirectory "btn_otdr_curve_unpress.bmp",
	BmpFileDirectory "btn_event_unpress.bmp",
	BmpFileDirectory "btn_dB_unpress.bmp",
	BmpFileDirectory "btn_linkMap_unpress.bmp",
};
static char *pBtnBmpf[OPTION_NUM] = {
	BmpFileDirectory "btn_otdr_curve_press.bmp",
	BmpFileDirectory "btn_event_press.bmp",
	BmpFileDirectory "btn_dB_press.bmp",
	BmpFileDirectory "btn_linkMap_press.bmp",
};
//波长按钮
static char *pBtnWaveBmp[CURVE_WAVE_NUM] = 
{
	BmpFileDirectory"btn_otdr_wave1_unpress.bmp", 
	BmpFileDirectory"btn_otdr_wave2_unpress.bmp",
};
static char *pBtnWaveBmpf[CURVE_WAVE_NUM] = 
{
	BmpFileDirectory"btn_otdr_wave1_press.bmp", 
	BmpFileDirectory"btn_otdr_wave2_press.bmp",
};

static int iSelectedWaveIndex = 0;//选中的波长选项

//设置波长按钮的使能
static void SetCoorWaveEnable(OTDRCOOR* pCoorObj, int iEnable, int iWaveIndex);

/***
  * 功能：
       	创建otdr坐标系
  * 参数：
  		MSGFUNC ppFunc[4]:4个选项卡的回调函数
  		MSGFUNC pCoorDn	 :波形区域点击处理函数
  		MSGFUNC pCoorUp	 :波形区域弹起处理函数
  		GUIWINDOW *pWnd  :调用控件的所属窗体
  		int iType		 :控件类型 COOR_NOMAL:普通，COOR_LARGE:全屏。
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 

OTDRCOOR *CreateOtdrCoor(MSGFUNC *ppFunc, MSGFUNC pCoorDn, MSGFUNC pCoorMv, 
						 MSGFUNC pCoorUp, MSGFUNC pCoorMT, int iFocus, 
						 GUIWINDOW *pWnd, int iType)
{
	OTDRCOOR *pCoorObj = NULL;
	int iTmp = 0;

	//坐标系控件分配空间
	pCoorObj = (OTDRCOOR *)malloc(sizeof(OTDRCOOR));
	if (NULL == pCoorObj)
	{
		return NULL;
	}
	
	//获取坐标系点击执行函数
	int i;
	for (i = 0; i < (FUNCTION_NUM + OPTION_NUM); ++i)
	{
		pCoorObj->pBtnFunc[i] = ppFunc[i];
	}

	//波形背景点击处理函数
	pCoorObj->pCoorFuncDn = pCoorDn;
	pCoorObj->pCoorFuncMv = pCoorMv;
	pCoorObj->pCoorFuncUp = pCoorUp;
	pCoorObj->pCoorFuncMT = pCoorMT;

	pCoorObj->pBgIcon = CreatePicture(0, 0, 40, 40, BmpFileDirectory"bg_otdr_icon.bmp");
	pCoorObj->pStrTitle = TransString("OTDR");
	pCoorObj->pLblTitle = CreateLabel(40, 12, 100, 16, pCoorObj->pStrTitle);
	pCoorObj->pCveBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"otdr_coor.bmp");
	//set,File
	pCoorObj->pBtnSet = CreatePicture(215, 47, 50, 35, BmpFileDirectory"btn_otdr_set_unpress.bmp");
	pCoorObj->pBtnFile = CreatePicture(268, 47, 50, 35, BmpFileDirectory"btn_otdr_file_unpress.bmp");
    pCoorObj->pBtnSwitchWave = CreatePicture(321, 47, 50, 35, BmpFileDirectory "btn_otdr_wave_unpress.bmp");
    //wave
	char *strWave[CURVE_WAVE_NUM] = {"1310nm", "1550nm"};
	for (iTmp = 0; iTmp < CURVE_WAVE_NUM; ++iTmp)
	{
		pCoorObj->pBtnWave[iTmp] = CreatePicture(452 + 95 * iTmp, 47, 90, 35, pBtnWaveBmp[i]);
		pCoorObj->pStrWave[iTmp] = TransString(strWave[iTmp]);
		pCoorObj->pLblWave[iTmp] = CreateLabel(464 + 90 * iTmp, 60, 60, 16, pCoorObj->pStrWave[iTmp]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pCoorObj->pLblWave[iTmp]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCoorObj->pLblWave[iTmp]);
	}

	//创建波形绘制区域控件
	if (0 == iType)
	{
		pCoorObj->pCurve = CreatePicture(COOR_S_SX, COOR_S_SY, 
 	   						     		 COOR_S_EX - COOR_S_SX + 1, 
 	   						     		 COOR_S_EY - COOR_S_SY + 1, 
 								 		 NULL);
		pCoorObj->pCurve->Visible.iCursor = 1;
		pCoorObj->pCurve->Visible.iLayer = OTDRCOOR_LAYER;
		SetCoorNomal(pOtdrTopSettings->pDisplayInfo);
	} 
	else//全屏状态
	{
		pCoorObj->pCurve = CreatePicture(COOR_B_SX, COOR_B_SY, 
 	   						     		 COOR_B_EX - COOR_B_SX + 1, 
 	   						     		 COOR_B_EY - COOR_B_SY + 1, 
 								 		 NULL);
		pCoorObj->pCurve->Visible.iCursor = 1;
		pCoorObj->pCurve->Visible.iLayer = OTDRCOOR_LAYER;
		SetCoorLarge(pOtdrTopSettings->pDisplayInfo);
	}

	pCoorObj->pPicFile = CreatePicture(190, 0, 241, 40, BmpFileDirectory"bg_otdr_fileName.bmp");
	//创建选项卡控件
	for (iTmp = 0; iTmp < OPTION_NUM; ++iTmp)
	{
		pCoorObj->pBtnOp[iTmp] = CreatePicture(3 + 53 * iTmp, 47, 50, 35, pBtnBmpResources[iTmp]);
	}

	pCoorObj->iFocus = iFocus;
	SetPictureBitmap(pBtnBmpf[iFocus], pCoorObj->pBtnOp[iFocus]);
	
	return pCoorObj;
}

//设置坐标轴选中项
void SetCoorSelectedOption(OTDRCOOR *pCoorObj, int iSelectedOption, int iFlush)
{
	SetPictureBitmap(BmpFileDirectory"btn_otdr_set_unpress.bmp", pCoorObj->pBtnSet);
	SetPictureBitmap(BmpFileDirectory"btn_otdr_file_unpress.bmp", pCoorObj->pBtnFile);
    SetPictureBitmap(BmpFileDirectory"btn_otdr_wave_unpress.bmp", pCoorObj->pBtnSwitchWave);
    //刷新曲线、event、db图标
	int i;
	for (i = 0; i < OPTION_NUM; ++i)
	{
		SetPictureBitmap(pBtnBmpResources[i], pCoorObj->pBtnOp[i]);
	}

	switch (iSelectedOption)
	{
	case 4://set
		SetPictureBitmap(BmpFileDirectory"btn_otdr_set_select.bmp", pCoorObj->pBtnSet);
		break;
	case 5://openfile
		SetPictureBitmap(BmpFileDirectory"btn_otdr_file_select.bmp", pCoorObj->pBtnFile);
		break;
	case 6: //wave
        SetPictureBitmap(BmpFileDirectory"btn_otdr_wave_select.bmp", pCoorObj->pBtnSwitchWave);
        break;
	case 7:
    case 8:
        SetPictureBitmap(BmpFileDirectory"btn_otdr_set_unpress.bmp", pCoorObj->pBtnSet);
		SetPictureBitmap(BmpFileDirectory"btn_otdr_file_unpress.bmp", pCoorObj->pBtnFile);
        SetPictureBitmap(BmpFileDirectory"btn_otdr_wave_unpress.bmp", pCoorObj->pBtnSwitchWave);
        break;
	default:
		{
			int iFocus = pCoorObj->iFocus;
			SetPictureBitmap(pBtnBmpf[iFocus], pCoorObj->pBtnOp[iFocus]);
		}
		break;
	}

	if (iFlush)
	{
		DisplayPicture(pCoorObj->pBtnSet);
		DisplayPicture(pCoorObj->pBtnFile);
        DisplayPicture(pCoorObj->pBtnSwitchWave);

        for (i = 0; i < OPTION_NUM; ++i)
		{
			DisplayPicture(pCoorObj->pBtnOp[i]);
		}
	}
}

//添加坐标轴控件到接受消息队列 注册消息处理函数
int AddAndLoginCoor(OTDRCOOR *pCoorObj, GUIWINDOW *pWnd)
{
	//添加控件到消息队列
	// AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
	// 			  pCoorObj->pCurve, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pCoorObj->pBtnSet, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pCoorObj->pBtnFile, pWnd);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                  pCoorObj->pBtnSwitchWave, pWnd);

    int i;
	for (i = 0; i < OPTION_NUM; ++i)
	{
		if (i < CURVE_WAVE_NUM)
		{
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				pCoorObj->pBtnWave[i], pWnd);
		}

		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
			pCoorObj->pBtnOp[i], pWnd);
	}
	//注册上控件的消息处理 
  	GUIMESSAGE *pMsg = GetCurrMessage();
	//选项卡消息注册
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnSet,
					CoorBtnSet_Down, (void *)pCoorObj, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnFile,
					CoorBtnFile_Down, (void *)pCoorObj, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnSwitchWave,
                    CoorBtnWave_Down, (void *)pCoorObj, 0, pMsg);

    for (i = 0; i < OPTION_NUM; ++i)
	{
		if (i < CURVE_WAVE_NUM)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnWave[i],
				CoorBtnWave_Down, (void *)pCoorObj, i, pMsg);
		}

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnOp[i],
			CoorBtn_Down, (void *)pCoorObj, i, pMsg);
	}

	//波形区域消息注册
	// LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pCurve, 
    //                 pCoorObj->pCoorFuncDn, (void *)pCoorObj, 0, pMsg);
	// LoginMessageReg(GUIMESSAGE_TCH_UP, pCoorObj->pCurve, 
    //                 pCoorObj->pCoorFuncUp, (void *)pCoorObj, 0, pMsg);
	// LoginMessageReg(GUIMESSAGE_MOV_CURSOR, pCoorObj->pCurve, 
    //                 pCoorObj->pCoorFuncMv, (void *)pCoorObj, 0, pMsg);
	// LoginMessageReg(GUIMESSAGE_MT, pCoorObj->pCurve, 
    //                 pCoorObj->pCoorFuncMT, (void *)pCoorObj, 0, pMsg);
	return 0;
}

//从队列摘下坐标轴控件并注销消息处理函数
int DelAndLogoutCoor(OTDRCOOR *pCoorObj, GUIWINDOW *pWnd)
{
	if ((pCoorObj == NULL) || (pWnd == NULL))
	{
		LOG(LOG_ERROR, "---pCoorObj == 0x%p---or pWnd == 0x%p-----\n", pCoorObj, pWnd);
		return -1;
	}
	//从控件队列摘下控件
	// DelWindowComp(pCoorObj->pCurve, pWnd);
	DelWindowComp(pCoorObj->pBtnSet, pWnd);
	DelWindowComp(pCoorObj->pBtnFile, pWnd);
    DelWindowComp(pCoorObj->pBtnSwitchWave, pWnd);
    int i;
	for (i = 0; i < OPTION_NUM; ++i)
	{
		if (i < CURVE_WAVE_NUM)
		{
			DelWindowComp(pCoorObj->pBtnWave[i], pWnd);
		}

		DelWindowComp(pCoorObj->pBtnOp[i], pWnd);
	}
	//注册上控件的消息处理 
  	GUIMESSAGE *pMsg = GetCurrMessage();
	//选项卡消息注册
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnSet, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnFile, pMsg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnSwitchWave, pMsg);

    for (i = 0; i < OPTION_NUM; ++i)
	{
		if (i < CURVE_WAVE_NUM)
		{
			LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnWave[i], pMsg);
		}

		LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pBtnOp[i], pMsg);
	}
	//波形区域消息注册
	// LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCoorObj->pCurve, pMsg);
	// LogoutMessageReg(GUIMESSAGE_TCH_UP, pCoorObj->pCurve, pMsg);
	// LogoutMessageReg(GUIMESSAGE_MOV_CURSOR, pCoorObj->pCurve, pMsg);
	// LogoutMessageReg(GUIMESSAGE_MT, pCoorObj->pCurve, pMsg);

	return 0;
}

//绘制坐标轴控件
int PaintCoor(OTDRCOOR *pCoorObj)
{
	int iErr = 0;
	DisplayPicture(pCoorObj->pCveBg);
	// DisplayPicture(pCoorObj->pBgIcon);
	DisplayPicture(pCoorObj->pBtnSet);
	DisplayPicture(pCoorObj->pBtnFile);
    DisplayPicture(pCoorObj->pBtnSwitchWave);
    int i;
	for (i = 0; i < OPTION_NUM; ++i)
	{
		DisplayPicture(pCoorObj->pBtnOp[i]);
	}
	
	DisplayPicture(pCoorObj->pPicFile);
	// DisplayLabel(pCoorObj->pLblTitle);
	// int iEnable[2] = {pUser_Settings->iWave[0], pUser_Settings->iWave[1]};
	// SetOptionWaveEnable(pCoorObj, iEnable);

	return iErr;
}

//销毁坐标轴控件
int DestroyCoor(OTDRCOOR **pCoorObj)
{
	int iErr = 0;
	
	DestroyPicture(&((*pCoorObj)->pBgIcon));
	DestroyPicture(&((*pCoorObj)->pCveBg));

	DestroyPicture(&((*pCoorObj)->pBtnSet));
	DestroyPicture(&((*pCoorObj)->pBtnFile));
    DestroyPicture(&((*pCoorObj)->pBtnSwitchWave));

    int i;
	for (i = 0; i < OPTION_NUM; ++i)
	{
		if (i < CURVE_WAVE_NUM)
		{
			DestroyPicture(&((*pCoorObj)->pBtnWave[i]));
			DestroyLabel(&((*pCoorObj)->pLblWave[i]));
			free((*pCoorObj)->pStrWave[i]);
		}

		DestroyPicture(&((*pCoorObj)->pBtnOp[i]));
	}

	DestroyPicture(&((*pCoorObj)->pPicFile));
	DestroyPicture(&((*pCoorObj)->pCurve));
	DestroyLabel(&((*pCoorObj)->pLblTitle));
	free((*pCoorObj)->pStrTitle);
	free(*pCoorObj);
	(*pCoorObj) = NULL;
	
	return iErr;
}

//返回主菜单按钮响应
// int CoorBtnHome_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
// {
// 	OTDRCOOR *pCoorObj = (OTDRCOOR *)pOutArg;
// 	//执行回调函数
// 	if (pCoorObj->pBtnFunc[OPTION_NUM])
// 	{
// 		(*(pCoorObj->pBtnFunc[OPTION_NUM]))(pInArg, iInLen, pOutArg, iOutLen);
// 	}

// 	return 0;
// }

//otdr设置按钮响应函数
int CoorBtnSet_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	OTDRCOOR *pCoorObj = (OTDRCOOR *)pOutArg;
	//执行回调函数
	if (pCoorObj->pBtnFunc[OPTION_NUM])
	{
		(*(pCoorObj->pBtnFunc[OPTION_NUM]))(pInArg, iInLen, pOutArg, iOutLen);
	}

	return 0;
}

//点击文件按钮响应函数
int CoorBtnFile_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	OTDRCOOR *pCoorObj = (OTDRCOOR *)pOutArg;
	//执行回调函数
	if (pCoorObj->pBtnFunc[OPTION_NUM + 1])
	{
		(*(pCoorObj->pBtnFunc[OPTION_NUM + 1]))(pInArg, iInLen, pOutArg, iOutLen);
	}

	return 0;
}

//点击波长按钮响应函数
int CoorBtnWave_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	OTDRCOOR *pCoorObj = (OTDRCOOR *)pOutArg;
	iSelectedWaveIndex = iOutLen;
	//执行回调函数
	if (pCoorObj->pBtnFunc[OPTION_NUM+2])
	{
		(*(pCoorObj->pBtnFunc[OPTION_NUM+2]))(pInArg, iInLen, pOutArg, iOutLen);
	}

	return 0;
}

//选项卡弹起消息处理
static int CoorBtn_Down(void *pInArg, int iInLen, 
					    void *pOutArg, int iOutLen)
{
	int iErr = 0;
	int iFocus = iOutLen;
	OTDRCOOR *pCoorObj = (OTDRCOOR *)pOutArg;

	if (pCoorObj->iFocus != iFocus)
	{
		//交换选中的选项卡和第一个选项卡
		SetPictureBitmap(pBtnBmpf[iFocus], pCoorObj->pBtnOp[iFocus]);
        SetPictureBitmap(pBtnBmpResources[pCoorObj->iFocus], 
						 pCoorObj->pBtnOp[pCoorObj->iFocus]);
	}
	else//切换同样选项卡
	{
		SetPictureBitmap(pBtnBmpf[iFocus], pCoorObj->pBtnOp[iFocus]);
	}

	DisplayPicture(pCoorObj->pBtnOp[iFocus]);
	DisplayPicture(pCoorObj->pBtnOp[pCoorObj->iFocus]);
	RefreshScreen(__FILE__, __func__, __LINE__);

	//执行回调函数
	if (pCoorObj->pBtnFunc[iFocus])
	{
		(*(pCoorObj->pBtnFunc[iFocus]))(pInArg, iInLen, pOutArg, iOutLen);
	}

	pCoorObj->iFocus = iFocus;

	return iErr;
}

//设置当前选中的选项卡
void SetCoorFocus(OTDRCOOR* pCoorObj, int iFocus)
{
    CoorBtn_Down(NULL, 0, pCoorObj, iFocus);    
}

//设置波长按钮的使能
static void SetCoorWaveEnable(OTDRCOOR* pCoorObj, int iEnable, int iWaveIndex)
{
	if (!iEnable)
	{
		SetPictureBitmap(pBtnWaveBmp[iWaveIndex], pCoorObj->pBtnWave[iWaveIndex]);
		DisplayPicture(pCoorObj->pBtnWave[iWaveIndex]);
		// SetPictureEnable(0, pCoorObj->pBtnWave[iWaveIndex]);
	}
	else
	{
		SetPictureEnable(1, pCoorObj->pBtnWave[iWaveIndex]);

		if (iWaveIndex == iSelectedWaveIndex)
		{
			SetPictureBitmap(pBtnWaveBmpf[iWaveIndex], pCoorObj->pBtnWave[iWaveIndex]);
			DisplayPicture(pCoorObj->pBtnWave[iWaveIndex]);
		}
		else
		{
			SetPictureBitmap(pBtnWaveBmp[iWaveIndex], pCoorObj->pBtnWave[iWaveIndex]);
			DisplayPicture(pCoorObj->pBtnWave[iWaveIndex]);
		}
	}
}

//设置波长选项卡的使能
void SetOptionWaveEnable(OTDRCOOR* pCoorObj, int* iEnable)
{
	//目前支持1310，1550波长
	SetCoorWaveEnable(pCoorObj, iEnable[0], 0);
	SetCoorWaveEnable(pCoorObj, iEnable[1], 1);
}

//设置初始选中的波长
void SetInitalWaveIndex(int iWaveIndex)
{
	iSelectedWaveIndex = iWaveIndex;
}

//重新设置曲线
void ResetCurve(OTDRCOOR* pCoorObj, int type)
{
	DestroyPicture(&(pCoorObj->pCurve));
	if (type == 0)
	{
		pCoorObj->pCurve = CreatePicture(COOR_S_SX, COOR_S_SY,
			COOR_S_EX - COOR_S_SX + 1,
			COOR_S_EY - COOR_S_SY + 1,
			NULL);
		pCoorObj->pCurve->Visible.iCursor = 1;
		pCoorObj->pCurve->Visible.iLayer = OTDRCOOR_LAYER;
		SetCoorNomal(pOtdrTopSettings->pDisplayInfo);
	}
	else
	{
		pCoorObj->pCurve = CreatePicture(COOR_B_SX, COOR_B_SY,
			COOR_B_EX - COOR_B_SX + 1,
			COOR_B_EY - COOR_B_SY + 1,
			NULL);
		pCoorObj->pCurve->Visible.iCursor = 1;
		pCoorObj->pCurve->Visible.iLayer = OTDRCOOR_LAYER;
		SetCoorLarge(pOtdrTopSettings->pDisplayInfo);
	}

	DisplayPicture(pCoorObj->pCurve);
}

//设置控件使能（主菜单+设置+文件）
void SetCoorEnabled(OTDRCOOR* pCoorObj, int iSelectedOption, int iEnabled)
{
	if (iEnabled)
	{
		SetPictureEnable(1, pCoorObj->pBtnSet);
		SetPictureEnable(1, pCoorObj->pBtnFile);
        SetPictureEnable(1, pCoorObj->pBtnSwitchWave);

        SetPictureBitmap(BmpFileDirectory"btn_otdr_set_unpress.bmp", pCoorObj->pBtnSet);
		SetPictureBitmap(BmpFileDirectory"btn_otdr_file_unpress.bmp", pCoorObj->pBtnFile);
        SetPictureBitmap(BmpFileDirectory"btn_otdr_wave_unpress.bmp", pCoorObj->pBtnSwitchWave);
        //恢复使能时恢复原有的选中项
		switch (iSelectedOption)
		{
		case 4://set
			SetPictureBitmap(BmpFileDirectory"btn_otdr_set_select.bmp", pCoorObj->pBtnSet);
			break;
		case 5://file
			SetPictureBitmap(BmpFileDirectory"btn_otdr_file_select.bmp", pCoorObj->pBtnFile);
			break;
        case 6: // wave
            SetPictureBitmap(BmpFileDirectory "btn_otdr_wave_select.bmp", pCoorObj->pBtnSwitchWave);
            break;
        default:
			break;
		}

		DisplayPicture(pCoorObj->pBtnSet);
		DisplayPicture(pCoorObj->pBtnFile);
        DisplayPicture(pCoorObj->pBtnSwitchWave);
    }
	else
	{
		SetPictureBitmap(BmpFileDirectory "btn_otdr_set_unpress.bmp", pCoorObj->pBtnSet);
		SetPictureBitmap(BmpFileDirectory "btn_otdr_file_unpress.bmp", pCoorObj->pBtnFile);
        SetPictureBitmap(BmpFileDirectory "btn_otdr_wave_unpress.bmp", pCoorObj->pBtnSwitchWave);

        DisplayPicture(pCoorObj->pBtnSet);
		DisplayPicture(pCoorObj->pBtnFile);
        DisplayPicture(pCoorObj->pBtnSwitchWave);

        SetPictureEnable(0, pCoorObj->pBtnSet);
		SetPictureEnable(0, pCoorObj->pBtnFile);
        SetPictureEnable(0, pCoorObj->pBtnSwitchWave);
    }
}

