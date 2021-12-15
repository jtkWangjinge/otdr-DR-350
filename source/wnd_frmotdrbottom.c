/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrbottom.c  
* 摘    要：  实现旋钮控件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2015-05-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmotdrbottom.h"
#include "guifont.h"
#include "app_frmotdr.h"
#include "app_unitconverter.h"

/*******************************************************************************
***                    定义wnd_frmotdrbottom.c所使用的宏                     ***
*******************************************************************************/
#define SMALL_RAD1		51 			//knob外半径
#define SMALL_RAD2     	10			//knob内半径
#define KNOB_X_1		219			//圆1的中心横坐标
#define KNOB_Y_1		402			//圆1的中心纵坐标
#define KNOB_X_2		407			//圆2的中心横坐标
#define KNOB_Y_2		402			//圆2的中心纵坐标
#define KNOB_X_3		597			//圆3的中心横坐标
#define KNOB_Y_3		402			//圆4的中心纵坐标
#define PI 				3.14159f	//圆周率

//获得knob选项图片文件 结果存储在result中 必须保证result空间有效
#define GetKnobItemBmpFile(result, size, name, state) \
	sprintf(result, BmpFileDirectory"%s_%s_%s.bmp", size, name, state)

/*******************************************************************************
***                      控件所引用的外部系统结构体变量                      ***
*******************************************************************************/
extern POTDR_TOP_SETTINGS pOtdrTopSettings;
static int iUnitConverterFlag;
/*******************************************************************************
***              定义wnd_frmotdrbottom.c所需要的静态全局变量                 ***
*******************************************************************************/
//OTDR量程
static char *KnobStr11[11] = {
	"auto",
	"1.3",
	"2.5",
	"5",
	"10",
	"20",
	"40",
	"80",
	"120",
	"160",
	"260",
};
static char *KnobStr111[11] = {
	"auto1",
	"0.81",
	"1.55",
	"3.11",
	"6.22",
	"12.4",
	"24.8",
	"49.6",
	"74.6",
	"99.4",
	"161.6",
};
static char *KnobStr1111[11] = {
	"auto2",
	"4.26",
	"8.2",
	"16.4",
	"32.8",
	"65.6",
	"131.2",
	"262.4",
	"393.6",
	"524.8",
	"852.8",
};
//OTDR脉冲宽度
#ifdef MINI2
static char *KnobStr13[13] = {
	"auto",
	"5ns",
	"10ns",
	"20ns",
	"30ns",
	"50ns",
	"100ns",
	"200ns",
	"500ns",
	"1us",
	"2us",
	"10us",
	"20us",
};
#else
static char *KnobStr12[12] = {
	"auto",
	"5ns",
	"10ns",
	"20ns",
	"50ns",
	"100ns",
	"200ns",
	"500ns",
	"1us",
	"2us",
	"10us",
	"20us",
};
#endif
//OTDR平均时间
static char *KnobStr9[9] = {
	"auto",
	"5",
	"15",
	"30",
	"60",
	"90",
	"120",
	"180",
	"rt",
};

static int iDist = 0;
static int iPulse = 0;
static int iTime = 0;
/*******************************************************************************
***               声明wnd_frmotdrbottom.c内部使用函数                        ***
*******************************************************************************/
//按钮事件处理函数
#ifdef WAVELENGTH_SWITCH
static int Wave1310_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int Wave1550_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
#endif

static int Wave1Select_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int Wave2Select_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int KnobBtn1_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int KnobBtn1_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int KnobBtn2_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int KnobBtn2_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int KnobBtn3_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int KnobBtn3_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int KnobCenter_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int KnobCenter_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

//设置圆的焦点函数
static int SetKnobFocus(WNDOTDRBOTTOM *pOtdrBottomObj, int iKnob1, int iKnob2, int iKnob3);
//设置圆1对圆2的约束条件
static int Knob1Knob2(WNDOTDRBOTTOM *pOtdrBottomObj);
//从当前控件保存参数到系统结构中
static int SaveParaToSystem(WNDOTDRBOTTOM *pOtdrBottomObj);
//从系统结构中获得参数到当前控件中
static int GetParaFromSystem(WNDOTDRBOTTOM *pOtdrBottomObj);
//设置系统结构中的当前曲线
#ifdef WAVELENGTH_SWITCH
static int SetCurWave(WNDOTDRBOTTOM *pOtdrBottomObj);
#endif

static int Knob1ToKnob2(WNDOTDRBOTTOM *pOtdrBottomObj);

