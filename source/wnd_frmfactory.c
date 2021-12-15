/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfac.c
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmfactory.h"
#include "wnd_frmfactorydr.h"
#include "app_installment.h"

/*******************************************************************************
*                   定义wnd_frmfac.c引用其他头文件
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmime.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmfactorlanguage.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmmaintancedate.h"
#include "wnd_frmauthorization.h"
#include "wnd_frmlinearity.h"
#include "app_systemsettings.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmfactoryfirmware.h"

#define SNInfoMaxCount 7
#define SNBITCount     20

#define MachineNameCount    1
#define MachineTypeCount    11
#define AddressCount        4
#define IndexCount          3

#define SnNumCount          100
static char sn[SNBITCount+1];
static int iInputLocation;
static char* pNumber = NULL;

extern PSerialNum pCurSN;
GUIWINDOW* CreateFactoryWindow() ;
GUIWINDOW* CreateFactoryDRWindow();
GUIWINDOW*CreateSelectLanguageWindow();
SerialNum stSn =     
{
    "I",
    "01",
    "A",
    "16",
    "01",
    "000",
    "A",
};

int iCurrentIndex[SNInfoMaxCount];
typedef enum _serial_number
{
    en_MachineName = 0,
    en_MachineType,
    en_Month,
    en_Year,
    en_Date,
    en_Number,
    en_Address
}SN;

short SNInfo[SNInfoMaxCount] = 
{
    1
};

char* machineNameStr[MachineNameCount] = 
{
    "I",
};

char* machineTypeStr[MachineTypeCount] = 
{
    "O10",
    "F215",
    "F218",
    "O10P",
    "F213",
    "F225",
    "F220",
    "F217",
    "O10PF",
    "F210",
    "O10L"
};

char* SnNumSet[SnNumCount] = 
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
};

char* cMonthSet[12] = 
{
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
    "K", "L"
};

char* addressStr[AddressCount] = 
{
    "1",
    "2",
    "3",
    "4",
};

int addressStr1[] = 
{
    1
};

/*******************************************************************************
*    
窗体frmfac中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmFactor = NULL;

/*******************************************************************************
*                   窗体frmfac 桌面上的背景控件
*******************************************************************************/
static GUIPICTURE *pFacLeftBg = NULL;
/*******************************************************************************
*                   窗体frmfac 桌面上的Hz显示控件
*******************************************************************************/
static GUICHAR *pFacStrTitle = NULL;
static GUILABEL *pFacLblTitle = NULL;

static GUICHAR *pFacStrTitle1 = NULL;
static GUILABEL *pFacLblTitle1 = NULL;

static GUICHAR *pFacStrMachineName[1] = {NULL};
static GUICHAR *pFacStrMachineType[MachineTypeCount] = {NULL};
static GUICHAR *pFacStrMonth[12] = {NULL};
static GUICHAR *pFacStrYear[99] = {NULL};
static GUICHAR *pFacStrDate[31] = {NULL};
static GUICHAR *pFacStrAddress[4] = {NULL};

static GUICHAR *pFacStrNum = NULL;
static GUILABEL *pFacLblNum= NULL;
static GUIPICTURE *pFacBgNum = NULL;

static SELECTOR *pFacSelector[6] = {NULL};

static GUILABEL *pFacLblInfo[SNInfoMaxCount] = {NULL};
static GUICHAR *pFacStrInfo[SNInfoMaxCount] = {NULL};


static GUILABEL *pFacSNLblTitle = NULL;
static GUICHAR *pFacSNStrTitle = NULL;

static GUILABEL *pFacSNLbl = NULL;
static GUICHAR *pFacSNStr = NULL;

/*******************************************************************************
*               窗体frmvls右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactorMenu = NULL;

/*******************************************************************************
*                   窗体frmvls内部函数声明
*******************************************************************************/
//初始化文本资源
static int FacTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FacTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);


static int FactorSNInput_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorSNInput_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
                            
static int FactorTab_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void FacMenuCallBack(int iOption);
static void ReCreateWindow(GUIWINDOW **pWnd);
static void RePskInputOk(void);


