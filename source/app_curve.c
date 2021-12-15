/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_curve.c
* 摘    要：  实现OTDR曲线显示函数
*
* 当前版本：  v1.1.0 (重写数据结构，与GUI的交互使用循环队列)
* 作    者：
* 完成日期：
*******************************************************************************/

#include "app_curve.h"

/**************************************
* 为实现APP_CURVE而需要引用的其他头文件
**************************************/
#include "app_queue.h"
#include "app_frmotdr.h"
#include "app_frminit.h"
#include "app_math.h"
#include "app_frmotdrmessage.h"
#include "app_log.h"
#include "guiphoto.h"

#include "app_algorithm_support.h"
#include "app_unitconverter.h"
/******************************
* 为实现APP_CURVE定义的调式方法
******************************/

//log 开关
#define ENABLE_CURVE_TIME_DEBUG         0
#define ENABLE_CURVE_CALL_STACK_DEBUG   0

#if (ENABLE_CURVE_TIME_DEBUG == 1)
#define DBG_INIT_TIME() struct timeval stv, etv; gettimeofday(&stv, NULL)
#define DBG_PRINT_TIME() \
		gettimeofday(&etv, NULL); \
		CURVE_DEBUG("[CURVE|%s: %d]: used %ld usec\n", __func__, __LINE__,\
		            (((etv).tv_sec) - ((stv).tv_sec)) * 1000000 + \
		            (((etv).tv_usec) - ((stv).tv_usec)))
#else
#define DBG_INIT_TIME()
#define DBG_PRINT_TIME()
#endif

#if (ENABLE_CURVE_CALL_STACK_DEBUG == 1)
#define CURVE_DBG_ENTER()		CURVE_DEBUG("#########Enter#########\n");
#define CURVE_DBG_EXIT(rtn)  	CURVE_DEBUG("#########Exit with code = (%d)#########\n", rtn)
#else
#define CURVE_DBG_ENTER()
#define CURVE_DBG_EXIT(rtn)
#endif

//绘图区域的最小显示高度 最大放大80倍
#define MIN_DISPLAY_HEGHT  (MAX_DATA_VALUE/80)

/****************************
* 为实现APP_CURVE外部变量声明
****************************/
extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构

/****************************
* 为实现APP_CURVE内部变量定义
****************************/
static PCURVE_DRAW_FLGS pDrawFlgs = NULL;		//曲线绘制标志集合
static GUIMUTEX mStatusMutex = PTHREAD_MUTEX_INITIALIZER;

/************************
* 为实现APP_CURVE内部函数
************************/
//初始化数据点
static int InitDataDot(DATA_DOT *pDot);
//初始化光标
static int InitCursor(CURSOR *pCursor);
//初始化参考起始点
static int InitReference(REFERENCE *pReference);
//初始化标记线
static int InitMarker(MARKER *pMarker);
//初始化事件标记线
static int InitEventMarker(EVENT_MARKER *pEventMarker);
//根据量程初始化标记
static void InitCurveMaker(int iPos, PDISPLAY_INFO pDisplay);
//初始化坐标系
static int InitCoordinate(COORDINATE *pCoord);
//初始化数据输出对象
static int InitDataOut(DATA_OUT *pOut);
//初始化数据输入
static int InitDataIn(DATA_IN *pDin);
//初始化波形对象
static int InitCurve(PCURVE_INFO *ppCurve);
//画线标志初始化
static int InitDrawFlgs();

// //提取数据的特征值
// static int GetEigenValue(UINT16 *pDest, UINT32 *pDestIndex, UINT32 iDestLen,
// 				  		 UINT16 *pSrc, UINT32 iSrcLen, UINT16 *pSrcLimit);
//根据显示信息，得到曲线的特征值
static int GetPixelValue(PDISPLAY_INFO pDisplay);
//预览曲线和预览区的特征值
static int GetPreviewPixelValue(PDISPLAY_INFO pDisplay);

//根据在原始数据中的索引值得到x坐标
static inline int GetPosX(int iDataIndex, PDISPLAY_INFO pDisplay);
//根据在原始数据中的索引值得到y坐标
static inline int GetPosY(int iDataValue, PDISPLAY_INFO pDisplay);


//获取显示对象参数
static int UpdateDisplayParam(PDISPLAY_INFO pDisplay);
//计算所有需要更新的资源
static int UpdateCurveAll(PDISPLAY_INFO pDisplay);
//清除波形输出信息
static void ClearDout(DATA_OUT *pDout);
//清除曲线信息
static void ClearCurve(PCURVE_INFO pCurve);
//绘制x坐标轴
static int ReCalcCoorX(PDISPLAY_INFO pDisplay);
//绘制y坐标轴
static int ReCalcCoorY(PDISPLAY_INFO pDisplay);
//计算文件名
static int ReCalcFileName(PDISPLAY_INFO pDisplay);
//查找最小的显示长度
static float FindMinDrawLen(PDISPLAY_INFO pDisplay);
//根据选择框获取放大倍数以及放大基准
static int SetScatchSacle(PDISPLAY_INFO pDisplay, PCURVE_INFO pCurve);
//重设标记线颜色
static int ResetMarkerColor(MARKER *pMaker);
static void ResetMarkerVisible(MARKER* pMaker);
//标记线按照增量移动
static int MarkerMoved(MARKER *pMaker, int iOff, PCURVE_INFO pCurve);
//计算自动移动的距离
static int OtdrMeasAutoMove(int direction, MARKER *marker);
//设置显示对象状态
static void SetDisplayStatus(PDISPLAY_INFO pDisplay, DISPLAY_STATUS enStatus);


//曲线控制消息处理
static int HandleLineCtl(PDISPLAY_INFO pDisplay, LINE_CTL *pLineCtl);
//附加值处理
static int HandleAdditionCmd(PDISPLAY_INFO pDisplay, ADD_CTL *pAddCtl);
//光标移到处理
static int HandleCursorMove(PDISPLAY_INFO pDisplay, CURSOR_CTL *pCursorCtl);
//标记线移动处理
static int HandleMakerMove(PDISPLAY_INFO pDisplay, MAKER_CTL *pMakerCtl);
//事件标记线处理
static int HandleEventMarkerMove(PDISPLAY_INFO pDisplay, EVENT_MARKER_CTL* pEventMarkerCtl);
//读取队列消息,做相应处理
static int HandleDisplayCmd(PDISPLAY_INFO pDisplay, QUEUE_MSG *pDrawCmd);


//曲线画图，将所有曲线及预览曲线画出
static int DrawCurve(PDISPLAY_INFO pDisplay);
//绘制光标
static int DrawCursor(PDISPLAY_INFO pDisplay);
//描绘标记点
static int DrawMarker(PDISPLAY_INFO pDisplay);
//描绘事件标记点
static int DrawEventMarker(PDISPLAY_INFO pDisplay);
//画事件点
static int DrawEvent(PDISPLAY_INFO pDisplay, int isDrawBg);
//画参考起始点
static int DrawStartRef(PDISPLAY_INFO pDisplay);
//绘制坐标轴
static int DrawCoor(PDISPLAY_INFO pDisplay);
//绘制光标信息
static int DrawCusorDistance(PDISPLAY_INFO pDisplay, int cursorType);
//画选择框
static int DrawScratchRect(PDISPLAY_INFO pDisplay);
//绘制文件名
static int DrawFilename(PDISPLAY_INFO pDisplay);
//测量时间
static int DrawMeasTime(PDISPLAY_INFO pDisplay);
//绘制预览区波形
static int DrawPreviewCurve(PDISPLAY_INFO pDisplay);
//绘制预览区光标
static int DrawPreviewCursor(PDISPLAY_INFO pDisplay);
//绘制显示对象所有元素
static int DrawCurveAll(PDISPLAY_INFO pDisplay);


void Display_print(PDISPLAY_INFO pDisplay, int line, FILE* out)
{
    if (out == stdout)
        printf(DEBUG_LOG_COLOR_RED);
    fprintf(out, "Line %d->DISPLAY_INFO : (%p) [\n", line, pDisplay);
    fprintf(out, "  xScale = %f\n", pDisplay->xScale);
    fprintf(out, "  yScale = %f\n", pDisplay->yScale);
    fprintf(out, "  xSclBase = %d\n", pDisplay->xSclBase);
    fprintf(out, "  ySclBase = %d\n", pDisplay->ySclBase);
    fprintf(out, "  xShift = %d\n", pDisplay->xShift);
    fprintf(out, "  yShift = %d\n", pDisplay->yShift);
    fprintf(out, "  fDrawLen = %f\n", pDisplay->fDrawLen);
    fprintf(out, "  iDrawHeight = %d\n", pDisplay->iDrawHeight);
    fprintf(out, "  iCurveNum = %d\n", pDisplay->iCurveNum);
    fprintf(out, "  iCurCurve = %d\n", pDisplay->iCurCurve);
    fprintf(out, "  iCurveNum = %d\n]\n", pDisplay->iCurveNum);
    if (out == stdout)
        printf(DEBUG_LOG_COLOR_CLEAR);
}

/************************
* 为实现APP_CURVE内部函数
************************/
//初始化数据点
static int InitDataDot(DATA_DOT *pDot)
{
	pDot->iIndex = 0;
	pDot->iValue = 0;
	pDot->fDist = 0.0f;

	pDot->iPosX = 0;
	pDot->iPosY = 0;

	return 0;
}

//初始化光标
static int InitCursor(CURSOR *pCursor)
{
	pCursor->uiNeedUpdate = 0;

	InitDataDot(&(pCursor->dDot));

	return 0;
}

//初始化参考起始点
static int InitReference(REFERENCE *pReference)
{
	pReference->uiIsVaild = 0;
	pReference->uiNeedUpdate = 0;

	InitDataDot(&(pReference->dDot));

	return 0;
}

//初始化标记线
static int InitMarker(MARKER *pMarker)
{
	pMarker->enPitchOn = EN_PATCH_ALL;
    pMarker->enMeas = EN_MEAS_LOSS;

	pMarker->uiNum = 4;
	pMarker->uiColor[0] = COLOR_BLUE;
	pMarker->uiColor[1] = COLOR_BLUE;
	pMarker->uiColor[2] = COLOR_BLACK;
	pMarker->uiColor[3] = COLOR_BLUE;

    pMarker->uiVisible[0] = 1;
    pMarker->uiVisible[1] = 1;
    pMarker->uiVisible[2] = 1;
    pMarker->uiVisible[3] = 1;

    pMarker->fLoss_AB = 0.0f;
	pMarker->fLoss_aABb = 0.0f;
    pMarker->fAttenAB = 0.0f;
    pMarker->fAtten_aABb = 0.0f;
	pMarker->fReturnLoss = 0.0f;
	pMarker->fReflect_AB = 0.0f;
    pMarker->fTotalReutrnLoss = RETURNLOSS_MAXVALUE;
    
	pMarker->uiPosiUpdate = 0;
	pMarker->uiLossUpdate = 0;

	pMarker->uiaAUpdated = 0;			
	pMarker->uiBbUpdated = 0;			
	
	InitDataDot(&(pMarker->dDot[0]));
	InitDataDot(&(pMarker->dDot[1]));
	InitDataDot(&(pMarker->dDot[2]));
	InitDataDot(&(pMarker->dDot[3]));
	
	return 0;
}

//初始化事件标记线
static int InitEventMarker(EVENT_MARKER *pEventMarker)
{
    pEventMarker->uiNeedUpdate = 0;

	InitDataDot(&(pEventMarker->dDot[0]));
    InitDataDot(&(pEventMarker->dDot[1]));
	
	return 0;
}

//初始化标记线
static int ClearMarker(MARKER *pMarker)
{
	//pMarker->enPitchOn = EN_PATCH_a;

	pMarker->uiNum = 4;
	//pMarker->uiColor[0] = COLOR_BLUE;
	//pMarker->uiColor[1] = COLOR_BLACK;
	//pMarker->uiColor[2] = COLOR_BLACK;
	//pMarker->uiColor[3] = COLOR_BLACK;
	pMarker->fLoss_AB = 0.0f;
	pMarker->fLoss_aABb = 0.0f;
    pMarker->fAtten_aABb = 0.0f;
    pMarker->fAttenAB = 0.0f;
	pMarker->fReturnLoss = 0.0f;
	pMarker->fReflect_AB = 0.0f;
    pMarker->fTotalReutrnLoss = RETURNLOSS_MAXVALUE;
    
	pMarker->uiPosiUpdate = 0;
	pMarker->uiLossUpdate = 0;

	pMarker->uiaAUpdated = 0;			
	pMarker->uiBbUpdated = 0;			

    //maker中的distance保留，威海需求
	pMarker->dDot[0].iPosX = 0;
	pMarker->dDot[0].iPosY = 0;
	pMarker->dDot[0].iValue = 0;

	pMarker->dDot[1].iPosX = 0;
	pMarker->dDot[1].iPosY = 0;
	pMarker->dDot[1].iValue = 0;

	pMarker->dDot[2].iPosX = 0;
	pMarker->dDot[2].iPosY = 0;
	pMarker->dDot[2].iValue = 0;

	pMarker->dDot[3].iPosX = 0;
	pMarker->dDot[3].iPosY = 0;
	pMarker->dDot[3].iValue = 0;
	//InitDataDot(&(pMarker->dDot[0]));
	//InitDataDot(&(pMarker->dDot[1]));
	//InitDataDot(&(pMarker->dDot[2]));
	//InitDataDot(&(pMarker->dDot[3]));
	
	return 0;
}

//根据量程初始化标记线
static void InitCurveMaker(int iPos, PDISPLAY_INFO pDisplay)
{
	MutexLock(&pDisplay->mMutex);

	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return ;
	}

	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];

	//标记线初始化
	int iLen = pCurve->dIn.uiCnt / 8;
	pCurve->mMarker.dDot[0].iIndex = iLen * 2;
	pCurve->mMarker.dDot[1].iIndex = iLen * 0;
	pCurve->mMarker.dDot[2].iIndex = iLen * 4;
	pCurve->mMarker.dDot[3].iIndex = iLen * 5;

	MutexUnlock(&pDisplay->mMutex);
}

//初始化坐标系
static int InitCoordinate(COORDINATE *pCoord)
{
	pCoord->iXNum = DIV_NUM_X;
	pCoord->iYNum = DIV_NUM_Y;
	pCoord->uiNeedUpdate = 0;
	
	pCoord->fDivX = 0.0f;
	pCoord->fDivY = 0.0f;
	pCoord->fMaxX = 0.0f;
	pCoord->fMinX = 0.0f;
	pCoord->fMinY = 0.0f;

	return 0;
}


//初始化数据输出对象
static int InitDataOut(DATA_OUT *pOut)
{
	int iErr = 0;

	if (!iErr) 
	{
		pOut->pValue = (UINT16 *)calloc(MAX_PIXEL, sizeof(UINT16));
		if (NULL == pOut->pValue)
		{
			iErr = -1;
		}
	}
	
	if (!iErr)
	{
		pOut->pIndex = (UINT32 *)calloc(MAX_PIXEL, sizeof(UINT32));
		if (NULL == pOut->pIndex)
		{
			iErr = -2;
		}
	}

	if (!iErr)
	{
		pOut->iValNum = 0;
		pOut->uiDrawCnt = 0;
		pOut->iLeftOffset = 0;
		pOut->iBotOffset = 0;
		pOut->uiColor = 0;

		pOut->fDataPix = 0.0f;
		pOut->fValPix = 0.0f;

		pOut->uiNeedUpdate = 0;
	}

	switch (iErr)
	{
	case -2:
		free(pOut->pValue);
	case -1:
		break;
	default:
		break;
	}

	return iErr;
}

//初始化数据输入
static int InitDataIn(DATA_IN *pDin)
{
	pDin->uiCnt = 0;
	pDin->pData = (UINT16*)calloc(MAX_DATA, sizeof(UINT16));
	if (NULL == pDin->pData)
	{
		return -1;
	}
	
	return 0;
}

//初始化波形对象
static int InitCurve(PCURVE_INFO *ppCurve)
{
	int iErr = 0;
	
	PPREVIEW_TRACE pPreview;
	PCURVE_INFO pCurve;

	DBG_ENTER();
	if (!iErr) 
	{
		pCurve = (PCURVE_INFO)calloc(1, sizeof(CURVE_INFO));
		if (NULL == pCurve)
		{
			iErr = -1;
		}
	}
	
	if (!iErr) 
	{
		//数据输入
		if (InitDataIn(&pCurve->dIn))
		{
			iErr = -2;
		}
	}

	if (!iErr)
	{
		//初始化参考起始点
		InitReference(&(pCurve->rReference));

		//初始化光标
		InitCursor(&(pCurve->cCursor));

		//初始化标记
		InitMarker(&(pCurve->mMarker));

        //初始化事件标记线
        InitEventMarker(&(pCurve->eEventMarker));

		//初始化坐标系
		InitCoordinate(&(pCurve->cCoor));
		
		//初始化输出信息
		if (InitDataOut(&(pCurve->dOut)))
		{
			iErr = -3;
		}
	}

	if (!iErr)
	{
		pCurve->uiIsVaild = 0;
		pCurve->pParam.iFactor = SCALE_FACTOR;
	}

	if (!iErr)
	{
		//初始化波形预览区
		pPreview = (PPREVIEW_TRACE)calloc(1, sizeof(PREVIEW_TRACE));
		if (NULL == pPreview)
		{
			iErr = -4;
		}
		
		pPreview->Rect.lSX = PRE_COOR_SX;
		pPreview->Rect.lSY = PRE_COOR_SY;
		pPreview->Rect.lEX = PRE_COOR_EX;
		pPreview->Rect.lEY = PRE_COOR_EY;
		pPreview->pPixVal = (UINT16 *)calloc(160, sizeof(UINT16));
		pPreview->pPixIndex = (UINT32 *)calloc(160, sizeof(UINT32));

		pCurve->pPreview = pPreview;
		*ppCurve = pCurve;
	}

	//错误处理
	switch (iErr)
	{
	case -4:
	case -3:
		free(pCurve->dIn.pData);
		pCurve->dIn.pData = NULL;
	case -2:
		free(pCurve);
		pCurve = NULL;
	case -1:
		break;

	default:
		break;
	}
	
	CURVE_DBG_EXIT(iErr);
	return iErr;
}


//画线标志初始化
static int InitDrawFlgs()
{
	//分配资源
	pDrawFlgs = (PCURVE_DRAW_FLGS)malloc(sizeof(CURVE_DRAW_FLGS));
	if (NULL == pDrawFlgs)
	{
		return -1;
	}

	//初始化资源
	InitMutex(&pDrawFlgs->mMutex, NULL);
	pDrawFlgs->uiBackground = 1;
	pDrawFlgs->uiCurve = 1;
	pDrawFlgs->uiCursor = 1;
	pDrawFlgs->uiCursorDist = 1;
	pDrawFlgs->uiMarker = 1;
    pDrawFlgs->uiEventMarker = 1;
	pDrawFlgs->uiEvent = 1;
	pDrawFlgs->uiReference = 1;
	pDrawFlgs->uiCoord = 1;
	pDrawFlgs->uiPreview = 0;
	pDrawFlgs->uiFileName = 1;
	pDrawFlgs->uiIsPass = 1;
	pDrawFlgs->uiScratch = 0;
	pDrawFlgs->uiMagnifier = 0;

	return 0;
}

//清除波形输出信息
static void ClearDout(DATA_OUT *pDout)
{
	pDout->fDataPix = 0.0f;
	pDout->fValPix = 0.0f;
	pDout->iBotOffset = 0;
	pDout->iLeftOffset = 0;
	pDout->iValNum = 0;
	pDout->uiNeedUpdate = 0;
	pDout->uiDrawCnt = 0;
	memset(pDout->pIndex, 0, MAX_PIXEL * sizeof(UINT32));
	memset(pDout->pValue, 0, MAX_PIXEL * sizeof(UINT16));
}

static void CopyDout(DATA_OUT *pDest, DATA_OUT *pSrc)
{
	pDest->fDataPix = pSrc->fDataPix;
	pDest->fValPix = pSrc->fValPix;
	pDest->iBotOffset = pSrc->iBotOffset;
	pDest->iLeftOffset = pSrc->iLeftOffset;
	pDest->iValNum = pSrc->iValNum;
	pDest->uiNeedUpdate = pSrc->uiNeedUpdate;
	pDest->uiDrawCnt = pSrc->uiDrawCnt;
	memcpy(pDest->pIndex, pSrc->pIndex, MAX_PIXEL * sizeof(UINT32));
	memcpy(pDest->pValue, pSrc->pValue, MAX_PIXEL * sizeof(UINT16));
}

//清除曲线信息
static void ClearCurve(PCURVE_INFO pCurve)
{	
	//预览区
	PPREVIEW_TRACE pPreview = pCurve->pPreview;

	//文件名
	memset(pCurve->strFName, 0x0, F_NAME_MAX);
	//原始数据
	memset(pCurve->dIn.pData, 0, MAX_DATA * sizeof(UINT16));
	
	//距离
	pCurve->pParam.fDistance = 0.0f;

	//输出信息
	ClearDout(&pCurve->dOut);
	
	//事件
	pCurve->Events.iEventsNumber = 0;
	pCurve->Events.iEventsStart = 0;

	//将所有辅助信息设为初始值
	//参考起始点
	InitReference(&pCurve->rReference);
	//标记线
	ClearMarker(&pCurve->mMarker);
	//光标
	InitCursor(&pCurve->cCursor);
    //事件标记线
    InitEventMarker(&pCurve->eEventMarker);
	//坐标系
	InitCoordinate(&pCurve->cCoor);
	
	//曲线有效标志
	pCurve->uiIsVaild = 0;
	pCurve->uiAnalysised = 0;

	//预览区
	memset(pPreview->pPixVal, 0, 160 * sizeof(UINT16));
	memset(pPreview->pPixIndex, 0, 160 * sizeof(UINT32));
	pPreview->uiNeedUpdate = 0;
	pPreview->iPixValNum = 0;
}


