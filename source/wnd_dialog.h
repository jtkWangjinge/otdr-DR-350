/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_dialog.h
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
#ifndef _WND_DIALOG_H_
#define _WND_DIALOG_H_

#include "guiglobal.h"


//Dialog图层，所有继承的控件必须将其内部包含的要接受事件的控件的图层设为该值
#define DIALOG_GUI_LAYER                0x10

//Dialog类型
typedef struct _dialog
{
    GUIVISIBLE Visiable;            //可视信息

    GUIPICTURE* Background;         //背景
    GUIMESSAGE* MsgHandlerList;     //注册的消息处理函数队列

    int HoldTime;                   //自动窗口的保持时间 单位为ms,默认为0(手动关闭)
    struct timeval EnterTime;       //进入时间，只有在设置了保持时间的情况下有效

    int WinOpacity;                 //窗体背景透明度，0(不透明) - 255(全透明)
    int Exit;                       //退出标志
    int LoopBackup;                 //保存原有窗体是否有loop消息，用于退出的时候恢复原有窗体的loop

    int (*Show)(void* _this);       //虚函数，子类实现时必须调用基类的默认实现
    void (*Destruct)(void* _this);  //虚函数，子类实现时必须调用基类的默认实现
} Dialog;

//虚函数类型
typedef int (*DialogShow)(void* _this);
typedef void (*DialogDestruct)(void* _this);


//构造一个Dialog
void Dialog_Construct(Dialog* _this, int x, int y, int w, int h);

//析构一个Dialog
void Dialog_Destruct(Dialog* _this);

//显示一个Dialog
void Dialog_Show(Dialog* _this);

//执行一个Dialog，注意:该函数执行完成之后会自动析构Dialog对象内部资源
//对于动态申请的Dialog，只需要在该函数返回后free相关的指针即可，对于局部变量的Dialog无需任何操作
//对于阻塞式的Dialog来说，这种策略符合绝大多数的使用场景
void Dialog_Exec(Dialog* _this);

//退出一个Dialog
void Dialog_Close(Dialog* _this);

//设置Dialog弹出时窗体背景透明度:0(不透明) - 255(全透明)
void Dialog_SetWinOpacity(Dialog* _this, int opacity);

//设置Dialog背景图片
void Dialog_SetBackground(Dialog* _this, const char* image);

//设置Dialog弹出保持的时间
void Dialog_SetHoldTime(Dialog* _this, int mesc);

//添加窗体组件队列
int Dialog_AddWindowComp(Dialog* _this, int type, int len, void* reciver);

//注册控件的消息处理函数
int Dialog_LoginMsgHandler(Dialog* _this, int code, void* reciver, MSGFUNC handler, void* outArg, int outLen);

//给指定的图像缓冲区混合颜色 图像缓冲区为rgb565
void BlendColor565(
    unsigned short* buffer, COLOR color, unsigned char alpha, 
    unsigned int x, unsigned int y, unsigned int w, unsigned int h
    );

//给指定的图像缓冲区混合颜色 图像缓冲区为rgb888
void BlendColor888(
    unsigned char* buffer, COLOR color, unsigned char alpha, 
    unsigned int x, unsigned int y, unsigned int w, unsigned int h
    );

//混合当前的帧缓冲已达到背景半透明的效果
void Dialog_BlendCurrFbmap(Dialog* _this);

#endif //_WND_DIALOG_H_
