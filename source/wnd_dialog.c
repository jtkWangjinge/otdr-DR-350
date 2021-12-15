/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_dialog.c
* 摘    要：  Dialog实现，其他需要modal显示的继承自该结构
*
*
* 当前版本：  v1.0.0(初始版本)
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_dialog.h"
#include "wnd_global.h"


//内部私有函数声明
static void Dialog_ProcessMessage(Dialog* _this, GUIMESSAGE_ITEM* item);
static void Dialog_MessageLoop(Dialog* _this);
//static int Dialog_KeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);


void Dialog_Construct(Dialog* _this, int x, int y, int w, int h)
{
    //可视信息初始化
    _this->Visiable.Area.Start.x = x;
    _this->Visiable.Area.Start.y = y;
    _this->Visiable.Area.End.x = x + w - 1;
    _this->Visiable.Area.End.y = y + h - 1;
    _this->Visiable.iFocus = 1;
    _this->Visiable.iEnable = 1;
    _this->Visiable.iCursor = 1;
    _this->Visiable.iLayer = DIALOG_GUI_LAYER;

    //GUI资源初始化
    _this->Background = CreatePicture(x, y, w, h, NULL);
    _this->MsgHandlerList = CreateMessage(1000, 1000);

    //其他属性初始化
    _this->HoldTime = 0;
    _this->WinOpacity = 100;
    _this->Exit = 0;
    _this->LoopBackup = 0;

    //虚函数初始化
    _this->Show = (DialogShow)Dialog_Show;
    _this->Destruct = (DialogDestruct)Dialog_Destruct;

    //注册按键处理
    Dialog_AddWindowComp(_this, OBJTYP_GUIWINDOW, sizeof(Dialog), _this);
    //Dialog_LoginMsgHandler(_this, GUIMESSAGE_KEY_UP, _this, Dialog_KeyUp, _this, sizeof(Dialog*));  
}

void Dialog_Destruct(Dialog* _this)
{
    //清除窗体控件队列
    while (1)
    {
        void* WindowComb = ReadMessageQueue(_this->MsgHandlerList);
        if (WindowComb)
            DelWindowComp(WindowComb, GetCurrWindow());
        else
            break;
    }

    ClearMessageReg(_this->MsgHandlerList);
    DestroyMessage(&_this->MsgHandlerList);
    DestroyPicture(&_this->Background);
}

void Dialog_Show(Dialog* _this)
{
    Dialog_BlendCurrFbmap(_this);
    DisplayPicture(_this->Background);
}

void Dialog_Exec(Dialog* _this)
{
    //显示
    _this->Show(_this);
    SyncCurrFbmap();

    //进入消息循环
    Dialog_MessageLoop(_this);

    //析构
    _this->Destruct(_this);

    //刷新当前窗体
    GUIWINDOW* CurrWindow = GetCurrWindow();
    if (CurrWindow)
    {
        CurrWindow->fnWndPaint(CurrWindow);

        //恢复原有窗体的loop服务
        if (_this->LoopBackup)
        {
            SendWndMsg_WindowLoop(CurrWindow);
        }
    }
}

void Dialog_Close(Dialog* _this)
{
    _this->Exit = 1;
}

void Dialog_SetWinOpacity(Dialog* _this, int opacity)
{
    _this->WinOpacity = opacity;
}

void Dialog_SetBackground(Dialog* _this, const char* image)
{
    SetPictureBitmap((char*)image, _this->Background);
}

void Dialog_SetHoldTime(Dialog* _this, int mesc)
{
    _this->HoldTime = mesc;
}

int Dialog_AddWindowComp(Dialog* _this, int type, int len, void* reciver)
{
    WriteMessageQueue((GUIMESSAGE_ITEM*)reciver, _this->MsgHandlerList, 0);
    return AddWindowComp(type, len, reciver, GetCurrWindow());
}

int Dialog_LoginMsgHandler(Dialog* _this, int code, void* reciver, MSGFUNC handler, void* outArg, int outLen)
{
    return LoginMessageReg(code, reciver, handler, outArg, outLen, _this->MsgHandlerList);
}

