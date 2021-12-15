/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmime.h
* 摘    要：  声明窗体wnd_frmime(输入法)的窗体处理线程及相关操作函数。该窗体
*             提供了输入法功能。
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-8-24
*
*******************************************************************************/

#include "wnd_frmime.h"

/*******************************************************************************
**						为实现窗体frmime而需要引用的其他头文件			  	  **
*******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "app_global.h"
#include "apptext.h"
#include "app_frmotdr.h"

#include "input_china_pinyin.h"
#include "hangulime.h"

#include "wnd_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmfilebrowse.h"
#include "wnd_frmscroll.h"
#include "wnd_saveasdialog.h"



/*******************************************************************************
**									宏定义									  **
*******************************************************************************/
#define MAXKEYNUM		52							//按键的数目
#define MAXINPUTNUM		512							//可输入的字符的最大数目
#define MAXCHINESENUM	12							//每次可显示的待候选的最大中文字符数

#define BACKNUM	    	21							//删除键的序号
#define ENTERNUM		50							//回车键的序号   
#define SHIFTNUM		33							//shift键的序号 
#define UPNUM			49							//上移键的序号
#define ESCNUM			51							//esc键的序号 
#define SPACENUM		49							//空格键的序号
#define CNENNUM			44 							//中英文切换键的序号
#define LEFTNUM			54							//左移键的序号
#define DOWNNUM			55							//下移键的序号
#define RIGHTNUM		56 							//右移键的序号

#define IMEWinX 		58		        			//窗体左上角X
#define IMEWinY    		58	            			//窗体左上角Y
#define IMEWinY1    	58	            			//窗体左上角Y

#define IMEDIALOGWinX 	120		        			//对话框窗体左上角X
#define IMEDIALOGWinY   90	            			//对话框窗体左上角Y

#define IMESPACEKEYWIDTH   86                         //空格键宽度 
#define IMESPECIALKEYWIDTH 86                          //esc/enter/backspace键宽度
#define IMEKEYHEIGHT       36                          //按钮高度
#define IMEKEYWIDTH        40                          //普通按钮的宽度  
#define IMELINE1Y       (IMEWinY+141)                   //第一行按键y坐标
#define IMELINEYGAP        45                          //按键行间距
#define IMELINEXGAP        47//46                          //按键左右间距
#define IMELINE1X          65//70                          //第1行第1个按键x
#define IMELINE2X          65//70                          //第2、4、5行第1个按键x
#define IMELINE3X          65//70                          //第3行第1个按键x
#define IMELINE42X   (IMELINE2X+IMESPECIALKEYWIDTH+7)  //第4、5行第2个按键x
#define IMELINE53X   (IMELINE42X+IMESPACEKEYWIDTH+7) //第5行第3个按键x，式中7为按键间的空白
#define IMEXOFF(x)   (IMELINEXGAP*x)                //按键横坐标偏移量
#define IMEKEYY(x)   (IMELINE1Y +IMELINEYGAP*x)     //按键Y坐标



/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
static unsigned int g_usLanguage = 0;				//当前所选的语言
static unsigned char g_ucKnobFlg = 0;				//旋转编码按下标志

static int GcShiftNum = 0;							//shift切换标志
static unsigned char g_ucEnCnFlg = 0;				//中英文切换标志，0:软键盘输入	1:中文输入 2:韩文输入
static int GiCursorPos   = 0;	    				//当前光标的位置
static int GiMaxInputNum = 0;						//允许输入的最大字符长度
static char GcKeyType = 0;				    		//键盘的类型，0:全键盘 1:只有数字有效 2:只有字母有效
static unsigned char g_ucDialogIMEFlg = 0;			//是否建立对话框的标志

