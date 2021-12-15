/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsloatable.h
* 摘    要：  声明主窗体frmsolatable的窗体处理线程及相关操作函数
*
* 当前版本：  v1.1.0
* 作    者：  wjg
* 完成日期：  2014-12-04
*
* 取代版本：  v1.0.2
* 原 作 者：  
* 完成日期：  2016-11-07
*******************************************************************************/
#ifndef _WND_FRMSOLATABLE_H
#define _WND_FRMSOLATABLE_H

#include "guiglobal.h"
#include "app_eventsearch.h"
#include "app_sola.h"

typedef struct _sola_table_line
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    GUIPICTURE* background;     //背景图片资源

    GUILABEL* TitleLabel;
    GUICHAR* TitleText;

    GUILABEL* Title1310Label;
    GUICHAR* Title1310Text;
    GUILABEL* Title1550Label;
    GUICHAR* Title1550Text;
    
    GUILABEL* TitleLineLossLabel;
    GUICHAR* TitleLineLossText;
    GUILABEL* TitleLineReturnLabel;
    GUICHAR* TitleLineReturnText;    

    GUILABEL* Line1310LossLabel;        //1310链路损耗标签
    GUICHAR* Line1310LossText;          //1310链路损耗文本
    GUILABEL* Line1310ReturnLossLabel;  //1310链路损耗标签
    GUICHAR* Line1310ReturnText;        //1310链路损耗文本

    GUILABEL* Line1550LossLabel;        //1550链路损耗标签
    GUICHAR* Line1550LossText;          //1550链路损耗文本
    GUILABEL* Line1550ReturnLossLabel;  //1550链路损耗标签
    GUICHAR* Line1550ReturnText;        //1550链路损耗文本

    GUIWINDOW* parent;          //所属的窗体

    int wavelen;                //测试的波长
    
    void (*setWaveLen)(struct _sola_table_line* , int );
    void (*setTotalLoss)(struct _sola_table_line* , SolaEvents*);

} SOLA_TABLE_LINE;


//SOLATABLE的主控件，提供背景图和显示事件数据
typedef struct _sola_table_event_item
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    GUIPICTURE* background;     //背景图片资源
    
    GUIPICTURE* passIcon;       //是否通过图标

    GUILABEL* typeTitleLabel;    //事件类型标签
    GUICHAR* typeTitle;          //事件类型文本    

    GUILABEL* distanceTitleLabel;//事件距离标签
    GUICHAR* distanceTitle;      //事件距离文本    

    GUILABEL* loss1310TitleLabel;    //事件损耗标签
    GUICHAR* loss1310Title;          //事件损耗文本    
    GUILABEL* IOR1310TitleLabel;     //事件反射率标签
    GUICHAR* IOR1310Title;              //事件反射率文本    

    GUILABEL* loss1550TitleLabel;    //事件损耗标签
    GUICHAR* loss1550Title;          //事件损耗文本    
    GUILABEL* IOR1550TitleLabel;     //事件反射率标签
    GUICHAR* IOR1550Title;              //事件反射率文本    


    GUILABEL* wavelen1310Label1;      //波长1显示标签
    GUICHAR* wavelen1310Title1;       //波长1显示文本 
    GUILABEL* wavelen1310Label2;      //波长2显示标签
    GUICHAR* wavelen1310Title2;       //波长2显示文本 

    GUILABEL* wavelen1550Label1;      //波长1显示标签
    GUICHAR* wavelen1550Title1;       //波长1显示文本 
    GUILABEL* wavelen1550Label2;      //波长2显示标签
    GUICHAR* wavelen1550Title2;       //波长2显示文本 

    
    GUIPICTURE* eventIcon;      //事件图标
    GUIPICTURE* downButton;     //向下事件选择按钮
    GUIPICTURE* upButton;       //向上事件选择按钮
    GUIPICTURE* downButtonTouch;//向下事件选择按钮触控区域
    GUIPICTURE* upButtonTouch;  //向上事件选择按钮触控区域
    
    GUILABEL* distanceLabel;    //距离标签
    GUICHAR* distanceText;      //距离文本

    GUILABEL* loss1310Label;        //损耗标签
    GUICHAR* loss1310;              //损耗文本    

    GUILABEL* IOR1310Label;         //折射率标签
    GUICHAR* IOR1310;               //折射率文本    

    GUILABEL* loss1550Label;        //损耗标签
    GUICHAR* loss1550;              //损耗文本    

    GUILABEL* IOR1550Label;         //折射率标签
    GUICHAR* IOR1550;               //折射率文本    

    GUIWINDOW* parent;          //所属的窗体

    int wavelen;                //测试的波长
    int eventSelectedIndex;     //当前选中的事件index
    
    void (*setWaveLen)(struct _sola_table_event_item* , int );

    void (*setEventItem)(struct _sola_table_event_item* , SolaEvents*, int);
    
    void (*setEventItemType)(struct _sola_table_event_item* , SolaEvents*, int, int);

} SOLA_TABLE_EVENT_ITEM;


typedef struct _sola_table_pass
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    GUIPICTURE* background;     //背景图片资源

    GUILABEL* passTitleLabel;   //是否通过标签
    GUICHAR* passTitle;         //是否通过文本    

    GUIPICTURE* passIcon;       //通过图标

    GUIWINDOW* parent;          //所属的窗体

    int isPassed;                   //是否通过0---pass, 1---fail

    void (*setIsPassed)(struct _sola_table_pass* , SolaEvents* );


} SOLA_TABLE_PASS;

SOLA_TABLE_LINE* CreateSolaLineTable(
    int x, int y, int w, int h,
    GUIWINDOW* parent
    );
void DisplaySolaLineTable(SOLA_TABLE_LINE* line);
void DestroySolaLineTable(SOLA_TABLE_LINE* line);

SOLA_TABLE_EVENT_ITEM* CreateSolaEventItemTable(
    int x, int y, int w, int h,
    GUIWINDOW* parent
    );
void DisplaySolaLossTable(SOLA_TABLE_EVENT_ITEM* item);
void DestroySolaLossTable(SOLA_TABLE_EVENT_ITEM* item);

SOLA_TABLE_PASS* CreateSolaPassTable(
    int x, int y, int w, int h,
    GUIWINDOW* parent
    );
void DisplaySolaPassTable(SOLA_TABLE_PASS* pass);
void DestroySolaPassTable(SOLA_TABLE_PASS* pass);

#endif  //_WND_FRMSOLATABLE_H
