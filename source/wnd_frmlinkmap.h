/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlinkmap.c
* 摘    要：  实现主窗体wnd_frmlinkmap的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_LINK_MAP_H_
#define _WND_FRM_LINK_MAP_H_

#include "guipicture.h"
#include "guiwindow.h"
#include "guilabel.h"

#include "app_algorithm_support.h"

//定义按钮点击处理函数接口
typedef int(*ITEM_CHECKED_HANDLE)(int);

//轮廓信息
typedef struct _sloa_rectangle
{
	int startX;
	int startY;
	int endX;
	int endY;
} RECTANGLE;

//代表界面上的每一个可点击的事件点元素
typedef struct _otdr_link_map_widget_item
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

	//所属的Otdr_Link_Map对象
	struct Otdr_Link_Map* parent;
} OTDR_LINK_MAP_WIDGET_ITEM;

//链路信息
typedef struct line_info
{
	GUILABEL *titleLabel;	   	//标题标签
	GUICHAR  *titleText;		//标题文本
	GUILABEL *valueLabel; 		//数据标签
	GUICHAR  *valueText;   		//数据文本
}LINE_INFO;

typedef struct Otdr_Link_Map
{
	GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
	GUIPICTURE* pBg;
	//链路信息
	LINE_INFO lineInfo[3];		//链路信息
	GUIPICTURE* startIcon;      //起始图标
	GUILABEL* distanceLabel;    //距离标签
	GUICHAR* distanceText;      //距离文本
	GUILABEL *promptLabel;		//提示信息标签
	GUICHAR *promptText;		//提示信息文本
	GUIWINDOW* window;          //所属的窗体
	RECTANGLE thumbnailArea;    //缩略图的绘制区域
	RECTANGLE displayArea;      //主绘制区域
	int checkedIndex;           //当前选中的item的索引
	int itemCount;              //事件点的个数
	OTDR_LINK_MAP_WIDGET_ITEM** items;   //指向所有item的指针数组
	float distance;             //光纤长度
	float distancePerPixel;     //每个像素代表的距离
	float thumbnailDistancePerPixel;//缩略图中每个像素代表的距离
	//用户自定义的回调函数，每个事件点被点击的时候，会调用该函数
	//int(*itemCheckedHandle)(int itemIndex);
	int iEnabled;
	int iFocus;
}OTDR_LINK_MAP;

/***
  * 功能：
		创建一个链路地图界面
  * 参数：
		无
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
OTDR_LINK_MAP *CreateOtdrLinkMap(int iStartX, int iStartY, int iWidth, int iHeight, GUIWINDOW* window);

/***
  * 功能：
		销毁OtdrLinkMap控件
  * 参数：
		1.OTDR_LINK_MAP **pOtdrLinkMapObj : 指向OtdrLinkMap控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyOtdrLinkMap(OTDR_LINK_MAP **pOtdrLinkMapObj);

/***
  * 功能：
		显示OtdrLinkMap控件
  * 参数：
		1.OTDR_LINK_MAP *pOtdrLinkMapObj : 指向OtdrLinkMap控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayOtdrLinkMap(OTDR_LINK_MAP *pOtdrLinkMapObj, int index);

/***
  * 功能：
		设置OtdrLinkMap控件信息显示
  * 参数：
		1.OTDR_LINK_MAP *pOtdrLinkMapObj : 指向pOtdrLinkMapObj控件
		2.int iEnabled:0,不使能，1，使能（显示参数信息）
		3., GUIWINDOW *pWnd:注册到窗体上
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int SetOtdrLinkMapEnable(OTDR_LINK_MAP *pOtdrLinkMapObj, int iEnabled, GUIWINDOW *pWnd);

/***
  * 功能：
		刷新OtdrLinkMap控件
  * 参数：
		1.OTDR_LINK_MAP *pOtdrLinkMapObj : 指向pOtdrLinkMapObj控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int RefreshOtdrLinkMap(OTDR_LINK_MAP *pOtdrLinkMapObj);

//设置图标焦点位置
void SethOtdrLinkMapFocus(OTDR_LINK_MAP *pOtdrLinkMapObj, int option);

//处理事件列表
int handleEventList(OTDR_LINK_MAP* me, EVENTS_TABLE *pEvents);

//重置链路地图列表
void ResetLinkMap(OTDR_LINK_MAP *me);

#endif