static unsigned short g_usUnicodeBuff[MAXINPUTNUM];	//输入的字符的缓存，UCS-2格式
static char GcKeyName[MAXKEYNUM][2] = { 
{'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'},
{'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {' ', ' '},
{'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'}, {'\'', '\''},
{' ', ' '}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'},
{' ', ' '}, {'=', '+'}, {'\\', '|'}, {'.', '>'}, {'`', '~'}, {' ', ' '},{' ', ' '}, {' ', ' '}
};

static char *GcKeyName1[MAXKEYNUM][10] = {
{"1", "!"},   {"2", "@"}, {"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"}, {"7", "&"},   {"8", "*"}, {"9", "("}, {"0", ")"}, {"-", "_"},
{"q", "Q"},   {"w", "W"}, {"e", "E"}, {"r", "R"}, {"t", "T"}, {"y", "Y"}, {"u", "U"},   {"i", "I"}, {"o", "O"}, {"p", "P"}, {"", ""},
{"a", "A"},   {"s", "S"}, {"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"}, {"j", "J"},   {"k", "K"}, {"l", "L"}, {";", ":"}, {"'", "\""},
{"", ""},     {"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"}, {"b", "B"}, {"n", "N"},   {"m", "M"}, {",", "<"}, {".", ">"}, {"/", "?"},
{"EN", "EN"}, {"=", "+"}, {"\\", "|"},{".", ">"}, {"`", "~"}, {"", ""},   {"OK", "OK"}, {"Cancel", "Cancel"}
};

//韩文字符
static char *GcKeyName2[MAXKEYNUM][10] = {
{"`", "~"},        {"1", "!"}, {"2", "@"}, {"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"}, {"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},  {"-", "_"},   {"=", "+"}, {"", ""},
{"ㅂ", "ㅃ"},        {"ㅈ", "ㅉ"}, {"ㄷ", "ㄸ"}, {"ㄱ", "ㄲ"}, {"ㅅ", "ㅆ"}, {"ㅛ", "ㅛ"}, {"ㅕ", "ㅕ"}, {"ㅑ", "ㅑ"}, {"ㅐ", "ㅒ"}, {"ㅔ", "ㅖ"}, {"[", "{"},  {"]", "}"}, {"\\", "|"},
{"ㅁ", "ㅁ"},        {"ㄴ", "ㄴ"}, {"ㅇ", "ㅇ"}, {"ㄹ", "ㄹ"}, {"ㅎ", "ㅎ"}, {"ㅗ", "ㅗ"}, {"ㅓ", "ㅓ"}, {"ㅏ", "ㅏ"}, {"ㅣ", "ㅣ"}, {";", ":"}, {"'", "\""}, {"Enter", "Enter"},
{"Shift", "Shift"},{"ㅋ", "ㅋ"}, {"ㅌ", "ㅌ"}, {"ㅊ", "ㅊ"}, {"ㅍ", "ㅍ"}, {"ㅠ", "ㅠ"}, {"ㅜ", "ㅜ"}, {"ㅡ", "ㅡ"}, {",", "<"}, {".", ">"}, {"", ""},    {"/", "?"},
{"Esc", "Esc"}
};

IMECALLLBACK IMEcallBackFunc 		     = NULL;	// 重绘前一个窗体的回调函数
IMECALLLBACKHANDLE IMEOkcallBackFunc     = NULL;	// IME窗体中按下enter按钮后的回调函数
IMECALLLBACKHANDLE IMECancelcallBackFunc = NULL;	// IME窗体中按下cancel按钮后的回调函数

static int iIMECurrSelected = 0;					//显示当前按键所在的index
static BTN_SCROLL *pIMEScroll = NULL;
static char g_cPinyin[PINYIN_STORE_MAXLEN];			//存贮用户输入的拼音
static WordList *pMainWordSet = NULL;
extern POTDR_TOP_SETTINGS pOtdrTopSettings;
/*******************************************************************************
**							窗体frmime中的控件定义部分					      **
*******************************************************************************/
GUIWINDOW *pFrmIME = NULL;
/**********************************背景控件定义********************************/ 
/* 键盘的背景图片 */
static GUIPICTURE *pIMESetBg = NULL;

/* 显示输入的字符的背景及Label控件 */
static GUIPICTURE *pIMEBgInStr  = NULL;
static GUILABEL   *pIMELblInStr = NULL;
static GUICHAR *pIMEStrKeyInput = NULL;

/* 按键的背景及Label控件 */
static GUIPICTURE *pIMEBtnKey[MAXKEYNUM];
static GUILABEL   *pIMELblKey[MAXKEYNUM];  
static GUICHAR *pIMEStrKeyName[MAXKEYNUM];  

//add han str
static GUICHAR *pIMEStrKeyHanName[MAXKEYNUM];

/* 显示输入的拼音的背景及Label控件 */
static GUIPICTURE *pIMEBtnPinYin = NULL;
static GUILABEL   *pIMELblPinYin = NULL; 
static GUICHAR *pIMEStrPinYin;  

/* 显示候选中文的背景及Label控件 */
static GUIPICTURE *pIMEBtnChinese[MAXCHINESENUM];
static GUILABEL   *pIMELblChinese[MAXCHINESENUM];  
static GUICHAR *pIMEStrChineseName[MAXCHINESENUM];  	

/* 上一页下一页刷新候选中文的背景图片 */
static GUIPICTURE *pIMEBtnPageUp = NULL;
static GUIPICTURE *pIMEBtnPageDown = NULL;


/*************************输入法内对话框控件定义*******************************/
static GUICHAR *pIMEDialogStrTitle = NULL;     		//桌面上对话框的标题文本			
static GUICHAR *pIMEDialogStr = NULL;				//桌面上对话框的正文文本
static GUICHAR *pIMEDialogStrOk     = NULL;			//桌面上对话框的确定取消键的文本

static GUILABEL *pIMEDialogLblTitle    = NULL;    	//对话框标题Lbl控件
static GUILABEL *pIMEDialogLblInfo     = NULL;     //对话框上显示信息的Lbl控件
static GUILABEL *pIMEDialogLblOk       = NULL;     //对话框确定按钮的Lbl控件

static GUIPICTURE *pIMEDialogBg        = NULL;		//对话框的背景图片
static GUIPICTURE *pIMEDialogBtnType   = NULL;		//对话框的类型图片
static GUIPICTURE *pIMEDialogBtnOk     = NULL;		//对话框的确定按钮图片 


/*******************************************************************************
**	    	窗体frmime中的初始化文本资源、 释放文本资源函数定义部分		  	  **
*******************************************************************************/
static int IMETextRes_Init(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int IMETextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);


/*******************************************************************************
**			    	窗体frmime中的控件事件处理函数定义部分				      **
*******************************************************************************/

/***************************窗体的按键事件处理函数*****************************/
static int IMEWndKey_Down(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
static int IMEWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int IMEWndKnob_Clock(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen);
static int IMEWndKnob_Anti(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen);


/*************************键盘上按钮的事件处理函数*****************************/
static int IMEBtnKey_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int IMEBtnPageUp_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int IMEBtnPageDown_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int IMEBtnSelectChinese_Down(void *pInArg,   int iInLen, 
                          			void *pOutArg, int iOutLen);

static int IMEBtnKey_Up(void *pInArg,   int iInLen, 
                        void *pOutArg, int iOutLen);
static int IMEBtnPageUp_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int IMEBtnPageDown_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int IMEBtnSelectChinese_Up(void *pInArg,   int iInLen, 
                          			void *pOutArg, int iOutLen);


/************************对话框上按钮的事件处理函数****************************/
static int IMEDialogBtnOk_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen);
static int IMEDialogBtnOk_Up(void *pInArg,   int iInLen, 
                        void *pOutArg, int iOutLen);


/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static int ShowBtn_Down(int iIndex);
static int ShowBtn_Up(int iIndex);
static void ShowKey_FuncDown(int index);
static void ShowKey_FuncUp(int index); 
static void DispShift(void);
static void DispInputStr(unsigned short *pBuf);
static void InsertChar(unsigned short usChar);
static void InsertHanChar(unsigned short usChar);
static void InsertPinYinChar(char cChar);
static void DeleteChar(void);
static void DeletePinYinChar(void);
// static void CursorLeftMove(void);
// static void CursorRightMove(void);
static int  IMEExit(void);
static void OkHandle(void);
static void CancelHandle(void);
void DeleteAChar(unsigned short *pString);
//static int DeleteEndSpace(unsigned short *pString);
/*
 * 实现物理按键选中按钮效果的函数声明	
 */
int IMEShowScrollBtnDown(int index, int *pCurr, BTN_SCROLL *pScroll);
int IMEShowScrollBtnUp(int iCurr, BTN_SCROLL *pScroll);
int IMEScrollScrollUpOrDown(int iPrev, int iCurr, BTN_SCROLL *pScroll);
static int IMESInitScroll(BTN_SCROLL *pScroll);

/* 中文输入法相关函数声明 */
static int PinYinToChinese(void);
static int WordListTurnPage(int iTurnFlag);
static int DisplayWordList(void);
static int DisplayPinYin(void);

static void ChangeLalColour(GUILABEL *pLblFx);

//处理enter键函数
static void HandleEnterKeyCallBack(void);

/***
  * 功能：
        窗体frmime的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmIMEInit(void *pWndObj)
{
	g_usLanguage = GetCurrLanguageSet();
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	int i = 0;
	int j = 0;
    int iKeyLblWd = 0;

	int GiKeyPos[MAXKEYNUM][2] = {
		{IMELINE1X, IMEKEYY(0)},{IMELINE1X+IMEXOFF(1), IMEKEYY(0)},{IMELINE1X+IMEXOFF(2), IMEKEYY(0)},{IMELINE1X+IMEXOFF(3), IMEKEYY(0)},
		{IMELINE1X+IMEXOFF(4), IMEKEYY(0)}, {IMELINE1X+IMEXOFF(5), IMEKEYY(0)}, {IMELINE1X+IMEXOFF(6), IMEKEYY(0)}, {IMELINE1X+IMEXOFF(7), IMEKEYY(0)},
		{IMELINE1X+IMEXOFF(8), IMEKEYY(0)}, {IMELINE1X+IMEXOFF(9), IMEKEYY(0)}, {IMELINE1X+IMEXOFF(10), IMEKEYY(0)}, 

		{IMELINE1X, IMEKEYY(1)},{IMELINE1X + IMEXOFF(1), IMEKEYY(1)},{IMELINE1X + IMEXOFF(2), IMEKEYY(1)},{IMELINE1X + IMEXOFF(3), IMEKEYY(1)},
		{IMELINE1X + IMEXOFF(4), IMEKEYY(1)}, {IMELINE1X + IMEXOFF(5), IMEKEYY(1)}, {IMELINE1X + IMEXOFF(6), IMEKEYY(1)}, {IMELINE1X + IMEXOFF(7), IMEKEYY(1)},
		{IMELINE1X + IMEXOFF(8), IMEKEYY(1)}, {IMELINE1X + IMEXOFF(9), IMEKEYY(1)}, {IMELINE1X + IMEXOFF(10), IMEKEYY(1)},

		{IMELINE1X, IMEKEYY(2)},{IMELINE1X + IMEXOFF(1), IMEKEYY(2)},{IMELINE1X + IMEXOFF(2), IMEKEYY(2)},{IMELINE1X + IMEXOFF(3), IMEKEYY(2)},
		{IMELINE1X + IMEXOFF(4), IMEKEYY(2)}, {IMELINE1X + IMEXOFF(5), IMEKEYY(2)}, {IMELINE1X + IMEXOFF(6), IMEKEYY(2)}, {IMELINE1X + IMEXOFF(7), IMEKEYY(2)},
		{IMELINE1X + IMEXOFF(8), IMEKEYY(2)}, {IMELINE1X + IMEXOFF(9), IMEKEYY(2)}, {IMELINE1X + IMEXOFF(10), IMEKEYY(2)},

		{IMELINE1X, IMEKEYY(3)},{IMELINE1X + IMEXOFF(1), IMEKEYY(3)},{IMELINE1X + IMEXOFF(2), IMEKEYY(3)},{IMELINE1X + IMEXOFF(3), IMEKEYY(3)},
		{IMELINE1X + IMEXOFF(4), IMEKEYY(3)}, {IMELINE1X + IMEXOFF(5), IMEKEYY(3)}, {IMELINE1X + IMEXOFF(6), IMEKEYY(3)}, {IMELINE1X + IMEXOFF(7), IMEKEYY(3)},
		{IMELINE1X + IMEXOFF(8), IMEKEYY(3)}, {IMELINE1X + IMEXOFF(9), IMEKEYY(3)}, {IMELINE1X + IMEXOFF(10), IMEKEYY(3)},

		{IMELINE1X, IMEKEYY(4)},{IMELINE1X + IMEXOFF(1), IMEKEYY(4)},{IMELINE1X + IMEXOFF(2), IMEKEYY(4)},{IMELINE1X + IMEXOFF(3), IMEKEYY(4)},
		{IMELINE1X + IMEXOFF(4), IMEKEYY(4)}, {IMELINE1X + IMEXOFF(5), IMEKEYY(4)}, {IMELINE1X + IMEXOFF(7), IMEKEYY(4)}, {IMELINE1X + IMEXOFF(9), IMEKEYY(4)}
	};	
    //得到当前窗体对象
    pFrmIME = (GUIWINDOW *) pWndObj;

	//初始化选中的按钮
	iIMECurrSelected = 0;

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    IMETextRes_Init(NULL, 0, NULL, 0);
    /* 键盘的背景图片 */
    pIMESetBg = CreatePicture(IMEWinX, IMEWinY1, 524, 364, BmpFileDirectory"frm_IME_bg.bmp");

	/* 显示输入的字符的背景及Label控件 */
	pIMEBgInStr  = CreatePicture(IMEWinX+8, IMEWinY1 + 7, 508, 36, BmpFileDirectory"inputstr_bg.bmp");
	pIMELblInStr = CreateLabel(IMEWinX+8+8, IMEWinY1 + 7 + 10 , 250, 16, pIMEStrKeyInput);

	/* 建立用户输入的拼音的背景及Label控件 */
	pIMEBtnPinYin = CreatePicture(IMEWinX+8, IMEWinY1 +97, 508, 36, BmpFileDirectory"inputstr_bg.bmp");
	pIMELblPinYin = CreateLabel(IMEWinX+8+8, IMEWinY1 + 97+10, 250, 16, pIMEStrPinYin);	

	/* 建立上一页下一页的背景图片 */
	pIMEBtnPageUp = CreatePicture(IMEWinX + 8, IMEWinY1 + 51, 36, 36, BmpFileDirectory "btn_pageup_press.bmp");
	pIMEBtnPageDown = CreatePicture(538, IMEWinY1 + 51, 36, 36, BmpFileDirectory "btn_pagedown_unpress.bmp");
	/* 建立中文候选区域的背景及Label */
	for(i=0, j=IMEWinX+8+38+3; i<MAXCHINESENUM; i++, j+=35)
	{
		pIMEBtnChinese[i] = CreatePicture(j, IMEWinY1 + 51+1, 35, 34, BmpFileDirectory"btn_pinyin_unpress.bmp");	
	}
	for (i = 0, j = IMEWinX + 8 + 38 + 3; i < MAXCHINESENUM; i++, j += 35)
	{
		pIMELblChinese[i] = CreateLabel(j+9, IMEWinY1 + 51+1+10, 35, 16, pIMEStrChineseName[i]);
	}
	/* 按键的背景及Label控件 */
	for(i=0; i<MAXKEYNUM; i++)
	{
	    iKeyLblWd = IMEKEYWIDTH;
		if(i == BACKNUM)
		{
			pIMEBtnKey[i] =  CreatePicture(GiKeyPos[i][0], GiKeyPos[i][1], IMEKEYWIDTH, IMEKEYHEIGHT, BACK_KEY_BMP);
		}
		else if (i == SHIFTNUM)
		{
			pIMEBtnKey[i] = CreatePicture(GiKeyPos[i][0], GiKeyPos[i][1], IMEKEYWIDTH, IMEKEYHEIGHT, SHIFT_KEY_BMP);
			iKeyLblWd = IMESPECIALKEYWIDTH;
		}
		else if(i == ENTERNUM || i == ESCNUM)
		{
			pIMEBtnKey[i] =  CreatePicture(GiKeyPos[i][0], GiKeyPos[i][1], IMESPECIALKEYWIDTH, IMEKEYHEIGHT, SPECIAL_KEY_BMP);
            iKeyLblWd = IMESPECIALKEYWIDTH;
		}
		else if(i == SPACENUM)
		{
			pIMEBtnKey[i] =  CreatePicture(GiKeyPos[i][0], GiKeyPos[i][1], IMESPECIALKEYWIDTH, IMEKEYHEIGHT, SPACE_KEY_BMP);
            iKeyLblWd = IMESPECIALKEYWIDTH;
		}
		else
		{
			pIMEBtnKey[i] =  CreatePicture(GiKeyPos[i][0], GiKeyPos[i][1], IMEKEYWIDTH, IMEKEYHEIGHT, NORMAL_KEY_BMP);
		}
        pIMELblKey[i] = CreateLabel(GiKeyPos[i][0], GiKeyPos[i][1]+10, iKeyLblWd, IMEKEYHEIGHT, pIMEStrKeyName[i]);
	}

//     if(LANG_CHINASIM == g_usLanguage)
//     {
//         SetPictureBitmap(EN_KEY_BMP, pIMEBtnKey[CNENNUM]);
//         SetLabelEnable(0, pIMELblKey[CNENNUM]);
//     }
    /*************************建立输入法上的对话框控件***************************/
    
	pIMEDialogBg = CreatePicture(IMEDIALOGWinX, IMEDIALOGWinY, 400, 300, BmpFileDirectory"bg_dialog.bmp");
	pIMEDialogBtnType =  CreatePicture(IMEDIALOGWinX+170, IMEDIALOGWinY+95, 60, 50,  BmpFileDirectory"bg_dialog_warring.bmp"); 
	pIMEDialogBtnOk =  CreatePicture(IMEDIALOGWinX + 101, IMEDIALOGWinY + 260, 200, 41, BmpFileDirectory"btn_dialog_press.bmp");

	pIMEDialogLblTitle = CreateLabel(IMEDIALOGWinX+8, IMEDIALOGWinY+9, 100, 16, pIMEDialogStrTitle);
	// SetLabelAlign(GUILABEL_ALIGN_CENTER, pIMEDialogLblTitle);

	pIMEDialogLblInfo = CreateLabel(IMEDIALOGWinX + 70, IMEDIALOGWinY + 177, 260, 90, pIMEDialogStr);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMEDialogLblInfo);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pIMEDialogLblInfo);
	pIMEDialogLblOk = CreateLabel(IMEDIALOGWinX + 101+50, IMEDIALOGWinY + 260+12, 100, 16, pIMEDialogStrOk);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pIMEDialogLblOk);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMEDialogLblOk);
	SetPictureEnable(0, pIMEDialogBtnOk);	
	
    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
	/* 拼音的文本颜色，黑色 */
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblPinYin);

	//设置键盘上字体的颜色，黑色
	for(i=0; i<MAXCHINESENUM; i++)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblChinese[i]);
	}	

	//设置键盘上字体的颜色，黑色
	for(i=0; i<MAXKEYNUM; i++)
	{
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblKey[i]);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pIMELblKey[i]);
	}

    //为了与另存为保持一致，在非中韩文时en为灰色
    if((g_usLanguage != LANG_CHINASIM)&&(g_usLanguage != LANG_KOREAN))
    {
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pIMELblKey[CNENNUM]);
    }
	
	//设置输入显示框内的字体，黑色
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblInStr);

	pIMEScroll = CreateScroll(MAXCHINESENUM+MAXKEYNUM+2, IMESInitScroll);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmIME, 
                  pFrmIME);    
	/* 注册中文候选区域的控件 */
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pIMEBtnPageUp, 
                  pFrmIME);	
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pIMEBtnPageDown, 
                  pFrmIME);	
	for(i=0; i<MAXCHINESENUM; i++)
	{
	    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pIMEBtnChinese[i], 
                      pFrmIME);
	}	
    //注册桌面上的控件
	for(i=0; i<MAXKEYNUM; i++)
	{
	    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pIMEBtnKey[i], 
                      pFrmIME);
	}

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pIMEDialogBtnOk, 
              		pFrmIME);	

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmIME, 
                    IMEWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmIME, 
                    IMEWndKey_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KNB_CLOCK, pFrmIME, 
                    IMEWndKnob_Clock, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KNB_ANTI, pFrmIME, 
                    IMEWndKnob_Anti, NULL, 0, pMsg);
	/* 注册中文候选区域的消息处理 */
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pIMEBtnPageUp, 
                    IMEBtnPageUp_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pIMEBtnPageDown, 
                    IMEBtnPageDown_Down, NULL, 0, pMsg);	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pIMEBtnPageUp, 
                    IMEBtnPageUp_Up, NULL, 0, pMsg);	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pIMEBtnPageDown, 
                    IMEBtnPageDown_Up, NULL, 0, pMsg);	
	for(i=0; i<MAXCHINESENUM; i++)
	{	
	    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pIMEBtnChinese[i], 
	                    IMEBtnSelectChinese_Down, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pIMEBtnChinese[i], 
	                    IMEBtnSelectChinese_Up, NULL, i, pMsg);		
	}	
    //注册桌面上控件的消息处理
	for(i=0; i<MAXKEYNUM; i++)
	{	
	    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pIMEBtnKey[i], 
	                    IMEBtnKey_Down, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pIMEBtnKey[i], 
	                    IMEBtnKey_Up, NULL, i, pMsg);		
	}

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pIMEDialogBtnOk, 
                    IMEDialogBtnOk_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pIMEDialogBtnOk, 
                    IMEDialogBtnOk_Up, NULL, 0, pMsg);

    return iReturn;
}


