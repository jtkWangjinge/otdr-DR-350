/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlanguageconfig.c
* 摘    要：  实现烧录工厂配置文件功能
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-27
*
*******************************************************************************/

#include "wnd_frmlanguageconfig.h"
#include "app_systemsettings.h"

/**********************************************************************************
**						为实现窗体frmlanguageconfig而需要引用的其他头文件 	    **
***********************************************************************************/
#include "app_global.h"
#include "app_parameter.h"
#include "app_systemsettings.h"
#include "app_frmupdate.h"
#include "app_getsetparameter.h"

#include "guiphoto.h"

#include "wnd_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmmenubak.h"

#include "wnd_frmsystemsetting.h"
#include "wnd_frmconfigureinfo.h"
#include "wnd_frmlinearity.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmauthorization.h"

/**********************************************************************************
**									变量定义						 			 **
***********************************************************************************/
#define LANGUAGE_NUM		8
extern PFactoryConfig  pFactoryConf;
extern CURR_WINDOW_TYPE enCurWindow;
static unsigned long long int languageMessager = 0;
static int GiLangSelect = 0;
static int iSelectedCount = 0;
static int iSelectedList[10] = {-1, 1, -1, -1, -1, -1, -1, -1, -1, -1};
static int iSelected = 0;//选中语言项

/**********************************************************************************
**							窗体Frmlanguageconfig中的控件定义部分				**
***********************************************************************************/
static GUIWINDOW *pFrmLanguageConfig = NULL;

/**********************************背景控件定义************************************/
static GUIPICTURE *pLanguageConfigBtnLeftBg  = NULL;

/******************************窗体标题栏控件定义**********************************/
static GUICHAR *pLanguageConfigStrLanguageSet = NULL;	  		//窗体左上角标题栏文本
static GUILABEL *pLanguageConfigLblLanguageSet = NULL;			//窗体左上角的Label

//语言列表
static GUIPICTURE *pLanguageConfigBtnSelected[LANGUAGE_NUM] = {NULL};
static GUICHAR *pLanguageConfigStrList[LANGUAGE_NUM] = {NULL};
static GUILABEL *pLanguageConfigLblList[LANGUAGE_NUM] = { NULL };
/********************************右侧菜单控件定义**********************************/
static WNDMENU1 *pLanguageConfigMenu = NULL;

/**********************************************************************************
**	    	窗体FrmLanguageConfig中的初始化文本资源、 释放文本资源函数定义部分	**
***********************************************************************************/
//初始化文本资源
static int LanguageConfigTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int LanguageConfigTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/**********************************************************************************
**			    	窗体FrmLanguageConfig中的控件事件处理函数定义部分			**
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int LanguageConfigWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int LanguageConfigWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);
//语言列表按键响应
static int LanguageConfigList_Down(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);
static int LanguageConfigList_Up(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);
/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/
//static void ReCreateWindow(GUIWINDOW **pWnd);
static void LanguageConfigWndMenuCallBack(int selected);
static void FlushLanguageList(void);

/***
  * 功能：
        窗体FrmLanguageConfig的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLanguageConfigInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	char* StrLanguageMenu[FACTORY_MENU_COUNT] = {
		"DebugInfo",
		"LanguageSet",
		"Non-linearity",
		"Uncertainty",
		"FactorySet",
		"Authorization"
	};
	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmLanguageConfig = (GUIWINDOW *) pWndObj;

   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    LanguageConfigTextRes_Init(NULL, 0, NULL, 0);
	//读取系统设置的值
	languageMessager = pFactoryConf->ulAvailableLanguage;
	/* 窗体背景图片 */
	pLanguageConfigBtnLeftBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
											 BmpFileDirectory "bg_factory.bmp");
	/* 窗体标题的Label */
	pLanguageConfigLblLanguageSet = CreateLabel(36, 12, 200, 16, pLanguageConfigStrLanguageSet);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLanguageConfigLblLanguageSet);
	//语言菜单列表
	int i;
	for (i = 0; i < LANGUAGE_NUM; ++i)
	{
		pLanguageConfigBtnSelected[i] = CreatePicture(8, 88 + i * 32, 512, 32,
													  BmpFileDirectory "bg_sys_unselect.bmp");
		pLanguageConfigLblList[i] = CreateLabel(8, 88 + i * 32 + 8, 520, 32,
												pLanguageConfigStrList[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLanguageConfigLblList[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pLanguageConfigLblList[i]);
	}
	//菜单栏控件
	pLanguageConfigMenu = CreateStringWndMenu(FACTORY_MENU_COUNT, sizeof(StrLanguageMenu),
							StrLanguageMenu, 0xff00, 1, 54, LanguageConfigWndMenuCallBack);
	//注册桌面右侧按钮
	AddWndMenuToComp1(pLanguageConfigMenu, pFrmLanguageConfig);	

	//注册桌面右侧按钮的消息处理
	LoginWndMenuToMsg1(pLanguageConfigMenu, pFrmLanguageConfig);	

	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmLanguageConfig, 
                  pFrmLanguageConfig);
	
	//注册语言列表
	for (i = 0; i < LANGUAGE_NUM; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pLanguageConfigBtnSelected[i], pFrmLanguageConfig);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pLanguageConfigLblList[i], pFrmLanguageConfig);
	}

	 //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    
	//注册语言列表的响应处理
	for ( i = 0; i < LANGUAGE_NUM; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLanguageConfigBtnSelected[i],
			LanguageConfigList_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pLanguageConfigBtnSelected[i],
			LanguageConfigList_Up, NULL, i, pMsg);

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLanguageConfigLblList[i],
			LanguageConfigList_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pLanguageConfigLblList[i],
			LanguageConfigList_Up, NULL, i, pMsg);
	}

    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmLanguageConfig, 
                    LanguageConfigWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmLanguageConfig, 
                    LanguageConfigWndKey_Up, NULL, 0, pMsg);
    
    return iReturn;
}


