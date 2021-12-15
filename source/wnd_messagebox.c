/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_messagebox.c
* 摘    要：  提供meessagebox相关的功能
*             
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*
*******************************************************************************/

#include "app_text.h"

#include "global_index.h"

#include "wnd_messagebox.h"
#include "wnd_dialog.h"
#include "wnd_global.h"


#define MessageBox_FirstButton  MessageBox_Ok
#define MessageBox_LastButton   MessageBox_Abort

static const int Width = 400;
static const int Height = 300;
static const int ButtonWidth = 200;
static const int ButtonHeight = 41;

//message box 声明
typedef struct _message_box
{
    Dialog super;          //基类 模态控件

    GUIPICTURE* Icon;
    GUILABEL* TitleLabel;
    GUILABEL* TextLabel;
    GUIFONT* Font;

    GUIPICTURE* Button1;
    GUILABEL* Button1Lbl;
    GUIPICTURE* Button2;
    GUILABEL* Button2Lbl;

    unsigned int Button;
    unsigned int ClickedButton;
} MessageBox;


//内部函数声明
void MessageBox_Show(MessageBox* _this);
void MessageBox_Destruct(MessageBox* _this);

static int Button1Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int Button1Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int Button2Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int Button2Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int MessageBoxKeyDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int MessageBoxKeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//构造
void MessageBox_Construct(MessageBox* _this, GUICHAR* title, GUICHAR* text)
{
    int x = (WINDOW_WIDTH - Width) / 2;
    int y = (WINDOW_HEIGHT - Height) / 2;

    GUICHAR* yes = GetCurrLanguageText(SYSTEM_LBL_CONFIRM);
    GUICHAR* no = GetCurrLanguageText(SYSTEM_LBL_CANCEL);

    //最先构造基类
    Dialog *dialog = (Dialog*)_this;
    Dialog_Construct(dialog, x, y, Width, Height);
    Dialog_SetBackground(dialog, BmpFileDirectory"bg_dialog.bmp");
    Dialog_SetWinOpacity(dialog, 40);

    //多态函数的实现重写
    dialog->Show = (DialogShow)MessageBox_Show;
    dialog->Destruct = (DialogDestruct)MessageBox_Destruct;

    //自身的属性初始化
    if (isFont24())
	{
		_this->Font = CreateFont(FNTFILE_BIG, 24, 24, 0x00000000, 0xFFFFFFFF);
	}
	else
	{
		_this->Font = CreateFont(FNTFILE_STD, 16, 16, 0x00000000, 0xFFFFFFFF);
	}
    _this->Icon = CreatePicture(x + 170, y + 95, 60, 50, BmpFileDirectory"bg_dialog_warring.bmp");
    _this->TitleLabel = CreateLabel(x + 8, y + 9, Width - 20, 48, title);
    // SetLabelAlign(GUILABEL_ALIGN_CENTER, _this->TitleLabel);
    
    _this->TextLabel = CreateLabel(x + 81, y + 177, 220, 90, text);
    SetLabelFont(_this->Font, _this->TextLabel);

    _this->Button1 = CreatePicture(x, y + 259, ButtonWidth, ButtonHeight, BmpFileDirectory"btn_dialog_unpress.bmp");
    _this->Button1->Visible.iLayer = DIALOG_GUI_LAYER;
    _this->Button1Lbl = CreateLabel(x + 84, y + 271, ButtonWidth, 24, yes);
    // SetLabelAlign(GUILABEL_ALIGN_CENTER, _this->Button1Lbl);
    SetLabelFont(_this->Font, _this->Button1Lbl);

    _this->Button2 = CreatePicture(x + 200, y + 259, ButtonWidth, ButtonHeight, BmpFileDirectory"btn_dialog_unpress.bmp");
    _this->Button2->Visible.iLayer = DIALOG_GUI_LAYER;
    _this->Button2Lbl = CreateLabel(x + 285, y + 271, ButtonWidth, 24, no);
    // SetLabelAlign(GUILABEL_ALIGN_CENTER, _this->Button2Lbl);
    SetLabelFont(_this->Font, _this->Button2Lbl);

    _this->ClickedButton = MessageBox_NoButton;

    Dialog_AddWindowComp(dialog, OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), _this->Button1);
    Dialog_AddWindowComp(dialog, OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), _this->Button2);

    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->Button1, Button1Down, _this, sizeof(MessageBox*));  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->Button1, Button1Up, _this, sizeof(MessageBox*));  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->Button2, Button2Down, _this, sizeof(MessageBox*));  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->Button2, Button2Up, _this, sizeof(MessageBox*));  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_KEY_DOWN, _this, MessageBoxKeyDown, _this, sizeof(MessageBox*));  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_KEY_UP, _this, MessageBoxKeyUp, _this, sizeof(MessageBox*));

    free(title);
    free(text);
    free(yes);
    free(no);

}