/***
  * 功能：
        窗体frmime的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmIMEExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    int i = 0;
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmIME = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmIME);
	
    //销毁桌面上的控件
    DestroyPicture(&pIMESetBg);	
	
    /* 销毁用户输入的拼音的背景及Label控件 */
	DestroyPicture(&pIMEBtnPinYin);
	DestroyLabel(&pIMELblPinYin);	
	
    /* 销毁显示输入的字符的背景及Label控件 */
	DestroyPicture(&pIMEBgInStr);
	DestroyLabel(&pIMELblInStr);

	/* 销毁中文候选区域的控件 */
	DestroyPicture(&pIMEBtnPageUp);
	DestroyPicture(&pIMEBtnPageDown);
	for(i=0; i<MAXCHINESENUM; i++)
	{	
		DestroyPicture(&pIMEBtnChinese[i]);
		DestroyLabel(&pIMELblChinese[i]);	
	}	

    /* 销毁按键的背景及Label控件 */	
	for(i=0; i<MAXKEYNUM; i++)
	{	
		DestroyPicture(&pIMEBtnKey[i]);
		DestroyLabel(&pIMELblKey[i]);	
	}	

    //释放文本资源
    //***************************************************************/
    IMETextRes_Exit(NULL, 0, NULL, 0);

	DestroyScroll(&pIMEScroll);

	//释放对话框上的控件
	DestroyPicture(&pIMEDialogBg);
	DestroyPicture(&pIMEDialogBtnType);
	DestroyPicture(&pIMEDialogBtnOk);

	DestroyLabel(&pIMEDialogLblTitle);	
	DestroyLabel(&pIMEDialogLblInfo);	
	DestroyLabel(&pIMEDialogLblOk);	

    return iReturn;
}


/***
  * 功能：
        窗体frmime的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmIMEPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

	int i = 0;
    unsigned int language = GetCurrLanguageSet();
    //得到当前窗体对象
    pFrmIME = (GUIWINDOW *) pWndObj;

	DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
    //显示桌面上的控件
    DisplayPicture(pIMESetBg);
	
    /* 显示 显示输入的字符的背景及Label控件 */
	DisplayPicture(pIMEBgInStr);
	DisplayLabel(pIMELblInStr);
	/* 显示中文候选区域 */
	DisplayPicture(pIMEBtnPageUp);
	DisplayPicture(pIMEBtnPageDown);
	for(i=0; i<MAXCHINESENUM; i++)
	{	
		DisplayPicture(pIMEBtnChinese[i]);
		DisplayLabel(pIMELblChinese[i]);	
	}
    /* 显示按键的背景及Label控件 */	
	for(i=0; i<MAXKEYNUM; i++)
	{	
		DisplayPicture(pIMEBtnKey[i]);
        DisplayLabel(pIMELblKey[i]);
	}
    //除简体中文和韩语以外，其他语言禁止语言切换
	if((language != LANG_CHINASIM)&&(language != LANG_KOREAN))
	{
        SetPictureEnable(0, pIMEBtnKey[CNENNUM]);
        SetLabelEnable(0, pIMELblKey[CNENNUM]);
	}
#ifdef BTN_SCROLL_DOWNOK

#endif	

	SetPowerEnable(0, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}


/***
  * 功能：
        窗体frmime的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmIMELoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	unsigned short g_usTmpUnicodeBuff[MAXINPUTNUM]; //输入的字符的缓存，UCS-2格式
	int i = 0;
	int iLen = 0;

    //禁止并停止窗体循环
    //SendWndMsg_LoopDisable(pWndObj);

	//用于计时
	static int iCurrTimeClicks = 0;	

	//记录时间，实现闪烁和更新进度条
	iCurrTimeClicks = getTimeCount();
	/* 未显示对话框 */
	if(!g_ucDialogIMEFlg)
	{
		static int iLastTimeClicks = 0;
		//时间间隔大于等于2秒
		if ((iCurrTimeClicks - iLastTimeClicks) >= 1)
		{
			static unsigned char ucFlg = 0;
			if(ucFlg)
			{
				ucFlg = 0;
				StringCpy1(g_usTmpUnicodeBuff, g_usUnicodeBuff);
				iLen = StringStrlen(g_usTmpUnicodeBuff);
				for(i=GiCursorPos; i<iLen; i++)
				{
					g_usTmpUnicodeBuff[i] = g_usUnicodeBuff[i+1];
				}
				//g_usTmpUnicodeBuff[GiCursorPos] = (unsigned short)' ';
				DispInputStr(g_usTmpUnicodeBuff);
				// RefreshScreen(__FILE__, __func__, __LINE__);
			}
			else
			{
				ucFlg = 1;
				DispInputStr(g_usUnicodeBuff);
				// RefreshScreen(__FILE__, __func__, __LINE__);
			}
			iLastTimeClicks = iCurrTimeClicks;
		}	
	}

    return iReturn;
}