/*
说明:
	提取数据的特征值
参数:
	1.UINT16 *pDest:		存储提取完成的值
	2.UINT32 *pDestIndex:	存储特征值在源数据中的位置
	3.UINT32 iDestLen:		需要提取特征值个数
	4.UINT16 *pSrc:			源数据
	5.UINT32 iSrcLen:		源数据要提取的个数
	6.UINT16 *pSrcLimit:	源数据缓冲区的末尾指针
返回:
	返回有效特征值数量
备注:
*/
int GetEigenValue(UINT16 *pDest, UINT32 *pDestIndex, UINT32 iDestLen,
				  		 UINT16 *pSrc, UINT32 iSrcLen, UINT16 *pSrcLimit)
{
	int iCnt, iDataNum;
	UINT16 *pData;

    CURVE_DBG_ENTER();
	
    for (iCnt = 0; iCnt < iDestLen; ++iCnt)
    {
        //计算目的单个数据内包含的源数据个数
        iDataNum = ((iCnt+1) * iSrcLen) / iDestLen - (iCnt*iSrcLen) / iDestLen;
		
      	//计算源数据的索引
        pDestIndex[iCnt] = (iCnt * iSrcLen) / iDestLen;
		
		pData = pSrc + pDestIndex[iCnt];
		if ((pData + iDataNum) >= pSrcLimit)
		{
			break;
		}

		//计算每个数据的特征值
        if (iDataNum > 1)
        {
            //根据平均值、最大值、最小值判断特征值
            UINT16 uiMean = GetMeanUint16(pData, iDataNum);
            UINT16 uiMax = GetMaxUint16(pData, iDataNum);
            UINT16 uiMin = GetMinUint16(pData, iDataNum);
            if ((uiMean > 8000) && 
				(((uiMax-uiMean) - (uiMean-uiMin)) > 3000))
                pDest[iCnt] = uiMax;
            else
                // pDest[iCnt] = uiMean;
                pDest[iCnt] = pData[iDataNum / 2];
        }
        else
        {
            pDest[iCnt] = pSrc[pDestIndex[iCnt]];
        }
	}
	
	CURVE_DBG_EXIT(iCnt);
	return iCnt;
}

//根据显示信息，得到曲线的特征值
static int GetPixelValue(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;

	int iPos, iDrawLen, iEignNum, iDataCnt;

    UINT16 *pDrawData;
    UINT16 *pPixVal;
    UINT32 *pPixIndex;
	PPREVIEW_TRACE pPreview;
	PCURVE_INFO pCurve;

    //计算每一组曲线的输出值
    for (iPos = 0; iPos < pDisplay->iCurveNum; ++iPos)
    {
    	pCurve = pDisplay->pCurve[iPos];
		pPreview = pCurve->pPreview;
		iDataCnt = pCurve->dIn.uiCnt;
	
        pDrawData = pCurve->dIn.pData + pCurve->dOut.iLeftOffset;
        iDrawLen = pCurve->dOut.uiDrawCnt;
		iEignNum = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
        pPixVal = pCurve->dOut.pValue;
        pPixIndex = pCurve->dOut.pIndex;

	#if 0
		iEignNum = GetEigenValue(pPixVal, pPixIndex, iEignNum, pDrawData,
								 iDrawLen, pCurve->dIn.pData + iDataCnt);
	#else
		if (iEignNum >= iDrawLen)
		{
			iEignNum = GetEigenValue(pPixVal, pPixIndex, iEignNum, pDrawData,
									 iDrawLen, pCurve->dIn.pData + iDataCnt);
            //解决曲线放大到一定程度画线不连续问题
			if(pCurve->dOut.iLeftOffset + iEignNum < iDataCnt)
			{
                double position1 = (pCurve->dOut.iLeftOffset + pPixIndex[iEignNum - 1])*pCurve->pParam.fSmpIntval;
                double value1 = pDrawData[pPixIndex[iEignNum - 1]];
                double position2 = (pCurve->dOut.iLeftOffset + pPixIndex[iEignNum - 1] + 1)*pCurve->pParam.fSmpIntval;
                double value2 = pDrawData[pPixIndex[iEignNum - 1] + 1];

                double k = (value1 - value2)/(position1 - position2);
                double b = value1 - k * position1;

                double positionXend = (double)pCurve->dOut.iLeftOffset * pCurve->pParam.fSmpIntval + pDisplay->fDrawLen;
                pPixVal[iEignNum - 2] = k * positionXend + b;
                pPixIndex[iEignNum - 2] += 1;
                pPixVal[iEignNum - 1] = pPixVal[iEignNum - 2];
                pPixIndex[iEignNum - 1] = pPixIndex[iEignNum - 2];
			}
		}
		else
		{
			iEignNum = GetEigenValue(pPixVal + 2, pPixIndex + 2, iEignNum, pDrawData,
									 iDrawLen, pCurve->dIn.pData + iDataCnt);
			pPixVal[0] = pPixVal[2];
			pPixVal[1] = pPixVal[2];
			pPixIndex[0] = pPixIndex[2];
			pPixIndex[1] = pPixIndex[2];
		}
	#endif
		pCurve->dOut.iValNum = iEignNum;
    }
	
    return iErr;
}

//预览曲线和预览区的特征值
static int GetPreviewPixelValue(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;

	int iPos, iDrawLen, iEignNum, iDataCnt;

    UINT16 *pDrawData;
    UINT16 *pPixVal;
    UINT32 *pPixIndex;
	PPREVIEW_TRACE pPreview;
	PCURVE_INFO pCurve;

	UINT16 *pFirstVal = (UINT16 *)malloc(sizeof(UINT16) * 600);
	UINT32 *pFirstIdx = (UINT32 *)malloc(sizeof(UINT32) * 600);

    //计算每一组曲线的输出值
    for (iPos = 0; iPos < pDisplay->iCurveNum; ++iPos)
    {
	  	pCurve = pDisplay->pCurve[iPos];
		pPreview = pCurve->pPreview;	
		iDataCnt = pCurve->dIn.uiCnt;

        pDrawData = pCurve->dIn.pData;
        iDrawLen = iDataCnt;
        iEignNum = pPreview->Rect.lEX - pPreview->Rect.lSX + 1;
        pPixVal = pPreview->pPixVal;
        pPixIndex = pPreview->pPixIndex;

		GetEigenValue(pFirstVal, pFirstIdx, 600, pDrawData, iDrawLen,
					  pCurve->dIn.pData + iDataCnt);	

		iEignNum = GetEigenValue(pPixVal, pPixIndex, iEignNum, pFirstVal, 
								 600, pFirstVal + 600);
		pPreview->iPixValNum = iEignNum;
    }

	free(pFirstVal);
	free(pFirstIdx);
	
    return iErr;
}


//根据在原始数据中的索引值得到x坐标
static inline int GetPosX(int iDataIndex, PDISPLAY_INFO pDisplay)
{
	int left, x; 

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];

	left = iDataIndex - pCurve->dOut.iLeftOffset;
	x = (int)(pCurve->dOut.fDataPix * (float)left + 0.5f);
	x += pDisplay->Rect.lSX;

	return x;
}


//根据在原始数据中的索引值得到y坐标
static inline int GetPosY(int iDataValue, PDISPLAY_INFO pDisplay)
{
	int bot, y; 
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];

	bot = iDataValue - pCurve->dOut.iBotOffset;
	y = (int)(pCurve->dOut.fValPix * (float)bot + 0.5f);
	y = pDisplay->Rect.lEY - y;

	return y;
}


//绘制x坐标轴
static int ReCalcCoorX(PDISPLAY_INFO pDisplay)
{
    //临时变量定义
    int i;
	double fValX, fStep, fEnd;
    int iUnitConverterFlag;
	char strTmp[16] = {0};
	DBG_INIT_TIME();	

	//坐标轴控件定义
	GUICHAR *pCoordStr;

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
    iUnitConverterFlag = GetCurrSystemUnit(MODULE_UNIT_OTDR);
	//创建坐标轴控件和标签
    CURVE_DBG_ENTER();
	fValX = pCurve->cCoor.fMinX - pCurve->rReference.dDot.fDist;
    fStep = pCurve->cCoor.fDivX;
	fEnd = pCurve->cCoor.fMaxX;
	for (i = 0; i < DIV_NUM_X; ++i)
	{
        //判断距离是否有效
		if (FLOATEQZERO(fEnd))
        {
            strTmp[0] = '\0';
        }      	
        else 
        {
            char *buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, fValX, 0);
            strcpy(strTmp, buff);
            free(buff);
            buff = NULL;
        }

		pCoordStr = TransString(strTmp);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->ppCoorX[i]);
        SetLabelText(pCoordStr, pDisplay->ppCoorX[i]);
        free(pCoordStr);

		fValX += fStep;
	}
    //char cBuf[10] = {};
    //转换得到单位名称
    //UnitConverterName(iUnitConverterFlag, fEnd, cBuf);
    pCoordStr = GetCurrSystemUnitGuiString(MODULE_UNIT_OTDR);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->ppCoorX[i]);
    SetLabelText(pCoordStr, pDisplay->ppCoorX[i]);
	free(pCoordStr);
 	DBG_PRINT_TIME();
    return 0;
}

//绘制y坐标轴
static int ReCalcCoorY(PDISPLAY_INFO pDisplay)
{
	//临时变量定义
	int i;
	float fValY, fStep;
	char strTmp[16];
	int maxNum;
	int size;
	//坐标轴控件定义
	GUICHAR *pCoordStr;
	DBG_INIT_TIME();

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];

	DBG_ENTER();
	size = pDisplay->CtlPara.enSize;
	maxNum = (size == EN_SIZE_LARGE) ? (DIV_NUM_Y) : (DIV_NUM_Y - 2);
	fStep = pCurve->cCoor.fDivY;
	fValY = pCurve->cCoor.fMaxY - fStep;

	for (i = 0; i < maxNum; ++i)
	{
		//转换坐标值
		sprintf(strTmp, "%.1f", fValY);
		pCoordStr = TransString(strTmp);
		//绘制坐标值
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->ppCoorY[i]);
		SetLabelText(pCoordStr, pDisplay->ppCoorY[i]);
		free(pCoordStr);

		fValY -= fStep;
	}

	//绘制坐标轴单位(y 轴 dB)
	pCoordStr = TransString("dB");
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->ppCoorY[i]);
	SetLabelText(pCoordStr, pDisplay->ppCoorY[i]);
	free(pCoordStr);
	DBG_PRINT_TIME();

	CURVE_DBG_EXIT(0);
	return 0;
}


//计算文件名
static int ReCalcFileName(PDISPLAY_INFO pDisplay)
{

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
    //去除后缀名
    char fileName[F_NAME_MAX] = {};
    memset(fileName, 0, strlen(fileName));
    char* buf = strrchr(pCurve->strFName, '.');
    if(buf)
    {
        memcpy(fileName, pCurve->strFName, strlen(pCurve->strFName) - strlen(buf));
        fileName[strlen(fileName)] = '\0';
    }
    
	GUICHAR *pStr = TransString(fileName);

	GUICHAR *pDot = NULL;
	//int fileNamePositionY = pDisplay->pLblFile->Visible.Area.Start.y;

	SetLabelText(pStr, pDisplay->pLblFile);
    #if 0
	if (GetLabelRealWidth(pDisplay->pLblFile) > 276)
	{
		SetLabelArea(405, fileNamePositionY, 405 + 276, fileNamePositionY + 24, pDisplay->pLblFile);
		pDot = TransString("...");
	}
	else
	{
		SetLabelArea(405, fileNamePositionY, 405 + 276, fileNamePositionY + 24, pDisplay->pLblFile);
		pDot = TransString("");
	}
    #endif
	SetLabelText(pDot, pDisplay->pLblDot);
	free(pStr);

	return 0;
}

//查找最小的显示长度
static float FindMinDrawLen(PDISPLAY_INFO pDisplay)
{
/*
	int i, iWidth;
	PCURVE_INFO pCurve;

	iWidth = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
	pCurve = pDisplay->pCurve[0];

	for (i = 1; i < pDisplay->iCurveNum; ++i)
	{
		if (pCurve->pParam.fDistance < pDisplay->pCurve[i]->pParam.fDistance)
		{
			pCurve = pDisplay->pCurve[i];
		}
	}

	return pCurve->pParam.fSmpIntval * iWidth / 6;
	*/
	switch(GetCurrSystemUnit(MODULE_UNIT_OTDR))
	{
        case UNIT_M:
        case UNIT_KM:
        return 0.6;
        case UNIT_FT:
        case UNIT_KFT:
        return UnitConverter_Dist_Float2Float(UNIT_FT, UNIT_M, 0.6);
        case UNIT_YD:
        case UNIT_MI:
        return UnitConverter_Dist_Float2Float(UNIT_MI, UNIT_M, 0.0006);
        default:
        break;
	}
	return 0.6;
}

//根据选择框获取放大倍数以及放大基准
static int SetScatchSacle(PDISPLAY_INFO pDisplay, PCURVE_INFO pCurve)
{
	int iPixNum, iHeight;
	int iSWidth, iSHeight;
	float fTmp;

	//计算显示对象高宽
	iPixNum = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
	iHeight = pDisplay->Rect.lEY - pDisplay->Rect.lSY + 1;
	iSWidth = abs(pDisplay->Scratch.lEX - pDisplay->Scratch.lSX + 1);
	iSHeight = abs(pDisplay->Scratch.lEY - pDisplay->Scratch.lSY + 1);

	//判断是否有效
	if (iSWidth < 10 || iSHeight < 10)
	{
		return -1;
	}
	
    //计算缩放比例
    pDisplay->xScale *= (float)iPixNum / (float)iSWidth;
    pDisplay->yScale *= (float)iHeight / (float)iSHeight;

	//计算x轴的放大基准线
    fTmp = (float)((pDisplay->Scratch.lSX + pDisplay->Scratch.lEX) / 2 - pDisplay->Rect.lSX);
	fTmp /= pCurve->dOut.fDataPix;
    pDisplay->xSclBase = pCurve->dOut.iLeftOffset + (int)fTmp;

	//计算y轴的放大基准线
    fTmp = (float)(pDisplay->Rect.lEY - (pDisplay->Scratch.lSY + pDisplay->Scratch.lEY) / 2);
	fTmp /= pCurve->dOut.fValPix;
    pDisplay->ySclBase = pCurve->dOut.iBotOffset + (int)fTmp;

	return 0;
}

// 重设标记线可见性
static void ResetMarkerVisible(MARKER* pMaker)
{
    switch (pMaker->enMeas)
    {
    // aABb 均可见
    case EN_MEAS_LOSS:
        if(pUser_Settings->sCommonSetting.iLossMethodMark)
        {
            //两点法
            pMaker->uiVisible[0] = 0;
            pMaker->uiVisible[3] = 0;
        }
        else
        {
            pMaker->uiVisible[0] = 1;
            pMaker->uiVisible[3] = 1;
        }
        pMaker->uiVisible[1] = 1;
        pMaker->uiVisible[2] = 1;
        break;
    // 仅aAB 可见
    case EN_MEAS_REFC:
        pMaker->uiVisible[0] = 1;
        pMaker->uiVisible[1] = 1;
        pMaker->uiVisible[2] = 1;
        pMaker->uiVisible[3] = 0;
        break;
    // 仅AB 可见
    case EN_MEAS_ATTE:
    case EN_MEAS_ORL:
        pMaker->uiVisible[0] = 0;
        pMaker->uiVisible[1] = 1;
        pMaker->uiVisible[2] = 1;
        pMaker->uiVisible[3] = 0;
        break;
    default: 
        pMaker->uiVisible[0] = 1;
        pMaker->uiVisible[1] = 1;
        pMaker->uiVisible[2] = 1;
        pMaker->uiVisible[3] = 1;
        break;
    }
}

//标记操作函数
//重设标记线颜色
static int ResetMarkerColor(MARKER *pMaker)
{
	//清除所有颜色
	pMaker->uiColor[0] = COLOR_BLACK;
	pMaker->uiColor[1] = COLOR_BLACK;
	pMaker->uiColor[2] = COLOR_BLACK;
	pMaker->uiColor[3] = COLOR_BLACK;

	//根据选择修改颜色
	switch(pMaker->enPitchOn)
	{
	case EN_PATCH_a:
		pMaker->uiColor[0] = COLOR_BLUE;
		break;
	case EN_PATCH_Aa:
		pMaker->uiColor[0] = COLOR_BLUE;
		pMaker->uiColor[1] = COLOR_BLUE;
		break;
	case EN_PATCH_Bb:
		pMaker->uiColor[2] = COLOR_BLUE;
		pMaker->uiColor[3] = COLOR_BLUE;
		break;
	case EN_PATCH_b:
		pMaker->uiColor[3] = COLOR_BLUE;
		break;
	case EN_PATCH_ab:
		pMaker->uiColor[0] = COLOR_BLUE;
		pMaker->uiColor[3] = COLOR_BLUE;
		break;
	case EN_PATCH_ALL:
		pMaker->uiColor[0] = COLOR_BLUE;
		pMaker->uiColor[1] = COLOR_BLUE;
		pMaker->uiColor[2] = COLOR_BLUE;
		pMaker->uiColor[3] = COLOR_BLUE;
		break;
	default:
		break;
	}

	return 0;
}

static int MarkerOnTouch(MARKER *pMaker, int iTouchX, PDISPLAY_INFO pDisplay)
{
    PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
    float fOffset = (float)(iTouchX - pDisplay->Rect.lSX);
    int iTargetIndex = pCurve->dOut.iLeftOffset + (int)(fOffset / pCurve->dOut.fDataPix);

    int iCurrIndex = pMaker->dDot[1].iIndex;
    
    //根据链接方式移动标记线
	switch (pMaker->enPitchOn)
	{
	case EN_PATCH_a:
        iCurrIndex = pMaker->dDot[0].iIndex;
		break;

    case EN_PATCH_Aa:
        iCurrIndex = pMaker->dDot[1].iIndex;
 	   break;
    
	case EN_PATCH_Bb:
        iCurrIndex = pMaker->dDot[2].iIndex;
        break;
        
    case EN_PATCH_b:
        iCurrIndex = pMaker->dDot[3].iIndex;
		break;
		
    case EN_PATCH_ab:   //改模式不能点击触摸屏移动
        iCurrIndex = iTargetIndex;
		break;
    
	case EN_PATCH_ALL: {
        iCurrIndex = pMaker->dDot[1].iIndex;
		break;
    }
	default:
		break;
	}

    return MarkerMoved(pMaker, iTargetIndex - iCurrIndex, pCurve);
}