/***
  * 功能：
        窗体FrmLanguageConfig的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLanguageConfigExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmLanguageConfig = (GUIWINDOW *) pWndObj;

	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmLanguageConfig);

    //销毁窗体控件
    //***************************************************************/
    //销毁桌面背景图片
    DestroyPicture(&pLanguageConfigBtnLeftBg);
	//销毁窗体左上角窗体Label
	DestroyLabel(&pLanguageConfigLblLanguageSet);
	//销毁右侧区域的Label
	DestroyWndMenu1(&pLanguageConfigMenu);
	//销毁语言列表
	int i;
	for ( i = 0; i < LANGUAGE_NUM; i++)
	{
		DestroyPicture(&pLanguageConfigBtnSelected[i]);
		DestroyLabel(&pLanguageConfigLblList[i]);
	}
	//释放文本资源
    //***************************************************************/
    LanguageConfigTextRes_Exit(NULL, 0, NULL, 0);
	//保存系统设置
	pFactoryConf->ulAvailableLanguage = languageMessager;

	SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
	SaveSettings(FACTORY_CONFIG);

    return iReturn;
}


/***
  * 功能：
        窗体FrmLanguageConfig的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLanguageConfigPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmLanguageConfig = (GUIWINDOW *) pWndObj;

    //显示桌面背景图片
    DisplayPicture(pLanguageConfigBtnLeftBg);	
	//显示窗体左上角窗体Label
	DisplayLabel(pLanguageConfigLblLanguageSet);

	//显示菜单控件
	DisplayWndMenu1(pLanguageConfigMenu);

	GiLangSelect = GetCurrLanguageSet();
	//语言图片显示
	FlushLanguageList();

	SetPowerEnable(1, 1);	

	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmLanguageConfig的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLanguageConfigLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);   	
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmLanguageConfig的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLanguageConfigPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体FrmLanguageConfig的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLanguageConfigResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    窗体FrmLanguageConfig中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/

/***
  * 功能：
        初始化文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageConfigTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//窗体左上角的文本
	pLanguageConfigStrLanguageSet = TransString("Language Set");
	//语言列表
	char* pStrLanguage[LANGUAGE_NUM] = {
	"China","English","Spanish","France","Portugal",
	"Russian","Italy","Thai"
// 	"Italy",
// 	"Portugal",
// 	"Polish",
// 	"Czech",
// 	"Thai",
// 	"Persian"
	};
	int i;
	for (i = 0; i < LANGUAGE_NUM; ++i)
	{
		pLanguageConfigStrList[i] = TransString(pStrLanguage[i]);
	}

    return iReturn;
}


/***
  * 功能：
        释放文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageConfigTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//释放左上角的文本
	GuiMemFree(pLanguageConfigStrLanguageSet);
	//语言列表
	int i;
	for (i = 0; i < LANGUAGE_NUM; ++i)
	{
		GuiMemFree(pLanguageConfigStrList[i]);
	}
    return iReturn;
}


/**********************************************************************************
**			    			窗体的按键事件处理函数				 				 **
***********************************************************************************/
/***
  * 功能：
        窗体的按键按下事件处理函数
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageConfigWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	
	switch (uiValue)
    {
		case KEYCODE_ESC:
			break;
		case KEYCODE_HOME:
			break;
		default:
			break;
    }
	
    return iReturn;
}

/***
  * 功能：
        窗体的按键弹起事件处理函数
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageConfigWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	
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
	
    return iReturn;
}                         

//语言列表按键响应
static int LanguageConfigList_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	return 0;
}

static int LanguageConfigList_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//临时变量定义
	int iTemp = iOutLen;
	LOG(LOG_INFO, "iTemp = %d\n", iTemp);
	GiLangSelect = iTemp;

	if (isBitSetted(languageMessager, GiLangSelect) && (GiLangSelect != 1))//英文选项不可清除
		clearBit(&languageMessager, GiLangSelect);
	else
		setBit(&languageMessager, GiLangSelect);
	FlushLanguageList();
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

 /***
  * 功能：
        用于对话框重新还原窗体时的回调函数
  * 参数：
        1.GUIWINDOW **pWnd:		指向窗体对象的指针的指针
  * 返回：
        无
  * 备注：
***/
//static void ReCreateWindow(GUIWINDOW **pWnd)
//{	
//    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
//                    FrmLanguageConfigInit, FrmLanguageConfigExit, 
//                    FrmLanguageConfigPaint, FrmLanguageConfigLoop, 
//			        FrmLanguageConfigPause, FrmLanguageConfigResume,
//                    NULL);
//}