/***
  * 功能：
        窗体frmime的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmIMEPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmime的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmIMEResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    	 窗体frmime中的初始化文本资源、 释放文本资源函数定义部分		  **
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
static int IMETextRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	int i = 0;
	
	/* 输入的文本 */
	pIMEStrKeyInput = GetUnicode(g_usUnicodeBuff);

	/* 拼音的文本 */
	pIMEStrPinYin = TransString("");

	/* 中文候选区域的文本 */
	for(i=0; i<MAXCHINESENUM; i++)
	{
		pIMEStrChineseName[i] = TransString("");
	}	
	   
	/* 键盘上的文本 */
	for(i=0; i<MAXKEYNUM; i++)
	{
		pIMEStrKeyName[i] = TransString(GcKeyName1[i][GcShiftNum]);
		pIMEStrKeyHanName[i] = TransString(GcKeyName2[i][GcShiftNum]);
	}
	pIMEStrKeyName[CNENNUM] = TransString("EN");

	//初始化对话框上的信息
	pIMEDialogStrTitle = TransString("Warning");
	pIMEDialogStrOk = GetCurrLanguageText(SYSTEM_LBL_CONFIRM);

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
static int IMETextRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	int i = 0;
	
	/* 输入的文本 */
	GuiMemFree(pIMEStrKeyInput);

	/* 拼音的文本 */
	GuiMemFree(pIMEStrPinYin);

	/* 中文候选区域的文本 */
	for(i=0; i<MAXCHINESENUM; i++)
	{
		GuiMemFree(pIMEStrChineseName[i]);
	}	
	    
	/* 键盘上的文本 */
	for(i=0; i<MAXKEYNUM; i++)
	{
		GuiMemFree(pIMEStrKeyName[i]);
		GuiMemFree(pIMEStrKeyHanName[i]);	
	}

	//释放对话框上的文本
    GuiMemFree(pIMEDialogStrTitle);
    GuiMemFree(pIMEDialogStr);
    GuiMemFree(pIMEDialogStrOk);	

    return iReturn;
}

static int IMESInitScroll(BTN_SCROLL *pScroll)
{
	int i = 0;

	if (NULL == pScroll)
	{
		return -1;
	}

	pScroll[0].pScrollBtn = pIMEBtnPageUp;
	pScroll[0].pScrollLbl = NULL;
	pScroll[0].pScrollBtn_Down = IMEBtnPageUp_Down;
	pScroll[0].pScrollBtn_Up = IMEBtnPageUp_Up;

	for(i=1; i<MAXCHINESENUM; i++)
	{
		pScroll[i].pScrollBtn = pIMEBtnChinese[i];
		pScroll[i].pScrollLbl = pIMELblChinese[i];
		pScroll[i].pScrollBtn_Down = IMEBtnSelectChinese_Down;
		pScroll[i].pScrollBtn_Up = IMEBtnSelectChinese_Up;		
	}	

	pScroll[0].pScrollBtn = pIMEBtnPageDown;
	pScroll[0].pScrollLbl = NULL;
	pScroll[0].pScrollBtn_Down = IMEBtnPageDown_Down;
	pScroll[0].pScrollBtn_Up = IMEBtnPageDown_Up;	

	for(i=(MAXCHINESENUM+2); i<(MAXCHINESENUM+MAXKEYNUM+2); i++)
	{
		pScroll[i].pScrollBtn = pIMEBtnKey[i];
		pScroll[i].pScrollLbl = pIMELblKey[i];
		pScroll[i].pScrollBtn_Down = IMEBtnKey_Down;
		pScroll[i].pScrollBtn_Up = IMEBtnKey_Up;		
	}
	
	return 0;
}


/*******************************************************************************
**			    			窗体的按键事件处理函数					 		  **
*******************************************************************************/
//刷新所有按键
static void RefreshBtnStatus(int iStatus)
{
    //iStatus:0,normal,1:pressed
    int i;
    for (i = 0; i < (MAXCHINESENUM+MAXKEYNUM+2); i++)
    {
        if (iStatus)
            ShowBtn_Down(i);
        else
            ShowBtn_Up(i);
    }
}

