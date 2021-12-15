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

#include "wnd_frmlinkmap.h"

#include "app_unitconverter.h"
#include "app_global.h"
#include "app_curve.h"

#include "guidraw.h"

#include "wnd_global.h"

//定义图标的宽高
#define ITEM_ICON_WIDTH         60
#define ITEM_ICON_HEIGHT        60

//绘制缩略图的距离直线
static int DrawWidgetThumbnailDistanceLine(OTDR_LINK_MAP* pOtdrLinkMapObj);
//绘制距离直线
static int DrawWidgetDistanceLine(OTDR_LINK_MAP* pOtdrLinkMapObj);
//显示item
static int DisplayLinkMapItem(OTDR_LINK_MAP_WIDGET_ITEM* me);
//绘制缩略图
static int DrawWidgetItemThumbnail(OTDR_LINK_MAP_WIDGET_ITEM* me);
//清除链路地图的items
static void ClearLinkMapItems(OTDR_LINK_MAP* pOtdrLinkMapObj);
//创建链路地图的item
static OTDR_LINK_MAP_WIDGET_ITEM* CreateLinkMapItem(const char* typeName, int index, GUICHAR* text, OTDR_LINK_MAP* parent);
//销毁链路地图的items
static int DestroyLinkMapItem(OTDR_LINK_MAP_WIDGET_ITEM** me);
//item按键响应
static int LinkMapItem_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int LinkMapItem_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//item点击
static void LinkMapItemClicked(OTDR_LINK_MAP* me, int index);
//设置链路地图缩略图的图标位置
static int SetLinkMapItemThumbnailPosition(OTDR_LINK_MAP_WIDGET_ITEM* me, int x, int y);
//设置链路地图通过图标
static int SetLinkMapItemPassed(OTDR_LINK_MAP_WIDGET_ITEM* me, int passed);
//获取链路地图的图标
static const char* GetLinkMapItemImageFile(int isPassed, int isChecked, const char* typeName);
//设置图标位置
static int SetLinkMapItemPosition(OTDR_LINK_MAP_WIDGET_ITEM* me, int x, int y);
//自适应链路地图图标位置
static void AdjustLinkMapItemsPostion(OTDR_LINK_MAP* me);
//设置选中项
static int SetLinkMapItemChecked(OTDR_LINK_MAP_WIDGET_ITEM* me, int checked);
//获取调整的位置
static int GetLinkMapItemXOffsetInDisplayArea(OTDR_LINK_MAP_WIDGET_ITEM* me);
//图标移动
static int LinkMapItemMove(OTDR_LINK_MAP_WIDGET_ITEM* me, int xOffset, int yOffset);
//移动图标位置
static void MoveLinkMapItemsPostion(OTDR_LINK_MAP* me, int xOffset, int yOffset);
//设置选中图标
static void SetLinkMapCheckIndex(OTDR_LINK_MAP* me, int index);
//获取事件类型
static int GetEventType(EVENTS_TABLE *pEvents, int index);
//获取事件类型的名字
static const char* OtdrEvents_GetEventTypeName(EVENTS_TABLE *pEvents, int index);
//获取事件损耗通过，0：通过，1：不通过
static int GetEventLossPass(EVENTS_TABLE *pEvents, int index);
//设置链路信息
static int SetlineInfo(OTDR_LINK_MAP *me, EVENTS_TABLE *pEvents);
//设置光标在事件上
static int SetFocusAtEvent(OTDR_LINK_MAP *pOtdrLinkMapObj);

//事件表
static EVENTS_TABLE * pLinkMapEvents = NULL;