//标记线按照增量移动
static int MarkerMoved(MARKER *pMaker, int iOff, PCURVE_INFO pCurve)
{   
    int iReturn = -1;

    // 移动间距为0，什么都不做，直接返回
    if (iOff == 0)
        return iReturn;
    
	//计算标记线之间的最小距离(0像素)
	int iMin = 0;
    
	//数据长度
	unsigned uiDataCnt = pCurve->dIn.uiCnt;
	DBG_INIT_TIME();	
	
	//根据链接方式移动标记线
	switch (pMaker->enPitchOn)
	{
	case EN_PATCH_a: {
		//左边界: 0
		if ((pMaker->dDot[0].iIndex+iOff) < 0)
			iOff = 0 - pMaker->dDot[0].iIndex;
		//右边界: 标记线 A
		if ((pMaker->dDot[0].iIndex+iOff) >= (pMaker->dDot[1].iIndex-iMin))
			iOff = pMaker->dDot[1].iIndex - iMin - pMaker->dDot[0].iIndex;

        if (iOff != 0)
        {
		    pMaker->dDot[0].iIndex += iOff;
	        pCurve->cCursor.dDot.iIndex = pMaker->dDot[0].iIndex;
		    pMaker->uiaAUpdated = 1;
            iReturn = 0;
        }      

		break;
    }

    case EN_PATCH_Aa: {
		//左边界: 0
		if ((pMaker->dDot[0].iIndex+iOff) < 0) {
            pMaker->dDot[0].iIndex = 0;
            if ((pMaker->dDot[1].iIndex+iOff) < (pMaker->dDot[0].iIndex+iMin)) 
                iOff =(pMaker->dDot[0].iIndex+iMin) - pMaker->dDot[1].iIndex;
            
            if (iOff > 0){
                pMaker->dDot[1].iIndex += iOff;// = pMaker->dDot[0].iIndex+iMin;
                pCurve->cCursor.dDot.iIndex = pMaker->dDot[1].iIndex;
	            pMaker->uiaAUpdated = 1;
                iReturn = 0;
            }
        }
		//右边界: 标记线 B
		else {
            if ((pMaker->dDot[1].iIndex+iOff) >= (pMaker->dDot[2].iIndex-iMin))
			    iOff = pMaker->dDot[2].iIndex - iMin - pMaker->dDot[1].iIndex;
            if (iOff > 0) {
     		    pMaker->dDot[0].iIndex += iOff;
		        pMaker->dDot[1].iIndex += iOff;
		        pCurve->cCursor.dDot.iIndex = pMaker->dDot[1].iIndex;
		        pMaker->uiaAUpdated = 1;
                iReturn = 0;
           }
       }
 	   break;
    }
    
	case EN_PATCH_Bb: {
		//右边界: 最大点数
		if ((pMaker->dDot[3].iIndex+iOff) >= (signed int)(uiDataCnt)) {
            pMaker->dDot[3].iIndex = uiDataCnt;
            
            if ((pMaker->dDot[2].iIndex+iOff) >= (pMaker->dDot[3].iIndex-iMin))
			    iOff = pMaker->dDot[3].iIndex - iMin - pMaker->dDot[2].iIndex;
    		
    		pMaker->dDot[2].iIndex = pMaker->dDot[3].iIndex;
    		pCurve->cCursor.dDot.iIndex  = pMaker->dDot[2].iIndex;
    		pMaker->uiBbUpdated = 1;
            iReturn = 0;
        }
        else {
            //左边界: 标记线 A
    		if ((pMaker->dDot[2].iIndex+iOff) < (pMaker->dDot[1].iIndex+iMin))
    			iOff = pMaker->dDot[1].iIndex + iMin - pMaker->dDot[2].iIndex;
    		pMaker->dDot[2].iIndex += iOff;
    		pMaker->dDot[3].iIndex += iOff;
    		pCurve->cCursor.dDot.iIndex  = pMaker->dDot[2].iIndex;
    		pMaker->uiBbUpdated = 1;
            iReturn = 0;
        }
		break;
    }

    case EN_PATCH_b: {
		//左边界: 标记线 B
		if ((pMaker->dDot[3].iIndex+iOff) < (pMaker->dDot[2].iIndex+iMin))
			iOff = pMaker->dDot[2].iIndex + iMin - pMaker->dDot[3].iIndex;
		//右边界: 最大点数
		if ((pMaker->dDot[3].iIndex+iOff) >= (signed int)(uiDataCnt))
			iOff = uiDataCnt - pMaker->dDot[3].iIndex - 1;

        if (iOff != 0)
        {
		    pMaker->dDot[3].iIndex += iOff;
	        pCurve->cCursor.dDot.iIndex = pMaker->dDot[0].iIndex;
		    pMaker->uiBbUpdated = 1;
            iReturn = 0;
        } 		
		break;
    }

    case EN_PATCH_ab: {
        int iOff_a = iOff;
        int iOff_b = 0 - iOff;
        int iDrawLen = pCurve->dOut.uiDrawCnt;
        int iLeftIndex = pCurve->dOut.iLeftOffset;
        int iRightIndex = iLeftIndex + iDrawLen;

		//左边界: 0
		if ((pMaker->dDot[0].iIndex+iOff_a) < iLeftIndex)
			iOff_a = iLeftIndex - pMaker->dDot[0].iIndex;
		//右边界: 标记线 A
		if ((pMaker->dDot[0].iIndex+iOff_a) >= (pMaker->dDot[1].iIndex-iMin))
			iOff_a = pMaker->dDot[1].iIndex - iMin - pMaker->dDot[0].iIndex;

		//左边界: 标记线 B
		if ((pMaker->dDot[3].iIndex+iOff_b) < (pMaker->dDot[2].iIndex+iMin))
			iOff_b = pMaker->dDot[2].iIndex + iMin - pMaker->dDot[3].iIndex;
		//右边界: 最大点数
		if ((pMaker->dDot[3].iIndex+iOff_b) >= (signed int)(iRightIndex))
			iOff_b = iRightIndex - pMaker->dDot[3].iIndex - 1;


        if(iOff_a == 0 || iOff_b == 0)
        {
            if(((pMaker->dDot[0].iIndex <= 0 && pMaker->dDot[3].iIndex >= (signed int)(iRightIndex-1))
                || (pMaker->dDot[0].iIndex <= 0 && (pMaker->dDot[3].iIndex - pMaker->dDot[2].iIndex >= (signed int)iDrawLen / 2))
                || (pMaker->dDot[3].iIndex >= (signed int)(iRightIndex-1) && (pMaker->dDot[1].iIndex - pMaker->dDot[0].iIndex >= (signed int)iDrawLen / 2)))
                && (iOff_a<=0 && iOff_b >=0)
               )
            {
    			iOff_a = pMaker->dDot[1].iIndex - iMin - pMaker->dDot[0].iIndex;
			    iOff_b = pMaker->dDot[2].iIndex + iMin - pMaker->dDot[3].iIndex;
            }
            
            if(pMaker->dDot[0].iIndex >= (pMaker->dDot[1].iIndex-iMin) 
                && pMaker->dDot[3].iIndex <= (pMaker->dDot[2].iIndex+iMin)
                && (iOff_a>=0 && iOff_b <=0))
            {
    			iOff_a = 0 - pMaker->dDot[0].iIndex;
			    iOff_b = iRightIndex - pMaker->dDot[3].iIndex - 1;
			    if(abs(iOff_a) > iDrawLen / 2)
			    {
                    iOff_a = 0 - iDrawLen / 2;
			    }
			    
			    if(abs(iOff_b) > iDrawLen / 2)
			    {
                    iOff_b = iDrawLen / 2;
			    }
            }
        }
        
        if (iOff_a != 0)
        {
		    pMaker->dDot[0].iIndex += iOff_a;
	        pCurve->cCursor.dDot.iIndex = pMaker->dDot[0].iIndex;
		    pMaker->uiaAUpdated = 1;
            iReturn = 0;
        }      

        if (iOff_b != 0)
        {
		    pMaker->dDot[3].iIndex += iOff_b;
	        pCurve->cCursor.dDot.iIndex = pMaker->dDot[0].iIndex;
		    pMaker->uiBbUpdated = 1;
            iReturn = 0;
        } 		
		break;
    }    
    
	case EN_PATCH_ALL: {
        //左边界: 0
		if ((pMaker->dDot[0].iIndex+iOff) < 0) {
            pMaker->dDot[0].iIndex = 0;
            if ((pMaker->dDot[1].iIndex+iOff) < (pMaker->dDot[0].iIndex+iMin)) 
                iOff =(pMaker->dDot[0].iIndex+iMin) - pMaker->dDot[1].iIndex;
            
            if (iOff != 0){
                pMaker->dDot[1].iIndex += iOff;
                pMaker->dDot[2].iIndex += iOff;
                pMaker->dDot[3].iIndex += iOff;
                pCurve->cCursor.dDot.iIndex = pMaker->dDot[0].iIndex;
	            pMaker->uiaAUpdated = 1;
                iReturn = 0;
            }
        }
        //右边界: 最大点数
        else if ((pMaker->dDot[3].iIndex+iOff) >= (signed int)(uiDataCnt)) {
            pMaker->dDot[3].iIndex = uiDataCnt;
            
            if ((pMaker->dDot[2].iIndex+iOff) >= (pMaker->dDot[3].iIndex-iMin))
			    iOff = pMaker->dDot[3].iIndex - iMin - pMaker->dDot[2].iIndex;
    		
            pMaker->dDot[0].iIndex += iOff;
            pMaker->dDot[1].iIndex += iOff;
    		pMaker->dDot[2].iIndex += iOff;
    		pCurve->cCursor.dDot.iIndex  = pMaker->dDot[0].iIndex;
    		pMaker->uiBbUpdated = 1;
            iReturn = 0;
        }
        else
        {
    		pMaker->dDot[0].iIndex += iOff;
    		pMaker->dDot[1].iIndex += iOff;
    		pMaker->dDot[2].iIndex += iOff;
    		pMaker->dDot[3].iIndex += iOff;
    		pCurve->cCursor.dDot.iIndex  = pMaker->dDot[0].iIndex;
    		pMaker->uiaAUpdated = 1;
    		pMaker->uiBbUpdated = 1;
            iReturn = 0;
        }
		break;
    }
	default:
		break;
	}

 	DBG_PRINT_TIME();  
	
	return iReturn;
}

//计算自动移动的距离
static int OtdrMeasAutoMove(int direction, MARKER *marker)
{
    float ratio[7] = {2.15, 2.73, 4.1, 6.17, 9.26, 13.89, 20.8};    //参考横河
    int currDistanceAa = 0;
    int currDistanceBa = 0;
    int nextDistanceAa = 0;
    int nextDistanceBa = 0;
    PDISPLAY_INFO displayInfo = pOtdrTopSettings->pDisplayInfo;
    CURVE_PRARM *currCurveParam = &displayInfo->pCurve[displayInfo->iCurCurve]->pParam;
    EVENTS_TABLE *currCurveEvents = &displayInfo->pCurve[displayInfo->iCurCurve]->Events;
    int iDrawLen = displayInfo->pCurve[displayInfo->iCurCurve]->dOut.uiDrawCnt;
    int iLeftIndex = displayInfo->pCurve[displayInfo->iCurCurve]->dOut.iLeftOffset;
    int nextIndex = 0;
    int i=0;
    int iMin, iMinAutoMark;
    
    if(!marker)
    {
        return -1;
    }
    
    //计算标记线之间的最小距离(2像素)
    iMinAutoMark = iDrawLen / ratio[6];
	iMin = 2 / displayInfo->pCurve[displayInfo->iCurCurve]->dOut.fDataPix;
	if (iMin < 1)
	{
		iMin = 1;
	}

	//计算B的位置 
	//若A在事件内部(事件的begin和end范围内)或在事件(begin点)前一个盲区内，若
	//事件为熔接事件，将B放在事件(end点)后一个盲区位置，若事件为反射事件，将B放在事件(end点)后两个盲区位置
	//若A后面一个盲区内没有事件，则将B放在A后一个盲区的位置
	int blind = GetBlindLength(currCurveParam->enPluse, currCurveParam->fSmpRatio);
	int minBlind = 10/2.5*currCurveParam->fSmpRatio;
	//int blind = pluse / 25 * fsmp;
	//dis =max(10/2.5*fsmp, blind)
	blind = blind >= minBlind ? blind : minBlind;
	LOG(LOG_INFO, "blind %d\n", blind);
	LOG(LOG_INFO, "currCurveParam->enPluse %d\n", currCurveParam->enPluse);

	for(i = 0; i < currCurveEvents->iEventsNumber; i++)
	{
        EVENTS_INFO *event = &currCurveEvents->EventsInfo[i];
        if((marker->dDot[1].iIndex <= event->iBegin && marker->dDot[1].iIndex + blind >= event->iBegin)
            || (marker->dDot[1].iIndex >= event->iBegin && marker->dDot[1].iIndex <= event->iEnd))
        {
            switch(event->iStyle)
            {
                case 0:
                case 1:
                    marker->dDot[2].iIndex = event->iEnd + blind;
                    break;
                case 2:
                case 3:
                case 4:
                    marker->dDot[2].iIndex = event->iEnd + 2 * blind;
                    break;
                case 5:
                    marker->dDot[2].iIndex = event->iEnd + blind;
                    break;
                case 10:
                case 11:
                    marker->dDot[2].iIndex = event->iEnd + blind;
                    break;
                case 12:
                case 13:
                case 14:
                    marker->dDot[2].iIndex = event->iEnd + 2 * blind;
                    break;
                case 15:
                case 20:
                case 21:
                    marker->dDot[2].iIndex = event->iEnd + blind;
                    break;
                default:
                    marker->dDot[2].iIndex = event->iEnd + blind;
                    break;
            }
            break;
        }
	}

	if(i == currCurveEvents->iEventsNumber)
	{
        marker->dDot[2].iIndex = marker->dDot[1].iIndex + blind;
	}

    currDistanceAa = marker->dDot[1].iIndex - marker->dDot[0].iIndex;
    currDistanceBa = marker->dDot[3].iIndex - marker->dDot[2].iIndex;

    //A或B不在显示区域时，不移动
    if((marker->dDot[1].iIndex <= (iLeftIndex+iMin)) 
        || (marker->dDot[2].iIndex >= (iLeftIndex + iDrawLen - iMin))
        || ((marker->dDot[1].iIndex <= (iLeftIndex+iMinAutoMark)) 
            && (marker->dDot[2].iIndex >= (iLeftIndex + iDrawLen - iMinAutoMark))))
    {
        return -2;
    }

    //找到下一个位置对应区间大小的索引
    for(i = 0; i < sizeof(ratio)/sizeof(ratio[0]); i++)
    {
        unsigned int ratioWidth = iDrawLen/ratio[i];
        if(currDistanceAa > ratioWidth || currDistanceBa > ratioWidth)
        {
            //direction>0 区域变大
            nextIndex = direction ? (i-1+7)%7 : i;
        }
        else if(currDistanceAa == ratioWidth || currDistanceBa == ratioWidth)
        {
            nextIndex = direction ? (i-1+7)%7 : (i+1)%7;
        }
        else
        {
            continue;
        }
        break;
    }

    if(i == sizeof(ratio)/sizeof(ratio[0]))
    {
        nextIndex = direction ? 0 : 6;
    }
    
    nextDistanceAa = iDrawLen/ratio[nextIndex];
    nextDistanceBa = nextDistanceAa;

    if(marker->dDot[1].iIndex - nextDistanceAa < iLeftIndex 
        && marker->dDot[2].iIndex + nextDistanceBa > iLeftIndex + iDrawLen)
    {
        if(direction)
        {
            marker->dDot[0].iIndex = marker->dDot[1].iIndex - iMinAutoMark >= iLeftIndex ? 
                                    marker->dDot[1].iIndex - iMinAutoMark : iLeftIndex;
            marker->dDot[3].iIndex = marker->dDot[2].iIndex + iMinAutoMark <= iLeftIndex + iDrawLen ?
                                    marker->dDot[2].iIndex + iMinAutoMark : iLeftIndex + iDrawLen;
        }
        else
        {
            marker->dDot[0].iIndex = iLeftIndex;
            marker->dDot[3].iIndex = iLeftIndex + iDrawLen;
        }
    }
    else if((marker->dDot[1].iIndex - nextDistanceAa) < iLeftIndex)
    {
        marker->dDot[0].iIndex = iLeftIndex;
        marker->dDot[3].iIndex = marker->dDot[2].iIndex + nextDistanceBa;
    }
    else if((marker->dDot[2].iIndex + nextDistanceBa) >= iLeftIndex + iDrawLen)
    {
        marker->dDot[0].iIndex = marker->dDot[1].iIndex - nextDistanceAa;
        marker->dDot[3].iIndex = iLeftIndex + iDrawLen-(1/displayInfo->pCurve[displayInfo->iCurCurve]->dOut.fDataPix);
    }
    else
    {
        marker->dDot[0].iIndex = marker->dDot[1].iIndex - nextDistanceAa;
        marker->dDot[3].iIndex = marker->dDot[2].iIndex + nextDistanceBa;
    }
        
    return 0;
}

static void SetCurvesFullScreen(PDISPLAY_INFO pCurve)
{    
    float fPeak = 0.0f;            //记录最高点对应的值；
    float fYScale = 0.0f;          //Y轴的放大倍数

    int i = 0, j = 0;
    
    // if(pCurve->yScale != 1.0f || pCurve->xScale != 1.0f)
    // {
        // return;
    // }

    for (i = 0; i < pCurve->iCurveNum;  i++)
	{
	    for(j = 0; j < pCurve->pCurve[i]->dIn.uiCnt; j++)
        {
            if(fPeak < pCurve->pCurve[i]->dIn.pData[j])
            {
                fPeak = pCurve->pCurve[i]->dIn.pData[j];
            }
        }   
	}

    fYScale = (MAX_DATA_VALUE) / (fPeak*1.2f);
    pCurve->yScale = fYScale;
	pCurve->ySclBase = 0;
    pCurve->yShift = 0;
	LOG(LOG_INFO, "*fYScale = %f**fPeak = %f****\n", fYScale, fPeak);
}


//曲线控制消息处理
static int HandleLineCtl(PDISPLAY_INFO pDisplay, LINE_CTL *pLineCtl)
{
	//临时变量
	static float fPreDrawlen = 0.0f;
	static int iPreDrawheight = 0;
	PCURVE_INFO pCurve;
	int ixNum, iyNum, i;

	//计算标志
	int iScaled = 0;
	int iShifted = 0;
	int iNewData = 0;
    int iModifyBase = 0;

    CURVE_DBG_ENTER();
	if (pDisplay->iCurveNum <= 0)
	{
		return -1;
	}
	DBG_INIT_TIME();	
	ixNum = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
	iyNum = pDisplay->Rect.lEY - pDisplay->Rect.lSY + 1;
	pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	//命令解析
	switch (pLineCtl->enType)
	{
    case EN_CTL_ZOOMIN://放大
		pDisplay->xScale *= pLineCtl->uValue.sScaleVal.fXScale;
		pDisplay->yScale *= pLineCtl->uValue.sScaleVal.fYScale;
		pDisplay->xSclBase = pCurve->cCursor.dDot.iIndex;
		pDisplay->ySclBase = pCurve->cCursor.dDot.iValue;
		iScaled = 1;
		break;
    case EN_CTL_ZOOMOUT://缩小
		pDisplay->xScale /= pLineCtl->uValue.sScaleVal.fXScale;
		pDisplay->yScale /= pLineCtl->uValue.sScaleVal.fYScale;
		pDisplay->xSclBase = pCurve->cCursor.dDot.iIndex;
		pDisplay->ySclBase = pCurve->cCursor.dDot.iValue;
		iScaled = 1;
		break;
	case EN_CTL_ZOOMVAL:
		pDisplay->xScale = pLineCtl->uValue.sScaleVal.fXScale;
		pDisplay->yScale = pLineCtl->uValue.sScaleVal.fYScale;
		pDisplay->xSclBase = pCurve->cCursor.dDot.iIndex;
		pDisplay->ySclBase = pCurve->cCursor.dDot.iValue;
		iScaled = 1;
		break;
	case EN_CTL_DOWN://拖拽放大起始点
		pDisplay->Scratch.lSX = pLineCtl->uValue.sMoveVal.iPosX;
		pDisplay->Scratch.lSY = pLineCtl->uValue.sMoveVal.iPosY;
		pDisplay->Scratch.lEX = pLineCtl->uValue.sMoveVal.iPosX;
		pDisplay->Scratch.lEY = pLineCtl->uValue.sMoveVal.iPosY;
		SetDrawScratch(1);
		break;
	case EN_CTL_MOVE://拖拽放大移动
		pDisplay->Scratch.lEX = pLineCtl->uValue.sMoveVal.iPosX;
		pDisplay->Scratch.lEY = pLineCtl->uValue.sMoveVal.iPosY;
		break;
    case EN_CTL_SCRATCH://拖拽框选择完成
		if (!SetScatchSacle(pDisplay, pCurve))
		{
		    //解决禅道19237 光标位置
			/*pCurve->cCursor.dDot.iIndex = pDisplay->xSclBase;
            pCurve->eEventMarker.dDot[0].iIndex = pDisplay->xSclBase;
            pCurve->eEventMarker.dDot[1].iIndex = -1;*/
			iScaled = 1;
		}
		SetDrawScratch(0);
		break;
	case EN_CTL_INIT://初始化
    {
		ReCalcFileName(pDisplay);
		fPreDrawlen = 0.0f;
		iPreDrawheight = 0;
        iScaled = 1;
        //由算法提供的经验值
        float fPeak[12] = {25000, 25000, 27500, 27500, 30000, 30000, 32500, 40000, 42500, 42500, 45000, 45000};
        int pulse = pCurve->pParam.enPluse;
        pDisplay->yScale = (MAX_DATA_VALUE) / (fPeak[pulse] * 1.2f);
        pDisplay->ySclBase = 0;
        pDisplay->yShift = 0;
        LOG(LOG_INFO, "reset coor pulse = %d, fPeak = %f, yScale = %f!!!!\n", pulse, fPeak[pulse], pDisplay->yScale);
    }
        break;
	case EN_CTL_RECOVER://恢复
		pDisplay->xScale = 1.0f;
		pDisplay->yScale = 1.0f;
		pDisplay->xSclBase = pCurve->cCursor.dDot.iIndex;
		pDisplay->ySclBase = pCurve->cCursor.dDot.iValue;
		iScaled = 1;
		break;
    case EN_CTL_DRAG://拖拽移动波形
		pDisplay->xShift = pLineCtl->uValue.sMoveVal.iPosX;
		pDisplay->yShift = pLineCtl->uValue.sMoveVal.iPosY;
		iShifted = 1;
		break;
	case EN_CTL_MT://多点放大
		pDisplay->xScale += pLineCtl->uValue.sScaleVal.fXScale;
		pDisplay->yScale += pLineCtl->uValue.sScaleVal.fYScale;
		pDisplay->xSclBase = pCurve->cCursor.dDot.iIndex;
		pDisplay->ySclBase = pCurve->cCursor.dDot.iValue;
		iScaled = 1;
		break;
	case EN_CTL_NEWDATA://刷新波形
		iNewData = 1;
		iScaled = 1;
		if (pDisplay->uiReadSor)
		{
			fPreDrawlen = 0.0f;
			iPreDrawheight = 0;
		    pDisplay->xScale = 1.0f;
		    pDisplay->yScale = 1.0f;
            pDisplay->xSclBase = pCurve->cCursor.dDot.iIndex;
            pDisplay->ySclBase = pCurve->cCursor.dDot.iValue;
        }
		break;
    case EN_CTL_ENDOFDAQ:
        SetCurvesFullScreen(pDisplay);
        iNewData = 1;
		iScaled = 1;
        pDisplay->uiEndOfDaq = 1;
        break;
	case EN_CTL_SWITCH://切换波形
        ReCalcFileName(pDisplay);
		pCurve->cCoor.uiNeedUpdate = 1;
		pCurve->cCursor.uiNeedUpdate = 1;
		break;
	case EN_CTL_MAGNIFIER://显示放大镜图标
		LOG(LOG_INFO, "display scale button!!!!\n");
		break;
	default:
		break;
	}

	//判断是否有缩放操作
	if (iScaled)
	{
		//限制x轴的放大倍数
		if (pDisplay->xScale < 1.0f)
		{
			pDisplay->xScale = 1.0f;
			pDisplay->fDrawLen = pCurve->pParam.fDistance / pDisplay->xScale;
		}
		else
		{
			float fMinDrawLen = FindMinDrawLen(pDisplay);
			pDisplay->fDrawLen = pCurve->pParam.fDistance / pDisplay->xScale;
			if (pDisplay->fDrawLen < fMinDrawLen)
			{
				pDisplay->fDrawLen = fMinDrawLen;
				pDisplay->xScale = pCurve->pParam.fDistance / pDisplay->fDrawLen;
			}
		}
		
		//限制y 的放大倍数
		if (pDisplay->yScale < 1.0f)
		{
			pDisplay->yScale = 1.0f;
			pDisplay->iDrawHeight = MAX_DATA_VALUE / pDisplay->yScale;
		}
		else
		{
			pDisplay->iDrawHeight = MAX_DATA_VALUE / pDisplay->yScale;
		    if (pDisplay->iDrawHeight < MIN_DISPLAY_HEGHT)
		    {
		        pDisplay->iDrawHeight = MIN_DISPLAY_HEGHT;
		        pDisplay->yScale = MAX_DATA_VALUE / MIN_DISPLAY_HEGHT;
		    }
		}

		//x 轴
		if (fPreDrawlen != pDisplay->fDrawLen)
		{
		    for (i = 0; i < pDisplay->iCurveNum; i++)
			{
				PCURVE_INFO pCTmp = pDisplay->pCurve[i];
				pCTmp->dOut.uiDrawCnt = pDisplay->fDrawLen / pCTmp->pParam.fSmpIntval;
				pCTmp->dOut.fDataPix = (float)ixNum / (float)pCTmp->dOut.uiDrawCnt;
		    }
			pCurve->dOut.iLeftOffset = pDisplay->xSclBase - (pCurve->dOut.uiDrawCnt / 2);
			fPreDrawlen = pDisplay->fDrawLen;
		}

		//y 轴
		// if (iPreDrawheight != pDisplay->iDrawHeight)
		{
			for (i = 0; i < pDisplay->iCurveNum; i++)
			{
				PCURVE_INFO pCTmp = pDisplay->pCurve[i];
				pCTmp->dOut.fValPix = (float)iyNum / (float)pDisplay->iDrawHeight;
			}
			pCurve->dOut.iBotOffset = pDisplay->ySclBase - (pDisplay->iDrawHeight / 2);
			iPreDrawheight = pDisplay->iDrawHeight;
		}
	}
	//判断是否有平移操作
	else if (((pDisplay->xScale != 1) || (pDisplay->yScale != 1)) &&
			 (iShifted))
    {	
    	float fMiddle;

		//计算偏移量
        fMiddle = (float)pDisplay->xShift / pCurve->dOut.fDataPix;        
		//fMiddle = (fMiddle > 0) ? Max(fMiddle, 1.0f) : Min(fMiddle, -1.0f);
		pCurve->dOut.iLeftOffset -= (int)fMiddle;

		fMiddle = (float)pDisplay->yShift / pCurve->dOut.fValPix;
		//fMiddle = (fMiddle > 0) ? Max(fMiddle, 1.0f) : Min(fMiddle, -1.0f);
		pCurve->dOut.iBotOffset += (int)fMiddle;
        iModifyBase = 1;
	}

	//判断是否需要计算偏移量
	if (iShifted || iScaled)
	{
		//右边界
	    if (pCurve->dOut.iLeftOffset + pCurve->dOut.uiDrawCnt > pCurve->dIn.uiCnt)
		{
	        pCurve->dOut.iLeftOffset = pCurve->dIn.uiCnt - pCurve->dOut.uiDrawCnt;
	    }
	    if (pCurve->dOut.iBotOffset + pDisplay->iDrawHeight > MAX_DATA_VALUE) 
		{
	        pCurve->dOut.iBotOffset = MAX_DATA_VALUE - pDisplay->iDrawHeight;
	    }
		//左边界
		if(pCurve->dOut.iLeftOffset < 0) 
		{
	        pCurve->dOut.iLeftOffset = 0;
	    }
	    if (pCurve->dOut.iBotOffset < 0) 
		{
	        pCurve->dOut.iBotOffset = 0;
	    }

        if(1 == iModifyBase)//解决禅道19649
        {
            pDisplay->xSclBase = pCurve->dOut.iLeftOffset + (pCurve->dOut.uiDrawCnt / 2);
            pDisplay->ySclBase = pCurve->dOut.iBotOffset + (pDisplay->iDrawHeight / 2);
            iModifyBase = 0;
        }

	    //恢复1:1
	    if(pLineCtl->enType == EN_CTL_RECOVER)
	    {
            pCurve->dOut.iLeftOffset = 0;
	    }
		//计算其他波形偏移值
	    for (i = 0; i < pDisplay->iCurveNum; i++)
	    {
	    	PCURVE_INFO pCTmp = pDisplay->pCurve[i];
	    	if (pCTmp == pCurve)
	    	{
				continue;
			}
			float fOffLen = pCurve->dOut.iLeftOffset * pCurve->pParam.fSmpIntval;
			pCTmp->dOut.iLeftOffset = fOffLen / pCTmp->pParam.fSmpIntval;
			pCTmp->dOut.iBotOffset = pCurve->dOut.iBotOffset;
	    }

		//设置需要更新的标志
		pCurve->dOut.uiNeedUpdate = 1;
		pCurve->cCoor.uiNeedUpdate = 1;
		pCurve->mMarker.uiPosiUpdate = 1;
		pCurve->cCursor.uiNeedUpdate = 1;
        pCurve->eEventMarker.uiNeedUpdate = 1;
	}

	//设置更新标志
	if (iNewData)
	{
		pCurve->pPreview->uiNeedUpdate = 1;
		pCurve->mMarker.uiLossUpdate = 1;
		pCurve->mMarker.uiaAUpdated = 1;
		pCurve->mMarker.uiBbUpdated = 1;
		ReCalcFileName(pDisplay);
	}
 	DBG_PRINT_TIME();  
	CURVE_DBG_EXIT(0);
	return 0;
}