/***
  * 功能：
		按键down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEWndKey_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    RefreshBtnStatus(0);
    return iReturn;
}


/***
  * 功能：
		按键up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEWndKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	int index = 0;

	switch (uiValue)
	{
	case KEYCODE_UP:
		if (iIMECurrSelected >= (MAXCHINESENUM + 2))
		{
			index = (iIMECurrSelected <= 18) ? (iIMECurrSelected - (MAXCHINESENUM + 2))
											 : (iIMECurrSelected - 11);
			IMEShowScrollBtnDown(index, &iIMECurrSelected, pIMEScroll);
		}
		break;
	case KEYCODE_DOWN:
		index = (iIMECurrSelected < (MAXCHINESENUM + 2)) ? (iIMECurrSelected + (MAXCHINESENUM + 2))
														 : (iIMECurrSelected + 11);
		if (index > (MAXKEYNUM - 1 + MAXCHINESENUM + 2))
		{
			index = MAXKEYNUM - 1 + MAXCHINESENUM + 2;
		}
		IMEShowScrollBtnDown(index, &iIMECurrSelected, pIMEScroll);
		break;
	case KEYCODE_LEFT:
		IMEWndKnob_Anti(NULL, 0, NULL, 0);
		break;
	case KEYCODE_RIGHT:
		IMEWndKnob_Clock(NULL, 0, NULL, 0);
		break;
	case KEYCODE_ENTER:
		HandleEnterKeyCallBack();
		break;
	case KEYCODE_ESC:
		if (g_ucDialogIMEFlg) //建立对话框
		{
			IMEDialogBtnOk_Down(NULL, 0, NULL, 0);
			IMEDialogBtnOk_Up(NULL, 0, NULL, 0);
		}
		else
		{
			ShowKey_FuncUp(ESCNUM);
			CancelHandle();
		}
		break;
	default:
		break;
	}

    RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

/***
  * 功能：
		完成旋钮顺时针旋转的操作
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEWndKnob_Clock(void *pInArg, int iInLen,
                         		   void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	int iPrev = iIMECurrSelected;

	iIMECurrSelected ++;
	if(iIMECurrSelected>(MAXCHINESENUM+MAXKEYNUM+1))
	{
		iIMECurrSelected = 0;
	}

	IMEScrollScrollUpOrDown(iPrev, iIMECurrSelected, pIMEScroll);
	
	// RefreshScreen(__FILE__, __func__, __LINE__);					

	return iReturn;
}


/***
  * 功能：
		完成旋钮逆时针旋转的操作
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEWndKnob_Anti(void *pInArg, int iInLen,
                         		  void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	int iPrev = iIMECurrSelected;
	iIMECurrSelected --;
    
	if(iIMECurrSelected < 0)
	{
		iIMECurrSelected = MAXCHINESENUM+MAXKEYNUM+1;
	}
    
	IMEScrollScrollUpOrDown(iPrev, iIMECurrSelected, pIMEScroll);
	
	// RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/*******************************************************************************
**							对话框上控件的事件处理函数						  **
*******************************************************************************/
/***
  * 功能：
		对话框上OK按钮down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEDialogBtnOk_Down(void *pInArg,   int iInLen, 
						  void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	TouchChange("btn_dialog_press.bmp", pIMEDialogBtnOk, NULL, pIMEDialogLblOk, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);	

	return iReturn;
}


/***
  * 功能：
		对话框上OK按钮up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEDialogBtnOk_Up(void *pInArg,   int iInLen, 
						  void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	g_ucDialogIMEFlg = 0;

	TouchChange("btn_dialog_unpress.bmp", pIMEDialogBtnOk, NULL, pIMEDialogLblOk, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);	

	ChangeAbleInArea(1, OBJTYP_GUIPICTURE, 120, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	IMEExit();	

	return iReturn;
}



/*******************************************************************************
**							键盘上的按钮的事件处理函数						  **
*******************************************************************************/
/***
  * 功能：
		键盘上按钮的down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/

static int IMEBtnKey_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    int iTemp = iOutLen;

	IMEShowScrollBtnDown(iTemp, &iIMECurrSelected, pIMEScroll);
	iTemp = iTemp - MAXCHINESENUM - 2;

	if(GcKeyType==0)
	{
		/* 中英文切换键 */
		if(iTemp == CNENNUM)
		{
			/* 暂用作中英文切换按键 */
			if(g_ucEnCnFlg == 0)
			{
				//英文状态下，按下切换键
				if(g_usLanguage == LANG_CHINASIM)
				{
					g_ucEnCnFlg = 1;
					SetPictureBitmap(NORMAL_KEY_PRESSED_BMP, pIMEBtnKey[iTemp]);
					GuiMemFree(pIMEStrKeyName[iTemp]);
					pIMEStrKeyName[iTemp] = TransString("CN");
				}
				else if (g_usLanguage == LANG_KOREAN)
				{
					g_ucEnCnFlg = 2;
					GuiMemFree(pIMEStrKeyName[iTemp]);
					pIMEStrKeyName[iTemp] = TransString("KR");

					int i = 0;
					for (i = 0; i < MAXKEYNUM; i++)
					{
						SetLabelText(pIMEStrKeyHanName[i], pIMELblKey[i]);
						DisplayPicture(pIMEBtnKey[i]);
						DisplayLabel(pIMELblKey[i]);
					}
				}

				SetLabelText(pIMEStrKeyName[iTemp], pIMELblKey[iTemp]);
				DisplayPicture(pIMEBtnKey[iTemp]);
				DisplayLabel(pIMELblKey[iTemp]);
			}
			else
			{			
				g_ucEnCnFlg = 0;
				GuiMemFree(pIMEStrKeyName[iTemp]);
				pIMEStrKeyName[iTemp] = TransString("EN");
				int i = 0;
				for (i = 0; i < MAXKEYNUM; i++)
				{
					SetLabelText(pIMEStrKeyName[i], pIMELblKey[i]);
					DisplayPicture(pIMEBtnKey[i]);
					DisplayLabel(pIMELblKey[i]);
				}

				SetLabelText(pIMEStrKeyName[iTemp], pIMELblKey[iTemp]);
				DisplayPicture(pIMEBtnKey[iTemp]);
				DisplayLabel(pIMELblKey[iTemp]);
				
				memset(g_cPinyin, 0, sizeof(g_cPinyin));
				DisplayPinYin();
				PinYinToChinese();
				DisplayWordList();	
			}
		}		
	}	

	/* 中文输入法 */
	if(g_ucEnCnFlg == 1)
	{
		/* 删除键 */
		if(iTemp == BACKNUM)
		{
			DeletePinYinChar();
			DisplayPinYin();
			PinYinToChinese();
			DisplayWordList();	
			DispInputStr(g_usUnicodeBuff);
		}
		/* 回车键 */
		else if(iTemp == ENTERNUM)
		{
			IMEShowScrollBtnUp(ENTERNUM, pIMEScroll);
			OkHandle();
		}
		/* shift键 */
		else if(iTemp == SHIFTNUM)
		{	
			if(GcShiftNum == 0)
			{
				GcShiftNum = 1;
				DispShift();
			}
			else
			{
				GcShiftNum = 0;
				DispShift();			
			}
		}
		/* Esc键 */
		else if(iTemp == ESCNUM)
		{
			ShowKey_FuncUp(ESCNUM);
			CancelHandle();
		}
		/* 字符输入按键 */
		else
		{
			/* 键盘类型为全键盘或字母键盘 */
			if(GcKeyType==0)
			{
				/* 输入的是字母 */
				if((iTemp>=11 && iTemp<=20) ||
				   (iTemp>=22 && iTemp<=30) ||
				   (iTemp>=34 && iTemp<=40))
				{
					InsertPinYinChar(GcKeyName[iTemp][GcShiftNum]);
					DisplayPinYin();
					PinYinToChinese();
					DisplayWordList();
				}
				/* 输入的是数字以及符号 */
				else
				{
					InsertChar(GcKeyName[iTemp][GcShiftNum]);
				}
			}
			/* 键盘类型为数字键盘 */
			if(GcKeyType==1)
			{
				//数字键盘应输入0~9，再加上小数点，42、47是按键小数点，该键是重复的，需修改。
				if ((iTemp >= 0 && iTemp <= 9 && GcShiftNum == 0) || ((iTemp == 42 || iTemp == 47) && GcShiftNum == 0))
				{
					InsertChar(GcKeyName[iTemp][GcShiftNum]);
				}
			}
			DispInputStr(g_usUnicodeBuff);
		}		
	}
	else if(g_ucEnCnFlg == 2)
	{
		/* 删除键 */
		if(iTemp == BACKNUM)
		{
			DeleteChar();
			DispInputStr(g_usUnicodeBuff);
		}
		/* 回车键 */
		else if(iTemp == ENTERNUM)
		{
			IMEShowScrollBtnUp(ENTERNUM, pIMEScroll);
			OkHandle();
		}
		/* shift键 */
		else if(iTemp == SHIFTNUM)
		{	
			if(GcShiftNum == 0)
			{
				GcShiftNum = 1;
				DispShift();
			}
			else
			{
				GcShiftNum = 0;
				DispShift();			
			}
		}
		/* Esc键 */
		else if(iTemp == ESCNUM)
		{
			ShowKey_FuncUp(ESCNUM);
			CancelHandle();
		}
		/* 其他按键处理 */
		else
		{
			/* 全键盘按键处理 */
			if(GcKeyType==0)
			{
				/* 韩语需组合判断字符 */
				if((iTemp>=14 && iTemp<=23) ||
				   (iTemp>=27 && iTemp<=35) ||
				   (iTemp>=40 && iTemp<=46))
				{
					GUICHAR *code = TransString(GcKeyName2[iTemp][GcShiftNum]);
					
					InsertHanChar(*code);
					GuiMemFree(code);
				}
				/* 可直接插入字符*/
				else
				{
					InsertChar(GcKeyName[iTemp][GcShiftNum]);
				}
			}
			/* 数字键盘 */
			if(GcKeyType==1)
			{
				//数字键盘应输入0~9，再加上小数点，42、47是按键小数点，该键是重复的，需修改。
				if ((iTemp >= 0 && iTemp <= 9 && GcShiftNum == 0) || ((iTemp == 42 || iTemp == 47) && GcShiftNum == 0))
				{
					InsertChar(GcKeyName[iTemp][GcShiftNum]);
				}
			}
			DispInputStr(g_usUnicodeBuff);
		}		
	}
	else
	/* 其他语言 */
	{
		/* 删除键 */
		if(iTemp == BACKNUM)
		{
			DeleteChar();
			DispInputStr(g_usUnicodeBuff);
		}
		/* 回车键 */
		else if(iTemp == ENTERNUM)
		{
			IMEShowScrollBtnUp(ENTERNUM, pIMEScroll);
			OkHandle();
		}
		/* shift键 */
		else if(iTemp == SHIFTNUM)
		{	
			if(GcShiftNum == 0)
			{
				GcShiftNum = 1;
				DispShift();
			}
			else
			{
				GcShiftNum = 0;
				DispShift();			
			}
		}
		/* Esc键*/
		else if(iTemp == ESCNUM)
		{
			ShowKey_FuncUp(ESCNUM);
			CancelHandle();
		}
		/* 字符输入按键 */
		else
		{
			switch(GcKeyType)
			{
				case 0:	/* 全键盘 */
					InsertChar(GcKeyName[iTemp][GcShiftNum]);
					break;
				case 1: /* 数字键盘 */
					//数字键盘应输入0~9，再加上小数点，42、47是按键小数点，该键是重复的，需修改。
					if ((iTemp >= 0 && iTemp <= 9 && GcShiftNum == 0) || ((iTemp == 42 || iTemp == 47) && GcShiftNum == 0))
					{
						InsertChar(GcKeyName[iTemp][GcShiftNum]);
					}
					break;
				case 2:	/* 字母键盘 */
					//if(iTemp<1 || iTemp>10 || GcShiftNum==1 )
					//{
					//	InsertChar(GcKeyName[iTemp][GcShiftNum]);
					//}
					InsertChar(GcKeyName[iTemp][GcShiftNum]);
					break;
				default :
					break;
			}
			DispInputStr(g_usUnicodeBuff);
		}	
	}

	// RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
		中文候选区域上一页down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnPageUp_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;	

	IMEShowScrollBtnDown(0, &iIMECurrSelected, pIMEScroll);	
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
		中文候选区域下一页down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnPageDown_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;	

	IMEShowScrollBtnDown(MAXCHINESENUM+1, &iIMECurrSelected, pIMEScroll);			
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
		选择候选区域中文的down事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnSelectChinese_Down(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;	
    //临时变量定义
    int iTemp = iOutLen;	

	IMEShowScrollBtnDown(iTemp, &iIMECurrSelected, pIMEScroll);
	iTemp -= 1;

	if(NULL == pIMELblChinese[iTemp]->pLblText->pTextData)
	{
		return -1;
	}
	else
	{
		if(pIMELblChinese[iTemp]->pLblText->pTextData[0] == (GUICHAR)'\0')
		{
			return -2;
		}
	}	
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
		键盘上按钮的up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnKey_Up(void *pInArg,   int iInLen, 
                        void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    int iTemp = iOutLen;

	if(g_ucKnobFlg)
	{
		g_ucKnobFlg = 0;
		IMEShowScrollBtnUp(iTemp, pIMEScroll);	
		iTemp = iTemp - MAXCHINESENUM - 2;
	}
	else
	{
		IMEShowScrollBtnUp(iTemp+MAXCHINESENUM+2, pIMEScroll);	
	}	

	/* 回车键 */
	if(iTemp == ENTERNUM)
	{
		OkHandle();
	}
	/* Esc键 */
	if(iTemp == ESCNUM)
	{
		CancelHandle();
	}	

	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
		中文候选区域上一页up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnPageUp_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;	

	IMEShowScrollBtnUp(0, pIMEScroll);	

	WordListTurnPage(0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
		中文候选区域下一页up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnPageDown_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;		

	IMEShowScrollBtnUp(MAXCHINESENUM+1, pIMEScroll);	

	WordListTurnPage(1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/***
  * 功能：
		选择候选区域中文的up事件
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int IMEBtnSelectChinese_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;	
    //临时变量定义
    int iTemp = iOutLen;	
	IMEShowScrollBtnUp(iTemp, pIMEScroll);
	ShowBtn_Down(iTemp);//使选中项高亮，便于识别
	iTemp -=1;

	if(NULL == pIMELblChinese[iTemp]->pLblText->pTextData)
	{
		return -1;
	}
	else
	{
		if(pIMELblChinese[iTemp]->pLblText->pTextData[0] == (GUICHAR)'\0')
		{
			return -2;
		}
	}

	InsertChar(pIMELblChinese[iTemp]->pLblText->pTextData[0]);
	DispInputStr(g_usUnicodeBuff);
	memset(g_cPinyin, 0, sizeof(g_cPinyin));
	DisplayPinYin();
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}


/*********************************************************************************************************
** 函数名  :         ShowKey_FuncDown
** 功  能  :         显示按键按下
** 输入参数:         index : 按键的索引号
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void ShowKey_FuncDown(int index)
{	
	if(index == BACKNUM)
	{
	    SetPictureBitmap(BACK_KEY_PRESSED_BMP, pIMEBtnKey[index]);
    	DisplayPicture(pIMEBtnKey[index]);
	}
	else if(index == ENTERNUM || index == ESCNUM)
	{
	    SetPictureBitmap(SPECIAL_KEY_PRESSED_BMP, pIMEBtnKey[index]);
    	DisplayPicture(pIMEBtnKey[index]);
		ChangeLalColour(pIMELblKey[index]);	
	}
    else if(index == SHIFTNUM)
    {
		if (GcShiftNum == 0)//小写
		{
			SetPictureBitmap(SHIFTBIG_PRESSED_BMP, pIMEBtnKey[index]);//按下
		}
		else//大写
		{
			SetPictureBitmap(SHIFT_KEY_PRESSED_BMP, pIMEBtnKey[index]);//按下
		}
    	DisplayPicture(pIMEBtnKey[index]);
		DisplayLabel(pIMELblKey[index]);
    }
	else if(index == SPACENUM)
	{
	    SetPictureBitmap(SPACE_KEY_PRESSED_BMP, pIMEBtnKey[index]);
    	DisplayPicture(pIMEBtnKey[index]);
	}
	else
	{
        if((LANG_CHINASIM == g_usLanguage)&&(CNENNUM == index))
        {
             if(1 == g_ucEnCnFlg)//选择为中文
             {
 				pIMEStrKeyName[index] = TransString("CN");
				SetPictureBitmap(NORMAL_KEY_PRESSED_BMP, pIMEBtnKey[index]);
             }
             else
             {
 				pIMEStrKeyName[index] = TransString("EN");
				SetPictureBitmap(NORMAL_KEY_PRESSED_BMP, pIMEBtnKey[index]);
             }
 
 			SetLabelText(pIMEStrKeyName[index], pIMELblKey[index]);
            DisplayPicture(pIMEBtnKey[index]);
			DisplayLabel(pIMELblKey[index]);
        }
        else
        {
            SetPictureBitmap(NORMAL_KEY_PRESSED_BMP, pIMEBtnKey[index]);
    	    DisplayPicture(pIMEBtnKey[index]);

    		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pIMELblKey[index]);
    	    DisplayLabel(pIMELblKey[index]);
    		// SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblKey[index]);
        }
	}	
}

/*********************************************************************************************************
** 函数名  :         ShowKey_FuncUp
** 功  能  :         显示按键弹起
** 输入参数:         index : 按键的索引号
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void ShowKey_FuncUp(int index)
{	
	if(index == BACKNUM)
	{
	    SetPictureBitmap(BACK_KEY_BMP, pIMEBtnKey[index]);
    	DisplayPicture(pIMEBtnKey[index]);
	}
	else if(index == ENTERNUM || index == ESCNUM)
	{
	    SetPictureBitmap(SPECIAL_KEY_BMP, pIMEBtnKey[index]);
    	DisplayPicture(pIMEBtnKey[index]);
		DisplayLabel(pIMELblKey[index]);	
	}
    else if(index == SHIFTNUM)
    {
		if (GcShiftNum == 0)//小写
		{
			SetPictureBitmap(SHIFT_KEY_BMP, pIMEBtnKey[index]);//未按下
		}
		else//大写
		{
			SetPictureBitmap(SHIFTBIG_KEY__BMP, pIMEBtnKey[index]);//未按下
		}
    	DisplayPicture(pIMEBtnKey[index]);
		DisplayLabel(pIMELblKey[index]);
    }
	else if(index == SPACENUM)
	{
	    SetPictureBitmap(SPACE_KEY_BMP, pIMEBtnKey[index]);
    	DisplayPicture(pIMEBtnKey[index]);
	}
	else
	{
	    if((LANG_CHINASIM == g_usLanguage)&&(CNENNUM==index))
        {
			if (1 == g_ucEnCnFlg)//选择为中文
			{
				pIMEStrKeyName[index] = TransString("CN");
				SetPictureBitmap(NORMAL_KEY_BMP, pIMEBtnKey[index]);
			}
			else
			{
				pIMEStrKeyName[index] = TransString("EN");
				SetPictureBitmap(NORMAL_KEY_BMP, pIMEBtnKey[index]);
			}

			SetLabelText(pIMEStrKeyName[index], pIMELblKey[index]);
			DisplayPicture(pIMEBtnKey[index]);
			DisplayLabel(pIMELblKey[index]);
		}
        else
        {
            SetPictureBitmap(NORMAL_KEY_BMP, pIMEBtnKey[index]);
            DisplayPicture(pIMEBtnKey[index]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblKey[index]);
            DisplayLabel(pIMELblKey[index]);
        }
	}	
}

/***
  * 功能：
        显示键盘按钮按下
  * 参数：
        1.GUIPICTURE *pBtnFx:   需要显示的键盘按钮图形框
        2.GUILABEL *pLblFx:     需要显示的键盘按钮标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int ShowBtn_Down(int iIndex)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	if(iIndex == 0)
	{
		SetPictureBitmap(BmpFileDirectory"btn_pageup_press.bmp", pIMEBtnPageUp);
		DisplayPicture(pIMEBtnPageUp);
	}
	else if((iIndex>=1) && (iIndex<=MAXCHINESENUM))
	{
		SetPictureBitmap(BmpFileDirectory"btn_pinyin_press.bmp", pIMEBtnChinese[iIndex-1]);
    	DisplayPicture(pIMEBtnChinese[iIndex-1]);

		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pIMELblChinese[iIndex-1]);
	    DisplayLabel(pIMELblChinese[iIndex-1]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pIMELblChinese[iIndex-1]);
	}
	else if(iIndex == (MAXCHINESENUM+1))
	{
		SetPictureBitmap(BmpFileDirectory"btn_pagedown_press.bmp", pIMEBtnPageDown);
		DisplayPicture(pIMEBtnPageDown);
	}
	else if((iIndex>=(MAXCHINESENUM+2)) && (iIndex<(MAXCHINESENUM+MAXKEYNUM+2)))
	{
		ShowKey_FuncDown(iIndex-MAXCHINESENUM-2);
	}

    return iReturn;
}

/***
  * 功能：
        显示键盘按钮按下
  * 参数：
        1.GUIPICTURE *pBtnFx:   需要显示的键盘按钮图形框
        2.GUILABEL *pLblFx:     需要显示的键盘按钮标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int ShowBtn_Up(int iIndex)
{
    //错误标志、返回值定义
    int iReturn = 0;

	if(iIndex == 0)
	{
		SetPictureBitmap(BmpFileDirectory"btn_pageup_unpress.bmp", pIMEBtnPageUp);
		DisplayPicture(pIMEBtnPageUp);
	}
	else if((iIndex>=1) && (iIndex<=MAXCHINESENUM))
	{
		SetPictureBitmap(BmpFileDirectory"btn_pinyin_unpress.bmp", pIMEBtnChinese[iIndex-1]);
    	DisplayPicture(pIMEBtnChinese[iIndex-1]);

	    DisplayLabel(pIMELblChinese[iIndex-1]);
	}
	else if(iIndex == (MAXCHINESENUM+1))
	{
		SetPictureBitmap(BmpFileDirectory"btn_pagedown_unpress.bmp", pIMEBtnPageDown);
		DisplayPicture(pIMEBtnPageDown);
	}
	else if((iIndex>=(MAXCHINESENUM+2)) && (iIndex<(MAXCHINESENUM+MAXKEYNUM+2)))
	{
		ShowKey_FuncUp(iIndex-MAXCHINESENUM-2);
	}	

    return iReturn;
}

/*********************************************************************************************************
** 函数名  :         ChangeLalColour
** 功  能  :         改变标签文本的颜色为白色
** 输入参数:         pLblFx : 需要改变颜色的标签
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void ChangeLalColour(GUILABEL *pLblFx)
{
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLblFx);
    DisplayLabel(pLblFx);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLblFx);
}

//处理enter键函数
static void HandleEnterKeyCallBack(void)
{
	if (g_ucDialogIMEFlg)//建立对话框
	{
		IMEDialogBtnOk_Down(NULL, 0, NULL, 0);
		IMEDialogBtnOk_Up(NULL, 0, NULL, 0);
	}
	else
	{
		if (iIMECurrSelected == 0)
		{
			IMEBtnPageUp_Down(NULL, 0, NULL, 0);
			IMEBtnPageUp_Up(NULL, 0, NULL, 0);
		}
		else if (iIMECurrSelected == (MAXCHINESENUM+1))
		{
			IMEBtnPageDown_Down(NULL, 0, NULL, 0);
			IMEBtnPageDown_Up(NULL, 0, NULL, 0);
		}
		else if (iIMECurrSelected > 0 && iIMECurrSelected <= MAXCHINESENUM)
		{
			IMEBtnSelectChinese_Down(NULL, 0, NULL, iIMECurrSelected);
			IMEBtnSelectChinese_Up(NULL, 0, NULL, iIMECurrSelected);
		}
		else
		{
			IMEBtnKey_Down(NULL, 0, NULL, iIMECurrSelected);
		}
	}
}

/*********************************************************************************************************
** 函数名  :         DispShift
** 功  能  :         按下shift按键后，更新按键上显示的文字
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void DispShift(void)
{
	int i = 0;

	for(i=0; i<MAXKEYNUM; i++)
	{
		if(i != CNENNUM)
		{
			pIMEStrKeyName[i] = TransString(GcKeyName1[i][GcShiftNum]);
			SetLabelText(pIMEStrKeyName[i], pIMELblKey[i]);
			DisplayPicture(pIMEBtnKey[i]);
			DisplayLabel(pIMELblKey[i]);
			
			if(g_usLanguage == LANG_KOREAN && g_ucEnCnFlg == 2)
			{
				pIMEStrKeyHanName[i] = TransString(GcKeyName2[i][GcShiftNum]);
				SetLabelText(pIMEStrKeyHanName[i], pIMELblKey[i]);
				DisplayPicture(pIMEBtnKey[i]);
				DisplayLabel(pIMELblKey[i]);
			}
		}
	}	
}

/*********************************************************************************************************
** 函数名  :         DispInputStr
** 功  能  :         更新显示输入的字符
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void DispInputStr(unsigned short *pBuf)
{
	pIMEStrKeyInput = GetUnicode(pBuf);
	SetLabelText(pIMEStrKeyInput, pIMELblInStr);
	DisplayPicture(pIMEBgInStr);
	DisplayLabel(pIMELblInStr);	
}

/*********************************************************************************************************
** 函数名  :         InsertCha
** 功  能  :         增加字符
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void InsertChar(unsigned short usChar)
{
	int i = 0;
	int iInStrLength = 1;	
	int iLenUniBuff = 0;
	
	i = StringStrlen(g_usUnicodeBuff);

	//去除光标
	iLenUniBuff = StringStrlen(g_usUnicodeBuff) - 1;
			
	/* 检查已输入字符是否达到最大缓冲 */
	if( i>= GiMaxInputNum )
	{
		return;
	}
	for(; i>GiCursorPos; i--)
	{
		g_usUnicodeBuff[i] = g_usUnicodeBuff[i-1];
	}
	g_usUnicodeBuff[GiCursorPos] = usChar;
	GiCursorPos++;
	
	iInStrLength = StringStrlen(g_usUnicodeBuff);
	
	g_usUnicodeBuff[iInStrLength] = (unsigned short)'\0';
}
/*********************************************************************************************************
** 函数名  :         InsertHanCha
** 功  能  :         增加韩文字符
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/

static void InsertHanChar(unsigned short usChar)
{
	int i = 0;
	int iInStrLength = 1;	
	int iLenUniBuff = 0;
	
	i = StringStrlen(g_usUnicodeBuff);

	//去除光标
	iLenUniBuff = StringStrlen(g_usUnicodeBuff) - 1;
	/* 检查已输入字符是否达到最大缓冲 */
	if( i>=GiMaxInputNum )
	{
		return;
	}
	
	//韩文光标 0尾部 1首部 2中部
	int posIsHead = 0;
	if(isJamo(usChar))
	{
		//韩文光标在首部 
		//韩文输入时，头插插时， '|'AB >> C'|'AB	
		if((GiCursorPos == 0) && (i > 1))
		{
			posIsHead = 1;
		}
		else if(GiCursorPos != i-1)
		{
			posIsHead = 2;
		}
		unsigned short inputunicode[2] = { 0 };
		unsigned short outunicode[2] = { 0 };
		
		//取上一个元素 即光标的 前一个
		if(GiCursorPos > 0)
		{
			inputunicode[0] = g_usUnicodeBuff[GiCursorPos-1];
		}
		else
		{
			inputunicode[0] = 0;
		}
		inputunicode[1] = usChar;
			
		int ret = hangulIME(inputunicode, outunicode);
		//把显示框中最后一个字符删除，替代成刚刚组合出来的最新的字符
		if (ret == 1)
        {	
			if(posIsHead == 1)		//    '|'A    B'|'A
           	{
           		for(; i>GiCursorPos; i--)
				{
					g_usUnicodeBuff[i] = g_usUnicodeBuff[i-1];
				}
				GiCursorPos++;
				g_usUnicodeBuff[GiCursorPos-1] = outunicode[0];
			}
			else			
			{
            	if(GiCursorPos > 0)//	A'|'		@'|'    或      A'|'B      @'|'B
            	{            		
            		g_usUnicodeBuff[GiCursorPos-1] = outunicode[0];					
				}
				else			  //	'|'         A'|'
				{
					GiCursorPos++;
					g_usUnicodeBuff[GiCursorPos] = g_usUnicodeBuff[GiCursorPos-1];				
					g_usUnicodeBuff[GiCursorPos-1] = outunicode[0];	
				}
			}
        }
		//把显示框中最后一个字符删除，显示框上追加输出来的两个字符
        else if (ret == 2)//如果返回值为2，返回了2个字符
        {
            if(posIsHead == 2)   //		A'|'B     AC'|'B
            {
            	for(; i>GiCursorPos; i--)
				{
					g_usUnicodeBuff[i] = g_usUnicodeBuff[i-1];
				}
				GiCursorPos++;
				
				g_usUnicodeBuff[GiCursorPos-2] = outunicode[0];
				g_usUnicodeBuff[GiCursorPos-1] = outunicode[1];	
            }
           	else				//       A'|'      AB'|'
           	{
				GiCursorPos++;
				g_usUnicodeBuff[GiCursorPos] = g_usUnicodeBuff[GiCursorPos-1];
				
				g_usUnicodeBuff[GiCursorPos-2] = outunicode[0];
				g_usUnicodeBuff[GiCursorPos-1] = outunicode[1];	
           	}
        }
        else//这种情况下，也就是说无法进行组合，显示框最后一个字符可能不是韩语字符
        {
            //所以只要往显示框追加新按键的字符就行了
			if(posIsHead == 0)
			{
				GiCursorPos++;
				g_usUnicodeBuff[GiCursorPos] = g_usUnicodeBuff[GiCursorPos-1];
				
            	g_usUnicodeBuff[GiCursorPos-1] = usChar;
			}
			else if(posIsHead == 1)
			{
				for(; i>GiCursorPos; i--)
				{
					g_usUnicodeBuff[i] = g_usUnicodeBuff[i-1];
				}
				GiCursorPos++;
				
				g_usUnicodeBuff[GiCursorPos-1] = usChar;
			}
			else
			{
				for(; i>GiCursorPos; i--)
				{
					g_usUnicodeBuff[i] = g_usUnicodeBuff[i-1];
				}
				GiCursorPos++;
				
				g_usUnicodeBuff[GiCursorPos-1] = usChar;	
			}  
        }
	}
	
	iInStrLength = StringStrlen(g_usUnicodeBuff);	
	g_usUnicodeBuff[iInStrLength] = (unsigned short)'\0';
}

/*********************************************************************************************************
** 函数名  :         InsertPinYinChar
** 功  能  :         中文输入法状态下增加拼音字母输入
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void InsertPinYinChar(char cChar)
{
	int iPinYinLen = 0;

	iPinYinLen = strlen(g_cPinyin);
	if(iPinYinLen < PINYIN_VALID_MAXLEN)
	{
		iPinYinLen = strlen(g_cPinyin);
		g_cPinyin[iPinYinLen] = cChar;
		g_cPinyin[iPinYinLen+1] = '\0';
	}
}


/*********************************************************************************************************
** 函数名  :         InsertCha
** 功  能  :         删除字符
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void DeleteChar(void)
{
	int i = 0;
	int j = 0;

	i = StringStrlen(g_usUnicodeBuff);
	if(GiCursorPos == 0)
	{
		return;
	}
	GiCursorPos--;
	for(j=GiCursorPos; j<i; j++)
	{
		g_usUnicodeBuff[j] = g_usUnicodeBuff[j+1];
	}
}

/*********************************************************************************************************
** 函数名  :         InsertPinYinCha
** 功  能  :         中文输入法状态下删除拼音字母
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         如果拼音字母不为空，则删除拼音字母，否则删除已输入的字符
*********************************************************************************************************/
static void DeletePinYinChar(void)
{
	int iPinYinLen = 0;

	iPinYinLen = strlen(g_cPinyin);
	
	if(iPinYinLen != 0)
	{
		iPinYinLen = strlen(g_cPinyin);
		g_cPinyin[iPinYinLen-1] = '\0';
	}
	/* 在中文输入法状态下，输入的拼音字母已删除完毕，则删除已输入的字符 */
	else
	{
		DeleteChar();
	}

}


/*********************************************************************************************************
** 函数名  :         CursorLeftMove
** 功  能  :         光标左移
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
// static void CursorLeftMove(void)
// {
// 	unsigned short usTmp;
// 	GiCursorPos--;
// 	if(GiCursorPos<0)
// 	{
// 		GiCursorPos = 0;
// 	}
// 	else
// 	{
// 		usTmp = g_usUnicodeBuff[GiCursorPos+1];
// 		g_usUnicodeBuff[GiCursorPos+1] = g_usUnicodeBuff[GiCursorPos];
// 		g_usUnicodeBuff[GiCursorPos] = usTmp;
// 	}
// }

/*********************************************************************************************************
** 函数名  :         CursorRightMove
** 功  能  :         光标右移
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
// static void CursorRightMove(void)
// {
// 	unsigned short usTmp;
// 	int iInStrLength = 1;	
// 
// 	iInStrLength = StringStrlen(g_usUnicodeBuff);
// 	
// 	GiCursorPos++;
// 	if(GiCursorPos>=iInStrLength)
// 	{
// 		GiCursorPos = iInStrLength-1;
// 	}
// 	else
// 	{
// 		usTmp = g_usUnicodeBuff[GiCursorPos];
// 		g_usUnicodeBuff[GiCursorPos] = g_usUnicodeBuff[GiCursorPos-1];
// 		g_usUnicodeBuff[GiCursorPos-1] = usTmp;
// 	}
// }

/*********************************************************************************************************
** 函数名  :         DeleteAChar
** 功  能  :         删除输入缓存区中额外加入的光标字符，将输入的UCS-2字符串转换为UTF-8格式
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
void DeleteAChar(unsigned short *pString)
{
	int i = 0;
	int j = 0;

	i = StringStrlen(pString);
	for(j=GiCursorPos; j<i; j++)
	{
		pString[j] = pString[j+1];
	}
}

#if 0
/***
  * 功能：
        删除文件名结尾额外的空格
  * 参数：
        1.unsigned short *pString:	宽字符串
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int DeleteEndSpace(unsigned short *pString)
{
	int iErr = 0;
	int iLen = 0;

	if(iErr == 0)
	{
		if(NULL == pString)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		for(;;)
		{
			iLen = StringStrlen(pString);
			if(pString[iLen-1] == ' ')
			{
				pString[iLen-1] = '\0';
			}
			else
			{
				break;
			}
		}
	}

	return iErr;
}

#endif
/*********************************************************************************************************
** 函数名  :         IMEExit
** 功  能  :         输出法退出
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static int  IMEExit(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

	GUIWINDOW *pWnd = NULL;

	memset(g_cPinyin, 0, sizeof(g_cPinyin));

    (*IMEcallBackFunc)(&pWnd);
    SendWndMsg_WindowExit(pFrmIME);      //发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);       //发送消息以便调用新的窗体

    return iReturn;
}

/*********************************************************************************************************
** 函数名  :         OkHandle
** 功  能  :         按下enter按钮后执行的函数
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void OkHandle(void)
{		
	if(IMEOkcallBackFunc != NULL)
	{
		(*IMEOkcallBackFunc)();			//执行按下ENTER后的回调函数
		
		if(g_ucDialogIMEFlg == 0)			//没有建立对话框
		{
			IMEExit();		
		}					
	}
	else
	{
		IMEExit();						
	}					
}

/*********************************************************************************************************
** 函数名  :         CancelHandle
** 功  能  :         按下Cancel按钮后执行的函数
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
static void CancelHandle(void)
{
	if(IMECancelcallBackFunc != NULL)
	{
		(*IMECancelcallBackFunc)();		//执行按下ESC后的回调函数
		
		if(g_ucDialogIMEFlg == 0)		//没有建立对话框
		{
			IMEExit();		
		}					
	}
	else
	{
		IMEExit();							
	}						
}


/*********************************************************************************************************
** 函数名  :         IMEInit
** 功  能  :         建立对话框
** 输入参数:         pInitInStr: 初始化输入缓存区，可输入NULL
**					 maxstr    : 允许输入的最大字符长度，其最大长度为 MAXINPUTNUM-2
**					 keytype   : 键盘的类型，0:全键盘 1:只有数字有效 2:只有字母和数字有效
**					 func	   : 还原窗体的回调函数
**					 okfunc	   : 按下enter按钮后执行的回调函数，可输入NULL
**					 cancelfunc: 按下cancel按钮后执行的回调函数，可输入NULL
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
void IMEInit(char *pInitInStr, int maxstr, char keytype,
			 IMECALLLBACK func, IMECALLLBACKHANDLE okfunc, IMECALLLBACKHANDLE cancelfunc)
{
    GUIWINDOW *pWnd = NULL;
	GiMaxInputNum = maxstr+1;	//把闪烁光标计算在内
	GcKeyType = keytype;
	IMEcallBackFunc = func;
	IMEOkcallBackFunc = okfunc;
	IMECancelcallBackFunc = cancelfunc;
	unsigned short *pUnicode = NULL;
	//char *pTmp = NULL;

	/* 默认为英文输入 */
	g_ucEnCnFlg = 0;

	/* 初始化输入缓冲 */
	if(pInitInStr == NULL)
	{
		memset(g_usUnicodeBuff, 0, sizeof(g_usUnicodeBuff));
		g_usUnicodeBuff[0] = (unsigned short)'|';	
		g_usUnicodeBuff[1] = 0; 
		GiCursorPos   = 0;
	}
	else
	{
		memset(g_usUnicodeBuff, 0, sizeof(g_usUnicodeBuff));
		/* 将输入的UTF-8字符串转换为UCS-2字符串 */
		pUnicode = GetUtf8ToUcs2(pInitInStr);
		StringCpy1(g_usUnicodeBuff, pUnicode);
		GuiMemFree(pUnicode);

		/* 计算光标的位置 */
		GiCursorPos = StringStrlen(g_usUnicodeBuff);

		/* 在宽字符串的末尾插入| */
		pUnicode = TransString("|");
		StringCat1(g_usUnicodeBuff, pUnicode);
		GuiMemFree(pUnicode);
        /*
		pTmp = strstr(pInitInStr, ".");
		if( (pTmp!=NULL) && (pTmp!=pInitInStr) )
		{
			CursorLeftMove();
			CursorLeftMove();
			CursorLeftMove();
			CursorLeftMove();	
		}*/
	}

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmIMEInit , FrmIMEExit, 
                        FrmIMEPaint, FrmIMELoop, 
					    FrmIMEPause, FrmIMEResume,
                        NULL);          //pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());	    //发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
}


