/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsloawidget.h
* 摘    要：  声明主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.1.0
* 作    者：  wjg
* 完成日期：  2014-12-04
*
* 取代版本：  v1.0.2
* 原 作 者：  wjg
* 完成日期：  2014-11-04
*******************************************************************************/
#ifndef _WND_FRMSOLAWIDGET_H
#define _WND_FRMSOLAWIDGET_H

#include "guiglobal.h"
#include "app_eventsearch.h"
#include "app_sola.h"

//定义按钮点击处理函数接口
typedef int (*ITEM_CHECKED_HANDLE)(int);

//轮廓信息
typedef struct _sloa_rectangle
{
    int startX;
    int startY;
    int endX;
    int endY;
} RECTANGLE;

//代表界面上的每一个可点击的事件点元素
typedef struct _sola_widget_item
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    RECTANGLE geometry;         //几何描述，为相对坐标
    RECTANGLE thumbnailGeometry;//对应的缩略图相对坐标
    RECTANGLE iconArea;         //icon 的区域 绝对坐标
    GUIPICTURE* icon;           //图标
    GUILABEL* label;            //标签
    GUIFONT* font;              //文本的字体

    int width;                  //item的宽度
    char* typeName;             //item的类型，不同类型的item的icon是不同的
    int isInWindowCombList;     //表示是否添加到了窗体的可视控件队列中
    int isChecked;              //表示是否被按下
    int isPassed;               //表示是否通过
    int index;                  //本身在_sola_widget列表中的索引，用于调用用户的回调函数的参数
    char marker[5];               //事件标识，如(A)、(B)

    float value;                //值，本项目中为所代表的实际距离

    //所属的SOLA_WIDGET对象
    struct _sola_widget* parent; 
} SOLA_WIDGET_ITEM;


//SOLA的主控件，提供缩略图以及主事件图的显示等功能
typedef struct _sola_widget
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    GUIPICTURE* background;     //背景图片资源
    GUIPICTURE* startIcon;      //其实图标
    GUILABEL* distanceLabel;    //距离标签
    GUICHAR* distanceText;      //距离文本
    GUIFONT* distanceFont;      //距离字体
    GUIPICTURE* leftArrow;      //左箭头按钮
    GUIPICTURE* rightArrow;     //右箭头按钮
    GUIPICTURE* leftBg;         //左箭头按钮
    GUIPICTURE* rightBg;        //右箭头按钮
    GUIWINDOW* window;          //所属的窗体
        
    RECTANGLE thumbnailArea;    //缩略图的绘制区域
    RECTANGLE displayArea;      //主绘制区域

    int checkedIndex;           //当前选中的item的索引
    int leftArrowEnable;        //左箭头点击使能
    int rightArrowEnable;       //右箭头点击使能
    
    int itemCount;              //事件点的个数
    SOLA_WIDGET_ITEM** items;   //指向所有item的指针数组
    float distance;             //光纤长度
    float distancePerPixel;     //每个像素代表的距离
    float thumbnailDistancePerPixel;//缩略图中每个像素代表的距离
    SolaEvents* solaEvents;     //

    //用户自定义的回调函数，每个事件点被点击的时候，会调用该函数
    int (*itemCheckedHandle)(int itemIndex);
} SOLA_WIDGET;


SOLA_WIDGET_ITEM* CreateSolaWidgetItem(const char* typeName, int index, GUICHAR* text, SOLA_WIDGET* parent);
int DisplaySolaWidgetItem(SOLA_WIDGET_ITEM* me);
int DestroySolaWidgetItem(SOLA_WIDGET_ITEM** me);
int SetSolaWidgetItemPosition(SOLA_WIDGET_ITEM* me, int x, int y);
int SetSolaWidgetItemThumbnailPosition(SOLA_WIDGET_ITEM* me, int x, int y);
int SetSolaWidgetItemChecked(SOLA_WIDGET_ITEM* me, int checked);
int SetSolaWidgetItemPassed(SOLA_WIDGET_ITEM* me, int passed);
int SetSolaWidgetItemType(SOLA_WIDGET_ITEM* me, const char* typeName);
int SetSolaWidgetItemText(SOLA_WIDGET_ITEM* me, GUICHAR* text);
int SolaWidgetItemMove(SOLA_WIDGET_ITEM* me, int xOffset, int yOffset);
int UpdateSolaWidgetItem(SOLA_WIDGET_ITEM* me);

SOLA_WIDGET* CreateSolaWidget(int x, int y, int w, int h, ITEM_CHECKED_HANDLE handle, GUIWINDOW* window);
int DisplaySolaWidget(SOLA_WIDGET* me);
int DestroySolaWidget(SOLA_WIDGET** me);
int SetSolaWidgetEvent(SOLA_WIDGET*me, SolaEvents* eventList);
void ClearSolaWidgetItems(SOLA_WIDGET* me);
void SolaWidgetItemsFocusPrev(SOLA_WIDGET* me);
void SolaWidgetItemsFocusNext(SOLA_WIDGET* me);
void SolaWidgetOnEventTypeChanged(SOLA_WIDGET* me);

#endif  //_WND_FRMSOLAWIDGET_H