//附加值处理
static int HandleAdditionCmd(PDISPLAY_INFO pDisplay, ADD_CTL *pAddCtl)
{
	switch (pAddCtl->enCmd)
	{
	case EN_ADD_REFSH:
		break;
	case EN_ADD_ADDTION:
		if (NULL != pDisplay->pAttach)
		{
			pDisplay->pAttach->pBgAddtion->fnDestroy((void **)&pDisplay->pAttach->pBgAddtion);
			pDisplay->pAttach->pLblAddtion->fnDestroy((void **)&pDisplay->pAttach->pLblAddtion);
			free(pDisplay->pAttach);
			pDisplay->pAttach = NULL;
		}
		pDisplay->pAttach = (CURVE_ADD *)pAddCtl->uValue.pAddition;
		break;
	case EN_ADD_MEATIME:
		pDisplay->iMeasTime = pAddCtl->uValue.iVaule;
		break;
//    case EN_ADD_END:
//        SetCurvesFullScreen1();
//        WriteOTDRMsgQueue(ENUM_OTDRMSG_STOP);
        break;
	default:
		break;
	}

	return 0;
}

//光标移到处理
static int HandleCursorMove(PDISPLAY_INFO pDisplay, CURSOR_CTL *pCursorCtl)
{
	int iDataShift, iCursor;
	float fMiddle;
	PCURVE_INFO pCurve;

	pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	if ((!pCurve->uiIsVaild) ||
		(pDisplay->iCurveNum <= 0))
	{
		return -1;
	}
	
	iCursor = pCurve->cCursor.dDot.iIndex;
	
	switch(pCursorCtl->enType)
	{
	case EN_MOVE_CLOCK: //旋钮逆时针
		iDataShift = pCursorCtl->iValue / pCurve->dOut.fDataPix;
		iDataShift = Max(1, iDataShift);
		iCursor += iDataShift;
		iCursor = Min(iCursor, pCurve->dIn.uiCnt - 1);
		break;
	case EN_MOVE_ANTI://旋钮顺时针
		iDataShift = pCursorCtl->iValue / pCurve->dOut.fDataPix;
		iDataShift = Max(1, iDataShift);
		iCursor -= iDataShift;
		iCursor = Max(iCursor, 0);
		break;
	case EN_MOVE_TOUCH://触摸滑动
		fMiddle = (float)(pCursorCtl->iValue - pDisplay->Rect.lSX);
		iCursor = pCurve->dOut.iLeftOffset + (int)(fMiddle / pCurve->dOut.fDataPix);
		break;
	case EN_MOVE_SETVAL://直接设置值
		{
            iCursor = pCursorCtl->iValue;
            int x = GetPosX(iCursor, pDisplay);
            if (x <= pDisplay->Rect.lSX || x >= pDisplay->Rect.lEX)
            {
                int w  = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
                int m = pDisplay->Rect.lSX + w/2;
                SendLineCtlMoveMsg(EN_CTL_DRAG, m - x, 0);
            }
        }
		break;
		
	default:
		break;
	}

	//设置光标位置
	pCurve->cCursor.dDot.iIndex = iCursor;
	pCurve->cCursor.uiNeedUpdate = 1;

	return 0;
}


//标记线移动处理
static int HandleMakerMove(PDISPLAY_INFO pDisplay, MAKER_CTL *pMakerCtl)
{
	//临时变量
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	MARKER *pMaker = &(pCurve->mMarker);
	float fTmp;
	int iOffset = 0;
    int iMoved = 0;

    //修改标记线是否可见不限制曲线是否存在
	if (((!pCurve->uiIsVaild) || (pDisplay->iCurveNum <= 0)) && 
		(pMakerCtl->enType != EN_MAKER_MEAS || pMakerCtl->enType != EN_MAKER_PATCH))
	{
		return -1;
	}
	
	switch(pMakerCtl->enType)
	{
	case EN_MAKER_ANTI://逆时针
	    iOffset = pMakerCtl->iValue / pCurve->dOut.fDataPix;
		if (iOffset < 1)
			iOffset = 1;
		iOffset = 0 - iOffset;
		iMoved = 1;
		break;
	case EN_MAKER_CLOCK://顺时针
	    iOffset = pMakerCtl->iValue / pCurve->dOut.fDataPix;
		if (iOffset < 1)
			iOffset = 1;
		iMoved = 1;
		break;
	case EN_MAKER_DOWN://触摸点击
        if (0 == MarkerOnTouch(pMaker, pMakerCtl->iValue, pDisplay))
        {
			pMaker->uiPosiUpdate = 1;
			pMaker->uiLossUpdate = 1;
			pCurve->cCursor.uiNeedUpdate = 1;
        }
		break;
	case EN_MAKER_MOVE://触摸移动
		fTmp = (float)(pMakerCtl->iValue - pDisplay->Rect.lSX);
		iOffset = pCurve->dOut.iLeftOffset + (int)(fTmp/pCurve->dOut.fDataPix);
		iOffset -= pMaker->dDot[pMaker->enPitchOn].iIndex;
	    iMoved = 1;
		break;
        
    case EN_MAKER_MEAS:
        {
             static unsigned int lossMethod = 2;
             if(pMaker->enMeas != pMakerCtl->iValue 
                 || lossMethod != pUser_Settings->sCommonSetting.iLossMethodMark)
             {
                 pMaker->enMeas = pMakerCtl->iValue;
            	//如果修改了展示的选项卡，重设光标可见性
                ResetMarkerVisible(pMaker);
         	    pMaker->uiLossUpdate = 1;
         	    lossMethod = pUser_Settings->sCommonSetting.iLossMethodMark;
     	    }
	    }
        break;
        
	case EN_MAKER_PATCH://修改链接方式
	    if(pMaker->enPitchOn != pMakerCtl->iValue)
	    {
        	pMaker->enPitchOn = pMakerCtl->iValue;
        	//如果修改了链接方式 重设光标颜色
            ResetMarkerColor(pMaker);
			pCurve->cCursor.dDot.iIndex = pMaker->dDot[pMaker->enPitchOn].iIndex;//按照标记线缩放
        }
        break;
		
	case EN_MAKER_AUTO://自动MARK
		if(0 == OtdrMeasAutoMove(pMakerCtl->iValue, pMaker))
		{
			pMaker->uiPosiUpdate = 1;
			pMaker->uiLossUpdate = 1;
			pCurve->cCursor.uiNeedUpdate = 1;
		    pMaker->uiaAUpdated = 1;
		    pMaker->uiBbUpdated = 1;
		}
	    break;

	default:
		break;
	}

    //如果发生移动
	if (iMoved)
	{
	 	if (!MarkerMoved(pMaker, iOffset, pCurve))
		{
			pMaker->uiPosiUpdate = 1;
			pMaker->uiLossUpdate = 1;
			pCurve->cCursor.uiNeedUpdate = 1;
		}		
	}

	return 0;
}

static void SetSectionEventPosition(int iXValue, int iYValue, PDISPLAY_INFO pDisplay)
{
    INT32 Xoffset = 0;
    INT32 Yoffset = 0;
    if (iXValue <= pDisplay->Rect.lSX || iXValue >= pDisplay->Rect.lEX)
    {
        int w  = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
        int m = pDisplay->Rect.lSX + w/2;
        Xoffset = m - iXValue;
    }
    
    if ((iYValue >= 0) && (iYValue <= pDisplay->Rect.lSY || iYValue >= pDisplay->Rect.lEY))
    {
        int w  = pDisplay->Rect.lEY - pDisplay->Rect.lSY + 1;
        int m = pDisplay->Rect.lSY + w/2;
        Yoffset = m - iYValue;
    }

    SendLineCtlMoveMsg(EN_CTL_DRAG, Xoffset, Yoffset);
}

//区段事件处理
static int HandleEventMarkerMove(PDISPLAY_INFO pDisplay, EVENT_MARKER_CTL *pEventMarkerCtl)
{
    int iDataShift;
	float fMiddle;
	PCURVE_INFO pCurve;

	pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	if ((!pCurve->uiIsVaild) ||
		(pDisplay->iCurveNum <= 0))
	{
		return -1;
	}

    int iLeftValue = pCurve->eEventMarker.dDot[0].iIndex;
    int iRightValue = pCurve->eEventMarker.dDot[1].iIndex;
	
	switch(pEventMarkerCtl->enType)
	{
	case EN_MOVE_CLOCK: //旋钮逆时针
	    iDataShift = pEventMarkerCtl->iLeftValue / pCurve->dOut.fDataPix;
		iDataShift = Max(1, iDataShift);
		iLeftValue += iDataShift;
		iLeftValue = Min(iLeftValue, pCurve->dIn.uiCnt - 1);
        iRightValue = -1;
        pCurve->cCursor.dDot.iIndex = iLeftValue;
		break;
	case EN_MOVE_ANTI://旋钮顺时针
	    iDataShift = pEventMarkerCtl->iLeftValue / pCurve->dOut.fDataPix;
		iDataShift = Max(1, iDataShift);
		iLeftValue -= iDataShift;
		iLeftValue = Max(iLeftValue, 0);
        iRightValue = -1;
        pCurve->cCursor.dDot.iIndex = iLeftValue;
		break;
	case EN_MOVE_TOUCH://触摸滑动
        fMiddle = (float)(pEventMarkerCtl->iLeftValue - pDisplay->Rect.lSX);
		iLeftValue = pCurve->dOut.iLeftOffset + (int)(fMiddle / pCurve->dOut.fDataPix);
        iRightValue = -1;
        pCurve->cCursor.dDot.iIndex = iLeftValue;
		break;
	case EN_MOVE_SETVAL://直接设置值
		{
            iLeftValue = pEventMarkerCtl->iLeftValue;
            iRightValue = pEventMarkerCtl->iRightValue;
            int iMaxFlag = pEventMarkerCtl->iMaxFlag;
            int leftValue = GetPosX(iLeftValue, pDisplay);
            int iAutoSpanFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iAutoSpanFlag;
            float fTotalPoint = (pCurve->pParam.fDistance) / (pCurve->pParam.fSmpIntval);
            float fScaleX = 0.0f;
            float fScaleY = 4.0f;

            if (iRightValue != -1)
            {
                int rightValue = GetPosX(iRightValue, pDisplay);
                int Xvalue = (leftValue + rightValue) / 2;
                int	Y = pCurve->dOut.pValue[Xvalue - pDisplay->Rect.lSX];
                int Yup = pCurve->dIn.pData[iLeftValue]; 
                int Ydown = pCurve->dIn.pData[iRightValue];
        		int	Yvalue = GetPosY(Y, pDisplay);
        		
                if(!pEventMarkerCtl->iTouchMove && !iMaxFlag)
                {
                    SetSectionEventPosition(Xvalue, Yvalue, pDisplay);
                }
                pCurve->cCursor.dDot.iIndex = (iLeftValue + iRightValue)/2;
                fScaleX = fTotalPoint / (iRightValue - iLeftValue) / 3;
                float temp = fabsf(Yup - Ydown) / pCurve->pParam.iFactor;
                fScaleY = RETURNLOSS_MAXVALUE / (temp * 2);
            }
            else
            {
                if(!pEventMarkerCtl->iTouchMove && !iMaxFlag)
                {
                    SetSectionEventPosition(leftValue, -1, pDisplay);
                }
                pCurve->cCursor.dDot.iIndex = iLeftValue;
                fScaleX = 16.0f;
            }
            
            if (iMaxFlag == 1)
            {
                fScaleY = fScaleY > 4.0f ? 4.0f : fScaleY;
                fScaleY = fScaleY < 1.0f ? 1.0f : fScaleY;
                SendLineCtlScaleMsg(EN_CTL_ZOOMVAL, fScaleX, fScaleY);
            }
            else if(!pEventMarkerCtl->iTouchMove)
            {                
                if (iAutoSpanFlag)
                    SendLineCtlScaleMsg(EN_CTL_ZOOMVAL, fScaleX, 1.0f);
            }
            
        }
		break;
		
	default:
		break;
	}

	//设置光标位置
	pCurve->eEventMarker.dDot[0].iIndex = iLeftValue;
    pCurve->eEventMarker.dDot[1].iIndex = iRightValue;
	pCurve->eEventMarker.uiNeedUpdate = 1;
    pCurve->cCursor.uiNeedUpdate = 1;

	return 0;
}
//获取显示对象参数
static int UpdateDisplayParam(PDISPLAY_INFO pDisplay)
{
    CURVE_DBG_ENTER();

	//判断参数是否发生改变
	if (pDisplay->CtlPara.enSize == EN_SIZE_NOMAL)
	{
		pDisplay->pBg = pDisplay->pSmallBg;
		pDisplay->ppCoorX = pDisplay->pSCoorX;
		pDisplay->ppCoorY = pDisplay->pSCoorY;
		pDisplay->Rect = pDisplay->SmallRect;
	}
	else if (pDisplay->CtlPara.enSize == EN_SIZE_LARGE)
	{
		pDisplay->pBg = pDisplay->pLargeBg;
		pDisplay->ppCoorX = pDisplay->pLCoorX;
		pDisplay->ppCoorY = pDisplay->pLCoorY;
		pDisplay->Rect = pDisplay->LargeRect;
	}
	
	if (pDisplay->CtlPara.enShot == EN_SHOT_YES)
	{
		pDisplay->pPreBg = pDisplay->pPreBgShot;
	}
	else if (pDisplay->CtlPara.enShot == EN_SHOT_NO)
	{
		pDisplay->pPreBg = pDisplay->pPreBgNoml;
	}

	int i;
	//损耗测量方法
	for (i = 0; i < CURVE_MAX; ++i)
	{
		pDisplay->pCurve[i]->mMarker.enMethd = pDisplay->CtlPara.enMethd;
		pDisplay->pCurve[i]->mMarker.enMethdAtten = pDisplay->CtlPara.enMethdAtten;
	}

	CURVE_DBG_EXIT(0);
	return 0;
}