/***
  * 功能：
        菜单栏控件回调
  * 参数：
        1、int selected:	按钮索引
  * 返回：
        无
  * 备注：
***/
static void LanguageConfigWndMenuCallBack(int selected)
{
	GUIWINDOW *pWnd = NULL;
	switch (selected)
	{
	case DEBUG_INFO://调取debug info窗体
		//设置当前界面
		enCurWindow = ENUM_LANGUAGE_CONFIG_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmConfigureInfoInit, FrmConfigureInfoExit,
		                    FrmConfigureInfoPaint, FrmConfigureInfoLoop,
					        FrmConfigureInfoPause, FrmConfigureInfoResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmLanguageConfig);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case LANGUAGE_SET://调取语言设置窗体
		break;
	case NON_LINEARITY:	//调取非线性度设置窗体
		//设置当前界面
		enCurWindow = ENUM_NON_LINEARITY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmLinearityInit, FrmLinearityExit,
								FrmLinearityPaint, FrmLinearityLoop,
								FrmLinearityPause, FrmLinearityResume,
								NULL);			
		SendWndMsg_WindowExit(pFrmLanguageConfig);	
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case UNCERTAINTY: //调取不确定性窗体
		//设置当前界面
		enCurWindow = ENUM_UNCERTAINTY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmUncertaintyInit, FrmUncertaintyExit,
								FrmUncertaintyPaint, FrmUncertaintyLoop,
								FrmUncertaintyPause, FrmUncertaintyResume,
								NULL);			
		SendWndMsg_WindowExit(pFrmLanguageConfig);	
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case FACTORY_SET: //调取软件升级窗体
		//设置当前界面
		enCurWindow = ENUM_FACTORY_SET_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmFactorySetInit, FrmFactorySetExit,
							FrmFactorySetPaint, FrmFactorySetLoop,
							FrmFactorySetPause, FrmFactorySetResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmLanguageConfig);	
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case AUTHORIZATION:
		enCurWindow = ENUM_AUTHORIZATION_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);
		SendWndMsg_WindowExit(pFrmLanguageConfig);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case BACK_DOWN:
	case HOME_DOWN://退出时间设置
		//设置当前界面
		enCurWindow = ENUM_OTHER_WIN;
		iSelected = 0;//清除选项
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmSystemSettingInit, FrmSystemSettingExit,
	                        FrmSystemSettingPaint, FrmSystemSettingLoop,
					        FrmSystemSettingPause, FrmSystemSettingResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmLanguageConfig);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
			
	default:
		break;
	}
}

//刷新语言列表
static void FlushLanguageList(void)
{
	//初始化
	iSelectedCount = 0;

	int i = 0;
	for (i = 0; i < LANGUAGE_NUM; i++)
	{
		if (isBitSetted(languageMessager, i))
		{
			SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pLanguageConfigBtnSelected[i]);
			iSelectedCount++;
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pLanguageConfigBtnSelected[i]);
		}

		DisplayPicture(pLanguageConfigBtnSelected[i]);
		DisplayLabel(pLanguageConfigLblList[i]);
	}
}

int LanguageConfigViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where)
{
	int iRet = 0;
	FACTORY_FIRMWARE factoryFirmware;
//     char SN[20];
    
    iRet = LoadOtdrConfig(&factoryFirmware, fileName);

//     GenerateSerialNumber(SN, pCurSN);

//     if(!iRet && strcmp(factoryFirmware.serialNo, SN) != 0)
//     {
//         iRet = OtdrConfigSerialNoError;
//     }
// 
//     if (!iRet)
// 	{
//         printf("old Available Language : %llx \n", pFactoryConf->ulAvailableLanguage);
//         pFactoryConf->ulAvailableLanguage = factoryFirmware.ulAvailableLanguage;
//         printf("new Available Language : %llx \n", pFactoryConf->ulAvailableLanguage);
//         
//         //保存参数到eeprom中
//         SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
//         SaveSettings(FACTORY_CONFIG);
// 
// 		DialogInit(120, 90, TransString("Warning"),
// 			TransString("FACTORY_IMPORT_PROFILE_SUCCESS"),
// 			0, 0, ReCreateWindow, NULL, NULL); 
//     }
	
	return iRet;
}

int isBitSetted(unsigned long long lw, int bit)
{
	unsigned long long mask = 0x1u;
	mask <<= bit;
	if (lw & mask)
		return 1;
	return 0;
}

void setBit(unsigned long long* lw, int bit)
{
	unsigned long long mask = 0x1u;
	mask <<= bit;
	(*lw) |= mask;
}

void clearBit(unsigned long long* lw, int bit)
{
	unsigned long long mask = 0x1u;
	mask <<= bit;
	(*lw) &= ~mask;
}

//获取选中的语言总数
int GetLanguageCount(void)
{
	int count = 0;
	//未进入工厂菜单时，需要自行获取语言值
	if (languageMessager == 0 || !iSelected)
	{
		languageMessager = pFactoryConf->ulAvailableLanguage;

		int i = 0;

		for (i = 0; i < LANGUAGE_NUM; i++)
		{
			if (isBitSetted(languageMessager, i))
			{
				count++;
			}
		}
	}
	else
	{
		count = iSelectedCount;
	}

	return count;
}
//获取选中的语言列表
int* GetSelectedLanguageList(void)
{
	int *pLanguageList = NULL;
	int pStrLanguage[LANGUAGE_NUM] = {
		SYSTEM_LANGUAGE_LBL_CHINA, SYSTEM_LANGUAGE_LBL_ENGLISH, SYSTEM_LANGUAGE_LBL_SPANISH,
		SYSTEM_LANGUAGE_LBL_FRANCE, SYSTEM_LANGUAGE_LBL_PORTUGAL, SYSTEM_LANGUAGE_LBL_RUSSIAN, 
		SYSTEM_LANGUAGE_LBL_ITALY, SYSTEM_LANGUAGE_LBL_THAI
	};

	int i = 0;
	int j = 0;
	pLanguageList = (int*)malloc(sizeof(int)*10);

	for (i = 0; i < LANGUAGE_NUM; ++i)
	{
		iSelectedList[i] = -1;
		if (isBitSetted(languageMessager, i))
		{
			pLanguageList[j] = pStrLanguage[i];
			iSelectedList[j] = i;
			j++;
		}
	}

	return pLanguageList;
}

//获取选中语言列表的index
int* GetSelectedLanguageListIndex(void)
{
	return iSelectedList;
}

//获取选中项的下标值
int GetSelectedIndex(int languageIndex)
{
	//检查参数
	if (languageIndex < 0 || languageIndex >= LANG_TYPE)
	{
		return -1;
	}

	int i;
	int iIndex = 0;

	for (i = 0; i < LANGUAGE_NUM; ++i)
	{
		if (iSelectedList[i] == languageIndex)
			iIndex = i;
	}

	return iIndex;
}

//语言列表按键回调函数
void LanguageKeyCallBack(int iOption)
{
	switch (iOption)
	{
	case 0://up
		if (iSelected)
		{
			iSelected--;
		}
		break;
	case 1://down
		if (iSelected >= 0 && iSelected < LANGUAGE_NUM)
		{
			iSelected++;
		}
		break;
	case 2://enter
		LanguageConfigList_Up(NULL, 0, NULL, iSelected);
		break;
	default:
		break;
	}
}