void BlendColor565(
    unsigned short* buffer, COLOR color, unsigned char alpha,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h
    )
{
    int x1, y1;
    unsigned char k = 0xff - alpha;

    for (x1 = 0; x1 < h; x1++)
    {
        for (y1 = 0; y1 < w; y1++)
        {
            unsigned char b = ((*buffer) & 0xF800) >> 8;
            unsigned char g = ((*buffer) & 0x07E0) >> 3;
            unsigned char r = ((*buffer) & 0x001F) << 3;

            r = ((color & 0xff0000) >> 16)*k / 0xff + r*alpha / 0xff;
            g = ((color & 0x00ff00) >> 8)*k / 0xff + g*alpha / 0xff;
            b = ((color & 0x0000ff) >> 0)*k / 0xff + b*alpha / 0xff;

            *buffer++ = ((r & 0xF8) >> 3) |   //RGB565，B分量:低5位
                ((g & 0xFC) << 3) |   //RGB565，G分量:中间6位
                ((b & 0xF8) << 8);    //RGB565，B分量:高5位
        }
    }
}

void BlendColor888(
    unsigned char* buffer, COLOR color, unsigned char alpha,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h
    )
{
    int x1, y1;
    unsigned char k = 0xff - alpha;

    for (x1 = 0; x1 < h; x1++)
    {
        for (y1 = 0; y1 < w; y1++)
        {
            buffer[2] = ((color & 0xff0000) >> 16)*k / 0xff + buffer[2] * alpha / 0xff;
            buffer[1] = ((color & 0x00ff00) >> 8)*k / 0xff + buffer[1] * alpha / 0xff;
            buffer[0] = ((color & 0x0000ff) >> 0)*k / 0xff + buffer[0] * alpha / 0xff;

            buffer += 3;
        }
    }
}

//混合当前的帧缓冲
void Dialog_BlendCurrFbmap(Dialog* _this)
{
    //0xff 表示纯透明，不需要进行alpha通道的叠加
    if (_this->WinOpacity == 100)
        return;
    GUIFBMAP* fb = GetCurrFbmap();
    unsigned alpha = _this->WinOpacity * 0xff / 100;
    BlendColor565((unsigned short*)fb->pMapBuff, 0, alpha, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

//获取两个时间之间的毫秒差
int GetTimeMescDiff(const struct timeval* st, const struct timeval* et)
{
    return (et->tv_sec - st->tv_sec) * 1000 + (et->tv_usec - st->tv_usec) / 1000;
}

static void Dialog_ProcessMessage(Dialog* _this, GUIMESSAGE_ITEM* item)
{
    //根据不同窗体消息编码进行相应处理
    //对于Dialog而言，窗体有关的消息一概忽略
    switch (item->iMsgCode)
    {
    case GUIMESSAGE_WND_INIT:
    case GUIMESSAGE_WND_EXIT:
    case GUIMESSAGE_WND_PAINT:
        break;
    case GUIMESSAGE_WND_LOOP:
        _this->LoopBackup = 1;
        break;
    case GUIMESSAGE_WND_PAUSE:
    case GUIMESSAGE_WND_RESUME:
    case GUIMESSAGE_LOP_ENABLE:
    case GUIMESSAGE_LOP_DISABLE:
        //do nothing
        break;

        //进行其他窗体消息分发处理处理(按键、触摸屏等消息) 
        //为了不影响已经存在的窗体注册消息队列，Dilaog有单独的消息注册队列
    default:
        ExecMessageProc(item, _this->MsgHandlerList);
        break;
    }
}

//消息循环
static void Dialog_MessageLoop(Dialog* _this)
{
    GUIMESSAGE* Message = GetCurrMessage();

    //判断是否设置了保持时间，是则记录进入时间
    if (_this->HoldTime)
    {
        gettimeofday(&_this->EnterTime, NULL);
    }

    //进入消息处理循环
    while (!_this->Exit)
    {
        //Dilaog为阻塞式的，在此处拦截了系统所有的窗体消息，进行自定义分发处理
        GUIMESSAGE_ITEM* Item = RecvSpecialMessage(GUIMESSAGE_TYP_WND, Message);
        if (NULL != Item)
        {
            Dialog_ProcessMessage(_this, Item);
            free(Item);
        }
        else
        {
            //没有消息的情况下休眠以节省CPU时间
            MsecSleep(10);
        }

        //判断是否设置了保持时间，是则记录当前事件并且计算Dialog已经保持的时间
        if (_this->HoldTime)
        {
            struct timeval CurrTime;
            gettimeofday(&CurrTime, NULL);

            //判断保持时间是否超时，是则关闭对话框
            if (GetTimeMescDiff(&_this->EnterTime, &CurrTime) >= _this->HoldTime)
            {
                Dialog_Close(_this);
            }
        }
    }
}
/*
static int Dialog_KeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    Dialog* _this = (Dialog*)pOutArg;
    switch (uiValue)
    {
	case KEYCODE_ESC:
        Dialog_Close(_this);
		break;
	default:
		break;
	}
	
	return iReturn;
}
*/