//析构
void MessageBox_Destruct(MessageBox* _this)
{   
    //销毁GUI资源
    DestroyPicture(&_this->Icon);
    DestroyLabel(&_this->TitleLabel);
    DestroyLabel(&_this->TextLabel);

    DestroyPicture(&_this->Button1);
    DestroyLabel(&_this->Button1Lbl);

    DestroyPicture(&_this->Button2);
    DestroyLabel(&_this->Button2Lbl);

    DestroyFont(&_this->Font);
    
    //析构基类
    Dialog_Destruct((Dialog*)_this);
}

//显示MessageBox
void MessageBox_Show(MessageBox* _this)
{
    Dialog_Show((Dialog*)_this);
    DisplayPicture(_this->Icon);
    DisplayLabel(_this->TitleLabel);
    DisplayLabel(_this->TextLabel);

    DisplayPicture(_this->Button1);
    DisplayLabel(_this->Button1Lbl);
    
    DisplayPicture(_this->Button2);
    DisplayLabel(_this->Button2Lbl);

    SetPowerEnable(0, 1);//半透明显示状态栏
}

//执行MessageBox
unsigned int MessageBox_Exec(MessageBox* _this)
{
    Dialog_Exec((Dialog*)_this);
    return _this->ClickedButton;
}

//退出MessageBox
void MessageBox_Exit(MessageBox* _this)
{
    Dialog_Close((Dialog*)_this);
}

//设置MessageBox的保持时间
void MessageBox_SetHoldTime(MessageBox* _this, int mesc)
{
    Dialog_SetHoldTime((Dialog*)_this, mesc);
}

/***
  * 功能：
        弹出一个标准的MessageBox
  * 参数：
        1.title  :标题
        2.text   :文本
  * 返回：
        执行完成返回用户点击的按钮
  * 备注：
***/
unsigned int ShowStandardMessageBox(GUICHAR* title, GUICHAR* text)
{
    MessageBox StandardMessageBox;
    MessageBox_Construct(&StandardMessageBox, title, text);
    return MessageBox_Exec(&StandardMessageBox);
}

/***
  * 功能：
        弹出一个自动关闭的的MessageBox
  * 参数：
        1.title   : 标题
        2.text    : 文本
        4.holdTime: 保持时间(ms)
  * 返回：
        执行完成返回用户点击的按钮
        自动关闭时不返回任何按钮
  * 备注：
***/
unsigned int ShowAutoCloseMessageBox(GUICHAR* title, GUICHAR* text, int holdTime)
{
    MessageBox StandardMessageBox;
    MessageBox_Construct(&StandardMessageBox, title, text);
    MessageBox_SetHoldTime(&StandardMessageBox, holdTime);
    return MessageBox_Exec(&StandardMessageBox);
}

static int Button1Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    MessageBox* _this = (MessageBox*)pOutArg;
    SetPictureBitmap(BmpFileDirectory"btn_dialog_press.bmp", _this->Button1);
    DisplayPicture(_this->Button1);
    DisplayLabel(_this->Button1Lbl);
    SyncCurrFbmap();
    return 0;
}

static int Button1Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{    
    MessageBox* _this = (MessageBox*)pOutArg;
    _this->ClickedButton = MessageBox_Yes;
    MessageBox_Exit(_this);
    return 0;   
}

static int Button2Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    MessageBox* _this = (MessageBox*)pOutArg;
    SetPictureBitmap(BmpFileDirectory"btn_dialog_press.bmp", _this->Button2);
    DisplayPicture(_this->Button2);
    DisplayLabel(_this->Button2Lbl);
    SyncCurrFbmap();
    return 0;
}

static int Button2Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    MessageBox* _this = (MessageBox*)pOutArg;
    _this->ClickedButton = MessageBox_No;
    MessageBox_Exit(_this);
    return 0;
}

static int MessageBoxKeyDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    
	switch (uiValue)
    {
	case KEYCODE_ESC:
		break;
	case KEYCODE_HOME:
		break;
    case KEYCODE_ENTER:
        break;
	default:
		break;
	}
	
	return iReturn;

}

static int MessageBoxKeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;

	switch (uiValue)
    {
	case KEYCODE_ESC:
	    Button2Up(NULL, 0, pOutArg, 0);
		break;
	case KEYCODE_HOME:
	#ifdef SAVE_SCREEN
		ScreenShot();
	#endif
		break;
    case KEYCODE_ENTER:
        Button1Up(NULL, 0, pOutArg, 0);
        break;
	default:
		break;
	}
	
	return iReturn;
}