/*  侧边菜单控件回调函数 */
static void FacMenuCallBack(int iOption);

void printSN()
{
    printf("MachineName %s\n", stSn.MachineName);    
    printf("MachineType %s\n", stSn.MachineType);
    printf("Month       %s\n", stSn.Month);
    printf("Year        %s\n", stSn.Year);
    printf("Date        %s\n", stSn.Date);
    printf("Number      %s\n", stSn.Number);
    printf("Address     %s\n", stSn.Address);

}

int checkIndex(char* arg, char** dest, int size)
{
    int i = 0;
    for(i = 0; i < size; i++)
    {
        if( strcmp(arg, *(dest + i)) == 0)
            return i;
    }

    return -1;
}

void RefreshSN()
{
	unsigned int uiOldColor;
	GUIPEN *pPen = NULL;
	pPen = GetCurrPen();

	uiOldColor = pPen->uiPenColor;
	SetPenColor(0xffffffff, pPen);
    DrawBlock(40, 360, 350, 479);
	SetPenColor(uiOldColor, pPen);

    sprintf(sn, "%s%s%s%s%s%s%s", stSn.MachineName,
                                  stSn.Month,
                                  stSn.Year,
                                  stSn.Date,
                                  stSn.Number,                                    
                                  stSn.MachineType,
                                  stSn.Address);
    
    printf("sn : %s\n", sn);
    if(NULL != pFacSNStr)
    {
        GuiMemFree(pFacSNStr);
        pFacSNStr = NULL;
    }
    pFacSNStr = TransString(sn);
    SetLabelText(pFacSNStr, pFacSNLbl);
    DisplayLabel(pFacSNLbl);
    DisplayLabel(pFacSNLblTitle);
    
    RefreshScreen(__FILE__, __func__, __LINE__);
}

static void FacMachineNameBackFunc (int iSelected)
{
    LOG(LOG_INFO, "FacMachineNameBackFunc %d\n", iSelected);
    strcpy(stSn.MachineName, "I");
    printSN();
    RefreshSN();
}
static void FacMachineTypeBackFunc (int iSelected)
{
    LOG(LOG_INFO, "FacMachineTypeBackFunc%d\n", iSelected);
    char buf[4] = {};
    sprintf(buf, "%02d", iSelected+1);
    strcpy(stSn.MachineType, buf);
    printSN();    
    RefreshSN();

}
static void FacMonthBackFunc (int iSelected)
{
    LOG(LOG_INFO, "FacMonthBackFunc %d\n", iSelected + 1);
    strcpy(stSn.Month, cMonthSet[iSelected]);
    printSN();
    RefreshSN();
}
static void FacYearBackFunc (int iSelected)
{
    LOG(LOG_INFO, "FacYearBackFunc %d\n", iSelected + 1);
    strcpy(stSn.Year, SnNumSet[iSelected + 1]);
    printSN();
    RefreshSN();
}
static void FacDateBackFunc (int iSelected)
{
    LOG(LOG_INFO, "FacDateBackFunc %d\n", iSelected + 1);
    sprintf(stSn.Date, "%02d", iSelected + 1);
    printSN();
    RefreshSN();
}
static void FacAddressBackFunc (int iSelected)
{
    LOG(LOG_INFO, "FacAddressBackFunc %d\n", iSelected);
    sprintf(stSn.Address, "%d", iSelected + 1);
    printSN();
    RefreshSN();
}