int IMEShowScrollBtnDown(int index, int *pCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}

	// ShowBtn_Up(*pCurr);
	ShowBtn_Down(index);
	*pCurr = index;

	return 0;
}

int IMEShowScrollBtnUp(int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}

	// ShowBtn_Down(iCurr);
	ShowBtn_Up(iCurr);
    
	return 0;
}

int IMEScrollScrollUpOrDown(int iPrev, int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}
	// ShowBtn_Up(iPrev);	
	ShowBtn_Down(iCurr);

	return 0;
}

/***
  * 功能：
        显示用户输入的拼音
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DisplayPinYin(void)
{
	//错误标志、返回值定义
	int iReturn = 0;	

	pIMEStrPinYin = TransString(g_cPinyin);
    SetLabelText(pIMEStrPinYin, pIMELblPinYin);
	DisplayPicture(pIMEBtnPinYin);
    DisplayLabel(pIMELblPinYin);	

	return iReturn;	
}

/***
  * 功能：
        显示中文候选区域
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int DisplayWordList(void)
{	
	//错误标志、返回值定义
	int iReturn = 0;
	int iTemp = 0;
	GUICHAR Word[2] = {0, 0};

	if (NULL == pMainWordSet)
    {
    	for (iTemp = 0; iTemp < MAXCHINESENUM; iTemp++)
	    {
			pIMEStrChineseName[iTemp] = TransString("");
	        SetLabelText(pIMEStrChineseName[iTemp], pIMELblChinese[iTemp]);
			DisplayPicture(pIMEBtnChinese[iTemp]);
	        DisplayLabel(pIMELblChinese[iTemp]);
	    }
        return -1;
    }

    if (pMainWordSet->iViewLen > MAXCHINESENUM)
    {
        return -2;
    }	

    for (iTemp = pMainWordSet->iHeadPos; 
         iTemp < pMainWordSet->iTailPos; 
         iTemp++)
    {
        Word[0] = pMainWordSet->pWordCode[iTemp];
		StringCpy1(pIMEStrChineseName[iTemp - pMainWordSet->iHeadPos], Word);
        SetLabelText(pIMEStrChineseName[iTemp - pMainWordSet->iHeadPos], 
					 pIMELblChinese[iTemp - pMainWordSet->iHeadPos]);
		DisplayPicture(pIMEBtnChinese[iTemp - pMainWordSet->iHeadPos]);
        DisplayLabel(pIMELblChinese[iTemp - pMainWordSet->iHeadPos]);
    }

    for (iTemp = pMainWordSet->iTailPos - pMainWordSet->iHeadPos;
         iTemp < MAXCHINESENUM;
         iTemp++)
    {
		pIMEStrChineseName[iTemp] = TransString("");
        SetLabelText(pIMEStrChineseName[iTemp], pIMELblChinese[iTemp]);
		DisplayPicture(pIMEBtnChinese[iTemp]);
        DisplayLabel(pIMELblChinese[iTemp]);
    }	

	return iReturn;
}

/***
  * 功能：
        实现候选中文区域的翻页功能
  * 参数：
        1.int iTurnFlag:	0:向上翻页	1.向下翻页
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WordListTurnPage(int iTurnFlag)
{
	//错误标志、返回值定义
	int iReturn = 0;	

    if (NULL == pMainWordSet)
    {
        return -1;
    }

	/* 向上翻页 */
    if (0 == iTurnFlag)
    {
    	/* 已到达第一页 */
        if (0 == pMainWordSet->iHeadPos)
        {
            return iReturn;
        }

        pMainWordSet->iHeadPos -= pMainWordSet->iViewLen;
        if (pMainWordSet->iHeadPos < 0)
        {
            pMainWordSet->iHeadPos = 0;
        }
    }
	/* 向下翻页 */
    else
    {
    	/* 已到达最后一页 */
        if (pMainWordSet->iTailPos == pMainWordSet->iWordCount)
        {
            return iReturn;
        }

        pMainWordSet->iHeadPos += pMainWordSet->iViewLen;
        if (pMainWordSet->iHeadPos >= pMainWordSet->iWordCount)
        {
            pMainWordSet->iHeadPos -= pMainWordSet->iViewLen;
        }
    }    
    pMainWordSet->iTailPos = pMainWordSet->iHeadPos + pMainWordSet->iViewLen;
    if (pMainWordSet->iTailPos > pMainWordSet->iWordCount)
    {
        pMainWordSet->iTailPos = pMainWordSet->iWordCount;
    }

    DisplayWordList();

    return iReturn;	
}