/***
  * 功能：
     	创建一个otdr界面底部参数设置界面
  * 参数：
  		无
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
WNDOTDRBOTTOM *CreateOtdrBottom(OTDRBACKFUNC BackCall)
{
	//错误标志定义
	int iErr = 0;
	int i = 0;
	//临时变量定义
	WNDOTDRBOTTOM *pOtdrBottomObj = NULL;
    iUnitConverterFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag;
	if (!iErr)
	{	//分配资源
		pOtdrBottomObj = (WNDOTDRBOTTOM *)calloc(1, sizeof(WNDOTDRBOTTOM));
		if (NULL == pOtdrBottomObj)
		{
			iErr = -1;
		}
	}

	if (!iErr) 
	{
		//获得系统参数
		GetParaFromSystem(pOtdrBottomObj);

		//初始化
		pOtdrBottomObj->WaveLength.pWaveLengthBg = CreatePicture(0, 330, 159, 150, BmpFileDirectory"otdr_wave_bg.bmp");
		pOtdrBottomObj->WaveLength.pWaveLengthStr = TransString("OTDR_LBL_WAVELENGTH");
		pOtdrBottomObj->WaveLength.pWaveLengthLbl = CreateLabel(0, 350, 130, 24, pOtdrBottomObj->WaveLength.pWaveLengthStr);
		SetLabelAlign(2, pOtdrBottomObj->WaveLength.pWaveLengthLbl);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrBottomObj->WaveLength.pWaveLengthLbl);

		if(pOtdrBottomObj->WaveLength.iWave[0] == 0)
		{
			pOtdrBottomObj->WaveLength.p1310nmPoint = CreatePicture(110, 370, 35, 40, BmpFileDirectory"otdr_point_unpress.bmp");
		}
		else
		{
			pOtdrBottomObj->WaveLength.p1310nmPoint = CreatePicture(110, 370, 35, 40, BmpFileDirectory"otdr_point_press.bmp");
		}
		if(pOtdrBottomObj->WaveLength.iWave[1] == 0)
		{
			pOtdrBottomObj->WaveLength.p1550nmPoint = CreatePicture(110, 420, 35, 40, BmpFileDirectory"otdr_point_unpress.bmp");
		}
		else
		{
			pOtdrBottomObj->WaveLength.p1550nmPoint = CreatePicture(110, 420, 35, 40, BmpFileDirectory"otdr_point_press.bmp");
		}
#ifdef WAVELENGTH_SWITCH
		if(pOtdrBottomObj->WaveLength.iCurWave == 0)
		{
			pOtdrBottomObj->WaveLength.p1310nmBg = CreatePicture(32, 390, 81, 25, BmpFileDirectory"otdr_1310_press.bmp");
			pOtdrBottomObj->WaveLength.p1550nmBg = CreatePicture(32, 427, 81, 25, BmpFileDirectory"otdr_1550_unpress.bmp");
		}
		else
		{
			pOtdrBottomObj->WaveLength.p1310nmBg = CreatePicture(32, 390, 81, 25, BmpFileDirectory"otdr_1310_unpress.bmp");
			pOtdrBottomObj->WaveLength.p1550nmBg = CreatePicture(32, 427, 81, 25, BmpFileDirectory"otdr_1550_press.bmp");
		}
#else
	    pOtdrBottomObj->WaveLength.p1310nmBg = CreatePicture(24, 370, 80, 40, BmpFileDirectory"otdr_1310_unpress.bmp");
		pOtdrBottomObj->WaveLength.p1550nmBg = CreatePicture(24, 420, 80, 40, BmpFileDirectory"otdr_1550_unpress.bmp");
#endif
		pOtdrBottomObj->WaveLength.iEnable = 1;
		
		
		//初始化圆1的控件
		pOtdrBottomObj->Knob[0].pKnobBg = CreatePicture(159, 330, 174, 150, BmpFileDirectory"otdr_dist_bg.bmp");
		pOtdrBottomObj->Knob[0].pCenter = CreateLabel(214, 382, 62, 54, NULL);
		pOtdrBottomObj->Knob[0].iItemCnt = 11;
        
		for(i = 0; i < pOtdrBottomObj->Knob[0].iItemCnt; i++)
		{
			pOtdrBottomObj->Knob[0].iEnable[i] = 1;
		}

        char strItemBmpFile[128] = {0};
        if(iUnitConverterFlag == UNIT_M || iUnitConverterFlag == UNIT_KM)
        {
            for(i = 0; i < pOtdrBottomObj->Knob[0].iItemCnt; ++i)
            {
                memset(strItemBmpFile, 0, 128);
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr11[i], "unpress");
                pOtdrBottomObj->Knob[0].pBtnItemPic[i] = CreatePicture(160, 337, 147, 133, strItemBmpFile);
            }
        }
		else if(iUnitConverterFlag == UNIT_MI || iUnitConverterFlag == UNIT_YD)
        {
            for(i = 0; i < pOtdrBottomObj->Knob[0].iItemCnt; ++i)
            {
                memset(strItemBmpFile, 0, 128);
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr111[i], "unpress");
                pOtdrBottomObj->Knob[0].pBtnItemPic[i] = CreatePicture(160, 337, 173, 133, strItemBmpFile);
            }
        } 
        else if(iUnitConverterFlag == UNIT_FT || iUnitConverterFlag == UNIT_KFT)
        {
            for(i = 0; i < pOtdrBottomObj->Knob[0].iItemCnt; ++i)
            {
                memset(strItemBmpFile, 0, 128);
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr1111[i], "unpress");
                pOtdrBottomObj->Knob[0].pBtnItemPic[i] = CreatePicture(160, 337, 173, 133, strItemBmpFile);
            }
        }

		pOtdrBottomObj->Knob[0].pBtnItem[0] = CreateLabel(227, 332, 36, 37, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[1] = CreateLabel(264, 341, 42, 32, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[2] = CreateLabel(277, 373, 46, 25, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[3] = CreateLabel(277, 399, 46, 28, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[4] = CreateLabel(272, 428, 44, 35, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[5] = CreateLabel(245, 441, 26, 39, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[6] = CreateLabel(219, 441, 26, 39, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[7] = CreateLabel(174, 428, 44, 35, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[8] = CreateLabel(167, 399, 46, 28, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[9] = CreateLabel(167, 373, 46, 25, NULL);
		pOtdrBottomObj->Knob[0].pBtnItem[10] = CreateLabel(184, 341, 42, 32, NULL);

		//初始化圆2的控件
		pOtdrBottomObj->Knob[1].pKnobBg = CreatePicture(333, 330, 174, 150, BmpFileDirectory"otdr_pulse_bg.bmp");
		pOtdrBottomObj->Knob[1].pCenter = CreateLabel(388, 382, 62, 54, NULL);
        #ifdef MINI2
        pOtdrBottomObj->Knob[1].iItemCnt = 13;
        #else
		pOtdrBottomObj->Knob[1].iItemCnt = 12;
        #endif
		for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt; i++)
		{
			pOtdrBottomObj->Knob[1].iEnable[i] = 1;
            memset(strItemBmpFile, 0, 128);
            #ifdef MINI2
            GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[i], "unpress");
            #else
            GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[i], "unpress");
            #endif
            pOtdrBottomObj->Knob[1].pBtnItemPic[i] = CreatePicture(334, 337, 162, 133, strItemBmpFile);
		}
        #ifdef MINI2
        pOtdrBottomObj->Knob[1].pBtnItem[0] = CreateLabel(334+65, 337+10-15, 40, 23, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[1] = CreateLabel(334+107, 337+19-15, 32, 19, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[2] = CreateLabel(334+122, 337+39-15, 44, 20, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[3] = CreateLabel(334+128, 337+60-15, 41, 23, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[4] = CreateLabel(334+128, 337+86-15, 41, 21, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[5] = CreateLabel(334+118, 337+109-15, 43, 19, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[6] = CreateLabel(334+88, 337+129-15, 44, 20, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[7] = CreateLabel(334+47, 337+128-15, 40, 21, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[8] = CreateLabel(334+12, 337+109-15, 46, 19, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[9] = CreateLabel(334+7, 337+88-15, 39, 20, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[10] = CreateLabel(334+5, 337+61-15, 37, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[11] = CreateLabel(334+5, 337+38-15, 42, 21, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[12] = CreateLabel(334+20, 337+15-15, 44, 22, NULL);
        #else
		pOtdrBottomObj->Knob[1].pBtnItem[0] = CreateLabel(401, 331, 32, 43, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[1] = CreateLabel(434, 335, 33, 36, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[2] = CreateLabel(449, 371, 51, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[3] = CreateLabel(449, 396, 51, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[4] = CreateLabel(449, 421, 51, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[5] = CreateLabel(438, 446, 45, 34, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[6] = CreateLabel(400, 444, 37, 36, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[7] = CreateLabel(354, 446, 45, 34, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[8] = CreateLabel(336, 421, 51, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[9] = CreateLabel(336, 396, 51, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[10] = CreateLabel(336, 371, 51, 24, NULL);
		pOtdrBottomObj->Knob[1].pBtnItem[11] = CreateLabel(367, 335, 33, 36, NULL);
        #endif
		
		//初始化圆3的控件
		pOtdrBottomObj->Knob[2].pKnobBg = CreatePicture(507, 330, 174, 150, BmpFileDirectory"otdr_time_bg.bmp");
		pOtdrBottomObj->Knob[2].pCenter = CreateLabel(560, 382, 62, 54, NULL);
		pOtdrBottomObj->Knob[2].iItemCnt = 9;
		for(i = 0; i < pOtdrBottomObj->Knob[2].iItemCnt; i++)
		{
			pOtdrBottomObj->Knob[2].iEnable[i] = 1;
            memset(strItemBmpFile, 0, 128);
            GetKnobItemBmpFile(strItemBmpFile, "time", KnobStr9[i], "unpress");
            pOtdrBottomObj->Knob[2].pBtnItemPic[i] = CreatePicture(508, 337, 145, 133, strItemBmpFile);
		}

        pOtdrBottomObj->Knob[2].pBtnItem[0] = CreateLabel(570, 331, 41, 39, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[1] = CreateLabel(612, 338, 42, 46, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[2] = CreateLabel(623, 385, 47, 35, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[3] = CreateLabel(623, 421, 47, 35, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[4] = CreateLabel(592, 441, 30, 38, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[5] = CreateLabel(561, 441, 30, 38, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[6] = CreateLabel(513, 421, 47, 34, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[7] = CreateLabel(513, 385, 47, 35, NULL);
		pOtdrBottomObj->Knob[2].pBtnItem[8] = CreateLabel(527, 338, 42, 46, NULL);
        
		//添加约束条件
		Knob1ToKnob2(pOtdrBottomObj);
	}
	pOtdrBottomObj->iEnable = 1;
	pOtdrBottomObj->BackCall = BackCall;
	pOtdrBottomObj->iKnobEnable = 1;
	
	return pOtdrBottomObj;
}


/***
  * 功能：
     	销毁OtdrBottom控件
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DestroyOtdrBottom(WNDOTDRBOTTOM **pOtdrBottomObj)
{
	int i = 0;
	int j = 0;
	//参数检测
	if (NULL == *pOtdrBottomObj)
	{
		return -1;
	}

	DestroyPicture(&((*pOtdrBottomObj)->WaveLength.pWaveLengthBg));
	DestroyPicture(&((*pOtdrBottomObj)->WaveLength.p1310nmBg));
	DestroyPicture(&((*pOtdrBottomObj)->WaveLength.p1550nmBg));
	DestroyPicture(&((*pOtdrBottomObj)->WaveLength.p1310nmPoint));
	DestroyPicture(&((*pOtdrBottomObj)->WaveLength.p1550nmPoint));
	DestroyLabel(&((*pOtdrBottomObj)->WaveLength.pWaveLengthLbl));
	GuiMemFree((*pOtdrBottomObj)->WaveLength.pWaveLengthStr);
	
	//销毁背景
	for(i = 0;i < 3; i++)
	{
		DestroyPicture(&((*pOtdrBottomObj)->Knob[i].pKnobBg));
		DestroyLabel(&((*pOtdrBottomObj)->Knob[i].pCenter));

		for (j = 0; j < (*pOtdrBottomObj)->Knob[i].iItemCnt; j++)
		{
			DestroyLabel(&((*pOtdrBottomObj)->Knob[i].pBtnItem[j]));
            DestroyPicture(&((*pOtdrBottomObj)->Knob[i].pBtnItemPic[j]));
		}
	}

	//释放空间
	free(*pOtdrBottomObj);
	*pOtdrBottomObj = NULL;
	
	return 0;
}

/***
  * 功能：
     	显示OtdrBottom控件
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DisplayOtdrBottom(WNDOTDRBOTTOM *pOtdrBottomObj)
{
	int i = 0;
	int j = 0;
	if (NULL == pOtdrBottomObj)
	{
		return -1;
	}
	if (0 == pOtdrBottomObj->iEnable)
	{
		return 0;
	}
	
	//显示背景
	DisplayPicture((pOtdrBottomObj)->WaveLength.pWaveLengthBg);
	DisplayPicture((pOtdrBottomObj)->WaveLength.p1310nmBg);
	DisplayPicture((pOtdrBottomObj)->WaveLength.p1550nmBg);
	DisplayPicture((pOtdrBottomObj)->WaveLength.p1310nmPoint);
	DisplayPicture((pOtdrBottomObj)->WaveLength.p1550nmPoint);

	for(i = 0;i < 3; i++)
	{
		DisplayPicture((pOtdrBottomObj)->Knob[i].pKnobBg);
		DisplayLabel((pOtdrBottomObj)->Knob[i].pCenter);

        GUIPICTURE *pPicObj = (pOtdrBottomObj)->Knob[i].pBtnItemPic[pOtdrBottomObj->Knob[i].iFocus];
        DisplayPicture(pPicObj);
        
		for (j = 0; j < (pOtdrBottomObj)->Knob[i].iItemCnt; j++)
		{
			DisplayLabel((pOtdrBottomObj)->Knob[i].pBtnItem[j]);
		}
	}
	
	return 0;
}


/***
  * 功能：
     	添加控件到接受消息的控件队列
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  		2.GUIWINDOW *pWnd   : 队列的所属窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int AddOtdrBottomToComp(WNDOTDRBOTTOM *pOtdrBottomObj, GUIWINDOW *pWnd)
{
	int i = 0;
	int j = 0;
	GUIMESSAGE *pMsg;

	if ((NULL == pOtdrBottomObj) || (NULL == pWnd))
	{
		return -1;
	}
	
	//添加控件
#ifdef WAVELENGTH_SWITCH
 	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrBottomObj->WaveLength.p1310nmBg, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrBottomObj->WaveLength.p1550nmBg, pWnd);
#endif
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrBottomObj->WaveLength.p1310nmPoint, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrBottomObj->WaveLength.p1550nmPoint, pWnd);
	//周围的图片
	for (i = 0; i < 3; i++)
	{
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
 				  pOtdrBottomObj->Knob[i].pCenter, pWnd);
		for(j = 0;j < pOtdrBottomObj->Knob[i].iItemCnt; j++)
		{
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
				  	  pOtdrBottomObj->Knob[i].pBtnItem[j], pWnd);
		}	
	}
	//注册消息
	pMsg = GetCurrMessage();
#ifdef WAVELENGTH_SWITCH
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1310nmBg,
               		Wave1310_Down, pOtdrBottomObj, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1550nmBg,
               		Wave1550_Down, pOtdrBottomObj, 4, pMsg);
#endif
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1310nmPoint,
               		Wave1Select_Down, pOtdrBottomObj, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1550nmPoint,
               		Wave2Select_Down, pOtdrBottomObj, 4, pMsg);

	
	for(j = 0; j < 3; j++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->Knob[j].pCenter,
            		KnobCenter_Down, pOtdrBottomObj, j, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrBottomObj->Knob[j].pCenter,
            		KnobCenter_Up, pOtdrBottomObj, j, pMsg);
	}

	//周围的图片
	for(j = 0;j < pOtdrBottomObj->Knob[0].iItemCnt; j++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->Knob[0].pBtnItem[j],
            		KnobBtn1_Down, pOtdrBottomObj, j, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrBottomObj->Knob[0].pBtnItem[j],
            		KnobBtn1_Up, pOtdrBottomObj, j, pMsg);
	}	

	for(j = 0;j < pOtdrBottomObj->Knob[1].iItemCnt; j++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->Knob[1].pBtnItem[j],
            		KnobBtn2_Down, pOtdrBottomObj, j, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrBottomObj->Knob[1].pBtnItem[j],
            		KnobBtn2_Up, pOtdrBottomObj, j, pMsg);
	}	

	for(j = 0;j < pOtdrBottomObj->Knob[2].iItemCnt; j++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->Knob[2].pBtnItem[j],
            		KnobBtn3_Down, pOtdrBottomObj, j, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrBottomObj->Knob[2].pBtnItem[j],
            		KnobBtn3_Up, pOtdrBottomObj, j, pMsg);
	}	
	
	return 0;
}

/***
  * 功能：
     	删除控件从接受消息的控件队列中
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  		2.GUIWINDOW *pWnd   : 队列的所属窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DelOtdrBottomFromComp(WNDOTDRBOTTOM *pOtdrBottomObj, GUIWINDOW *pWnd) 
{
	int i = 0;
	int j = 0;
	GUIMESSAGE *pMsg;
	pMsg = GetCurrMessage();
	if ((NULL == pOtdrBottomObj) || (NULL == pWnd))
	{
		return -1;
	}
	
	//删除控件
	DelWindowComp(pOtdrBottomObj->WaveLength.p1310nmBg, pWnd);
	DelWindowComp(pOtdrBottomObj->WaveLength.p1550nmBg, pWnd);
	DelWindowComp(pOtdrBottomObj->WaveLength.p1310nmPoint, pWnd);
	DelWindowComp(pOtdrBottomObj->WaveLength.p1550nmPoint, pWnd);
	//周围的图片
	for (i = 0; i < 3; i++)
	{
		DelWindowComp(pOtdrBottomObj->Knob[i].pCenter , pWnd);
		for(j = 0;j < pOtdrBottomObj->Knob[i].iItemCnt; j++)
		{
			DelWindowComp(pOtdrBottomObj->Knob[i].pBtnItem[j], pWnd);
		}	
	}

	//注销消息
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1310nmBg, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1550nmBg, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1310nmPoint, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->WaveLength.p1550nmPoint, pMsg);
	
	for(i = 0;i < 3; i++)
	{
		LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->Knob[i].pCenter, pMsg);
		LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrBottomObj->Knob[i].pCenter, pMsg);
		for(j = 0;j < pOtdrBottomObj->Knob[i].iItemCnt; j++)
		{
			LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrBottomObj->Knob[i].pBtnItem[j], pMsg);
			LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrBottomObj->Knob[i].pBtnItem[j], pMsg);
		}
	}
	return 0;
} 

#ifdef WAVELENGTH_SWITCH
//当前曲线选择1310nm按下处理函数
static int Wave1310_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;

	if(pOtdrBottomObj->WaveLength.iEnable == 1)
	{
		if(pOtdrBottomObj->WaveLength.iCurWave == 1)
		{
			SetPictureBitmap(BmpFileDirectory"otdr_1550_unpress.bmp", pOtdrBottomObj->WaveLength.p1550nmBg);
			SetPictureBitmap(BmpFileDirectory"otdr_1310_press.bmp", pOtdrBottomObj->WaveLength.p1310nmBg);

			pOtdrBottomObj->WaveLength.iCurWave = 0;
			SetCurWave(pOtdrBottomObj);
			GetParaFromSystem(pOtdrBottomObj);
			Knob1ToKnob2(pOtdrBottomObj);
			DisplayOtdrBottom(pOtdrBottomObj);
			
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
	
	return 0;
}


//当前曲线选择1550nm按下处理函数
static int Wave1550_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	
	if(pOtdrBottomObj->WaveLength.iEnable == 1)
	{
		if(pOtdrBottomObj->WaveLength.iCurWave == 0)
		{
			SetPictureBitmap(BmpFileDirectory"otdr_1310_unpress.bmp", pOtdrBottomObj->WaveLength.p1310nmBg);
			SetPictureBitmap(BmpFileDirectory"otdr_1550_press.bmp", pOtdrBottomObj->WaveLength.p1550nmBg);

			pOtdrBottomObj->WaveLength.iCurWave = 1;
			SetCurWave(pOtdrBottomObj);
			GetParaFromSystem(pOtdrBottomObj);
			Knob1ToKnob2(pOtdrBottomObj);
			DisplayOtdrBottom(pOtdrBottomObj);
			
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}	
		
	return 0;
}
#endif

//曲线选择1310nm按下函数
static int Wave1Select_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	
	if(pOtdrBottomObj->WaveLength.iEnable == 1)
	{
		if(pOtdrBottomObj->WaveLength.iWave[1] == 1)
		{
			if(pOtdrBottomObj->WaveLength.iWave[0] == 1)
			{
				SetPictureBitmap(BmpFileDirectory"otdr_point_unpress.bmp", pOtdrBottomObj->WaveLength.p1310nmPoint);
				pOtdrBottomObj->WaveLength.iWave[0] = 0;
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"otdr_point_press.bmp", pOtdrBottomObj->WaveLength.p1310nmPoint);
				pOtdrBottomObj->WaveLength.iWave[0] = 1;
			}

			SaveParaToSystem(pOtdrBottomObj);
			DisplayOtdrBottom(pOtdrBottomObj);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
	
	return 0;
}



//曲线选择1550nm按下函数
static int Wave2Select_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->WaveLength.iEnable == 1)
	{
		if(pOtdrBottomObj->WaveLength.iWave[0] == 1)
		{
			if(pOtdrBottomObj->WaveLength.iWave[1] == 1)
			{
				SetPictureBitmap(BmpFileDirectory"otdr_point_unpress.bmp", pOtdrBottomObj->WaveLength.p1550nmPoint);
				pOtdrBottomObj->WaveLength.iWave[1] = 0;
			}
			else
			{
				SetPictureBitmap(BmpFileDirectory"otdr_point_press.bmp", pOtdrBottomObj->WaveLength.p1550nmPoint);
				pOtdrBottomObj->WaveLength.iWave[1] = 1;
			}

			SaveParaToSystem(pOtdrBottomObj);
			DisplayOtdrBottom(pOtdrBottomObj);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
	return 0;
}

//圆1的按下函数
static int KnobBtn1_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iTouch = iOutLen;
		//改变按钮状态
		if(pOtdrBottomObj->Knob[0].iEnable[iTouch])
		{
			char strItemBmpFile[128] = {0};
            if(iUnitConverterFlag == UNIT_M || iUnitConverterFlag == UNIT_KM)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr11[iTouch], "unpress");
            }
            else if(iUnitConverterFlag == UNIT_MI || iUnitConverterFlag == UNIT_YD)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr111[iTouch], "unpress");
            }
            else
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr1111[iTouch], "unpress");
            }
            
            GUIPICTURE *pPicObj = pOtdrBottomObj->Knob[0].pBtnItemPic[iTouch];
    		SetPictureBitmap(strItemBmpFile, pPicObj);
			if(pOtdrBottomObj->Knob[0].iFocus != iTouch)
			{
				iDist = 1;
			}
			else
			{
				iDist = 0;
			}
			pOtdrBottomObj->Knob[0].iFocus = iTouch;

			SaveParaToSystem(pOtdrBottomObj);
            DisplayPicture(pOtdrBottomObj->Knob[0].pBtnItemPic[iTouch]);
			DisplayAllKnob(pOtdrBottomObj);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
	return 0;
}

//圆1的弹起函数
static int KnobBtn1_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	//获得按键参数
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int Pusle_Focus = pOtdrBottomObj->Knob[1].iFocus;
		int iTouch = iOutLen;
		if(pOtdrBottomObj->Knob[0].iEnable[iTouch])
		{
			char strItemBmpFile[128] = {0};
            if(iUnitConverterFlag == UNIT_M || iUnitConverterFlag == UNIT_KM)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr11[iTouch], "unpress");
            }
            else if(iUnitConverterFlag == UNIT_MI || iUnitConverterFlag == UNIT_YD)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr111[iTouch], "unpress");
            }
    		else
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr1111[iTouch], "unpress");
            }      
            GUIPICTURE *pPicObj = pOtdrBottomObj->Knob[0].pBtnItemPic[iTouch];
    		SetPictureBitmap(strItemBmpFile, pPicObj);
		}
		else
		{
			return 0;
		}

		if(iTouch == 0)
		{
			SetKnobFocus(pOtdrBottomObj, 0, 0, 0);
		}

		Knob1Knob2(pOtdrBottomObj);
		SaveParaToSystem(pOtdrBottomObj);
		DisplayPicture(pOtdrBottomObj->Knob[0].pBtnItemPic[iTouch]);
		DisplayAllKnob(pOtdrBottomObj);
        
		if(iDist == 0)
		{
			if(Pusle_Focus != pOtdrBottomObj->Knob[1].iFocus)
				iDist = 1;
			else
				iDist = 0;
		}
        
		if(pOtdrBottomObj->BackCall != NULL)
			pOtdrBottomObj->BackCall(iDist);
		
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
    
	return 0;
}

//圆2的按下函数
static int KnobBtn2_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iTouch = iOutLen;
		//改变按钮状态
		if(pOtdrBottomObj->Knob[1].iEnable[iTouch])
		{
			char strItemBmpFile[128] = {0};
            #ifdef MINI2
            GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[iTouch], "unpress");
            #else
			GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[iTouch], "unpress");
            #endif
			SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[iTouch]);
			if(pOtdrBottomObj->Knob[1].iFocus != iTouch)
			{
				iPulse= 1;
			}
			else
			{
				iPulse = 0;
			}
			pOtdrBottomObj->Knob[1].iFocus = iTouch;
			
			SaveParaToSystem(pOtdrBottomObj);
			DisplayPicture(pOtdrBottomObj->Knob[1].pBtnItemPic[iTouch]);
			DisplayAllKnob(pOtdrBottomObj);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
	
	return 0;
}

//圆2的弹起函数
static int KnobBtn2_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	//获得按键参数
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iTouch = iOutLen;

		if(pOtdrBottomObj->Knob[1].iEnable[iTouch])
		{
			char strItemBmpFile[128] = {0};
            #ifdef MINI2
            GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[iTouch], "unpress");
            #else
			GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[iTouch], "unpress");
            #endif
			SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[iTouch]);
		}
		else
		{
			return 0;
        
		}
		
		if(iTouch == 0)
		{
			SetKnobFocus(pOtdrBottomObj, 0, 0, 0);
			int i = 0;
			for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt;i++)
			{
				pOtdrBottomObj->Knob[1].iEnable[i] = 1;
			}

			char strItemBmpFile[128] = {0};
	
			for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt; i++)
			{
				if(pOtdrBottomObj->Knob[1].iEnable[i] == 1)
				{
				    #ifdef MINI2
                    GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[i], "unpress");
                    #else
					GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[i], "unpress");
                    #endif
					SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[i]);
				}
				else
				{
				    #ifdef MINI2
                    GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[i], "disable");
                    #else
					GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[i], "disable");
                    #endif
					SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[i]);
				}
			}
	    
		}
        
		SaveParaToSystem(pOtdrBottomObj);
 		DisplayPicture(pOtdrBottomObj->Knob[1].pBtnItemPic[iTouch]);
		DisplayAllKnob(pOtdrBottomObj);
		if(pOtdrBottomObj->BackCall != NULL)
			pOtdrBottomObj->BackCall(iPulse);
		
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}

//圆3的按下函数
static int KnobBtn3_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iTouch = iOutLen;

		//改变按钮状态
		if(pOtdrBottomObj->Knob[2].iEnable[iTouch])
		{
			char strItemBmpFile[128] = {0};
			GetKnobItemBmpFile(strItemBmpFile, "time", KnobStr9[iTouch], "unpress");
			SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[2].pBtnItemPic[iTouch]);
			if(pOtdrBottomObj->Knob[2].iFocus != iTouch)
			{
			    if(iTouch == 0 || iTouch == (pOtdrBottomObj->Knob[2].iItemCnt - 1)
			        || pOtdrBottomObj->Knob[2].iFocus == 0 
			        || pOtdrBottomObj->Knob[2].iFocus == (pOtdrBottomObj->Knob[2].iItemCnt - 1))
			    {
				    iTime = 1;
				}
				else
				{
                    iTime = 2;
				}
			}
			else
			{
				iTime = 2;
			}
			pOtdrBottomObj->Knob[2].iFocus = iTouch;
			
			SaveParaToSystem(pOtdrBottomObj);
			DisplayPicture(pOtdrBottomObj->Knob[2].pBtnItemPic[iTouch]);
			DisplayAllKnob(pOtdrBottomObj);
			//为了刷新时间旋钮
    		if(pOtdrBottomObj->BackCall != NULL)
    			pOtdrBottomObj->BackCall(3);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
	return 0;
}

//圆3的弹起函数
static int KnobBtn3_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	//获得按键参数
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iTouch = iOutLen;
		if(pOtdrBottomObj->Knob[2].iEnable[iTouch])
		{
			char strItemBmpFile[128] = {0};
			GetKnobItemBmpFile(strItemBmpFile, "time", KnobStr9[iTouch], "unpress");
			SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[2].pBtnItemPic[iTouch]);
		}
		else
			return 0;

		if(iTouch == 0)
		{
			SetKnobFocus(pOtdrBottomObj, 0, 0, 0);
			int i = 0;
			for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt;i++)
			{
				pOtdrBottomObj->Knob[1].iEnable[i] = 1;
			}

			char strItemBmpFile[128] = {0};
	
			for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt; i++)
			{
				if(pOtdrBottomObj->Knob[1].iEnable[i] == 1)
				{
				    #ifdef MINI2
                    GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[i], "unpress");
                    #else
					GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[i], "unpress");
                    #endif
					SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[i]);
				}
			}
		}
        
		SaveParaToSystem(pOtdrBottomObj);
		DisplayPicture(pOtdrBottomObj->Knob[2].pBtnItemPic[iTouch]);
		DisplayAllKnob(pOtdrBottomObj);
		if(pOtdrBottomObj->BackCall != NULL)
			pOtdrBottomObj->BackCall(iTime);
		
		RefreshScreen(__FILE__, __func__, __LINE__);
	}	
	return 0;
}

static int KnobCenter_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iKnob = iOutLen;
		int iFocus = pOtdrBottomObj->Knob[iKnob].iFocus;

		iFocus = (iFocus + 1) % (pOtdrBottomObj->Knob[iKnob].iItemCnt);
		char strItemBmpFile[128] = {0};
		if(iKnob == 0)
		{		
            if(iUnitConverterFlag == UNIT_M || iUnitConverterFlag == UNIT_KM)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr11[iFocus], "unpress");
            }
            else if(iUnitConverterFlag == UNIT_MI || iUnitConverterFlag == UNIT_YD)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr111[iFocus], "unpress");
            }
    		else
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr1111[iFocus], "unpress");
            }
		}
		else if(iKnob == 1)
		{
		    #ifdef MINI2
            GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[iFocus], "unpress");
            #else
			GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[iFocus], "unpress");
            #endif
		}
		else
		{
			GetKnobItemBmpFile(strItemBmpFile, "time", KnobStr9[iFocus], "unpress");
		}
		SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[iKnob].pBtnItemPic[iFocus]);
		pOtdrBottomObj->Knob[iKnob].iFocus = iFocus;
		
		SaveParaToSystem(pOtdrBottomObj);
		DisplayPicture(pOtdrBottomObj->Knob[iKnob].pBtnItemPic[iFocus]);
		DisplayAllKnob(pOtdrBottomObj);
	    //为了刷新时间旋钮
		if(iKnob == 2)
		{
    		if(pOtdrBottomObj->BackCall != NULL)
    			pOtdrBottomObj->BackCall(3);
		}
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}
static int KnobCenter_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen)
{
	//获得按键参数
	WNDOTDRBOTTOM *pOtdrBottomObj = (WNDOTDRBOTTOM *)pOutArg;
	if(pOtdrBottomObj->iKnobEnable == 1)
	{
		int iKnob = iOutLen;
		int iFocus = pOtdrBottomObj->Knob[iKnob].iFocus;
		int callBackPara = 1;
		char strItemBmpFile[128] = {0};
		if(iKnob == 0)
		{		
            if(iUnitConverterFlag == UNIT_M || iUnitConverterFlag == UNIT_KM)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr11[iFocus], "unpress");
            }
            else if(iUnitConverterFlag == UNIT_MI || iUnitConverterFlag == UNIT_YD)
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr111[iFocus], "unpress");
            }
    		else
            {
                GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr1111[iFocus], "unpress");
            }      
			Knob1Knob2(pOtdrBottomObj);
		}
		else if(iKnob == 1)
		{
		    #ifdef MINI2
            GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[iFocus], "unpress");
            #else
			GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[iFocus], "unpress");
            #endif
		}
		else
		{
			GetKnobItemBmpFile(strItemBmpFile, "time", KnobStr9[iFocus], "unpress");
			if(iFocus < (pOtdrBottomObj->Knob[iKnob].iItemCnt - 1) && iFocus > 0)
			{
                callBackPara = 2;
			}
		}
		SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[iKnob].pBtnItemPic[iFocus]);
		if(iFocus == 0)
		{
			SetKnobFocus(pOtdrBottomObj, 0, 0, 0);
			int i = 0;
			for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt;i++)
			{
				pOtdrBottomObj->Knob[1].iEnable[i] = 1;
			}

			char strItemBmpFile[128] = {0};
	
			for(i = 0; i < pOtdrBottomObj->Knob[1].iItemCnt; i++)
			{
				if(pOtdrBottomObj->Knob[1].iEnable[i] == 1)
				{
				    #ifdef MINI2
                    GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[i], "unpress");
                    #else
					GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[i], "unpress");
                    #endif
					SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[i]);
				}
			}
		}

		SaveParaToSystem(pOtdrBottomObj);
		DisplayPicture(pOtdrBottomObj->Knob[iKnob].pBtnItemPic[iFocus]);
		DisplayAllKnob(pOtdrBottomObj);
		if(pOtdrBottomObj->BackCall != NULL)
			pOtdrBottomObj->BackCall(callBackPara);
		
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}

//设置三个圆的焦点
static int SetKnobFocus(WNDOTDRBOTTOM *pOtdrBottomObj, int iKnob1, int iKnob2, int iKnob3)
{
	if (NULL == pOtdrBottomObj || iKnob1 < -1 || iKnob1 > 11 || \
		iKnob2 < -1 || iKnob2 > 12 || iKnob3 < -1 || iKnob3 > 9)
	{
		return -1;
	}
	
	pOtdrBottomObj->Knob[0].iFocus = iKnob1;
	pOtdrBottomObj->Knob[1].iFocus = iKnob2;
	pOtdrBottomObj->Knob[2].iFocus = iKnob3;
		
	return 0;
}

//圆1对圆2的约束条件设置
static int Knob1Knob2(WNDOTDRBOTTOM *pOtdrBottomObj)
{
	return 0;
}

//圆1对圆2的约束条件设置
static int Knob1ToKnob2(WNDOTDRBOTTOM *pOtdrBottomObj)
{
	return 0;

}
//保存参数到系统数据结构中
static int SaveParaToSystem(WNDOTDRBOTTOM *pOtdrBottomObj)
{
    int i;
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;

	pUser_Setting->iWave[0] = pOtdrBottomObj->WaveLength.iWave[0];
	pUser_Setting->iWave[1] = pOtdrBottomObj->WaveLength.iWave[1];
	pUser_Setting->enWaveCurPos = pOtdrBottomObj->WaveLength.iCurWave;

    for(i=0;i<3;i++){
        pUser_Setting->sFrontSetting[i].enFiberRange = pOtdrBottomObj->Knob[0].iFocus;
        pUser_Setting->sFrontSetting[i].enPulseTime = pOtdrBottomObj->Knob[1].iFocus;
        pUser_Setting->sFrontSetting[i].enAverageTime = pOtdrBottomObj->Knob[2].iFocus;
    }
    
	return 0;
}

//获取系统数据结构参数到当前控件中
static int GetParaFromSystem(WNDOTDRBOTTOM *pOtdrBottomObj)
{
    int i;
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;

	pOtdrBottomObj->WaveLength.iWave[0] = pUser_Setting->iWave[0];
	pOtdrBottomObj->WaveLength.iWave[1] = pUser_Setting->iWave[1];
	
	pOtdrBottomObj->WaveLength.iCurWave = pUser_Setting->enWaveCurPos;
    
	pOtdrBottomObj->Knob[0].iFocus = pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enFiberRange;
	pOtdrBottomObj->Knob[1].iFocus = pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enPulseTime;
	pOtdrBottomObj->Knob[2].iFocus = pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enAverageTime;

    for(i=0;i<3;i++){
        pUser_Setting->sFrontSetting[i].enFiberRange = pOtdrBottomObj->Knob[0].iFocus;
        pUser_Setting->sFrontSetting[i].enPulseTime = pOtdrBottomObj->Knob[1].iFocus;
        pUser_Setting->sFrontSetting[i].enAverageTime = pOtdrBottomObj->Knob[2].iFocus;
    }


	return 0;
}

#ifdef WAVELENGTH_SWITCH
//设置当前选择的曲线到系统结构中
static int SetCurWave(WNDOTDRBOTTOM *pOtdrBottomObj)
{
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;

	pUser_Setting->enWaveCurPos = pOtdrBottomObj->WaveLength.iCurWave;
	
	return 0;
}
#endif

/***
  * 功能：
     	设置控件左边波长部分使能
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  		2.int iEnable: 1--可以处理事件处理函数，0--关闭事件处理函数
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetWaveEnable(WNDOTDRBOTTOM *pOtdrBottomObj,int iEnable)
{
	if((pOtdrBottomObj == NULL) || (iEnable < 0) || (iEnable > 1))
		return -1;

	pOtdrBottomObj->WaveLength.iEnable = iEnable;

	return 0;
}


/***
  * 功能：
     	设置选中当前波长函数
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  		2.int iSelect: 0--1310nm,  1--1550nm
  * 返回：
        成功返回0，失败非0
  * 备注：
***/
int SetWaveSelect(WNDOTDRBOTTOM *pOtdrBottomObj,int iSelect)
{
	if((pOtdrBottomObj == NULL) || (iSelect < 0) || (iSelect > 1))
		return -1;
    
#ifdef WAVELENGTH_SWITCH
	if(iSelect == 0)
	{	
		SetPictureBitmap(BmpFileDirectory"otdr_1550_unpress.bmp", pOtdrBottomObj->WaveLength.p1550nmBg);
		SetPictureBitmap(BmpFileDirectory"otdr_1310_press.bmp", pOtdrBottomObj->WaveLength.p1310nmBg);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"otdr_1310_unpress.bmp", pOtdrBottomObj->WaveLength.p1310nmBg);
		SetPictureBitmap(BmpFileDirectory"otdr_1550_press.bmp", pOtdrBottomObj->WaveLength.p1550nmBg);
	}