//计算所有需要更新的资源
static int UpdateCurveAll(PDISPLAY_INFO pDisplay)
{
	float fTmp;
	CURVE_DRAW_FLGS DrawFlags;
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	DBG_INIT_TIME();	
    
	//判断是否有曲线数据
	if (pDisplay->iCurveNum <= 0)
	{
		return -1;
	}

	//获得画线标志
	MutexLock(&pDrawFlgs->mMutex);
	memcpy(&DrawFlags, pDrawFlgs, sizeof(CURVE_DRAW_FLGS));
	MutexUnlock(&pDrawFlgs->mMutex);

	//曲线特征值提取
	if (pCurve->dOut.uiNeedUpdate)
	{
		GetPixelValue(pDisplay);

		pCurve->dOut.uiNeedUpdate = 0;
		pCurve->uiIsVaild = 1;
		CURVE_DEBUG("Update Curve\n");
	}

	//预览图曲线特征值提取
	if (pCurve->pPreview->uiNeedUpdate)
	{
		GetPreviewPixelValue(pDisplay);
		
		pCurve->pPreview->uiNeedUpdate = 0;
		CURVE_DEBUG("Update Preview Curve\n");
	}

	//坐标系
	if (pCurve->cCoor.uiNeedUpdate)
	{
		//计算x 坐标系
		fTmp = pCurve->pParam.fSmpIntval;
		pCurve->cCoor.fDivX = pDisplay->fDrawLen / (float)DIV_NUM_X;
		pCurve->cCoor.fMinX = CalcRealDist(pCurve->dOut.iLeftOffset, fTmp);
		pCurve->cCoor.fMaxX = pCurve->cCoor.fMinX + pDisplay->fDrawLen;

		//计算y 坐标系
		fTmp = (float)pDisplay->iDrawHeight / (float)pCurve->pParam.iFactor;
        int size = pDisplay->CtlPara.enSize;
        pCurve->cCoor.fDivY = (size == EN_SIZE_LARGE) ? (fTmp / (float)DIV_NUM_Y)
                                                      : (fTmp / (float)(DIV_NUM_Y - 2));
        pCurve->cCoor.fMinY = (float)pCurve->dOut.iBotOffset / (float)pCurve->pParam.iFactor;
		pCurve->cCoor.fMaxY = pCurve->cCoor.fMinY + fTmp;

		ReCalcCoorX(pDisplay);
		ReCalcCoorY(pDisplay);

		pCurve->cCoor.uiNeedUpdate = 0;
		CURVE_DEBUG("Update Coor\n");
	}

	//光标
	if (pCurve->cCursor.uiNeedUpdate)
	{
		int x, iDataVal, iPixNum, iIndex;

		iIndex = pCurve->cCursor.dDot.iIndex;
		
		iPixNum = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
		x = GetPosX(iIndex, pDisplay);
		if((x >= pDisplay->Rect.lSX) && (x <= pDisplay->Rect.lEX))
		{
    		if (iPixNum > pCurve->dOut.uiDrawCnt)
    	    	iDataVal = pCurve->dOut.pValue[x - pDisplay->Rect.lSX + 1];
    		else
    	    	iDataVal = pCurve->dOut.pValue[x - pDisplay->Rect.lSX];

    		pCurve->cCursor.dDot.iPosX = x;
    		pCurve->cCursor.dDot.iPosY = GetPosY(iDataVal, pDisplay);
    		pCurve->cCursor.dDot.fDist = CalcRealDist(iIndex, pCurve->pParam.fSmpIntval);
    		pCurve->cCursor.dDot.iValue =iDataVal;// pCurve->dIn.pData[iIndex];
        }
        else
        {
            pCurve->cCursor.dDot.iPosX = x;//解决禅道19237 光标位置
        }
		pCurve->cCursor.uiNeedUpdate = 0;
		CURVE_DEBUG("Update Cursor\n");
	}

    //事件标记线位置
	if (pCurve->eEventMarker.uiNeedUpdate)
	{
		int iLeftIndex = pCurve->eEventMarker.dDot[0].iIndex;
        int iRightIndex = pCurve->eEventMarker.dDot[1].iIndex;
        int x1 = GetPosX(iLeftIndex, pDisplay);
        int x2 = (iRightIndex != -1) ? GetPosX(iRightIndex, pDisplay) : -1;

		int iPixNum = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;
		int iDataVal;
		if((x1 >= pDisplay->Rect.lSX) && (x1 <= pDisplay->Rect.lEX))
		{
    		if (iPixNum > pCurve->dOut.uiDrawCnt)
    		{
    	    	iDataVal = pCurve->dOut.pValue[x1 - pDisplay->Rect.lSX + 1];
    	    }
    		else
    		{
    	    	iDataVal = pCurve->dOut.pValue[x1 - pDisplay->Rect.lSX];
    	    }

            pCurve->eEventMarker.dDot[0].iPosX = x1;
    		pCurve->eEventMarker.dDot[0].fDist = CalcRealDist(iLeftIndex, pCurve->pParam.fSmpIntval);
            pCurve->eEventMarker.dDot[0].iValue = iDataVal;
            pCurve->eEventMarker.dDot[1].iPosX = x2;
    		pCurve->eEventMarker.dDot[1].fDist = CalcRealDist(iRightIndex, pCurve->pParam.fSmpIntval);
		}
        else
        {
            //解决禅道19237 光标位置
            pCurve->eEventMarker.dDot[0].iPosX = x1;
            pCurve->eEventMarker.dDot[1].iPosX = x2;
        }
		pCurve->eEventMarker.uiNeedUpdate = 0;
		CURVE_DEBUG("Update Event Marker\n");
	}

	//标记线位置
	if (pCurve->mMarker.uiPosiUpdate && DrawFlags.uiMarker)
	{
		MARKER *pMaker = &(pCurve->mMarker);
		
		pMaker->dDot[0].iPosX = GetPosX(pMaker->dDot[0].iIndex, pDisplay);
		pMaker->dDot[1].iPosX = GetPosX(pMaker->dDot[1].iIndex, pDisplay);
		pMaker->dDot[2].iPosX = GetPosX(pMaker->dDot[2].iIndex, pDisplay);
		pMaker->dDot[3].iPosX = GetPosX(pMaker->dDot[3].iIndex, pDisplay);

		pMaker->uiPosiUpdate = 0;
	}
    
	//标记线损耗距离等
	if (pCurve->mMarker.uiLossUpdate)
	{
		int i, iTmp;
		float fTmp;		
		
		MARKER *pMaker = &(pCurve->mMarker);

		//基本信息
		for (i = 0; i < pMaker->uiNum; ++i)
		{
			iTmp = pMaker->dDot[i].iIndex;
			pMaker->dDot[i].iPosX = GetPosX(iTmp, pDisplay);
			pMaker->dDot[i].iValue = pCurve->dIn.pData[iTmp];
			float fDist = CalcRealDist(iTmp, pCurve->pParam.fSmpIntval);
			float fRange = pCurve->pParam.fDistance;
			//范围内显示真实值
			if (fDist > 0.0f && fDist < fRange)
				pMaker->dDot[i].fDist = fDist;
			else
			{
				if (fDist <= 0.0f) //左边界
					pMaker->dDot[i].fDist = 0.0f;
				if (fDist >= fRange) //右边界
					pMaker->dDot[i].fDist = fRange;
			}
		}

		//dB/km
		//两点法衰减TPA
		iTmp = pMaker->dDot[2].iValue - pMaker->dDot[1].iValue;
		fTmp = (pMaker->dDot[2].fDist - pMaker->dDot[1].fDist) / 1000.0f;
		pMaker->fAttenAB = fabsf((float)iTmp / fTmp / (float)pCurve->pParam.iFactor);

        //四点法衰减  LSA
        SLINE tmpLine = FittingLine(pCurve->dIn.pData, 
										pMaker->dDot[1].iIndex, 
						 				pMaker->dDot[2].iIndex);
	    pMaker->fAtten_aABb = fabsf(1000.0f * tmpLine.k / pCurve->pParam.fSmpIntval / (float)pCurve->pParam.iFactor);
        

		//拟合aA直线
		if (pCurve->mMarker.uiaAUpdated)
		{
			pCurve->mMarker.lLineA = FittingLine(pCurve->dIn.pData, 
												 pMaker->dDot[0].iIndex, 
							 					 pMaker->dDot[1].iIndex);
			pCurve->mMarker.uiaAUpdated = 0;
		}

		//lLineA = FittingLine(pCurve->dIn.pData, pMaker->dDot[0].iIndex, 
		//					 pMaker->dDot[1].iIndex);
		//4点法损耗
		//拟合Bb直线
		if (pCurve->mMarker.uiBbUpdated)
		{
			pCurve->mMarker.lLineB = FittingLine(pCurve->dIn.pData, 
												 pMaker->dDot[2].iIndex, 
							 					 pMaker->dDot[3].iIndex);
			pCurve->mMarker.uiBbUpdated = 0;
		}
		//lLineB = FittingLine(pCurve->dIn.pData, pMaker->dDot[2].iIndex, 
		//					  pMaker->dDot[3].iIndex);
		pMaker->fLoss_aABb = LossLSA(pMaker->dDot[1].iIndex, pCurve->pParam.iFactor, 
									 &pCurve->mMarker.lLineA, &pCurve->mMarker.lLineB);
		//2点法损耗
		pMaker->fLoss_AB = pMaker->dDot[1].iValue - pMaker->dDot[2].iValue;
		pMaker->fLoss_AB /= pCurve->pParam.iFactor;

		//反射率
		fTmp = CalcRayleigh(pCurve->pParam.enPluse, pCurve->pParam.fSmpRatio, 
							pCurve->pParam.fBackScatter);
		pMaker->fReflect_AB = CalcReflect(pCurve->dIn.pData, pMaker->dDot[1].iIndex,
										  pMaker->dDot[2].iIndex, fTmp, 
										  pCurve->pParam.iFactor, &pCurve->mMarker.lLineA);
		//光回损
		pMaker->fReturnLoss = ReturnLoss(pCurve->dIn.pData, pMaker->dDot[1].iIndex,
										 pMaker->dDot[2].iIndex, 
										 pCurve->pParam.fBackScatter,
										 pCurve->pParam.fSmpRatio);
		pMaker->uiLossUpdate = 0;

		WriteOTDRMsgQueue(ENUM_OTDRMSG_MEASARGS);

		CURVE_DEBUG("Update Marker\n");
	}
 	DBG_PRINT_TIME();  

	return 0;
}


//读取队列消息,做相应处理
static int HandleDisplayCmd(PDISPLAY_INFO pDisplay, QUEUE_MSG *pDrawCmd)
{
    int iErr = 0;
	
    CURVE_DBG_ENTER();
	DBG_INIT_TIME();	

	//解析命令
	switch (pDrawCmd->enType)
	{
	case EN_CURSOR_CTL:
		CURVE_DEBUG("Cursor Ctl\n");
		HandleCursorMove(pDisplay, (CURSOR_CTL *)pDrawCmd->pContent);
		break;
	case EN_CURVE_ADD:
		CURVE_DEBUG("Addition Ctl\n");
		HandleAdditionCmd(pDisplay, (ADD_CTL *)pDrawCmd->pContent);
		break;
	case EN_LINE_CTL:
		CURVE_DEBUG("Line Ctl\n");
		HandleLineCtl(pDisplay, (LINE_CTL *)pDrawCmd->pContent);
		break;
	case EN_MAKER_CTL:
		CURVE_DEBUG("Maker Ctl\n");
		HandleMakerMove(pDisplay, (MAKER_CTL *)pDrawCmd->pContent);
        break;
    case EN_EVENT_MARKER_CTL:
        HandleEventMarkerMove(pDisplay, (EVENT_MARKER_CTL*)pDrawCmd->pContent);
        break;
            
	default:
		break;
	}

 	DBG_PRINT_TIME();  
	CURVE_DBG_EXIT(iErr);
    return iErr;
}

//曲线画图，将所有曲线及预览曲线画出
static int DrawCurve(PDISPLAY_INFO pDisplay)
{
    int iRet = 0;
    PCURVE_INFO pCurve = NULL;
    int iCurrPos;
    int iStartRef;
    int iCurrY,iNextY;
    float k;
    int x,x1,x2;
    GUIPEN *pPen;
    UINT32 iColor;
	int iDisplayHeigth;
	int i, iDrawX, iDrawY;
	UINT16 *pPixValue = NULL;//像素特征值
	int iFirstCurve, iLastCurve;
    
	DBG_INIT_TIME();	
    //获得画笔，得到当前颜色，以便画完曲线后设回原来颜色
	pPen = GetCurrPen();
	iColor = pPen->uiPenColor;

    if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iOnlyShowCurrCurve)
    {
        iFirstCurve = pDisplay->iCurCurve;
        iLastCurve = pDisplay->iCurCurve+1;
    }
    else
    {
        iFirstCurve = 0;
        iLastCurve = pDisplay->iCurveNum;
    }
    
	//获取当前模式（）
	// int size = pDisplay->CtlPara.enSize;
	// float scale = (size == EN_SIZE_LARGE) ? ((float)COOR_B_EY / (float)COOR_S_EY) : 1.0f;
    float scale = 1.0f;
    //画出所有曲线图	
	for (iCurrPos = iFirstCurve; iCurrPos < iLastCurve; ++iCurrPos)
	{				
		UINT32 *pPixIndex;//每个像素对应的原始数据索引值
		int iCurIndex;
		int iCurvePos = iCurrPos;

		pCurve = pDisplay->pCurve[iCurvePos];
	    iDisplayHeigth = pDisplay->Rect.lEY - pDisplay->Rect.lSY + 1;
	    pPixValue = pCurve->dOut.pValue;
	    pPixIndex = pCurve->dOut.pIndex;
	    pPen->uiPenColor = pCurve->dOut.uiColor;
	    iDrawX = pDisplay->Rect.lSX;
 	    iDrawY = pDisplay->Rect.lEY;
	    //描绘曲线
	    iCurIndex = 0;

		iStartRef = -1;
		if (pCurve->rReference.uiIsVaild)
		{
			iStartRef = pCurve->rReference.dDot.iIndex;
		}

		//画出所有特征值
        for (i = 0; i < pCurve->dOut.iValNum - 1; i++)
    	{		
			//获得点的y坐标
			iCurrY = iDrawY - GetPosY(pPixValue[i], pDisplay);
    	    iNextY = iDrawY - GetPosY(pPixValue[i + 1], pDisplay);
			//不同模式下平移曲线
			iCurrY *= scale;
			iNextY *= scale;
			//判断是否存在参考起始点，若存在，当前曲线的参考点前方颜色灰化
    	    if ((iStartRef != -1) && 
			   	(pPixIndex[iCurIndex] + pCurve->dOut.iLeftOffset < iStartRef) &&
			   	(iCurvePos == pDisplay->iCurCurve))
    	    {
    	        pPen->uiPenColor = COLOR_GRAY;
    	    }
    	    else if(iCurvePos == pDisplay->iCurCurve)
    	    {
				//当前波长显示为红色，默认所有曲线都是黑色
    	        pPen->uiPenColor = COLOR_RED;
    	    }
	
    	    //判断是否进行连线,显示少量数据时，只对存在不同数据的像素点进行连线即可
    	    if (pPixIndex[i+1] > pPixIndex[iCurIndex])
    	    {
    	        //在索引满足连线要求的情况下，需要判断特征点是否在界外
    	        if(iCurrY >= 0 && iCurrY <= iDisplayHeigth && iNextY >= 0 && iNextY <= iDisplayHeigth)
    	        {
    	            //两点都在界内，则正常连线(加粗曲线)
    	            DrawLine(iDrawX + iCurIndex, iDrawY - iCurrY, iDrawX + i + 1, iDrawY - iNextY);
					DrawLine(iDrawX + iCurIndex, iDrawY - iCurrY + 1, iDrawX + i + 1, iDrawY - iNextY + 1);
				}
    	        else if((iCurrY < 0 && iNextY < 0) || (iCurrY > iDisplayHeigth && iNextY > iDisplayHeigth))
    	        {
    	            //两点都在界外同一侧，则无需连线
    	        }
    	        else if((iCurrY < 0 && iNextY > iDisplayHeigth) || (iCurrY > iDisplayHeigth && iNextY < 0))
    	        {
    	            //两点在界外两侧，需要找到两个边界点的交点，进行连线
    	            k = (float)(iNextY - iCurrY)/(i + 1 - iCurIndex);
    	            if(k >= 0.0f)  //第一个点在界外下方，第二个点在界外上方
    	            {
    	                x1 = (int)(0 - iCurrY) / k;
    	                x2 = (int)(iDisplayHeigth - iCurrY) / k;
    	                DrawLine(iDrawX + iCurIndex + x1, iDrawY, iDrawX + iCurIndex + x2, pDisplay->Rect.lSY);
    	            }
    	            else        //第一个点在界外上方，第二个点在界外下方
    	            {
     	                x1 = (int)(iDisplayHeigth - iCurrY) / k;
    	                x2 = (int) (0 - iCurrY) / k;
    	                DrawLine(iDrawX + iCurIndex + x1, pDisplay->Rect.lSY, iDrawX + iCurIndex + x2, iDrawY);
    	            }
    	        }
    	        else if((iCurrY >= 0 && iCurrY <= iDisplayHeigth) && (iNextY > iDisplayHeigth || iNextY < 0))
    	        {
    	            //第一个在界内，第二个在界外，需找到边界点交点，进行连线
    	            k = (float)(iNextY - iCurrY)/(i + 1 - iCurIndex);    	            
    	            if(k >= 0.0f) //第二个点在界外上方
    	            {
    	                x = iCurIndex + (int)(iDisplayHeigth - iCurrY) / k;
    	                DrawLine(iDrawX + iCurIndex, iDrawY - iCurrY+1, iDrawX + x, pDisplay->Rect.lSY);
    	            }
    	            else       //第二个点在界外下方
    	            {
    	                x = iCurIndex + (int)(0 - iCurrY) / k;
    	                DrawLine(iDrawX + iCurIndex, iDrawY - iCurrY, iDrawX + x, pDisplay->Rect.lEY);
    	            }
    	        }
    	        else if((iCurrY < 0 || iCurrY > iDisplayHeigth) && (iNextY <= iDisplayHeigth && iNextY >= 0))
    	        {
    	            //第一个点在界外，第二个点在界内
    	            k = (float)(iNextY - iCurrY)/(i + 1 - iCurIndex);    	            
    	            if(k >= 0.0f)  //第一个点在界外下方
    	            {
    	                x = (int)(0 - iCurrY) / k;
    	                DrawLine(iDrawX + iCurIndex + x, pDisplay->Rect.lEY, iDrawX + i + 1, iDrawY - iNextY);
    	            }
    	            else        //第一个点在界外上方
    	            {
    	                x = (int)(iDisplayHeigth - iCurrY) / k;
    	                DrawLine(iDrawX + iCurIndex + x, pDisplay->Rect.lSY, iDrawX + i + 1, iDrawY - iNextY);
    	            }
    	        }
    	        
    	        iCurIndex = i+1;
    	    }
    	}
	}
	pPen->uiPenColor = iColor;

 	DBG_PRINT_TIME();  

    return iRet;
}


//绘制光标
static int DrawCursor(PDISPLAY_INFO pDisplay)
{
	int iErr = 0;

	int x, y; 
    
    UINT32 uiColor;
    GUIPEN *pPen;
	
    CURVE_DBG_ENTER();
	DBG_INIT_TIME();
	
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	
	//获取数据索引	
    pPen = GetCurrPen();

	//获取x y坐标
	x = pCurve->cCursor.dDot.iPosX;
	y = pCurve->cCursor.dDot.iPosY;

	//设置画笔颜色
    uiColor = pPen->uiPenColor;
    pPen->uiPenColor = COLOR_BLUE;
	
	if ((x >= pDisplay->Rect.lSX) && (x <= pDisplay->Rect.lEX))
	{
    	DrawLine(x, pDisplay->Rect.lSY+55, x, pDisplay->Rect.lEY);
		
// 		if ((y >= pDisplay->Rect.lSY) && (y <= pDisplay->Rect.lEY))
//     	{
// 			DrawLine(pDisplay->Rect.lSX, y, pDisplay->Rect.lEX, y);
// 		}
	}
    pPen->uiPenColor = uiColor;
	
 	DBG_PRINT_TIME();  
	CURVE_DBG_EXIT(iErr);
	return iErr;
}

static void DrawEventDot(PDISPLAY_INFO pDisplay, int x)
{    
    if ((x >= pDisplay->Rect.lSX) && (x <= pDisplay->Rect.lEX))
	{
    	DrawLine(x, pDisplay->Rect.lSY, x, pDisplay->Rect.lEY);
	}
}

//描绘事件标记点
static int DrawEventMarker(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;
    const char *ppMarkNum[] = {"a", "A"};
	const int xalgin[] = {3, 3};
	const int yalgin[] = {20, 20};
	int x, y; 
    UINT32 uiColor;
    GUIPEN *pPen;
	SetDrawCursor(0);
    CURVE_DBG_ENTER();
	DBG_INIT_TIME();
	
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	EVENT_MARKER eEventMarker = pCurve->eEventMarker;
	//获取数据索引	
    pPen = GetCurrPen();

	//设置画笔颜色
    uiColor = pPen->uiPenColor;
    pPen->uiPenColor = COLOR_BLACK;
    DrawEventDot(pDisplay, eEventMarker.dDot[0].iPosX);
    DrawEventDot(pDisplay, eEventMarker.dDot[1].iPosX);
	
    pPen->uiPenColor = uiColor;
    
    if (eEventMarker.dDot[0].iPosX == eEventMarker.dDot[1].iPosX)
    {
        x = eEventMarker.dDot[0].iPosX;
        
        if ((x >= pDisplay->Rect.lSX) &&
		    (x <= pDisplay->Rect.lEX))
        {
            y = pDisplay->Rect.lSY + yalgin[0];
        	x += xalgin[0];
        	x = ((x < pDisplay->Rect.lSX) || (x > pDisplay->Rect.lEX - 12)) ? -1 : x;

        	//显示序号
        	if (-1 != x) 
        	{
                GUICHAR *pStr = TransString((char *)ppMarkNum[0]);
            	GUILABEL *pLbl = CreateLabel(x, y, 16, 24, pStr);
            	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLbl);
            	DisplayLabel(pLbl);
            	GuiMemFree(pStr);
            	DestroyLabel(&pLbl);
        	}
        }
        return iErr;
    }
    
    int i;
    for (i = 0; i < 2; ++i)
    {
        x = eEventMarker.dDot[i].iPosX;
        if ((x >= pDisplay->Rect.lSX) &&
			(x <= pDisplay->Rect.lEX))
		{
            //确定标记序号位置
        	y = pDisplay->Rect.lSY + yalgin[i];
        	x += xalgin[i];
        	x = ((x < pDisplay->Rect.lSX) || (x > pDisplay->Rect.lEX - 12)) ? -1 : x;

        	//显示序号
        	if (-1 != x) 
        	{
        		GUICHAR *pStr = TransString((char *)ppMarkNum[i]);
        		GUILABEL *pLbl = CreateLabel(x, y, 16, 24, pStr);
        		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLbl);
        		DisplayLabel(pLbl);
        		GuiMemFree(pStr);
        		DestroyLabel(&pLbl);
        	}
        }
    }
    
    
 	DBG_PRINT_TIME();  
	CURVE_DBG_EXIT(iErr);
	return iErr;
}

//描绘标记点
static int DrawMarker(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;
	// const char *ppMarkNum[] = {"a", "A", "B", "b"};
	// const int xalgin[] = {3, 10, 10, -8};
	const int yalgin[] = {10, 20, 45, 10};
	
	int x, y, i;
	MARKER *pMarker;
	
    GUIPEN *pPen;
    UINT32 iColor;
   	
    CURVE_DBG_ENTER();
	DBG_INIT_TIME();	
	
	//中间变量的赋值
	pMarker = &(pDisplay->pCurve[pDisplay->iCurCurve]->mMarker);

	//获得画笔，设置曲线颜色
	pPen = GetCurrPen();

    //从A标记线开始绘制，最后绘制a，避免只选中a并aA在同一位置时，A会把A覆盖
    int iStart = 1;
    int iEnd = pMarker->uiNum + 1;
	
	for (i = iStart; i < iEnd; ++i)
	{
	    if(i == pMarker->uiNum)
	    {
            i = 0;
	    }
	    
        //不可见的标记线不绘制
	    if (!pMarker->uiVisible[i])
        {
	    	if (i == 0)
				break;
			else
				continue;
	    }
        
		//得到坐标
		x = pMarker->dDot[i].iPosX;
		//检测边界值
		if (x < pDisplay->Rect.lSX)
			x = pDisplay->Rect.lSX;
		else if (x > pDisplay->Rect.lEX)
			x = pDisplay->Rect.lEX;
		{
			//设置画笔颜色
			iColor = pPen->uiPenColor;
			pPen->uiPenColor = pMarker->uiColor[i];
			if (i == 1)//显示A
			{
				DrawLine(x, pDisplay->Rect.lSY+25, x, pDisplay->Rect.lEY);
				DrawLine(x+1, pDisplay->Rect.lSY + 25, x+1, pDisplay->Rect.lEY);//加粗标杆
			}
			else if (i == 2)
			{
				DrawLine(x, pDisplay->Rect.lSY + 50, x, pDisplay->Rect.lEY);
				DrawLine(x + 1, pDisplay->Rect.lSY + 50, x + 1, pDisplay->Rect.lEY); //加粗标杆
			}
			pPen->uiPenColor = iColor;

			//确定标记序号位置
			y = pDisplay->Rect.lSY + yalgin[i] + 10;
		// x += xalgin[i];
			x = (i < 2) ?
			((x < pDisplay->Rect.lSX)? pDisplay->Rect.lSX : x) :
			((x > pDisplay->Rect.lEX)? pDisplay->Rect.lEX : x);

			//显示序号
			if (-1 != x) 
			{
				if ((i == 1) || (i == 2))//只显示A，B
				{
					char *cDist = UnitConverter_Dist_M2System_Float2String(
						MODULE_UNIT_OTDR, pMarker->dDot[i].fDist, 1);
					char buffer[32] = {0};
					if (i == 1)
						sprintf(buffer, "A(%s)", cDist);
					else
						sprintf(buffer, "B(%s)", cDist);
					free(cDist);
					cDist = NULL;
					//绘制背景块
					GUIPICTURE *pBg = NULL;
					GUICHAR *pStr = TransString(buffer);
					GUILABEL *pLbl = NULL;
					//判断边界位置
					if ((x + 100) <= pDisplay->Rect.lEX)
					{
						pBg = (pMarker->uiColor[i] == COLOR_BLUE) ? CreatePicture(x, y - 5, 100, 20, BmpFileDirectory "bg_marker_press.bmp")
																  : CreatePicture(x, y - 5, 100, 20, BmpFileDirectory "bg_marker_unpress.bmp");
						pLbl = CreateLabel(x, y - 5 + 2, 100, 16, pStr);
					}
					else
					{
						pBg = (pMarker->uiColor[i] == COLOR_BLUE) ? CreatePicture(x-100, y - 5, 100, 20, BmpFileDirectory "bg_marker_press.bmp")
																  : CreatePicture(x-100, y - 5, 100, 20, BmpFileDirectory "bg_marker_unpress.bmp");
						pLbl = CreateLabel(x-100, y - 5 + 2, 100, 16, pStr);
					}
					//判断显示背景文字条件
					if (pBg && pLbl && pStr)
					{
						SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLbl);
						SetLabelAlign(GUILABEL_ALIGN_CENTER, pLbl);
						DisplayPicture(pBg);
						DisplayLabel(pLbl);
						GuiMemFree(pStr);
						DestroyPicture(&pBg);
						DestroyLabel(&pLbl);
					}
				}
			}
		}

		if(i == 0)
		{
            break;
		}
	}
	
 	DBG_PRINT_TIME();  
	CURVE_DBG_EXIT(iErr);
	return iErr;
}