/***
  * 功能：
        根据输入的拼音检索出对应的汉字
  * 参数：
        1.int iTurnFlag:	0:向上翻页	1.向下翻页
  * 返回：
        成功返回零，失败返回非零值
  * 备注：检索出的汉字存贮在pMainWordSet结构体指针当中
***/
static int PinYinToChinese(void)
{
	//错误标志、返回值定义
	int iReturn = 0;	

	if (NULL != pMainWordSet)
    {
        if (pMainWordSet->iWordCount)
        {
            GuiMemFree(pMainWordSet->pWordCode);
        }
        GuiMemFree(pMainWordSet);
    }
    pMainWordSet = QueryWordPinyin(g_cPinyin, MAXCHINESENUM);

	return iReturn;	
}


/***
  * 功能：
        创建软键盘中的对话框
  * 参数：
        1.GUICHAR *pTextStr:	需要显示的警告信息
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		当软键盘输入了一个不合法的值后，需要弹出警告对话框，
  		此时涉及到了多重窗体，返回时容易出问题，不能使用通用的对话框，
  		为此专门为软键盘建立了一个对话框供调用。
***/
int CreateIMEDialog(GUICHAR *pTextStr)
{
	int iReturn = 0;

	if(iReturn == 0)
	{
		if(pTextStr == NULL)
		{
			iReturn = -1;
		}
	}

	if(iReturn == 0)
	{
		SendWndMsg_LoopDisable(GetCurrWindow());
		MsecSleep(50);
	
		/* 置位对话框建立标志 */
		g_ucDialogIMEFlg = 1;
		
		pIMEDialogStr = pTextStr;

		ChangeAbleInArea(0, OBJTYP_GUIPICTURE, 120, 90, 400, 300);
		DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		SetPictureEnable(1, pIMEDialogBtnOk);

		//显示对话框上的控件
		SetLabelText(pIMEDialogStr, pIMEDialogLblInfo);
		DisplayPicture(pIMEDialogBg);
		DisplayPicture(pIMEDialogBtnType);
		DisplayPicture(pIMEDialogBtnOk);

		DisplayLabel(pIMEDialogLblTitle);	
		DisplayLabel(pIMEDialogLblInfo);	
		DisplayLabel(pIMEDialogLblOk);	

		RefreshScreen(__FILE__, __func__, __LINE__);		
	}	

	return iReturn;
}