#endif
	pOtdrBottomObj->WaveLength.iCurWave = iSelect;

	GetParaFromSystem(pOtdrBottomObj);
	
	Knob1ToKnob2(pOtdrBottomObj);
	SaveParaToSystem(pOtdrBottomObj);
	
	return 0;
}

/***
  * 功能：
     	设置OtdrBottom控件使能，也是注册或者注销消息
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  		2.int iEnbale: 1--注册，0--注销
  		3.GUIWINDOW *pWnd :当前窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
***/
int SetOtdrBottomEnable(WNDOTDRBOTTOM *pOtdrBottomObj, int iEnbale, GUIWINDOW *pWnd)
{
	//检查参数
	if (NULL == pOtdrBottomObj || iEnbale > 1 || iEnbale < 0)
	{
		return -1;
	}

	//设置使能状态
	if(iEnbale == 1)
	{
		AddOtdrBottomToComp(pOtdrBottomObj, pWnd);
	}
	else
	{
		DelOtdrBottomFromComp(pOtdrBottomObj, pWnd);
	}

	pOtdrBottomObj->iEnable = iEnbale;
	pOtdrBottomObj->iKnobEnable = iEnbale;
	
	return 0;
}

int DisplayAllKnob(WNDOTDRBOTTOM *pOtdrBottomObj)
{
	int i = 0;
    
	for(i = 0;i < 3; i++)
	{
		DisplayLabel((pOtdrBottomObj)->Knob[i].pCenter);
	}
    
	char strItemBmpFile[128] = {0};
    
    if(iUnitConverterFlag == UNIT_M || iUnitConverterFlag == UNIT_KM)
    {
        GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr11[pOtdrBottomObj->Knob[0].iFocus], "unpress");
    }
    else if(iUnitConverterFlag == UNIT_MI || iUnitConverterFlag == UNIT_YD)
    {
        GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr111[pOtdrBottomObj->Knob[0].iFocus], "unpress");
    }
	else
    {
        GetKnobItemBmpFile(strItemBmpFile, "dist", KnobStr1111[pOtdrBottomObj->Knob[0].iFocus], "unpress");
    }
    
	SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[0].pBtnItemPic[pOtdrBottomObj->Knob[0].iFocus]);

    #ifdef MINI2
    GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr13[pOtdrBottomObj->Knob[1].iFocus], "unpress");
    #else
	GetKnobItemBmpFile(strItemBmpFile, "pulse", KnobStr12[pOtdrBottomObj->Knob[1].iFocus], "unpress");
    #endif
	SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[1].pBtnItemPic[pOtdrBottomObj->Knob[1].iFocus]);

	GetKnobItemBmpFile(strItemBmpFile, "time", KnobStr9[pOtdrBottomObj->Knob[2].iFocus], "unpress");
	SetPictureBitmap(strItemBmpFile, pOtdrBottomObj->Knob[2].pBtnItemPic[pOtdrBottomObj->Knob[2].iFocus]);

	DisplayPicture(pOtdrBottomObj->Knob[0].pKnobBg);
	DisplayPicture(pOtdrBottomObj->Knob[1].pKnobBg);
	DisplayPicture(pOtdrBottomObj->Knob[2].pKnobBg);

	DisplayPicture(pOtdrBottomObj->Knob[0].pBtnItemPic[pOtdrBottomObj->Knob[0].iFocus]);
	DisplayPicture(pOtdrBottomObj->Knob[1].pBtnItemPic[pOtdrBottomObj->Knob[1].iFocus]);
	DisplayPicture(pOtdrBottomObj->Knob[2].pBtnItemPic[pOtdrBottomObj->Knob[2].iFocus]);

	return 0;
}