//画事件点(isDrawBg: 0,不绘制背景，1,绘制背景)
static int DrawEvent(PDISPLAY_INFO pDisplay, int isDrawBg)
{
    int iErr = 0;

	int iDataIndex, iDataVal, iPixNum;
	int iPosX, x, iPosY, y;
	char strTmp[8];
	UINT16 *pPixVal;
    EVENTS_TABLE *pEvtTab;

	GUIPEN *pPen;
    UINT32 uiColor;
  
    CURVE_DBG_ENTER();
	DBG_INIT_TIME();
    
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];

    //中间变量的赋值
	pEvtTab = &pCurve->Events;
    if (pEvtTab->iEventsNumber <= 0)
        return 0;
	
	pPixVal = pCurve->dOut.pValue;
	iPixNum = pDisplay->Rect.lEX - pDisplay->Rect.lSX + 1;

	//获得画笔
	pPen = GetCurrPen();
	int iStartIndex = (pEvtTab->iLaunchFiberIndex > 0) ? pEvtTab->iLaunchFiberIndex : pEvtTab->iEventsStart;
    int iEndIndex = (pEvtTab->iRecvFiberIndex > 0) ? pEvtTab->iRecvFiberIndex : pEvtTab->iEventsNumber - 1;
	int i;

	if(pEvtTab->iIsThereASpan == 0)
	{
        iEndIndex = iStartIndex;
	}
	
    for (i = 0; i < pEvtTab->iEventsNumber; ++i)
    {
	    iDataIndex = pEvtTab->EventsInfo[i].iBegin;

		//判断标记点横坐标是否在显示范围
		if ((iDataIndex < pCurve->dOut.iLeftOffset) ||
			(iDataIndex >= pCurve->dOut.iLeftOffset + pCurve->dOut.uiDrawCnt))
		{
			continue;
		}
        if (pEvtTab->iEventsStart > 0 && i == pEvtTab->iEventsStart )
        {
            continue;
        }

		//计算x，y坐标
		iPosX = GetPosX(iDataIndex, pDisplay);
		if (iPixNum > pCurve->dOut.uiDrawCnt)
			iDataVal = pPixVal[iPosX - pDisplay->Rect.lSX + 1];
		else
			iDataVal = pPixVal[iPosX - pDisplay->Rect.lSX];
		iPosY = GetPosY(iDataVal, pDisplay);

		y = iPosY;
		x = iPosX;

		//判断y坐标是否在显示范围内
		if ((y >= pDisplay->Rect.lSY) &&
			(y <= pDisplay->Rect.lEY))
		{
			//画事件标记线
			if (y < pDisplay->Rect.lSY + 20)
			{
				y = pDisplay->Rect.lSY + 20;
			}
			else if (y > pDisplay->Rect.lEY - 20)
			{
				y = pDisplay->Rect.lEY - 20;
			}

			//计算事件序号
			if (i == iStartIndex)
			{
			    if(iStartIndex == iEndIndex)
			    {
                    strTmp[0] = 'S';
                    strTmp[1] = '/';
                    strTmp[2] = 'E';
                    strTmp[3] = '\0';
			    }
			    else
			    {
                    strTmp[0] = 'S';
                    strTmp[1] = '\0';
				}
			}
			else if (i == iEndIndex)
			{
				strTmp[0] = 'E';
				strTmp[1] = '\0';
			}
			else if (i > iStartIndex && i < iEndIndex)
			{
				sprintf(strTmp, "%d", i - iStartIndex);
			}
            else 
            {
                strTmp[0] = '*';
				strTmp[1] = '\0';
            }

			//确定序号显示坐标
			y = iPosY;
			if (x > pDisplay->Rect.lEX - 16)
			{
				x -= 12;
			}

			if (y > pDisplay->Rect.lEY - 24)
			{
				y = pDisplay->Rect.lEY - 24;
			}
			
            x += 10;
			//绘制标记竖线
			uiColor = pPen->uiPenColor;
			int iEvtPosX = pCurve->eEventMarker.dDot[0].iPosX;
			//设置选中竖线的颜色
			if (x == (iEvtPosX + 10)) //选中项标记为蓝色
				pPen->uiPenColor = COLOR_BLUE;
			else //其他显示为黑色
				pPen->uiPenColor = COLOR_BLACK;

			DrawLine(x-10, y - 20, x-10, y + 20);
			DrawLine(x-10 + 1, y - 20, x-10 + 1, y + 20); //加粗标记线
			pPen->uiPenColor = uiColor;
			//绘制序号
			GUICHAR *pStr = TransString(strTmp);
			GUILABEL *pLbl = CreateLabel(x-10+1, y+25, 16, 16, pStr);
			GUIPICTURE *pBg = NULL;
			SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLbl);
			SetLabelAlign(GUILABEL_ALIGN_LEFT, pLbl);
			//绘制背景
			if (isDrawBg)
			{
				iEvtPosX = pCurve->eEventMarker.dDot[0].iPosX;
				if (x == (iEvtPosX + 10))
					pBg = CreatePicture(x - 10+1-4, y +25, 16, 16, BmpFileDirectory "bg_marker_press.bmp");
				else
					pBg = CreatePicture(x - 10+1-4, y +25, 16, 16, BmpFileDirectory "bg_marker_disable.bmp");
				DisplayPicture(pBg);
			}
			//显示事件点文本
			DisplayLabel(pLbl);
			GuiMemFree(pStr);
			//销毁背景
			if (!pBg)
				DestroyPicture(&pBg);
			DestroyLabel(&pLbl);
		}
	}
	
 	DBG_PRINT_TIME();  
	CURVE_DBG_EXIT(iErr);
    return iErr;
}


//画参考起始点
static int DrawStartRef(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;
	
    UINT32 uiRefIndex;
	INT32 x, y;
	char strTmp[4];

	GUIPEN *pPen;
    UINT32 iColor;

    CURVE_DBG_ENTER();
	DBG_INIT_TIME();
	
	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	
    //判断当前波形的参考起始点是否设置
    if (!pCurve->rReference.uiIsVaild)
    {    
    	CURVE_DBG_EXIT(iErr);
        return iErr;
    }
	
	uiRefIndex = pCurve->rReference.dDot.iIndex;
	    
    //判断标记点横坐标是否在显示范围
    if ((uiRefIndex >= pCurve->dOut.iLeftOffset) &&
     	(uiRefIndex < pCurve->dOut.iLeftOffset + pCurve->dOut.uiDrawCnt))
    {
        //获得x y 值		
        x = GetPosX(uiRefIndex, pDisplay);
        y = pDisplay->Rect.lSY;

	    //获得画笔
		pPen = GetCurrPen();
		iColor = pPen->uiPenColor;
		pPen->uiPenColor = COLOR_BLACK;

		//画线
		DrawLine(x, y, x, pDisplay->Rect.lEY);
		pPen->uiPenColor = iColor;

		//调整x, y
	    x = Max(x, pDisplay->Rect.lSX + 9);
	    x = Min(x, pDisplay->Rect.lEX - 9);

	    sprintf(strTmp, "%s", "R");
		
		GUICHAR *pStr = TransString(strTmp);
		GUILABEL *pLbl = CreateLabel(x, y+20, 24, 24, pStr);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLbl);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pLbl);
		DisplayLabel(pLbl);
		
		DestroyLabel(&pLbl);
		GuiMemFree(pStr);
    }
	
 	DBG_PRINT_TIME();  
    CURVE_DBG_EXIT(iErr);
    return iErr;
}


//绘制坐标轴
static int DrawCoor(PDISPLAY_INFO pDisplay)
{	
	int i = 0;

    CURVE_DBG_ENTER();
	DBG_INIT_TIME();
	
	for (i = 0; i < DIV_NUM_X + 1; ++i)
	{
		DisplayLabel(pDisplay->ppCoorX[i]);
	}

	for (i = 0; i < DIV_NUM_Y+1; ++i)
	{
		if (pDisplay->CtlPara.enSize == EN_SIZE_NOMAL)
		{
			//去除零点
			if ((i < DIV_NUM_Y - 1) && (i != DIV_NUM_Y - 3))
			{
				DisplayLabel(pDisplay->ppCoorY[i]);
			}
		}
		else
		{
			//去除零点
			if (i != DIV_NUM_Y - 1)
				DisplayLabel(pDisplay->ppCoorY[i]);
		}
	}
	
	DBG_PRINT_TIME();
	CURVE_DBG_EXIT(0);
    return 0;
}

//绘制光标信息 cursorType:1 OTDR第一个界面光标 2 事件列表光标
static int DrawCusorDistance(PDISPLAY_INFO pDisplay, int cursorType)
{
	GUILABEL *pLbl = NULL;
	GUICHAR *pStr = NULL;

	float fDist;
	float fValue;
	char buffer[32] = {0};
	DBG_INIT_TIME();

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	//显示AB标记线信息
	if(cursorType == 1)
	{
		char *cDistA = UnitConverter_Dist_M2System_Float2String(
			MODULE_UNIT_OTDR, pCurve->mMarker.dDot[1].fDist, 1);
		char *cDistB = UnitConverter_Dist_M2System_Float2String(
			MODULE_UNIT_OTDR, pCurve->mMarker.dDot[2].fDist, 1);
		float fValueA = (float)pCurve->mMarker.dDot[1].iValue / SCALE_FACTOR;
		float fValueB = (float)pCurve->mMarker.dDot[2].iValue / SCALE_FACTOR;
		sprintf(buffer, "A(%s, %.3fdB), B(%s, %.3fdB)", cDistA, fValueA, cDistB, fValueB);
		free(cDistA);
		free(cDistB);
		cDistA = NULL;
		cDistB = NULL;
	}
	else
	{
		fDist = pCurve->eEventMarker.dDot[0].fDist - pCurve->rReference.dDot.fDist;
		fValue = (float)pCurve->eEventMarker.dDot[0].iValue / SCALE_FACTOR;
		char *cTemp = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, fDist, 1);
		sprintf(buffer, "(%s, %.3fdB)", cTemp, fValue);
		free(cTemp);
		cTemp = NULL;
	}

	pStr = TransString(buffer);
	pLbl = CreateLabel(50, 95, 300, 24, pStr);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLbl);
	DisplayLabel(pLbl);
	DestroyLabel(&pLbl);
	free(pStr);
	DBG_PRINT_TIME();
	return 0;
}

//画选择框
static int DrawScratchRect(PDISPLAY_INFO pDisplay)
{
	unsigned int uiColor = 0;
	GUIPEN *pPen = GetCurrPen();

	uiColor = pPen->uiPenColor;
	pPen->uiPenColor = COLOR_BLACK;
	DrawRectangle(pDisplay->Scratch.lSX, pDisplay->Scratch.lSY, 
				  pDisplay->Scratch.lEX, pDisplay->Scratch.lEY);
 	pPen->uiPenColor = uiColor;
	
	return 0;
}

//绘制文件名
static int DrawFilename(PDISPLAY_INFO pDisplay)
{
    //绘制文件名
	DBG_INIT_TIME();
	SetPictureBitmap(BmpFileDirectory"bg_otdr_fileName.bmp", pDisplay->pPicFile);
	DisplayPicture(pDisplay->pPicFile);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pDisplay->pLblFile);
	DisplayLabel(pDisplay->pLblFile);
 	//DisplayLabel(pDisplay->pLblDot);
 	DBG_PRINT_TIME();

    return 0;
}

//显示激光器发射中
int DrawLaserIsOn(int isChange)
{
    static int  colorFlag = 0;
    GUICHAR     *pStrWarning = NULL;
    GUICHAR     *pStrLaserIsOn = NULL;
    GUILABEL    *pLblWarning = NULL;
    GUILABEL    *pLblLaserIsOn = NULL;
	GUIPICTURE  *pPicWarningBg = NULL;

	pStrWarning = TransString("Warning!");
	pStrLaserIsOn = TransString("Laser is ON.");
    pLblWarning = CreateLabel(698, 205, 100, 24, pStrWarning);
    pLblLaserIsOn = CreateLabel(698, 221, 100, 24, pStrLaserIsOn);
    
    pPicWarningBg = CreatePicture(681, 199, 119, 110, BmpFileDirectory"bg_otdr_warning_yellow.bmp");
    
    if((!isChange) || (!colorFlag && isChange))
    {
        SetLabelFont(getGlobalFnt(EN_FONT_RED), pLblWarning);
        SetLabelFont(getGlobalFnt(EN_FONT_RED), pLblLaserIsOn);
    }
    else
    {
        SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLblWarning);
        SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLblLaserIsOn);
        SetPictureBitmap(BmpFileDirectory"bg_otdr_warning_red.bmp", pPicWarningBg);
    }
    
    colorFlag = colorFlag ? 0 : 1;
    DisplayPicture(pPicWarningBg);   

	DisplayLabel(pLblWarning);
	DisplayLabel(pLblLaserIsOn);
	
    DestroyPicture(&pPicWarningBg);
    DestroyLabel(&pLblWarning);
    DestroyLabel(&pLblLaserIsOn);
    free(pStrWarning);
    free(pStrLaserIsOn);

    return 0;
}

//测量时间
static int DrawMeasTime(PDISPLAY_INFO pDisplay)
{
    static int lastTime = -1;
    
	//GUIPICTURE *pPicProcessBg = NULL;
	GUIPICTURE *pPicProcessValue = NULL;

	int iTotalTime = pDisplay->pCurve[pDisplay->iCurCurve]->pParam.uiAvgTime;


    if(pDisplay->iMeasTime != 1)
    {
        if(lastTime == pDisplay->iMeasTime)
        {
            return -1;
        }
        lastTime = pDisplay->iMeasTime;
    }
    else
    {
        static struct timeval tv_last;
        struct timeval tv_current;
    	gettimeofday(&tv_current, NULL);
        
        unsigned long timeDiff = (tv_current.tv_sec - tv_last.tv_sec)*1000000 + tv_current.tv_usec - tv_last.tv_usec;
            
    	if(timeDiff < 600000)
    	{
            return -1;
        }
        tv_last = tv_current;
    }

	// pPicProcessBg = CreatePicture(98, 0, 140, 20, BmpFileDirectory"bg_otdr_fileName.bmp");
	pPicProcessValue = CreatePicture(190, 10, 241, 20, BmpFileDirectory"bg_otdr_test_process0.bmp");
    
	char temp[64] = { 0 };
	float process = (float)(pDisplay->iMeasTime) / (float)iTotalTime;
	int num = 10 * (1 - process);
	memset(temp, 0, 64);
	sprintf(temp, BmpFileDirectory"bg_otdr_test_process%d.bmp", num);
	SetPictureBitmap(temp, pPicProcessValue);
    
	// DisplayPicture(pPicProcessBg);
	if (1 != pDisplay->pCurve[pDisplay->iCurCurve]->pParam.uiAvgTime)
	{
		DisplayPicture(pPicProcessValue);
	}
// 	DrawLaserIsOn(1);
    
	// DestroyPicture(&pPicProcessBg);
	DestroyPicture(&pPicProcessValue);
    
	return 0;
}


//绘制预览区波形
static int DrawPreviewCurve(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;
	
    GUIPEN *pPen;
    UINT32 uiColor;
	
	float fTmp;
	
    int i, iCurrY, iNextY;
    int x1, x2, y1, y2;
	int iPixNum, iPixHeight;

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	PPREVIEW_TRACE pPreview = pCurve->pPreview;
	DBG_INIT_TIME();
	
    //获得画笔
	pPen = GetCurrPen();

	//显示宽度高度计算
	iPixHeight = pPreview->Rect.lEY - pPreview->Rect.lSY + 1;
	iPixNum = pPreview->Rect.lEX - pPreview->Rect.lSX + 1;

	//描绘预览曲线
	fTmp = (float)iPixHeight / MAX_DATA_VALUE;
	for (i = 0; i < pPreview->iPixValNum - 1; ++i)
	{
		//当前Y值计算
	    iCurrY = pPreview->Rect.lEY - (int)(pPreview->pPixVal[i] * fTmp);
	    if (iCurrY < pPreview->Rect.lSY)
		{
	        iCurrY = pPreview->Rect.lSY;
	    }
		else if (iCurrY > pPreview->Rect.lEY)
		{
	        iCurrY = pPreview->Rect.lEY;
		}

		//下一个点的Y值
	    iNextY = pPreview->Rect.lEY - (int)(pPreview->pPixVal[i+1] * fTmp);
	    if (iNextY < pPreview->Rect.lSY)
	    {
	        iNextY = pPreview->Rect.lSY;
	    }
	    else if (iNextY > pPreview->Rect.lEY)
    	{
        	iNextY = pPreview->Rect.lEY;
    	}

		uiColor = pPen->uiPenColor;
		pPen->uiPenColor = COLOR_BLUE;
		DrawLine(pPreview->Rect.lSX + i, iCurrY, 
				 pPreview->Rect.lSX + i + 1, iNextY);
		pPen->uiPenColor = uiColor;
	}
	
	//描绘选择框
	fTmp = pCurve->pParam.fDistance / iPixNum;
	x1 = pCurve->cCoor.fMinX / fTmp;
	x2 = pCurve->cCoor.fMaxX / fTmp;
	
	fTmp = MAX_DATA_VALUE /(float)pCurve->pParam.iFactor / iPixHeight;
	y1 = pCurve->cCoor.fMaxY / fTmp;
	y2 = pCurve->cCoor.fMinY / fTmp;

	//描绘区域框
	uiColor = pPen->uiPenColor;
	pPen->uiPenColor = COLOR_BLACK;
	DrawRectangle(pPreview->Rect.lSX + x1, pPreview->Rect.lEY - y1 + 1,
				  pPreview->Rect.lSX + x2 - 1, pPreview->Rect.lEY - y2);
	pPen->uiPenColor = uiColor;
	DBG_PRINT_TIME();
	
    return iErr;
}


//绘制预览区光标
static int DrawPreviewCursor(PDISPLAY_INFO pDisplay)
{
	int iErr = 0;
	
	float fTemp;
	int x, y, iDataVal; 
    
    UINT32 uiColor;
    GUIPEN *pPen;	
	int iWidth, iHeight;

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	PPREVIEW_TRACE pPreview = pCurve->pPreview;

	DBG_INIT_TIME();
    CURVE_DBG_ENTER();
	
	iWidth = pPreview->Rect.lEX - pPreview->Rect.lSX + 1;
	iHeight = pPreview->Rect.lEY - pPreview->Rect.lSY + 1;

	//计算x，y坐标
	fTemp = (float)pCurve->cCursor.dDot.iIndex / (float)pCurve->dIn.uiCnt;
    x = pPreview->Rect.lSX + (int)(fTemp * iWidth);
	
	iDataVal = pPreview->pPixVal[x - pPreview->Rect.lSX];
    fTemp = (float)((float)iDataVal / (float)MAX_DATA_VALUE);
    y = pPreview->Rect.lEY - (fTemp * iHeight);

	//设置画笔颜色
	pPen = GetCurrPen();
	uiColor = pPen->uiPenColor;
    pPen->uiPenColor = COLOR_RED;
	
    if ((y >= pPreview->Rect.lSY) && 
    	(y <= pPreview->Rect.lEY))
    {
		DrawLine(pPreview->Rect.lSX, y, pPreview->Rect.lEX, y);
	}
	
	if ((x >= pPreview->Rect.lSX) &&
    	(x <= pPreview->Rect.lEX))
	{
    	DrawLine(x, pPreview->Rect.lSY + 1, x, pPreview->Rect.lEY);
	}
    pPen->uiPenColor = uiColor;

	DBG_PRINT_TIME();
	CURVE_DBG_EXIT(iErr);
	return iErr;
}

//绘制放大镜图标, cursorType: 1,表示标记线，2,表示事件
static int DrawMagnifier(PDISPLAY_INFO pDisplay, int cursorType)
{
	int iErr = 0;
	int x = 0;
	int y = 0;
	MARKER *pMarker;
	GUIPICTURE *pBg = NULL;
	CURVE_DBG_ENTER();
	DBG_INIT_TIME();

	PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
	UINT16 *pPixVal = pCurve->dOut.pValue;
	pMarker = &(pCurve->mMarker);

	if (cursorType == 1)//标记线
	{
		//选中的标记线作为绘制的标记
		if (pMarker->uiColor[1] == COLOR_BLUE)
		{
			x = pMarker->dDot[1].iPosX;
		}
		else if (pMarker->uiColor[2] == COLOR_BLUE)
		{
			x = pMarker->dDot[2].iPosX;
		}
	}
	else//事件
	{
		EVENT_MARKER eEventMarker = pCurve->eEventMarker;
		x = eEventMarker.dDot[0].iPosX;
	}

	int iDataVal = pPixVal[x - pDisplay->Rect.lSX];
	y = GetPosY(iDataVal, pDisplay);
	//判断边界值
	if ((y + 20) >= pDisplay->Rect.lEY)
        pBg = CreatePicture(x + 15, pDisplay->Rect.lEY - 20, 20, 20, BmpFileDirectory "bg_scaleBtn.bmp");
    else if ((y + 20) < pDisplay->Rect.lSY)
        pBg = CreatePicture(x + 15, pDisplay->Rect.lSY + 20, 20, 20, BmpFileDirectory "bg_scaleBtn.bmp");
    else
        pBg = CreatePicture(x + 15, y + 20, 20, 20, BmpFileDirectory "bg_scaleBtn.bmp");
    DisplayPicture(pBg);
	DestroyPicture(&pBg);

	DBG_PRINT_TIME();
	CURVE_DBG_EXIT(iErr);
	return iErr;
}