/***
  * 功能：
		获取软件盘输入，剔除了前后的空格
  * 参数：
		无
  * 返回：
		0、成功
	  非0、失败	
  * 备注：
  		自己保证pBuff缓冲区足够大
***/
int GetIMEInputBuff(char *pBuff)
{
	int iErr = 0;

	char InputBuff[MAXINPUTNUM];			//输入的字符的缓存，UTF-8格式

	if(iErr == 0)
	{
		if(NULL == pBuff)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		DeleteAChar(g_usUnicodeBuff);
		//DeleteEndSpace(g_usUnicodeBuff);
		StringUcs2TrimSpace(g_usUnicodeBuff);
		GetUcs2ToUtf8(g_usUnicodeBuff, InputBuff);
		strcpy(pBuff, InputBuff);
	}	

	return iErr;
}

/***
  * 功能：
		获取软件盘原始输入
  * 参数：
		无
  * 返回：
		0、成功
	  非0、失败	
  * 备注：
  		自己保证pBuff缓冲区足够大
***/

int GetIMEInputBuffWithoutSpaceTrim(char *pBuff)
{
	char InputBuff[MAXINPUTNUM];			//输入的字符的缓存，UTF-8格式

	if(NULL == pBuff)
	{
		return -1;
	}

	DeleteAChar(g_usUnicodeBuff);
	GetUcs2ToUtf8(g_usUnicodeBuff, InputBuff);
	strcpy(pBuff, InputBuff);

	return 0;
}