/***
  * 功能：
     	设置控件右边所有圆控件使能
  * 参数：
  		1.WNDOTDRBOTTOM **pOtdrBottomObj : 指向OtdrBottom控件
  		2.int iEnable: 1--可以处理事件处理函数，0--关闭事件处理函数
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int SetKnobEnable(WNDOTDRBOTTOM *pOtdrBottomObj,int iEnable)
{
	if((pOtdrBottomObj == NULL) || (iEnable < 0) || (iEnable > 1))
		return -1;

	pOtdrBottomObj->iKnobEnable = iEnable;

	return 0;
}

int RefreshKnobTime(WNDOTDRBOTTOM *pOtdrBottomObj, int elapsedTime)
{
    int i = 0;
    static int lastDisableItemCount = 0;
    int disableItemCount = 0;
    
    struct item_Area
    {
        int x;
        int y;
        int width;
        int high;
    } itemArea[] = {{622, 352, 12, 14},
                    {638, 385, 15, 15},
                    {632, 422, 21, 13},
                    {601, 445, 20, 16},
                    {563, 445, 20, 14},
                    {529, 424, 24, 15}
                   };
                   
    if(pOtdrBottomObj->iKnobEnable == 0)
    {
        return 0;
    }
    
    if((pOtdrBottomObj->Knob[2].iFocus == 0 
        || pOtdrBottomObj->Knob[2].iFocus == (pOtdrBottomObj->Knob[2].iItemCnt - 1))
        && elapsedTime != 0)
        return 0;
    
    if(elapsedTime >= 180)
    {

    }
    else if(elapsedTime >= 120)
    {
        disableItemCount = 6;
    }
    else if(elapsedTime >= 90)
    {
        disableItemCount = 5;
    }
    else if(elapsedTime >= 60)
    {
        disableItemCount = 4;
    }
    else if(elapsedTime >= 30)
    {
        disableItemCount = 3;
    }
    else if(elapsedTime >= 15)
    {
        disableItemCount = 2;
    }
    else if(elapsedTime >= 5)
    {
        disableItemCount = 1;
    }
    else if(elapsedTime == -1)
    {
        disableItemCount = lastDisableItemCount;
    }

    if(lastDisableItemCount == disableItemCount && elapsedTime != -1)
    {
        return 0;
    }
        
    lastDisableItemCount = disableItemCount;

    DisplayPicture(pOtdrBottomObj->Knob[2].pBtnItemPic[pOtdrBottomObj->Knob[2].iFocus]);
    for(i = 1; i < pOtdrBottomObj->Knob[2].iItemCnt - 2; i++)
    {
        if(i <= disableItemCount)
        {
            SetLabelEnable(0, pOtdrBottomObj->Knob[2].pBtnItem[i]);
            pOtdrBottomObj->Knob[2].iEnable[i] = 0;
            DispTransparentNew(150, 0x0, itemArea[i-1].x, itemArea[i-1].y, 
                itemArea[i-1].width, itemArea[i-1].high); //灰度
        }
        else
        {
            SetLabelEnable(1, pOtdrBottomObj->Knob[2].pBtnItem[i]);
            pOtdrBottomObj->Knob[2].iEnable[i] = 1;
        }
    }

    return 0;
}