/***
  * 功能：
        窗体frmfac的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmFacInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
    int i, index;
	unsigned int StrVlsMenu[] = 
    {
        1
    };


    //获取系统的SN
    memcpy(&stSn, pCurSN, sizeof(SerialNum));
    if (NULL != pNumber){
        strcpy(stSn.Number, pNumber);
        free(pNumber);
        pNumber = NULL;
    }
    
	//得到当前窗体对象 
    pFrmFactor = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FacTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFacLeftBg = CreatePicture(0, 0, 681, 480, BmpFileDirectory"sysset_time_bg5.bmp");
	pFacLblTitle = CreateLabel(0, 24, 100, 24, pFacStrTitle);
	pFacLblTitle1 = CreateLabel(100, 24, 100, 24, pFacStrTitle1);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacLblTitle1);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFacLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFacLblTitle1);
    
    for(i = 0; i < SNInfoMaxCount; i++)
    {
        if(i < 4)
        {
            pFacLblInfo[i] = CreateLabel(40, 120 + 60 * i, 120, 24, pFacStrInfo[i]);
        }
        else
        {
            pFacLblInfo[i] = CreateLabel(390, 120 + 60 * (i-4), 100, 24, pFacStrInfo[i]);
        }
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pFacLblInfo[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFacLblInfo[i]);
    }

    index = checkIndex(stSn.MachineName, machineNameStr, 1);
    index = index < 0 ? 0:index;
    pFacSelector[0] = CreateSelector(150, 110, 1, 
								 0, pFacStrMachineName, 
								 FacMachineNameBackFunc, 1);
    
    index = checkIndex(stSn.MachineType, SnNumSet, MachineTypeCount+1);
    index = (index < 0 || index > MachineTypeCount)? 1:index;
    pFacSelector[1] = CreateSelector(150, 170, MachineTypeCount, 
							        index-1, pFacStrMachineType, 
								 FacMachineTypeBackFunc, 1);
    
    index = checkIndex(stSn.Year, SnNumSet, SnNumCount);
    index = index < 0? 1:index;
    pFacSelector[3] = CreateSelector(450, 110, 99, 
								 index-1, pFacStrYear, 
								 FacYearBackFunc, 1);

    index = checkIndex(stSn.Month, cMonthSet, 12);
    LOG(LOG_INFO, "index Month = %d\n", index);
    index = (index < 0 || index >= 12) ? 0 : index;
    pFacSelector[2] = CreateSelector(450, 170, 12, 
								 index, pFacStrMonth, 
								 FacMonthBackFunc, 1);
    
    index = checkIndex(stSn.Date, SnNumSet, SnNumCount);
    index = (index <= 0 || index > 31) ? 0 : index - 1;
    pFacSelector[4] = CreateSelector(450, 230, 31, 
								 index, pFacStrDate, 
								 FacDateBackFunc, 1);

    pFacBgNum = CreatePicture(150, 295, 120, 25, 
                            BmpFileDirectory"otdr_input_loss_unpress.bmp");
    pFacLblNum = CreateLabel(164, 300, 92, 24, pFacStrNum);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacLblNum);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFacLblNum);

    
    index = checkIndex(stSn.Address, addressStr, AddressCount);
    LOG(LOG_INFO, "index Address= %d\n", index);
    index = index < 0? 0:index;
    pFacSelector[5] = CreateSelector(150, 230, 4, 
								 index, pFacStrAddress, 
								 FacAddressBackFunc, 1);
    
    for(i = 0; i < 6; i++)
    {
        AddSelectorToComp(pFacSelector[i], pFrmFactor);
    }

    pFacSNLblTitle = CreateLabel(40, 360, 100, 24, pFacSNStrTitle);
    pFacSNLbl = CreateLabel(150, 360, 150, 24, pFacSNStr);
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFacSNLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pFacSNLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFacSNLbl);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pFacSNLbl);


    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFactorMenu = CreateWndMenu1(factoryRank, sizeof(StrVlsMenu), StrVlsMenu,  0xffff,
								2, 0, 40, FacMenuCallBack);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactor, pFrmFactor);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFacBgNum, 
                      pFrmFactor);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFacLblTitle1, 
                      pFrmFactor);
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFactorMenu, pFrmFactor);
	
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFactorMenu, pFrmFactor);

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFacBgNum, 
                    FactorSNInput_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFacBgNum,
                    FactorSNInput_Up, NULL, 0, pMsg);
                    
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFacLblTitle1,
                    FactorTab_Down, NULL, 0, pMsg);
    for(i = 0; i < 6; i++)
    {
        LoginSelectorToMsg(pFacSelector[i], pMsg);
    }

	return iRet;
}


/***
  * 功能：
        窗体frmvls的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmFacExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    int i;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactor = (GUIWINDOW *) pWndObj;

    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactor);
	
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFacLeftBg);
	DestroyLabel(&pFacLblTitle);
	DestroyLabel(&pFacLblTitle1);
    
    for(i = 0; i < 6; i++)
    {
        DestroySelector(&(pFacSelector[i]));
    }
    
    for(i = 0; i < SNInfoMaxCount; i++)
    {
        DestroyLabel(&(pFacLblInfo[i]));
        pFacStrInfo[i] = TransString("SNInfo[i]");
    }

    DestroyPicture(&pFacBgNum);
    DestroyLabel(&pFacLblNum);


    DestroyLabel(&pFacSNLblTitle);
    DestroyLabel(&pFacSNLbl);
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFactorMenu);

    //释放文本资源
    FacTextRes_Exit(NULL, 0, NULL, 0);

    //save the sn
    memcpy(pCurSN, &stSn, sizeof(SerialNum));
    SetSettingsData((void*)pCurSN, sizeof(SerialNum), SERIAL_NUM);
    SaveSettings(SERIAL_NUM);

	//保存到分期付款结构体中
	char SN[20] = {0};
	INSTALLMENT inst;
	GetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
	GenerateSerialNumber(SN, pCurSN);
	InstSetSerialNum(&inst, SN);
    SetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
    SaveSettings(INSTALLMENT_SET);
    
    return iRet;
}

/***
  * 功能：
        窗体frmvls的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFacPaint(void *pWndObj)
{
    ///错误标志、返回值定义
    int iRet = 0;
    int i;
    //得到当前窗体对象
    pFrmFactor = (GUIWINDOW *) pWndObj;

    /***************************************************************************
    *                      显示桌面上各个区域的背景控件
    ***************************************************************************/
	DisplayPicture(pFacLeftBg);
	DisplayLabel(pFacLblTitle);
	DisplayLabel(pFacLblTitle1);
    
    for(i = 0; i < SNInfoMaxCount; i++)
    {        
        DisplayLabel(pFacLblInfo[i]);
        pFacStrInfo[i] = TransString("SNInfo[i]");
    }

    DisplaySelector(pFacSelector[0]);
    DisplaySelector(pFacSelector[1]);
    DisplaySelector(pFacSelector[2]);
    DisplaySelector(pFacSelector[3]);
    DisplaySelector(pFacSelector[4]);
    DisplayPicture(pFacBgNum);
    DisplayLabel(pFacLblNum);
    DisplaySelector(pFacSelector[5]);
    
    DisplayLabel(pFacSNLblTitle);
    DisplayLabel(pFacSNLbl);

    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFactorMenu);
	SetPowerEnable(1, 1);
    RefreshSN();

    return iRet;
}