//绘制显示对象所有元素
static int DrawCurveAll(PDISPLAY_INFO pDisplay)
{
    int iErr = 0;
	CURVE_DRAW_FLGS DrawFlags;

    CURVE_DBG_ENTER();
	DBG_INIT_TIME();

	//检查参数
	if (NULL == pDisplay)
	{
		iErr = -1;
		CURVE_DBG_EXIT(iErr);
		return iErr;
	}
	
	//获得曲线元素绘制标识
	MutexLock(&pDrawFlgs->mMutex);
	memcpy(&DrawFlags, pDrawFlgs, sizeof(CURVE_DRAW_FLGS));
	MutexUnlock(&pDrawFlgs->mMutex);

	//曲线背景
	if (DrawFlags.uiBackground)
	{	
 		DisplayPicture(pDisplay->pBg);
		//预览区背景
		if (DrawFlags.uiPreview)
		{
			DisplayPicture(pDisplay->pPreBg);
		}
	}
	
	//如果曲线有效，则绘制曲线元素
	if (pDisplay->pCurve[pDisplay->iCurCurve]->uiIsVaild)
	{
		//坐标系
		if (DrawFlags.uiCoord)
		{
			DrawCoor(pDisplay);
		}

		//波形曲线
		if (DrawFlags.uiCurve)
		{
	    	DrawCurve(pDisplay);
		}

		//光标
		if (DrawFlags.uiCursor)
		{
			DrawCursor(pDisplay);
		}

		//绘制光标距离
		if (DrawFlags.uiCursorDist)
		{
			DrawCusorDistance(pDisplay, 1);
		}

		//事件标记线
		if (DrawFlags.uiEventMarker)
		{
		    DrawEventMarker(pDisplay);
		}
        
		//标记线
		if (DrawFlags.uiMarker)
		{
			DrawMarker(pDisplay);
			//显示放大镜图标
			if (DrawFlags.uiMagnifier)
			{
				DrawMagnifier(pDisplay, 1);
			}
		}

		//事件点
		if (DrawFlags.uiEvent)
		{
			DrawEvent(pDisplay, 1);
			//显示放大镜图标
			if (DrawFlags.uiMagnifier)
			{
				DrawMagnifier(pDisplay, 2);
			}
		}
		
		//参考起始点
		if (DrawFlags.uiReference)
		{
			DrawStartRef(pDisplay);
		}

		//选择框
		if (DrawFlags.uiScratch)
		{
			DrawScratchRect(pDisplay);
		}

		//有曲线则绘制标识
		if (DrawFlags.uiCurve)
		{
			//DisplayPicture(pDisplay->pCurveID[pDisplay->iCurCurve]);
		}

		//文件名
		if (DrawFlags.uiFileName)
		{
			DrawFilename(pDisplay);
		}
		
	    //是否通过
		if (DrawFlags.uiIsPass)
		{
			DisplayPicture(pDisplay->pPicIsPass[pDisplay->iCurCurve]);
			// DisplayLabel(pDisplay->pLabIsPass[pDisplay->iCurCurve]);
		}

		//预览区所有元素
		if (DrawFlags.uiPreview)
		{
			//曲线
			DrawPreviewCurve(pDisplay);
			//光标
			DrawPreviewCursor(pDisplay);
		}
	}
	
	//其他附加信息的绘制
	//测量时间
	if (-1 != pDisplay->iMeasTime)
	{
		DrawMeasTime(pDisplay);
	}
	else//未测试状态下显示文件名
	{
		DrawFilename(pDisplay);
	}

	//附加信息
	if (pDisplay->pAttach)
	{
		pDisplay->pAttach->pBgAddtion->fnDisplay((void *)pDisplay->pAttach->pBgAddtion);
		pDisplay->pAttach->pLblAddtion->fnDisplay((void *)pDisplay->pAttach->pLblAddtion);
	}

	DBG_PRINT_TIME();
	CURVE_DBG_EXIT(iErr);
    return iErr;
}

//设置显示对象状态
static void SetDisplayStatus(PDISPLAY_INFO pDisplay, DISPLAY_STATUS enStatus)
{
	MutexLock(&mStatusMutex);
	pDisplay->enStatus = enStatus;
	MutexUnlock(&mStatusMutex);
}


//初始化当前曲线显示结构体，内存分配和成员变量的初始化
int InitDisplayInfo(PDISPLAY_INFO pDisplay)
{
	//返回值
	int iErr = 0, i;
	const UINT32 uiColor[CURVE_MAX] = {COLOR_BLACK, COLOR_BLACK};
		
    CURVE_DBG_ENTER();
	if (NULL == pDisplay)
	{
	   	iErr = -1;
		CURVE_DBG_EXIT(iErr);
		return iErr;
	}

	//初始化锁
	InitMutex(&pDisplay->mMutex, NULL);
	InitMutex(&pDisplay->mStatusMutex, NULL);

	//基本信息
	pDisplay->uiDrawCurve = 1;
	pDisplay->enStatus = EN_STATUS_FREE;
	pDisplay->pAttach = NULL;
	pDisplay->iMeasTime = -1;
	pDisplay->uiReadSor = 0;
    pDisplay->uiEndOfDaq = 0;
	
	//图片背景等GUI资源
	pDisplay->pCurveID[0] = CreatePicture(598, 44, 82, 27, 
										  BmpFileDirectory"bg_otdr_curwave0.bmp");
	pDisplay->pCurveID[1] = CreatePicture(598, 44, 82, 27, 
										  BmpFileDirectory"bg_otdr_curwave1.bmp");
										  
	pDisplay->pPicIsPass[0] = CreatePicture(589, 87, 40, 28, 
										  BmpFileDirectory"otdr_pass_bg.bmp");
	pDisplay->pPicIsPass[1] = CreatePicture(589, 87, 40, 28, 
										  BmpFileDirectory"otdr_pass_bg.bmp");

	pDisplay->pLabIsPass[0] = CreateLabel(549+30, 87+6, 50, 16, NULL);
	pDisplay->pLabIsPass[1] = CreateLabel(549+30, 87+6, 50, 16, NULL);
	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pLabIsPass[0]);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pLabIsPass[1]);
    pDisplay->pLargeBg = CreatePicture(0, 82, 640, 296, BmpFileDirectory"curve_l_bg.bmp");
	pDisplay->pSmallBg = CreatePicture(0, 82, 640, 223, BmpFileDirectory"curve_s_bg.bmp");
	pDisplay->pPreBgShot = CreatePicture(PRE_COOR_SX, PRE_COOR_SY, 
										 PRE_COOR_EX - PRE_COOR_SX + 1, 
										 PRE_COOR_EY - PRE_COOR_SY + 1, 
		     							 BmpFileDirectory"screen_shot.bmp");
	pDisplay->pPreBgNoml = CreatePicture(PRE_COOR_SX, PRE_COOR_SY, 
										 PRE_COOR_EX - PRE_COOR_SX + 1, 
										 PRE_COOR_EY - PRE_COOR_SY + 1, 
										 BmpFileDirectory"bg_pre_trace.bmp");
	//文件名
	pDisplay->pPicFile = CreatePicture(190, 0, 241, 40, BmpFileDirectory "bg_otdr_fileName.bmp");
	pDisplay->pLblFile = CreateLabel(190, 12, 241, 16, NULL);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pLblFile);
	pDisplay->pLblDot = CreateLabel(401 + 265, 24, 24, 24, NULL);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pDisplay->pLblDot);

	//坐标系GUI资源
	for (i = 0; i < DIV_NUM_X; ++i)
	{
		pDisplay->pSCoorX[i] = CreateLabel(16 + i*98, 286, 100, 16, NULL);
		pDisplay->pLCoorX[i] = CreateLabel(16 + i*98, 356, 100, 16, NULL);
	}
	pDisplay->pSCoorX[i] = CreateLabel(16+i*98, 286, 24, 16, NULL);
	pDisplay->pLCoorX[i] = CreateLabel(16+i*98, 356, 24, 16, NULL); //unit

	for (i = 0; i < DIV_NUM_Y; ++i)
	{
		if (i < DIV_NUM_Y - 2)
		{
			pDisplay->pSCoorY[i] = CreateLabel(10, 111 + i * 26, 30, 16, NULL);
		}
		pDisplay->pLCoorY[i] = CreateLabel(10, 114 + i*30, 30, 16, NULL);
	}
	pDisplay->pSCoorY[DIV_NUM_Y - 2] = CreateLabel(16, 84, 20, 16, NULL);
	pDisplay->pLCoorY[i] = CreateLabel(16, 84, 20, 16, NULL);//db
	
	for (i = 0; i < DIV_NUM_X + 1; ++i)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->pLCoorX[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->pSCoorX[i]);
		if(i < DIV_NUM_X)
		{
            SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pLCoorX[i]);
            SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pSCoorX[i]);
		}
	}
	
	for (i = 0; i < DIV_NUM_Y + 1; ++i)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->pLCoorY[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pLCoorY[i]);
		if (i < DIV_NUM_Y - 1)
		{
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->pSCoorY[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pDisplay->pSCoorY[i]);
		}
	}

	//画线区域初始化
	pDisplay->LargeRect.lSX = COOR_B_SX;	
	pDisplay->LargeRect.lSY = COOR_B_SY;
	pDisplay->LargeRect.lEX = COOR_B_EX;
	pDisplay->LargeRect.lEY = COOR_B_EY;
	pDisplay->SmallRect.lSX = COOR_S_SX;	
	pDisplay->SmallRect.lSY = COOR_S_SY;
	pDisplay->SmallRect.lEX = COOR_S_EX;
	pDisplay->SmallRect.lEY = COOR_S_EY;

	//所有曲线初始化
	for (i = 0; i < CURVE_MAX; i++)
	{
		if (InitCurve(&(pDisplay->pCurve[i])))
		{
			iErr = -2;
			break;
		}

		pDisplay->pCurve[i]->dOut.uiColor = uiColor[i];
	}

	//画线标志
	if (!iErr)
	{
		InitDrawFlgs();
	}
	
	//画线线程队列
	if (!iErr)
	{
		InitCurveQueue(128);
	}
	
	CURVE_DBG_EXIT(iErr);
	return iErr;
}


//释放显示部分资源
void ReleaseDisplayInfo(PDISPLAY_INFO pDisplay)
{

}


//曲线画图线程
void *DrawCurveThread(void * pThreadArg)
{
	//返回值，和错误代码
	static int iErr = 0;
	static int iExit = 0;
	static PDISPLAY_INFO pDisplay;

	static QUEUE_MSG DrawMsg;
	static int iNeedFrameCopy = 0;
	pDisplay = pOtdrTopSettings->pDisplayInfo;

    CURVE_DBG_ENTER();
	
	PowerTableCreat();//added by  2016.2.29
	//主循环
	while (!iExit)
	{
		DBG_INIT_TIME();
		
		DrawMsg.enType = 0;
		DrawMsg.pContent = NULL;
		iNeedFrameCopy = 0;

		//读取画线命令
		if (ReadCurveQueue(&DrawMsg))
		{
			MsecSleep(20);
			continue;
		}

		//设置状态为忙
		SetDisplayStatus(pDisplay, EN_STATUS_BUSY);
		CURVE_DEBUG("Read a CMD\n");
		//加锁
		MutexLock(&(pDisplay->mMutex));
        CURVE_DEBUG("Mutex locked\n");

        //如果是SOLA模式，则不计算参数，不绘制波形
        if (pDisplay->uiDrawCurve == EN_SOLA_MODE)
        {
            // do Nothing
        }
        else 
        {
    		//更新参数	
    		if (pDisplay->CtlPara.uiNeedUpdate)
    		{
    			UpdateDisplayParam(pDisplay);
    			pDisplay->CtlPara.uiNeedUpdate = 0;
    		}
    		
    		//处理命令并计算参数
    		HandleDisplayCmd(pDisplay, &DrawMsg);
    		
    		// 正常模式则更新参数并画波形
    		if (pDisplay->uiDrawCurve == EN_NORMAL_MODE)
    		{
    			//更新显示对象所有元素
    			UpdateCurveAll(pDisplay);
                CURVE_DEBUG("UpdateCurveAll Over\n");

    			DrawCurveAll(pDisplay);
                CURVE_DEBUG("DrawCurveAll Over\n");

    			iNeedFrameCopy = 1;
                CURVE_DEBUG("iNeedFrameCopy Over\n");
    		}
            else //EN_NOT_DRAW_CURVE_MODE
            {
                // do Nothing
            }
        }
        
		//释放消息资源
		free(DrawMsg.pContent);
        CURVE_DEBUG("free(DrawMsg.pContent) Over\n");
		
		//解锁
		MutexUnlock(&(pDisplay->mMutex));
        CURVE_DEBUG("Mutex unlocked\n");

        //设置状态为空闲
		SetDisplayStatus(pDisplay, EN_STATUS_FREE);
		CURVE_DEBUG("CMD Handle Over\n");

		//刷新framebuffer
		if (iNeedFrameCopy)
		{
			SyncCurrFbmap();
		}

        //判断是否读取SOR
		if (pDisplay->uiReadSor)
		{
			WriteOTDRMsgQueue(ENUM_OTDRMSG_READSOR);
			pDisplay->uiReadSor = 0;
		}

        //判断是否采集结束
        if (pDisplay->uiEndOfDaq)
        {
            WriteOTDRMsgQueue(ENUM_OTDRMSG_STOP);
            pDisplay->uiEndOfDaq = 0;
        }

		DBG_PRINT_TIME();
	}

	//释放资源
	ReleaseDisplayInfo(pDisplay);
	
	CURVE_DBG_EXIT(iErr);
	ThreadExit(&iErr);
	return &iErr;
}

//设置背景绘制标志
void SetDrawBg(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiBackground = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置曲线绘制标志
void SetDrawCurveData(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiCurve = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置光标绘制标志
void SetDrawCursor(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiCursor = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置光标距离绘制标志
void SetDrawCursorDist(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiCursorDist = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置标记绘制标志
void SetDrawMarker(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiMarker = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置标记绘制标志
void SetDrawEventMarker(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiEventMarker = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置事件绘制标志
void SetDrawEvent(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiEvent = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置参考起点绘制标志
void SetDrawReference(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiReference = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置预览图绘制标志
void SetDrawScratch(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiScratch = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置预览图绘制标志
void SetDrawPreview(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiPreview = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置绘制坐标系标志
void SetDrawCoord(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiCoord = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置绘制坐标系标志
void SetDrawFileName(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiFileName = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置绘制坐标系标志
void SetDrawIsPass(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiIsPass = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}

//设置放大镜图标绘制标志
void SetDrawMagnifier(UINT32 uiFlg)
{
	MutexLock(&pDrawFlgs->mMutex);
	pDrawFlgs->uiMagnifier = uiFlg;
	MutexUnlock(&pDrawFlgs->mMutex);
}
//设置画线总开关
void SetCurveThreadMode(int iFlag)
{
	MutexLock(&pOtdrTopSettings->pDisplayInfo->mMutex);
	pOtdrTopSettings->pDisplayInfo->uiDrawCurve = iFlag;
	MutexUnlock(&pOtdrTopSettings->pDisplayInfo->mMutex);
}

//获得显示对象状态
DISPLAY_STATUS  CurrDisplayStatus()
{
	DISPLAY_STATUS enStatus;
	
	MutexLock(&mStatusMutex);
	enStatus = pOtdrTopSettings->pDisplayInfo->enStatus;
	MutexUnlock(&mStatusMutex);
	
	return enStatus;
}

//读SOR 标志
void SetDisplaySor(PDISPLAY_INFO pDisplay, UINT32 iFlag)
{
	MutexLock(&pDisplay->mMutex);
	pDisplay->uiReadSor = iFlag;
	MutexUnlock(&pDisplay->mMutex);
}


//当前的是否曲线有效
UINT32 CurveIsVaild(int iPos, PDISPLAY_INFO pDisplay)
{
	UINT32 uiIsVaild = 0;
	
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	uiIsVaild = pDisplay->pCurve[iPos]->uiIsVaild;
	MutexUnlock(&pDisplay->mMutex);

	return uiIsVaild;
}

//当前的波形是否分析完成
UINT32 CurveIsAnalysised(int iPos, PDISPLAY_INFO pDisplay)
{
	UINT32 uiAnalysised = 0;
	
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	uiAnalysised = pDisplay->pCurve[iPos]->uiAnalysised;
	MutexUnlock(&pDisplay->mMutex);

	return uiAnalysised;
}

//获得曲线数量
int GetCurveNum(PDISPLAY_INFO pDisplay)
{
	int iReturn;

	MutexLock(&pDisplay->mMutex);
	iReturn = pDisplay->iCurveNum;
	MutexUnlock(&pDisplay->mMutex);

	return iReturn;
}

//设置当前曲线的指针
int SetCurrCurvePos(int iPos, PDISPLAY_INFO pDisplay)
{
	int iErr = 0;
	
	MutexLock(&pDisplay->mMutex);
	if (iPos < pDisplay->iCurveNum)
	{
		pDisplay->iCurCurve = iPos;
	}
	else
	{
		iErr = -1;
	}
	MutexUnlock(&pDisplay->mMutex);

	return iErr;
}

//获得当前波形的指针
int GetCurrCurvePos(PDISPLAY_INFO pDisplay)
{
	int iErr = 0;

	MutexLock(&pDisplay->mMutex);
	if (pDisplay->iCurveNum <= 0)
	{
		iErr = -1;
	}
	else
	{
		iErr = pDisplay->iCurCurve;
	}
	MutexUnlock(&pDisplay->mMutex);

	return iErr;
}

//设置当前曲线的指针
int NextCurve(PDISPLAY_INFO pDisplay)
{
	int iErr = 0;

	MutexLock(&pDisplay->mMutex);
	if (pDisplay->iCurveNum <= 0)
	{
		iErr = -1;
	}
	else
	{
		pDisplay->iCurCurve = (pDisplay->iCurCurve+1) % pDisplay->iCurveNum;
		iErr = pDisplay->iCurCurve;
	}
	MutexUnlock(&pDisplay->mMutex);

	return iErr;
}


//增加一条曲线
int DisplayAddCurve(PDISPLAY_INFO pDisplay, CURVE_PRARM *pPara, DATA_IN *pIn)
{
	//修改曲线数量已经当前曲线的指针	
	MutexLock(&pDisplay->mMutex);
	if (0 == pDisplay->iCurveNum)
		pDisplay->iCurCurve = 0;
	else
		pDisplay->iCurCurve += 1;
	pDisplay->iCurveNum += 1;
	MutexUnlock(&pDisplay->mMutex);

	//设置参数
	SetCurvePara(CURR_CURVE, pDisplay, pPara);
	
	//设置数据
	SetCurveData(CURR_CURVE, pDisplay, pIn);

	//根据量程初始化记线数据
	InitCurveMaker(CURR_CURVE, pDisplay);
	return 0;
}

//获得曲线参数
int GetCurvePara(int iPos, PDISPLAY_INFO pDisplay, CURVE_PRARM *pPara)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	memcpy(pPara, &pDisplay->pCurve[iPos]->pParam, sizeof(CURVE_PRARM));
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}

//设置曲线参数
int SetCurvePara(int iPos, PDISPLAY_INFO pDisplay, CURVE_PRARM *pPara)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	memcpy(&pDisplay->pCurve[iPos]->pParam, pPara, sizeof(CURVE_PRARM));
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}


//获得当前波形的数据
int GetCurveData(int iPos, PDISPLAY_INFO pDisplay, DATA_IN *pIn)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	pIn->uiCnt = pCurve->dIn.uiCnt;
	memcpy(pIn->pData, pCurve->dIn.pData, sizeof(UINT16) * pCurve->dIn.uiCnt);
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}

//设置当前波形的数据
int SetCurveData(int iPos, PDISPLAY_INFO pDisplay, DATA_IN *pIn)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	pCurve->dIn.uiCnt = pIn->uiCnt;
	memcpy(pCurve->dIn.pData, pIn->pData, sizeof(UINT16) * pIn->uiCnt);
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}


//设置文件名
int SetCurveFile(int iPos, PDISPLAY_INFO pDisplay, char *strFile)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		LOG(LOG_ERROR, "iPos is %d\n", iPos);
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	strcpy(pCurve->strFName, strFile);
	MutexUnlock(&pDisplay->mMutex);
	LOG(LOG_INFO, "ipCurve->strFName = %s\n", pCurve->strFName);

	return 0;
}

//获得文件名
int GetCurveFile(int iPos, PDISPLAY_INFO pDisplay, char *strFile)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	strcpy(strFile, pCurve->strFName);
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}

//获得曲线名
int GetCurveName(int iPos, PDISPLAY_INFO pDisplay, char *strFile)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos && pDisplay->iCurveNum)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	switch(pCurve->pParam.enWave)
	{
        case WAVELEN_1310:
    	    strcpy(strFile, "1310nm");
            break;
        case WAVELEN_1550:
    	    strcpy(strFile, "1550nm");
            break;
        case WAVELEN_1625:
    	    strcpy(strFile, "1625nm");
            break;
        default:
            break;
	}
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}

//发送光标控制消息
int SendCursorCtlMsg(CURSOR_CMD enMove, INT32 iValue)
{
	CURSOR_CTL *pCursorCtl;

	pCursorCtl = (CURSOR_CTL *)malloc(sizeof(CURSOR_CTL));
	if (NULL == pCursorCtl)
	{
		return -1;
	}
	
	pCursorCtl->enType = enMove;
	pCursorCtl->iValue = iValue;
	
	if (WriteCurveCmd(EN_CURSOR_CTL, (void *)pCursorCtl))
	{
		return -2;
	}

	return 0;
}

//发送曲线控制消息
int SendLineCtlMoveMsg(CTRL_CMD enCtlType, INT32 iPosX, INT32 iPosY)
{
	LINE_CTL *pLineCtl;

	pLineCtl = (LINE_CTL *)malloc(sizeof(LINE_CTL));
	if (NULL == pLineCtl)
	{
		return -1;
	}
	
	pLineCtl->enType = enCtlType;
	pLineCtl->uValue.sMoveVal.iPosX = iPosX;
	pLineCtl->uValue.sMoveVal.iPosY = iPosY;
	
	if (WriteCurveCmd(EN_LINE_CTL, (void *)pLineCtl))
	{
		return -2;
	}

	return 0;
}

//发送曲线缩放消息
int SendLineCtlScaleMsg(CTRL_CMD enCtlType, float fXScale, float fYScale)
{
	LINE_CTL *pLineCtl;

	pLineCtl = (LINE_CTL *)malloc(sizeof(LINE_CTL));
	if (NULL == pLineCtl)
	{
		return -1;
	}

	pLineCtl->enType = enCtlType;
	pLineCtl->uValue.sScaleVal.fXScale = fXScale;
	pLineCtl->uValue.sScaleVal.fYScale = fYScale;
	
	if (WriteCurveCmd(EN_LINE_CTL, (void *)pLineCtl))
	{
		return -2;
	}

	return 0;
}

//发送标记控制消息
int SendMakerCtlMsg(CTRL_CMD enCtlType, INT32 iValue)
{
	MAKER_CTL *pMakerCtl;

	pMakerCtl = (MAKER_CTL *)malloc(sizeof(MAKER_CTL));
	if (NULL == pMakerCtl)
	{
		return -1;
	}
	
	pMakerCtl->enType = enCtlType;
	pMakerCtl->iValue = iValue;
	
	if (WriteCurveCmd(EN_MAKER_CTL, (void *)pMakerCtl))
	{
		return -2;
	}

	return 0;
}

//事件标记线控制
int SendEventMakerCtlMsg(CURSOR_CMD enCtlType, INT32 iLeftValue, INT32 iRightValue, 
    INT32 iMaxFlag, INT32 isTouchMove)
{
    EVENT_MARKER_CTL *pEventMakerCtl;

	pEventMakerCtl = (EVENT_MARKER_CTL *)malloc(sizeof(EVENT_MARKER_CTL));
	if (NULL == pEventMakerCtl)
	{
		return -1;
	}
	
	pEventMakerCtl->enType = enCtlType;
	pEventMakerCtl->iLeftValue = iLeftValue;
    pEventMakerCtl->iRightValue = iRightValue;
    pEventMakerCtl->iMaxFlag = iMaxFlag;
    pEventMakerCtl->iTouchMove = isTouchMove;
	
	if (WriteCurveCmd(EN_EVENT_MARKER_CTL, (void *)pEventMakerCtl))
	{
		return -2;
	}

	return 0;
}

//发送附加信息命令
int SendCurveAdditionCmd(ADD_CMD enCmd, INT32 iValue)
{
	ADD_CTL *pAddCtl;

	pAddCtl = (ADD_CTL *)malloc(sizeof(ADD_CTL));
	if (NULL == pAddCtl)
	{
		return -1;
	}
	
	pAddCtl->enCmd = enCmd;
	pAddCtl->uValue.iVaule = iValue;
	
	if (WriteCurveCmd(EN_CURVE_ADD, (void *)pAddCtl))
	{
		return -2;
	}

	return 0;
}

//设置参考起始点
void SetReference(INT32 iPos, PDISPLAY_INFO pDisplay, INT32 iIndex)
{	
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return ;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	pCurve->rReference.dDot.iIndex = iIndex;
	pCurve->rReference.dDot.fDist = CalcRealDist(iIndex, pCurve->pParam.fSmpIntval);
	pCurve->rReference.uiIsVaild = 1;
	MutexUnlock(&pDisplay->mMutex);
}

//清除参考起始点
void ClearReference(INT32 iPos, PDISPLAY_INFO pDisplay)
{	
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return ;
	}
	PCURVE_INFO pCurve = pDisplay->pCurve[iPos];
	pCurve->rReference.dDot.iIndex = 0;
	pCurve->rReference.dDot.fDist = 0.0f;
	pCurve->rReference.uiIsVaild = 0;
	MutexUnlock(&pDisplay->mMutex);
}

//参考起始点是否有效
UINT32 ReferenceIsVaild(INT32 iPos, PDISPLAY_INFO pDisplay)
{
	UINT32 uiIsVaild;
	
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return 0;
	}
	uiIsVaild = pDisplay->pCurve[iPos]->rReference.uiIsVaild;
	MutexUnlock(&pDisplay->mMutex);

	return uiIsVaild;
}

//获得光标
int GetCursor(int iPos, PDISPLAY_INFO pDisplay)
{
	int iReturn = 0;

	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return 0;
	}
	iReturn = pDisplay->pCurve[iPos]->cCursor.dDot.iIndex;
	MutexUnlock(&pDisplay->mMutex);
	
	return iReturn;
}

//获取事件标记
int GetEventMarker(int iPos, PDISPLAY_INFO pDisplay)
{
	int iReturn = 0;

	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return 0;
	}
	iReturn = pDisplay->pCurve[iPos]->eEventMarker.dDot[0].iIndex;
	MutexUnlock(&pDisplay->mMutex);
	
	return iReturn;
}