/***
  * 功能：
		创建一个链路地图界面
  * 参数：
		无
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
OTDR_LINK_MAP * CreateOtdrLinkMap(int iStartX, int iStartY, int iWidth, int iHeight, GUIWINDOW* window)
{
	//错误标志定义
	int iErr = 0;
	//临时变量定义
	OTDR_LINK_MAP *pOtdrLinkMapObj = NULL;

	if (!window)
		return NULL;

	if (!iErr)
	{	//分配资源
		pOtdrLinkMapObj = (OTDR_LINK_MAP *)calloc(1, sizeof(OTDR_LINK_MAP));
		if (NULL == pOtdrLinkMapObj)
		{
			LOG(LOG_ERROR, "NULL == pOtdrLinkMapObj");
			iErr = -1;
		}
	}

	if (!iErr)
	{
		//基本信息赋值
		pOtdrLinkMapObj->visible.Area.Start.x = iStartX;
		pOtdrLinkMapObj->visible.Area.Start.y = iStartY;
		pOtdrLinkMapObj->visible.Area.End.x = iStartX + iWidth - 1;
		pOtdrLinkMapObj->visible.Area.End.y = iStartY + iHeight - 1;
		pOtdrLinkMapObj->visible.iEnable = 1;
		pOtdrLinkMapObj->visible.iLayer = 0;
		pOtdrLinkMapObj->visible.iFocus = 0;
		pOtdrLinkMapObj->visible.iCursor = 0;
		pOtdrLinkMapObj->itemCount = 0;
		pOtdrLinkMapObj->checkedIndex = 0;
		pOtdrLinkMapObj->items = NULL;
		pOtdrLinkMapObj->window = window;
		//pOtdrLinkMapObj->itemCheckedHandle = handle;
		pOtdrLinkMapObj->visible.fnDisplay = (DISPFUNC)DisplayOtdrLinkMap;
		pOtdrLinkMapObj->visible.fnDestroy = (DESTFUNC)DestroyOtdrLinkMap;
		pOtdrLinkMapObj->iFocus = 0;
		pOtdrLinkMapObj->iEnabled = 0;
		//主显示区域的排布控件排布 依次为：左边距空白-左边箭头-显示区域-右边箭头-右边距空白
        int width = iWidth - ITEM_ICON_WIDTH * 2;
        pOtdrLinkMapObj->displayArea.startX = iStartX + ITEM_ICON_WIDTH;
        pOtdrLinkMapObj->displayArea.startY = iStartY+97;
		pOtdrLinkMapObj->displayArea.endX = pOtdrLinkMapObj->displayArea.startX + width - 1;
		pOtdrLinkMapObj->displayArea.endY = pOtdrLinkMapObj->displayArea.startY + 37 - 1;
		//缩略图区域的绝对坐标
		pOtdrLinkMapObj->thumbnailArea.startX = iStartX + 20;
		pOtdrLinkMapObj->thumbnailArea.startY = iStartY + 63;
        pOtdrLinkMapObj->thumbnailArea.endX = pOtdrLinkMapObj->thumbnailArea.startX + iWidth - ITEM_ICON_WIDTH;
        pOtdrLinkMapObj->thumbnailArea.endY = pOtdrLinkMapObj->thumbnailArea.startY + 3;

		pOtdrLinkMapObj->pBg = CreatePicture(iStartX, iStartY, iWidth, iHeight, BmpFileDirectory"linkmap_bg.bmp");
		pOtdrLinkMapObj->startIcon = CreatePicture(iStartX, iStartY + 5, 28, 28, BmpFileDirectory"start_icon.bmp");
		pOtdrLinkMapObj->distanceLabel = CreateLabel(pOtdrLinkMapObj->thumbnailArea.endX + 3, iStartY + 34, 24, 16,
													 pOtdrLinkMapObj->distanceText);
		//提示信息
		pOtdrLinkMapObj->promptText = GetCurrLanguageText(OTDR_LBL_SWITCH_EVENT_TABLE);
		pOtdrLinkMapObj->promptLabel = CreateLabel(iStartX + 15, iStartY + 33 + 132 + 6, 400, 16,
												   pOtdrLinkMapObj->promptText);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrLinkMapObj->distanceLabel);
		SetLabelAlign(2, pOtdrLinkMapObj->distanceLabel);
		//链路信息
		int i;
		unsigned int lineTitleInfo[3] =
			{OTDR_LBL_TOTAL_ATT_1, OTDR_LBL_ATT_FACTOR_1, OTDR_LBL_FIBER_LENGTH};
		for (i = 0; i < 3; ++i)
		{
			//创建链路信息文本标签
			pOtdrLinkMapObj->lineInfo[i].titleText = GetCurrLanguageText(lineTitleInfo[i]);
			pOtdrLinkMapObj->lineInfo[i].titleLabel = CreateLabel(iStartX + 27 + 213*i, iStartY+11, 100, 24,
																  pOtdrLinkMapObj->lineInfo[i].titleText);
			pOtdrLinkMapObj->lineInfo[i].valueText = TransString(" ");
			pOtdrLinkMapObj->lineInfo[i].valueLabel = CreateLabel(iStartX + 110 + 213*i, iStartY + 11, 100, 24,
																  pOtdrLinkMapObj->lineInfo[i].valueText);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrLinkMapObj->lineInfo[i].titleLabel);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrLinkMapObj->lineInfo[i].valueLabel);
			// SetLabelAlign(2, pOtdrLinkMapObj->lineInfo[i].titleLabel);
			// SetLabelAlign(2, pOtdrLinkMapObj->lineInfo[i].valueLabel);
		}
	}

	return pOtdrLinkMapObj;
}

/***
  * 功能：
		销毁OtdrLinkMap控件
  * 参数：
		1.OTDR_LINK_MAP **pOtdrLinkMapObj : 指向OtdrLinkMap控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyOtdrLinkMap(OTDR_LINK_MAP **pOtdrLinkMapObj)
{
	//参数检测
	if (NULL == *pOtdrLinkMapObj)
	{
		return -1;
	}

	//清除可能的Items
	ClearLinkMapItems(*pOtdrLinkMapObj);

	//销毁GUI资源
	DestroyPicture(&((*pOtdrLinkMapObj)->pBg));
	DestroyPicture(&((*pOtdrLinkMapObj)->startIcon));
	DestroyLabel(&((*pOtdrLinkMapObj)->distanceLabel));
	DestroyLabel(&((*pOtdrLinkMapObj)->promptLabel));
	free((*pOtdrLinkMapObj)->distanceText);
	(*pOtdrLinkMapObj)->distanceText = NULL;
	free((*pOtdrLinkMapObj)->promptText);
	(*pOtdrLinkMapObj)->promptText = NULL;
	int i;
	for(i = 0; i < 3; ++i)
	{
		DestroyLabel(&((*pOtdrLinkMapObj)->lineInfo[i].titleLabel));
		DestroyLabel(&((*pOtdrLinkMapObj)->lineInfo[i].valueLabel));
		free((*pOtdrLinkMapObj)->lineInfo[i].titleText);
		(*pOtdrLinkMapObj)->lineInfo[i].titleText = NULL;
		free((*pOtdrLinkMapObj)->lineInfo[i].valueText);
		(*pOtdrLinkMapObj)->lineInfo[i].valueText = NULL;
	}
	//释放自身所占用的资源
	free(*pOtdrLinkMapObj);
	*pOtdrLinkMapObj = NULL;

	return 0;
}

/***
  * 功能：
		显示OtdrLinkMap控件
  * 参数：
		1.OTDR_LINK_MAP *pOtdrLinkMapObj : 指向OtdrLinkMap控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayOtdrLinkMap(OTDR_LINK_MAP *pOtdrLinkMapObj, int index)
{
	if (NULL == pOtdrLinkMapObj)
	{
		LOG(LOG_ERROR, "pOtdrLinkMapObj is null!!!\n");
		return -1;
	}

	int i;
	DisplayPicture(pOtdrLinkMapObj->pBg);
	DisplayPicture(pOtdrLinkMapObj->startIcon);
	//显示链路信息
	for (i = 0; i < 3; ++i)
	{
		DisplayLabel(pOtdrLinkMapObj->lineInfo[i].titleLabel);
		DisplayLabel(pOtdrLinkMapObj->lineInfo[i].valueLabel);
	}
	DrawWidgetThumbnailDistanceLine(pOtdrLinkMapObj);
	DrawWidgetDistanceLine(pOtdrLinkMapObj);
	//设置选中项
	/*SetLinkMapItemChecked(pOtdrLinkMapObj->items[pOtdrLinkMapObj->checkedIndex], 0);
	SetLinkMapItemChecked(pOtdrLinkMapObj->items[index], 1);
	pOtdrLinkMapObj->checkedIndex = index;*/
	SetLinkMapCheckIndex(pOtdrLinkMapObj, index);
	
	for (i = 0; i < pOtdrLinkMapObj->itemCount; ++i)
	{
		DisplayLinkMapItem(pOtdrLinkMapObj->items[i]);
	}

	//DisplayLabel(pOtdrLinkMapObj->distanceLabel);
	// DisplayLabel(pOtdrLinkMapObj->promptLabel);

	return 0;
}

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
int SetOtdrLinkMapEnable(OTDR_LINK_MAP *pOtdrLinkMapObj, int iEnabled, GUIWINDOW *pWnd)
{
	//检查参数
	if (NULL == pOtdrLinkMapObj || iEnabled > 1 || iEnabled < 0)
	{
		return -1;
	}

	if (pOtdrLinkMapObj->iEnabled != iEnabled)
	{
		pOtdrLinkMapObj->iEnabled = iEnabled;
	}

	return 0;
}