/***
  * 功能：
        窗体frmvls的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFacLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;
	SendWndMsg_LoopDisable(pWndObj);
    return iRet;
}

/***
  * 功能：
        窗体frmvls的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFacPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmvls的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmFacResume(void *pWndObj)
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
static int FacTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
	int i;
	/***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	pFacStrTitle = TransString("FACTORY_SERIAL_NUMBER");
	pFacStrTitle1 = TransString("FCT_LINEARITY");
    
    for(i = 0; i < SNInfoMaxCount; i++)
    {
        pFacStrInfo[i] = GetCurrLanguageText(SNInfo[i]);
    }

    for(i = 0; i < 1; i++)
    {
        pFacStrMachineName[i] = TransString("OTDR");
    }

    for(i = 0; i < MachineTypeCount; i++)
    {
        pFacStrMachineType[i] = TransString(machineTypeStr[i]);
    }

    for(i = 0; i < 12; i++)
    {
        pFacStrMonth[i] = TransString(SnNumSet[i+1]);
    }
    for(i = 0; i < 99; i++)
    {
        pFacStrYear[i] = TransString(SnNumSet[i+1]);
    }
    for(i = 0; i < 31; i++)
    {
        pFacStrDate[i] = TransString(SnNumSet[i+1]);
    }

    LOG(LOG_INFO, "Number = %s\n", stSn.Number);
    pFacStrNum = TransString(stSn.Number);

    for(i = 0; i < AddressCount; i++)
    {
        pFacStrAddress[i] = GetCurrLanguageText(addressStr1[i]);
    }


    pFacSNStrTitle = TransString("FCT_SERIAL_NUMBER");
    
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
static int FacTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    int i;
    /***************************************************************************
    *                           释放桌面上的文本
    ***************************************************************************/
	GuiMemFree(pFacStrTitle);
	GuiMemFree(pFacStrTitle1);
    GuiMemFree(pFacStrNum);
    GuiMemFree(pFacSNStrTitle);
    GuiMemFree(pFacSNStr);

    for(i = 0; i < SNInfoMaxCount; i++)
    {
       GuiMemFree(pFacStrInfo[i]);
    }
    
    for(i = 0; i < 1; i++)
    {
        GuiMemFree(pFacStrMachineName[i]);
    }

    for(i = 0; i < MachineTypeCount; i++)
    {
        GuiMemFree(pFacStrMachineType[i]);
    }

    for(i = 0; i < 12; i++)
    {
        GuiMemFree(pFacStrMonth[i]);
    }
    for(i = 0; i < 99; i++)
    {
        GuiMemFree(pFacStrYear[i]);
    }
    for(i = 0; i < 31; i++)
    {
        GuiMemFree(pFacStrDate[i]);
    }
    
    for(i = 0; i < AddressCount; i++)
    {
        GuiMemFree(pFacStrAddress[i]);
    }

	return 0;
}