//设置显示对象参数
int SetDisplayCtlPara(PDISPLAY_INFO pDisplay, DISPLAY_PARA *pPara)
{
	if (NULL == pPara)
	{
		return -1;
	}

	MutexLock(&pDisplay->mMutex);
	pDisplay->CtlPara = *pPara;
	pDisplay->CtlPara.uiNeedUpdate = 1;
	MutexUnlock(&pDisplay->mMutex);
	
	return 0;
}

//获得显示对象参数
int GetDisplayCtlPara(PDISPLAY_INFO pDisplay, DISPLAY_PARA *pPara)
{
	if (NULL == pPara)
	{
		return -1;
	}
	
	MutexLock(&pDisplay->mMutex);
	*pPara = pDisplay->CtlPara;
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}


//判断点击是否在标记线上
int TouchOnMaker(int iPos, PDISPLAY_INFO pDisplay, INT32 iTchX)
{
	int i, iCur, iMin = 30;
	int iReturn = -1;
	MARKER *pMaker;

	MutexLock(&(pDisplay->mMutex));
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
    
	pMaker =  &(pDisplay->pCurve[iPos]->mMarker);
    
	for (i = 0; i < 4; i++)
	{	
	    //不可见的标记线无法点击选中
	    if (!pMaker->uiVisible[i])
            continue;
		iCur = abs(pMaker->dDot[i].iPosX - iTchX);
		if (iCur < iMin) 
		{
			iMin = iCur;
			iReturn = i;
		}
	}
	
	MutexUnlock(&(pDisplay->mMutex));

	return iReturn;
}

//获取标记
int GetCurveMarker(int iPos, PDISPLAY_INFO pDisplay, MARKER *pMarker)
{
	PCURVE_INFO pCurve;

	if (pDisplay == NULL)
	{
		LOG(LOG_ERROR, "-pDisplay = 0x%p-", pDisplay);
		pDisplay = pOtdrTopSettings->pDisplayInfo;
	}

	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
    else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		return -1;
	}

	pCurve = pDisplay->pCurve[iPos];
	memcpy(pMarker, &pCurve->mMarker, sizeof(MARKER));
	//MutexUnlock(&pDisplay->mMutex);
    //返回值用于判断当前波形数量是否>0 由于架构原因，此处该返回值需要特殊用途
    //详见 wnd_frmotdrmeas.c Line937 wnd_frmotdrmeasfs.c Line628
	if (pDisplay->iCurveNum <= 0)
        return 1;

	return 0;
}

//事件操作函数
int GetCurveEvents(int iPos, PDISPLAY_INFO pDisplay, EVENTS_TABLE *pEvents)
{

	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	memcpy(pEvents, &(pDisplay->pCurve[iPos]->Events), sizeof(EVENTS_TABLE));
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}

//回写事件列表
int SetCurveEvents(int iPos, PDISPLAY_INFO pDisplay, EVENTS_TABLE *pEvents)
{
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	memcpy(&(pDisplay->pCurve[iPos]->Events), pEvents, sizeof(EVENTS_TABLE));
	pDisplay->pCurve[iPos]->uiAnalysised = 1;
    
	MARKER *pMaker = &(pDisplay->pCurve[iPos]->mMarker);
    pMaker->fTotalReutrnLoss = GetSpanORL(pEvents, pDisplay->pCurve[iPos]);

	MutexUnlock(&pDisplay->mMutex);

	return 0;
}


//判断点击是否在事件上
int TouchOnEvent(int iPos, PDISPLAY_INFO pDisplay, int iTchX)
{
	int iReturn = -1;

	int i, x, iTmp;
	int iMin = 6;
	EVENTS_TABLE *pEvents;

	//加锁
	MutexLock(&pDisplay->mMutex);
	if (CURR_CURVE == iPos)
	{
		iPos = pDisplay->iCurCurve;
	}
	else if (iPos < 0 || iPos >= pDisplay->iCurveNum)
	{
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}
	//获得事件表
	pEvents = &pDisplay->pCurve[iPos]->Events;
	
	//遍历事件列表，寻找离给定坐标最近的事件索引 
	for (i = 0; i < pEvents->iEventsNumber; ++i)
	{		
		x = GetPosX(pEvents->EventsInfo[i].iBegin, pDisplay);
		iTmp = abs(iTchX - x);
		if (iTmp < iMin)
		{
			iMin = iTmp;
			iReturn = i;
		}
	}
	
	//解锁
	MutexUnlock(&pDisplay->mMutex);

	return iReturn;
}

//设置otdr坐标系为普通模式
int SetCoorNomal(PDISPLAY_INFO pDisplay)
{
	if (NULL == pDisplay)
	{
		return -1;
	}

	DISPLAY_PARA CtlPara;

	GetDisplayCtlPara(pDisplay, &CtlPara);
	CtlPara.enSize = EN_SIZE_NOMAL;
	SetDisplayCtlPara(pDisplay, &CtlPara);
	
	return 0;
}

//设置otdr坐标系为伸展模式
int SetCoorLarge(PDISPLAY_INFO pDisplay)
{
	if (NULL == pDisplay)
	{
		return -1;
	}

	DISPLAY_PARA CtlPara;

	GetDisplayCtlPara(pDisplay, &CtlPara);
	CtlPara.enSize = EN_SIZE_LARGE;
	SetDisplayCtlPara(pDisplay, &CtlPara);
	
	return 0;
}

int SetSavedFlag(PDISPLAY_INFO pDisplay, UINT32 saved)
{
	//加锁
	MutexLock(&pDisplay->mMutex);
	pDisplay->uiIsSaved = saved;	
	//解锁
	MutexUnlock(&pDisplay->mMutex);
    return 0;
}

UINT32 IsCurveSaved(PDISPLAY_INFO pDisplay)
{
    UINT32 saved = 0;
	//加锁
	MutexLock(&pDisplay->mMutex);
	saved = pDisplay->uiIsSaved;	
	//解锁
	MutexUnlock(&pDisplay->mMutex);
    return saved;
}

//储存中清除指定的波形
int ClearSaveOtdrCurve(int iPos)
{
	PCURVE_INFO pCurve, pNext;
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;

	//加锁
	MutexLock(&pDisplay->mMutex);
	
	//判断是否有波形
	if ((iPos > (pDisplay->iCurveNum - 1)) ||
		(pDisplay->iCurveNum <= 0))
	{	
		MutexUnlock(&pDisplay->mMutex);
		return -1;
	}

	//清除当前波形
	ClearCurve(pDisplay->pCurve[iPos]);
        
    pDisplay->pCurve[iPos]->mMarker.enPitchOn = EN_PATCH_ALL;
    pDisplay->pCurve[iPos]->mMarker.uiColor[0] = COLOR_BLUE;
	pDisplay->pCurve[iPos]->mMarker.uiColor[1] = COLOR_BLUE;
	pDisplay->pCurve[iPos]->mMarker.uiColor[2] = COLOR_BLUE;
	pDisplay->pCurve[iPos]->mMarker.uiColor[3] = COLOR_BLUE;
	
	//依次拷贝波形数据(所有波形前移)
	for (; iPos < pDisplay->iCurveNum - 1; ++iPos)
	{
		pCurve = pDisplay->pCurve[iPos];
		pNext = pDisplay->pCurve[iPos + 1];

		//拷贝数据
		memcpy(pCurve->dIn.pData, pNext->dIn.pData, MAX_DATA * sizeof(UINT16));
		memcpy(&pCurve->Events, &pNext->Events, sizeof(EVENTS_TABLE));
		strcpy(pCurve->strFName, pNext->strFName);

		pCurve->dIn.uiCnt = pNext->dIn.uiCnt;

		CopyDout(&pCurve->dOut, &pNext->dOut);

		//参数
		pCurve->pParam = pNext->pParam;
		pCurve->uiIsVaild  = pNext->uiIsVaild;
		pCurve->uiAnalysised = pNext->uiAnalysised;

		pNext->uiIsVaild = 0;
		pNext->uiAnalysised = 0;
		
		//附加数据
		pCurve->rReference = pNext->rReference;
		pCurve->cCursor = pNext->cCursor;
		pCurve->cCoor = pNext->cCoor;
		pCurve->mMarker = pNext->mMarker;

		memcpy(pCurve->pPreview->pPixVal, pNext->pPreview->pPixVal, 160 * sizeof(UINT16));
		memcpy(pCurve->pPreview->pPixIndex, pNext->pPreview->pPixIndex, 160 * sizeof(UINT32));
		pCurve->pPreview->iPixValNum = pNext->pPreview->iPixValNum;
		pCurve->pPreview->Rect = pNext->pPreview->Rect;
		pCurve->pPreview->uiColor = pNext->pPreview->uiColor;
		pCurve->pPreview->uiNeedUpdate = 1;
	}

	//pDisplay->xScale = 1.0f;
	//pDisplay->yScale = 1.0f;
	//pDisplay->xShift = 0;
	//pDisplay->yShift = 0;
	
	//波形数量减少1
	pDisplay->iCurveNum --;
	pDisplay->iCurCurve = 0;
	
	//解锁
	MutexUnlock(&pDisplay->mMutex);

	return 0;
}

//清除所有的波形
int ClearAllCurve(void)
{
	int i;

	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	
	MutexLock(&pDisplay->mMutex);
	for (i = 0; i < pDisplay->iCurveNum; i++)
	{
		ClearCurve(pDisplay->pCurve[i]);
	}
	pDisplay->iCurveNum = 0;
    pDisplay->uiIsSaved = 0;
	pDisplay->iCurCurve = 0;
	//pDisplay->xScale = 1.0f;
	//pDisplay->yScale = 1.0f;
	//pDisplay->xShift = 0;
	//pDisplay->yShift = 0;
	MutexUnlock(&pDisplay->mMutex);	

	return 0;
}


//设置参考起始点
int SetStartRef(float *pInSignal, int iSigLen, int iBlind, int iDIndex,
				EVENTS_TABLE *pEvents, PDISPLAY_INFO pDisplay)
{
	int iTmp1;
	CURVE_PRARM Param;

	//没有事件
	if (pEvents->iEventsNumber <= 0 || pEvents->iLaunchFiberIndex > 0 || pEvents->iRecvFiberIndex > 0)
	{
		return -2;
	}

	//参考起始点必须在起终点事件区间内
	iTmp1 = pEvents->iEventsNumber;
	if ((iDIndex <= pEvents->EventsInfo[0].iBegin) ||
		(iDIndex >= pEvents->EventsInfo[iTmp1-1].iBegin))
	{
		return -3;
	}
	
	//如果参考起始点一设置 则 删除原先的参考起始点
	if (ReferenceIsVaild(CURR_CURVE, pDisplay))
	{
		ClearStartRef(pInSignal, iSigLen, pEvents, pDisplay);
	}

	//计算参考起始点的位置
	//for (iTmp2 = 1; iTmp2 < pEvents->iEventsNumber; ++iTmp2)
	//{
	//	if (pEvents->EventsInfo[iTmp2].iBegin >= iDIndex)
	//	{
	//		break;
	//	}
	//}

	//获取当前曲线参数
	GetCurvePara(CURR_CURVE, pDisplay, &Param);
	
	//删除参考起始点前除起点外的事件点
	//while (--iTmp2)
	//{
	//	DelEventsTable(pInSignal, iSigLen, pEvents, 1, Param.fSmpRatio);
	//}
    int i, index = -1;
    for (i=0; i<pEvents->iEventsNumber; i++)
	{
		if ((pEvents->EventsInfo[i].iBegin <= iDIndex)
			&& (pEvents->EventsInfo[i].iEnd >= iDIndex))
		{
		    index = i;
		}
	}
	
	//将参考起始点作为新事件插入事件表中
	pEvents->iIsNewEvent = 0;
	if(index < 0)
	{
	    index = AddEventsTable(pInSignal, iSigLen, pEvents, iBlind, iDIndex, Param.fSmpRatio);
	    pEvents->iIsNewEvent = 1;
	}
    if (index > 0)
	    pEvents->iEventsStart = index;
    else 
        pEvents->iEventsStart = 0;

	//设置参考起始点
	SetReference(CURR_CURVE, pDisplay, iDIndex);
	SetCurveEvents(CURR_CURVE, pDisplay, pEvents);

	return 0;
}

//清除参考起始点
int ClearStartRef(float *pInSignal, int iSigLen, EVENTS_TABLE *pEvents,	
				  PDISPLAY_INFO pDisplay)
{
	CURVE_PRARM Param;

	//未设置参考起始点
	if ((!ReferenceIsVaild(CURR_CURVE, pDisplay)) && 
		(0 == pEvents->iEventsStart))
	{
		return -1;
	}

	//清除参考起始点
	ClearReference(CURR_CURVE, pDisplay);

	//获取当前曲线参数
	GetCurvePara(CURR_CURVE, pDisplay, &Param);
    
	//删除参考起始点的事件
	int eventStartIndex = pEvents->iEventsStart;
    pEvents->iEventsStart = 0;
    if(pEvents->iIsNewEvent == 1)
    {
    	if(DelEventsTable(pInSignal, iSigLen, pEvents, eventStartIndex, Param.fSmpRatio))
    	{
            pEvents->iEventsStart = eventStartIndex;
    	}
	}

	SetCurveEvents(CURR_CURVE, pDisplay, pEvents);

	return 0;
}
/***
  * 功能：
        返回不同单位的距离
  * 参数：
        1.int iFlag:        距离单位的标志位(0:m/km, 1:miles, 2: ft/kft)
        2.float fDist:      距离(默认单位是m)
  * 返回：
        转换单位之后的距离
  * 备注：
***/
float UnitConverter(int iFlag, float fDist)
{
    switch(iFlag)
    {
        case 0:
            if(fabsf(fDist) >= 1000.0f)
            {
                fDist = fDist / 1000.0f;
            }
            break;
        case 1:
            fDist = fDist / 1609.35;
            break;
        case 2:
            fDist = 3.28 *fDist;
            if(fabsf(fDist) >= 1000.0f)
            {
                fDist = fDist / 1000.0f;
            }
            break;
        default:
            break;
    }
    return fDist;
}
/***
  * 功能：
        提供转换之后的单位名称
  * 参数：
        1.int iFlag:        距离单位的标志位(0:m/km, 1:miles, 2: ft/kft)
        2.float fEnd:       距离(转换单位之前的距离)
        3.char* cBuf:       提供转换之后的单位名称
  * 返回：
        
  * 备注：
***/
void UnitConverterName(int iFlag, float fEnd, char* cBuf)
{
    switch(iFlag)
    {
        case UNIT_M:
        case UNIT_KM:
            sprintf(cBuf, "%s", "Km");
            break;
        case UNIT_MI:
            sprintf(cBuf, "%s", "mi");
            break;
        case UNIT_FT:
        case UNIT_KFT:
            sprintf(cBuf, "%s", "kft");
            break;
        default:
            break;
    }
}

//根据数据点数计算真实距离
inline float CalcRealDist(int iInputValue, float fSmpIntval)
{
	return (float)iInputValue * fSmpIntval;
}

//更新通过/不通过标志
void changeIsPassUI(int isPass, int CurveIndex)
{
    PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
    GUICHAR *pStr = NULL;
    
    if(isPass)
    {
    	pStr = GetCurrLanguageText(OTDR_LBL_PASS);
        SetPictureBitmap(BmpFileDirectory"otdr_pass_bg.bmp", pDisplay->pPicIsPass[CurveIndex]);
        SetLabelText(pStr, pDisplay->pLabIsPass[CurveIndex]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->pLabIsPass[CurveIndex]);
    }
    else
    {
    	pStr = GetCurrLanguageText(OTDR_LBL_FAIL);
        SetPictureBitmap(BmpFileDirectory"otdr_nopass_bg.bmp", pDisplay->pPicIsPass[CurveIndex]);
        SetLabelText(pStr, pDisplay->pLabIsPass[CurveIndex]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDisplay->pLabIsPass[CurveIndex]);
    }

    GuiMemFree(pStr);
}

//获得当前曲线测量时间 成功返回0. 失败返回非0
int getCurveMeasureDate(char *date, char *time, int dateFormat)
{
    if(!date && !time)
    {
        return -1;
    }

    if(pOtdrTopSettings->pDisplayInfo->iCurveNum == 0)
    {
        return -2;
    }
    
	struct tm *newtime;
	
	newtime = localtime(&(pOtdrTopSettings->pDisplayInfo->pCurve[0]->pParam.ulDTS));
	
	if(date)
	{
    	if(dateFormat == 0)
        {
    	    strftime(date, 128, "%d/%m/%Y", newtime);
        }
        else if(dateFormat == 1)
        {
            strftime(date, 128, "%m/%d/%Y", newtime);
        }
        else
        {
            strftime(date, 128, "%Y/%m/%d", newtime);
        }
	}

	if(time)
	{
	    strftime(time, 128, "%T", newtime);
	}

	return 0;
}