/***
  * 功能：
		刷新OtdrLinkMap控件
  * 参数：
		1.OTDR_LINK_MAP *pOtdrLinkMapObj : 指向pOtdrLinkMapObj控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int RefreshOtdrLinkMap(OTDR_LINK_MAP *pOtdrLinkMapObj)
{
	if (pOtdrLinkMapObj->iEnabled)
	{
		SetFocusAtEvent(pOtdrLinkMapObj);
		DisplayOtdrLinkMap(pOtdrLinkMapObj, pOtdrLinkMapObj->checkedIndex);
	}

	return 0;
}

//设置图标焦点位置
void SethOtdrLinkMapFocus(OTDR_LINK_MAP *pOtdrLinkMapObj, int option)
{
	//判断是否显示
	if (0 == pOtdrLinkMapObj->iEnabled)
	{
		return;
	}

	int checkedIndex = pOtdrLinkMapObj->checkedIndex;

	switch (option)
	{
	case 0: //left
		if (checkedIndex != 0)
		{
			checkedIndex--;
		}
		break;
	case 1: //right
		if (checkedIndex != (pOtdrLinkMapObj->itemCount - 1))
		{
			checkedIndex++;
		}
		break;
	default:
		break;
	}

	SetLinkMapCheckIndex(pOtdrLinkMapObj, checkedIndex);
	DisplayOtdrLinkMap(pOtdrLinkMapObj, checkedIndex);
	//设置界面事件点位置
	SetFocusAtEvent(pOtdrLinkMapObj);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

//绘制缩略图的距离直线
static int DrawWidgetThumbnailDistanceLine(OTDR_LINK_MAP* pOtdrLinkMapObj)
{
    //存在事件点才可操作
    if (pOtdrLinkMapObj->itemCount <= 0)
    {
        return -1;
    }
    GUIPEN* pen = GetCurrPen();
	COLOR oldColor = pen->uiPenColor;
	pen->uiPenColor = 0xCB1200;

	int iErr = DrawBlock(
		pOtdrLinkMapObj->thumbnailArea.startX, pOtdrLinkMapObj->thumbnailArea.startY,
		pOtdrLinkMapObj->thumbnailArea.endX, pOtdrLinkMapObj->thumbnailArea.endY);
	pen->uiPenColor = 0xA8A8A8;
	iErr = DrawBlock(
		pOtdrLinkMapObj->thumbnailArea.startX, pOtdrLinkMapObj->thumbnailArea.endY+3,
		pOtdrLinkMapObj->thumbnailArea.endX, pOtdrLinkMapObj->thumbnailArea.endY+6);

	pen->uiPenColor = oldColor;

	return iErr;
}

//绘制距离直线
static int DrawWidgetDistanceLine(OTDR_LINK_MAP* pOtdrLinkMapObj)
{
	if (pOtdrLinkMapObj->itemCount <= 0)
	{
		return -1;
	}

	int startX = pOtdrLinkMapObj->displayArea.startX > pOtdrLinkMapObj->items[0]->iconArea.startX ?
		pOtdrLinkMapObj->displayArea.startX : pOtdrLinkMapObj->items[0]->iconArea.startX;
	int endX = pOtdrLinkMapObj->displayArea.endX < pOtdrLinkMapObj->items[pOtdrLinkMapObj->itemCount - 1]->iconArea.startX ?
		pOtdrLinkMapObj->displayArea.endX : pOtdrLinkMapObj->items[pOtdrLinkMapObj->itemCount - 1]->iconArea.startX;

    int startY = pOtdrLinkMapObj->displayArea.startY + ITEM_ICON_HEIGHT/ 2 - 1; //调整连接线的纵坐标位置
    int endY = startY + 4 - 1;

	if ((startX >= pOtdrLinkMapObj->displayArea.startX)
		&& (endX <= pOtdrLinkMapObj->displayArea.endX)
		&& (startX < endX))
	{
		GUIPEN* pen = GetCurrPen();
		COLOR oldColor = pen->uiPenColor;
		pen->uiPenColor = 0x0672BD;
		int iErr = DrawBlock(startX, startY, endX, endY);
		pen->uiPenColor = oldColor;

		return iErr;
	}

	return -1;
}

//显示item
static int DisplayLinkMapItem(OTDR_LINK_MAP_WIDGET_ITEM* me)
{
	//绘制缩略图
	DrawWidgetItemThumbnail(me);
	//如果控件是使能的则显示控件
	if (me->visible.iEnable)
	{
		DisplayPicture(me->icon);
		DisplayLabel(me->label);
		return 0;
	}
	else
	{
		return -1;
	}
}

//绘制缩略图
static int DrawWidgetItemThumbnail(OTDR_LINK_MAP_WIDGET_ITEM* me)
{
	GUIPEN* pen = GetCurrPen();
	COLOR oldColor = pen->uiPenColor;
	//pen->uiPenColor = 0xBDBDBD;
	if (me->isPassed == 0)
	{
		pen->uiPenColor = 0x49E765;
	}
	else if (me->isPassed == 1)
	{
		pen->uiPenColor = 0xF45E5E;
	}
	else
	{
		pen->uiPenColor = 0x939393;
	}

	//绘制缩略图
	int iErr = DrawBlock(
		me->thumbnailGeometry.startX + me->parent->thumbnailArea.startX,
		me->thumbnailGeometry.startY + me->parent->thumbnailArea.startY - 7,
		me->thumbnailGeometry.endX + me->parent->thumbnailArea.startX,
		me->thumbnailGeometry.endY + me->parent->thumbnailArea.startY - 7
	);

	//如果是选中的则绘制选中框
	if (!iErr && me->isChecked)
	{
		pen->uiPenColor = 0x1A64B7;
		iErr = DrawRectangle(
			me->thumbnailGeometry.startX + me->parent->thumbnailArea.startX - 2,
			me->thumbnailGeometry.startY + me->parent->thumbnailArea.startY - 7 - 2,
			me->thumbnailGeometry.endX + me->parent->thumbnailArea.startX + 2,
			me->thumbnailGeometry.endY + me->parent->thumbnailArea.startY - 7 + 2
		);
	}

	pen->uiPenColor = oldColor;

	return iErr;
}

//清除链路地图的items
static void ClearLinkMapItems(OTDR_LINK_MAP* pOtdrLinkMapObj)
{
	//销毁可能已经存在的items
	if (pOtdrLinkMapObj->items && pOtdrLinkMapObj->itemCount > 0)
	{
		int i;
		for (i = 0; i < pOtdrLinkMapObj->itemCount; ++i)
		{
			DestroyLinkMapItem(&(pOtdrLinkMapObj->items[i]));
		}

		free(pOtdrLinkMapObj->items);
		free(pOtdrLinkMapObj->distanceText);
		pOtdrLinkMapObj->distanceText = TransString(" ");
		SetLabelText(pOtdrLinkMapObj->distanceText, pOtdrLinkMapObj->distanceLabel);
	}

	//清除关键数据的值为初始值
	pOtdrLinkMapObj->items = NULL;
	pOtdrLinkMapObj->itemCount = 0;
	pOtdrLinkMapObj->checkedIndex = 0;
	pOtdrLinkMapObj->distance = 0.0f;
	pOtdrLinkMapObj->distancePerPixel = 0.0f;
	pOtdrLinkMapObj->thumbnailDistancePerPixel = 0.0f;
}

//创建链路地图的item
static OTDR_LINK_MAP_WIDGET_ITEM* CreateLinkMapItem(const char* typeName, int index, GUICHAR* text, OTDR_LINK_MAP* parent)
{
	if (!parent)
		return NULL;

	OTDR_LINK_MAP_WIDGET_ITEM* me = (OTDR_LINK_MAP_WIDGET_ITEM*)malloc(sizeof(OTDR_LINK_MAP_WIDGET_ITEM));

	//基本信息赋值
	me->visible.iEnable = 0;
	me->visible.iFocus = 0;
	me->visible.iCursor = 0;
	me->visible.iLayer = 3;
	me->isPassed = 0;
	me->isChecked = 0;
	me->typeName = (char*)typeName;
	me->index = index;
	me->isInWindowCombList = 0;
	me->width = 80;
	me->value = 0.0f;
	me->parent = parent;
	me->visible.fnDisplay = (DISPFUNC)DisplayLinkMapItem;
	me->visible.fnDestroy = (DESTFUNC)DestroyLinkMapItem;

	//几何描述
	me->geometry.startX = 0;
	me->geometry.startY = 0;
    me->geometry.endX = ITEM_ICON_WIDTH - 1;
    me->geometry.endY = ITEM_ICON_HEIGHT - 1;

    me->thumbnailGeometry.startX = 0;
	me->thumbnailGeometry.startY = 0;
	me->thumbnailGeometry.endX = 0 + 8 - 1;
	me->thumbnailGeometry.endY = 0 + 20 - 1;

	me->iconArea.startX = 0;
	me->iconArea.startY = 0;
    me->iconArea.endX = ITEM_ICON_WIDTH - 1;
    me->iconArea.endY = ITEM_ICON_HEIGHT - 1;

    //创建GUI资源
	const char* iconBmpFile = GetLinkMapItemImageFile(me->isPassed, me->isChecked, typeName);
    me->icon = CreatePicture(0, 70, ITEM_ICON_WIDTH, ITEM_ICON_HEIGHT, (char *)iconBmpFile);
    me->label = CreateLabel(0, 0, me->width, 24, text);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, me->label);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), me->label);
	//GUILABEL中本身已经有文本的缓冲，此处释放文本
	//注册点击处理消息
	GUIMESSAGE* currMesg = GetCurrMessage();
	LoginMessageReg(
		GUIMESSAGE_TCH_DOWN, me, LinkMapItem_Down,
		(void*)me, sizeof(me), currMesg
	);
	LoginMessageReg(
		GUIMESSAGE_TCH_UP, me, LinkMapItem_Up,
		(void*)me, sizeof(me), currMesg
	);

	free(text);

	return me;
}

//销毁链路地图的items
static int DestroyLinkMapItem(OTDR_LINK_MAP_WIDGET_ITEM** me)
{
	//清除可能的窗体控件队列
	if ((*me)->isInWindowCombList)
	{
		DelWindowComp(*me, (*me)->parent->window);
	}

	GUIMESSAGE* currMesg = GetCurrMessage();
	//清除消息注册项
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, *me, currMesg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, *me, currMesg);

	//释放GUI资源
	DestroyPicture(&((*me)->icon));
	DestroyLabel(&((*me)->label));
	//DestroyFont(&(*me)->font);
	//释放自身所占用的资源
	free(*me);
	*me = NULL;

	return 0;
}

//item按键响应
static int LinkMapItem_Down(
	void *pInArg, int iInLen,
	void *pOutArg, int iOutLen
)
{
	return 0;
}

static int LinkMapItem_Up(
	void *pInArg, int iInLen,
	void *pOutArg, int iOutLen
)
{
	OTDR_LINK_MAP_WIDGET_ITEM* me = (OTDR_LINK_MAP_WIDGET_ITEM*)pOutArg;
	LinkMapItemClicked(me->parent, me->index);
	return 0;
}

//item点击
static void LinkMapItemClicked(OTDR_LINK_MAP* me, int index)
{
	if (me->checkedIndex != index)
	{
		SetLinkMapCheckIndex(me, index);
		DisplayOtdrLinkMap(me, index);
		SyncCurrFbmap();
	}
}

//设置链路地图缩略图的图标位置
static int SetLinkMapItemThumbnailPosition(OTDR_LINK_MAP_WIDGET_ITEM* me, int x, int y)
{
	me->thumbnailGeometry.startX = x;
	me->thumbnailGeometry.startY = y;
	me->thumbnailGeometry.endX = x + 8 - 1;
	me->thumbnailGeometry.endY = y + 25 - 1;

	return 0;
}

//设置链路地图通过图标
static int SetLinkMapItemPassed(OTDR_LINK_MAP_WIDGET_ITEM* me, int passed)
{
	if (passed != me->isPassed)
	{
		me->isPassed = passed;
		const char* iconBmpFile = GetLinkMapItemImageFile(passed, me->isChecked, me->typeName);

		SetPictureBitmap((char*)iconBmpFile, me->icon);
	}

	return 0;
}

//获取链路地图的图标
static const char* GetLinkMapItemImageFile(int isPassed, int isChecked, const char* typeName)
{
	static const char* itemCheckedString[] =
	{
		"unchecked",
		"checked"
	};

	static const char* itemPassedString[] =
	{
		"passed",
		"unpassed"
	};

	static char fileName[128];

	const char* passedString = NULL;
	if (isPassed == 2) {
		passedString = "unknow";
	}
	else if ((isPassed == 1) || (isPassed == 0)) {
		passedString = itemPassedString[isPassed];
	}

	sprintf(
		fileName, BmpFileDirectory"type_%s_%s_%s.bmp",
		typeName,
		itemCheckedString[isChecked],
		passedString
	);

	return fileName;
}

//设置图标位置
static int SetLinkMapItemPosition(OTDR_LINK_MAP_WIDGET_ITEM* me, int x, int y)
{
	int displayAreaStartX = me->parent->displayArea.startX;
	int displayAreaStartY = me->parent->displayArea.startY;
	int displayAreaWidth = me->parent->displayArea.endX - displayAreaStartX + 1;
	//修改几何描述信息
	me->geometry.startX = x;
	me->geometry.startY = y;
    me->geometry.endX = ITEM_ICON_WIDTH - 1;
    me->geometry.endY = ITEM_ICON_HEIGHT - 1;

    //item区域超出显示区域（左边，右边），则清除使能标志，并将其从窗体的控件队列摘下
    if ((x <= (0 - ITEM_ICON_WIDTH)) || (x >= displayAreaWidth))
    {
		me->visible.iEnable = 0;
		if (me->isInWindowCombList)
		{
			DelWindowComp(me, me->parent->window);
			me->isInWindowCombList = 0;
		}
		//icon的位置
		int w = me->icon->Visible.Area.End.x - me->icon->Visible.Area.Start.x;
		int h = me->icon->Visible.Area.End.y - me->icon->Visible.Area.Start.y;
		int leftTopX = displayAreaStartX + x;
		int leftTopY = displayAreaStartY + y;

		leftTopX += (me->width - w) >> 1;
        leftTopY += ITEM_ICON_HEIGHT - h;
        me->iconArea.startX = leftTopX;
		me->iconArea.startY = leftTopY;
		me->iconArea.endX = leftTopX + w - 1;
		me->iconArea.endY = leftTopY + h - 1;
	}
	else
	{
		//label的位置
		int w = me->label->Visible.Area.End.x - me->label->Visible.Area.Start.x;
		int h = me->label->Visible.Area.End.y - me->label->Visible.Area.Start.y;
		int leftTopX = displayAreaStartX + x;
		int leftTopY = displayAreaStartY + y;
        SetLabelArea(leftTopX, leftTopY + ITEM_ICON_HEIGHT, leftTopX + w, leftTopY + ITEM_ICON_HEIGHT + h, me->label);

        //icon的位置 
		w = me->icon->Visible.Area.End.x - me->icon->Visible.Area.Start.x;
		h = me->icon->Visible.Area.End.y - me->icon->Visible.Area.Start.y;
		leftTopX += (me->width - w) >> 1;
        leftTopY += ITEM_ICON_HEIGHT - h;
        SetPictureArea(leftTopX, leftTopY, leftTopX + w, leftTopY + h, me->icon);

		//设置控件的可点击区域为icon的区域
		me->visible.Area.Start.x = leftTopX;
		me->visible.Area.Start.y = leftTopY;
		me->visible.Area.End.x = leftTopX + w;
		me->visible.Area.End.y = leftTopY + h;
		me->iconArea.startX = leftTopX;
		me->iconArea.startY = leftTopY;
		me->iconArea.endX = leftTopX + w - 1;
		me->iconArea.endY = leftTopY + h - 1;

		//设置使能标志等
		me->visible.iEnable = 1;
		if (!me->isInWindowCombList)
		{
			AddWindowComp(
				OBJTYP_GUIBUTTON, sizeof(OTDR_LINK_MAP_WIDGET_ITEM),
				me, me->parent->window
			);
			me->isInWindowCombList = 1;
		}
	}
	return 0;
}

//自适应链路地图图标位置
static void AdjustLinkMapItemsPostion(OTDR_LINK_MAP* me)
{
	int *pInterval = (int*)calloc(sizeof(int), me->itemCount);
	//计算每事件点显示的相对坐标
	int i;
	for (i = 0; i < me->itemCount - 1; ++i)
	{
		//计算时间对应的SOLA_WIDGET_ITEM的显示位置
		if (FLOATEQZERO(me->distancePerPixel))
		{
			break;
		}
		int x1 = me->items[i]->value / me->distancePerPixel;
		int x2 = me->items[i + 1]->value / me->distancePerPixel;
		int inerval = x2 - x1;
        if (inerval < ITEM_ICON_WIDTH)
        {
            inerval = ITEM_ICON_WIDTH;
        }
		pInterval[i] = inerval;
	}

	int x = 0;
	for (i = 0; i < me->itemCount; ++i)
	{
		SetLinkMapItemPosition(me->items[i], x, 0);
		x += pInterval[i];
	}

	free(pInterval);
}

//设置选中项
static int SetLinkMapItemChecked(OTDR_LINK_MAP_WIDGET_ITEM* me, int checked)
{
	if (checked != me->isChecked)
	{
		me->isChecked = checked;
		const char* iconBmpFile = GetLinkMapItemImageFile(me->isPassed, checked, me->typeName);
		SetPictureBitmap((char*)iconBmpFile, me->icon);
	}

	return 0;
}

//获取调整的位置
static int GetLinkMapItemXOffsetInDisplayArea(OTDR_LINK_MAP_WIDGET_ITEM* me)
{
	int xOffset = 0;
	int displayAreaWidth = me->parent->displayArea.endX - me->parent->displayArea.startX + 1;
	int rightLimit = displayAreaWidth - me->width;

	if (me->geometry.startX < 0)
	{
		xOffset = 0 - me->geometry.startX;
	}
	else if (me->geometry.startX > rightLimit)
	{
		xOffset = rightLimit - me->geometry.startX;
	}

	return xOffset;
}

//图标移动
static int LinkMapItemMove(OTDR_LINK_MAP_WIDGET_ITEM* me, int xOffset, int yOffset)
{
	int x = me->geometry.startX + xOffset;
	int y = me->geometry.startY + yOffset;

	return SetLinkMapItemPosition(me, x, y);
}

//移动图标位置
static void MoveLinkMapItemsPostion(OTDR_LINK_MAP* me, int xOffset, int yOffset)
{
	int i;
	for (i = 0; i < me->itemCount; ++i)
	{
		LinkMapItemMove(me->items[i], xOffset, yOffset);
	}
}

//设置选中图标
static void SetLinkMapCheckIndex(OTDR_LINK_MAP* me, int index)
{
	if (!me->items || me->itemCount <= 0)
	{
		return;
	}

	SetLinkMapItemChecked(me->items[me->checkedIndex], 0);
	SetLinkMapItemChecked(me->items[index], 1);
	me->checkedIndex = index;

	//调整显示位置
	int xOffset = GetLinkMapItemXOffsetInDisplayArea(me->items[index]);

	if (xOffset != 0)
	{
		MoveLinkMapItemsPostion(me, xOffset, 0);
	}
}

//获取事件类型
static int GetEventType(EVENTS_TABLE *pEvents, int index)
{
	EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[index]);
	int type = 0;

	if (pEvtsInfo)
	{
		int iStyle = pEvtsInfo->iStyle;
		type = iStyle;
		if (iStyle >= 10) {
			type = 4;
		}
		if (20 == iStyle) {
			type = 5;
		}
		else if (21 == iStyle) {
			type = 6;
		}
		else if (5 == iStyle) {
			//宏弯曲事件
			type = 8;
		}
	}

	if (index == 0)//起始事件
	{
		type = 7;
	}

	return type;

}

//获取事件类型的名字
static const char* OtdrEvents_GetEventTypeName(EVENTS_TABLE *pEvents, int index)
{
	//不同类型的事件的名字
	static const char* allNames[] =
	{
		"no-reflect",		//熔接事件
		"gain",				//无
		"reflect",			//连接器
		"echo",				//无
		"end",				//结束
		"continue",			//无
		"analysisEnd",		//无
		"start",			//起始
		"macrobend",		//宏弯曲
		"section"			//无
	};

	const char* typeName = NULL;

	//如果是分路器事件，则使用分路器类型
	int type = GetEventType(pEvents, index);
	typeName = allNames[type];
	return typeName;
}

//获取事件损耗通过，0：通过，1：不通过
static int GetEventLossPass(EVENTS_TABLE *pEvents, int index)
{
	EVENTS_ISPASS* pEvtsIsPass = &(pEvents->EventsIsPass[index]);
	if (pEvtsIsPass)
	{
		return (pEvtsIsPass->iIsEventLossPass || pEvtsIsPass->iIsReflectPass || pEvtsIsPass->iIsAttenuationPass);
	}

	return 0;
}

//设置链路信息
static int SetlineInfo(OTDR_LINK_MAP *me, EVENTS_TABLE *pEvents)
{
	//检查参数
	if(!pEvents || !me)
		return -1;
	int iEventNum = pEvents->iEventsNumber;
	EVENTS_INFO* pEventInfo = &(pEvents->EventsInfo[iEventNum-1]);
	//线路总损耗(最后一个事件点的totalLoss)
	float fTotalLoss = 0.000f;
	//光纤长度
	float fTotalFiberLength = 0.00f;
	//平均损耗
	float fAvgLoss = 0.000f;
	//当超过一个事件时，计算各个数值，其他情况都为0
	if(iEventNum > 1)
	{
		fTotalLoss = pEventInfo->fTotalLoss;
		fTotalFiberLength = pEventInfo->fEventsPosition;
		fAvgLoss = fTotalLoss / UnitConverter_Dist_Float2Float(UNIT_M, UNIT_KM, fTotalFiberLength);
	}

	me->lineInfo[0].valueText = Float2GuiStringUnit(LOSS_PRECISION, fTotalLoss, " dB");
	me->lineInfo[1].valueText = Float2GuiStringUnit(LOSS_PRECISION, fAvgLoss, " dB/km");
	me->lineInfo[2].valueText = UnitConverter_Dist_M2System_Float2GuiString(
														MODULE_UNIT_OTDR, fTotalFiberLength, 1);
	int i;
	for (i = 0; i < 3; ++i)
	{
		SetLabelText(me->lineInfo[i].valueText, me->lineInfo[i].valueLabel);
	}
	
	return 0;
}

//设置光标在事件上
static int SetFocusAtEvent(OTDR_LINK_MAP *pOtdrLinkMapObj)
{
	int iPos = pOtdrLinkMapObj->checkedIndex;

	if (pLinkMapEvents)
	{
		SendEventMakerCtlMsg(EN_MOVE_SETVAL, pLinkMapEvents->EventsInfo[iPos].iBegin, -1, 0, 0);
	}

	return 0;
}

//处理事件列表
int handleEventList(OTDR_LINK_MAP* me, EVENTS_TABLE *pEvents)
{
	if (pEvents == NULL)
	{
		return -1;
	}
	//获取事件表
	pLinkMapEvents = pEvents;
	
	int eventCount = pEvents->iEventsNumber;
	if (eventCount <= 0)
	{
		LOG(LOG_ERROR, "Event list has no event\n");
		return -1;
	}

	//清除可能的图标
	ClearLinkMapItems(me);
	//为items分配内存
	me->items = (OTDR_LINK_MAP_WIDGET_ITEM**)malloc(eventCount * sizeof(OTDR_LINK_MAP_WIDGET_ITEM*));
	me->itemCount = eventCount;
	//获得事件的总距离
	me->distance = pEvents->EventsInfo[eventCount - 1].fEventsPosition - pEvents->EventsInfo[0].fEventsPosition;
	//计算距离标签显示的内容
	char buffer[16];
	float SpanDistance = me->distance;
	if (pEvents->iRecvFiberIndex > 0)
	{
		SpanDistance = pEvents->EventsInfo[pEvents->iRecvFiberIndex].fEventsPosition;
	}

	if (pEvents->iLaunchFiberIndex > 0)
	{
		SpanDistance -= pEvents->EventsInfo[pEvents->iLaunchFiberIndex].fEventsPosition;
	}
	//切换单位
	free(me->distanceText);
	me->distanceText = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, SpanDistance, 1);
	SetLabelText(me->distanceText, me->distanceLabel);
	//判定实际链路长度范围
	//judgeDistance(me);

	//计算在缩略图区域中，每个像素的所表示的实际光纤的距离
	int width = me->thumbnailArea.endX - me->thumbnailArea.startX + 1;
	me->thumbnailDistancePerPixel = me->distance / (float)width;
	//计算显示区域中每个像素所表示的实际光线的距离
	width = me->displayArea.endX - me->displayArea.startX + 1;
    width -= ITEM_ICON_WIDTH/2;
	me->distancePerPixel = me->distance / (float)width;
	//计算链路信息
	SetlineInfo(me, pEvents);
	//计算每个事件的距离，类型，以及缩略图的位置等
	int i;
	int launchFiberIndex = pEvents->iLaunchFiberIndex;// solaEvent->EventTable.iLaunchFiberIndex;
	int recvFiberIndex = pEvents->iRecvFiberIndex;//solaEvent->EventTable.iRecvFiberIndex;
	float launchFiberPosition = pEvents->EventsInfo[pEvents->iLaunchFiberIndex].fEventsPosition;//solaEvent->EventTable.fLaunchFiberPosition;

	for (i = 0; i < eventCount; ++i)
	{
		const EVENTS_INFO* event = pEvents->EventsInfo + i;
		//计算事件实际距离，如果距离大于1000m则使用'km'作为单位否则使用'm'作为单位
		float distance = event->fEventsPosition;
		if (launchFiberIndex > 0)
		{
			distance -= launchFiberPosition;
		}
		//切换单位
		char* temp = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, distance, 1);
		sprintf(buffer, "%s", temp);
		GuiMemFree(temp);
		char marker[5] = { 0 };
		//起点
		if ((launchFiberIndex > 0 && launchFiberIndex == i)
			|| (launchFiberIndex <= 0 && i == 0))
		{
			//strcat(buffer, "(A)");
			strcpy(marker, "(A)");
		}
		//终点
		else if ((recvFiberIndex > 0 && recvFiberIndex == i)
			|| (recvFiberIndex <= 0 && eventCount > 1 && i == (eventCount - 1)))
		{
			//strcat(buffer, "(B)");
			strcpy(marker, "(B)");
		}
		//为当前事件创建一个对应的SOLA_WIDGET_ITEM
		OTDR_LINK_MAP_WIDGET_ITEM* item = CreateLinkMapItem(OtdrEvents_GetEventTypeName(pEvents, i), i, TransString(buffer), me);
		item->value = distance;
		strcpy(item->marker, marker);
		//计算当前事件对应的SOLA_WIDGET_ITEM缩略图在缩略图显示区域中的位置
		int x = FLOATEQZERO(me->thumbnailDistancePerPixel) ? 0 : (distance + launchFiberPosition) / me->thumbnailDistancePerPixel;
		SetLinkMapItemThumbnailPosition(item, x, 0);
		//判断当前事件损耗是否通过 第0个默认通过
		int passed = GetEventLossPass(pEvents, i);//IsSolaEventPassed(solaEvent, i);
		SetLinkMapItemPassed(item, passed);
		me->items[i] = item;
	}

	//根据事件之间的距离自适应每个事件图标的位置
	AdjustLinkMapItemsPostion(me);
	SetLinkMapCheckIndex(me, launchFiberIndex);

	return 0;
}

//重置链路地图列表
void ResetLinkMap(OTDR_LINK_MAP *me)
{
	//清除文本
	int i;

	for (i = 0; i < 3; ++i)
	{
		//清除链路信息文本标签
		me->lineInfo[i].valueText = TransString(" ");
		SetLabelText(me->lineInfo[i].valueText, me->lineInfo[i].valueLabel);
	}

	ClearLinkMapItems(me);
}