static int FactorSNInput_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    LOG(LOG_INFO, "Up input num : %d\n", iOutLen);
    iInputLocation = iOutLen;
    IMEInit("", 3, 1, ReCreateWindow, RePskInputOk, NULL);
    return 0;
}

static int FactorSNInput_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
static int FactorTab_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
	
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
					   FrmLinearityInit, FrmLinearityExit, 
					   FrmLinearityPaint, FrmLinearityLoop, 
					   FrmLinearityPause, FrmLinearityResume,
                        NULL);          
    SendWndMsg_WindowExit(pFrmFactor);  
    SendSysMsg_ThreadCreate(pWnd); 
    
    return 0;
}

static void ReCreateWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacInit,FrmFacExit,
					  FrmFacPaint,FrmFacLoop,
					  FrmFacPause,FrmFacResume,
					  NULL);
}

static void RePskInputOk(void)
{
	char buff[512] = {0};
	GetIMEInputBuff(buff);
    LOG(LOG_INFO, "return IME buff strlen %d\n", strlen(buff));
    LOG(LOG_INFO, "return IME0 %c  %d\n", buff[0], buff[0]);
    LOG(LOG_INFO, "return IME1 %c  %d\n", buff[1], buff[1]);
    LOG(LOG_INFO, "return IME2 %c  %d\n", buff[2], buff[2]);

    if((buff[0] == '0') && (buff[1] == '0') && (buff[2] == '0'))
    {
        buff[0] = '0';
        buff[1] = '0';
        buff[2] = '1';
    }
    int num = atoi(buff);
    pNumber = (char*)malloc(24);
    if(strlen(buff) == 0)
    {
        num = 1;
    }
    sprintf(pNumber, "%03d", num);
}

static void FacMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	
	switch (iOption)
	{
	case 0: 
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAuthorizationInit, FrmAuthorizationExit, 
							FrmAuthorizationPaint, FrmAuthorizationLoop, 
							FrmAuthorizationPause, FrmAuthorizationResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactor);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
    case 1:        
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactoryFirmwareInit, FrmFactoryFirmwareExit, 
                        FrmFactoryFirmwarePaint, FrmFactoryFirmwareLoop, 
				        FrmFactoryFirmwarePause, FrmFactoryFirmwareResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactor);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 2:
        break;
    case 3:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacDRInit,FrmFacDRExit,
					  FrmFacDRPaint,FrmFacDRLoop,
					  FrmFacDRPause,FrmFacDRResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactor);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactor);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactor);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactor);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

GUIWINDOW* CreateFactoryWindow() 
{
	GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
            					   FrmFacInit, FrmFacExit, 
            					   FrmFacPaint, FrmFacLoop, 
            					   FrmFacPause, FrmFacResume,
            					   NULL);
    return pWnd;
}

