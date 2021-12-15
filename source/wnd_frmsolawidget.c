/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsloawidget.c
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

#include <limits.h>

#include "wnd_frmsolawidget.h"
#include "wnd_frmsolaconfig.h"
#include "wnd_frmsolamessager.h"
#include "guibase.h"
#include "guiglobal.h"
#include "app_frminit.h"
#include "app_sola.h"
#include "guiphoto.h"
#include "app_unitconverter.h"

/*********************
* 显示区域相关的宏定义
*********************/
#define SW_ITEM_WIDTH               94
#define SW_ITEM_HEIGHT              71
#define SW_ARROW_WIDTH              93//SW_ITEM_WIDTH
#define SW_ARROW_HEIGHT             38//SW_ITEM_HEIGHT
#define SW_ARROW_X_OFF              0
#define SW_ARROW_Y_OFF              (5 + 28 + 15 + 30)
#define SW_ICON_WIDTH               39
#define SW_ICON_HEIGHT              54


/************************
* 内部使用的静态函资源定义
************************/

extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构
extern SOLA_MESSAGER* pSolaMessager;
int disPassed = 0;
/***********************
* 内部使用的功能静态函数
***********************/
//获得item的icon的图片文件名
static const char* GetSolaWidgetItemImageFile(int isPassed, int isChecked, const char* typeName);
static int DrawWidgetItemThumbnail(SOLA_WIDGET_ITEM* me);
static void SolaWidgetItemClicked(SOLA_WIDGET* me, int index);
static int DrawWidgetThumbnailDistanceLine(SOLA_WIDGET* me);
static int DrawWidgetDistanceLine(SOLA_WIDGET* me);
static void SetSolaWidgetCheckIndex(SOLA_WIDGET* me, int index);
static void judgeDistance(SOLA_WIDGET* me);
static int handleEventList(SOLA_WIDGET* me, SolaEvents* solaEvent);
static void SetSolaWidgetLeftArrowEnable(SOLA_WIDGET* me, int enable);
static void SetSolaWidgetRightArrowEnable(SOLA_WIDGET* me, int enable);
static int GetSolaItemXOffsetInDisplayArea(SOLA_WIDGET_ITEM* me);
static void MoveSolaItemsPostion(SOLA_WIDGET* me, int xOffset, int yOffset);


/*******************
* 按钮处理静态函数
*******************/
//item的按下处理函数
static int SolaWidgetItem_Down(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    );
//item的弹起处理函数
static int SolaWidgetItem_Up(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    );
//左箭头按下处理函数
static int SolaWidgetLeftArrow_Down(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    );
//左箭头弹起处理函数
static int SolaWidgetLeftArrow_Up(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    );
//右箭头按下处理函数
static int SolaWidgetRightArrow_Down(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    );
//右箭头弹起处理函数
static int SolaWidgetRightArrow_Up(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    );


/***********************
* 所有公有函数的具体实现
***********************/
SOLA_WIDGET_ITEM* CreateSolaWidgetItem(const char* typeName, int index, GUICHAR* text, SOLA_WIDGET* parent)
{
    if (!parent)
        return NULL;

    SOLA_WIDGET_ITEM* me = (SOLA_WIDGET_ITEM*)malloc(sizeof(SOLA_WIDGET_ITEM));
 
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
    me->width = SW_ITEM_WIDTH;
    me->value = 0.0f;
    me->parent = parent;
    me->visible.fnDisplay = (DISPFUNC)DisplaySolaWidgetItem;
    me->visible.fnDestroy = (DESTFUNC)DestroySolaWidgetItem;

    //几何描述
    me->geometry.startX = 0;
    me->geometry.startY = 0;
    me->geometry.endX = SW_ITEM_WIDTH - 1;
    me->geometry.endY = SW_ITEM_HEIGHT - 1;

    me->thumbnailGeometry.startX = 0;
    me->thumbnailGeometry.startY = 0;
    me->thumbnailGeometry.endX = 0 + 2 - 1;
    me->thumbnailGeometry.endY = 0 + 8 - 1;

    me->iconArea.startX = 0;
    me->iconArea.startY = 0;
    me->iconArea.endX = SW_ICON_WIDTH - 1;
    me->iconArea.endY = SW_ICON_HEIGHT - 1;

    //创建GUI资源
    const char* iconBmpFile = GetSolaWidgetItemImageFile(me->isPassed, me->isChecked, typeName);
    me->icon = CreatePicture(0, 0, SW_ICON_WIDTH, SW_ICON_HEIGHT, (char*)iconBmpFile);
    me->label = CreateLabel(0, 0, me->width, 24, text);
	if (isFont24())
	{
		me->font = CreateFont(FNTFILE_BIG, 24, 24, 0x000000, COLOR_TRANS);
	}
	else
	{
		me->font = CreateFont(FNTFILE_STD, 16, 16, 0x000000, COLOR_TRANS);
	}
    SetLabelAlign(GUILABEL_ALIGN_CENTER, me->label);
    SetLabelFont(me->font, me->label);

    //注册点击处理消息
    GUIMESSAGE* currMesg = GetCurrMessage();
    LoginMessageReg(
        GUIMESSAGE_TCH_DOWN, me, SolaWidgetItem_Down, 
        (void*)me, sizeof(me), currMesg
        );
    LoginMessageReg(
        GUIMESSAGE_TCH_UP, me, SolaWidgetItem_Up, 
        (void*)me, sizeof(me), currMesg
        );

    //GUILABEL中本身已经有文本的缓冲，此处释放文本
    free(text);

    return me;
}


int DisplaySolaWidgetItem(SOLA_WIDGET_ITEM* me)
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

int UpdateSolaWidgetItem(SOLA_WIDGET_ITEM* me)
{
    return DisplaySolaWidgetItem(me);
}


int DestroySolaWidgetItem(SOLA_WIDGET_ITEM** me)
{
    GUIMESSAGE* currMesg = GetCurrMessage();

    //清除可能的窗体控件队列
    if ((*me)->isInWindowCombList)
    {
        DelWindowComp(*me, (*me)->parent->window);
    }

    //清除消息注册项
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, *me, currMesg);
    LogoutMessageReg(GUIMESSAGE_TCH_UP, *me, currMesg);

    //释放GUI资源
    DestroyPicture(&((*me)->icon));
    DestroyLabel(&((*me)->label));
    DestroyFont(&(*me)->font);

    //释放自身所占用的资源
    free(*me);
    *me = NULL;

    return 0;
}


int SetSolaWidgetItemPosition(SOLA_WIDGET_ITEM* me, int x, int y)
{
    int displayAreaStartX = me->parent->displayArea.startX;
    int displayAreaStartY = me->parent->displayArea.startY;
    int displayAreaWidth = me->parent->displayArea.endX - displayAreaStartX + 1;

    //修改几何描述信息
    me->geometry.startX = x;
    me->geometry.startY = y;
    me->geometry.endX = SW_ITEM_WIDTH - 1;
    me->geometry.endY = SW_ITEM_HEIGHT - 1;

    //item区域超出显示区域（左边，右边），则清除使能标志，并将其从窗体的控件队列摘下
    if ((x <= (0 - SW_ICON_WIDTH)) || (x >= displayAreaWidth))
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
        leftTopY += SW_ITEM_HEIGHT - h;
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
        SetLabelArea(leftTopX, leftTopY, leftTopX + w, leftTopY + h, me->label);
        
        //icon的位置 
        w = me->icon->Visible.Area.End.x - me->icon->Visible.Area.Start.x;
        h = me->icon->Visible.Area.End.y - me->icon->Visible.Area.Start.y;
        leftTopX += (me->width - w) >> 1;
        leftTopY += SW_ITEM_HEIGHT - h;
        SetPictureArea(leftTopX, leftTopY, leftTopX + w, leftTopY+h, me->icon);

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
                OBJTYP_GUIBUTTON, sizeof(SOLA_WIDGET_ITEM), 
                me, me->parent->window
                );
            me->isInWindowCombList = 1;
        }
    }
    return 0;
}


int SetSolaWidgetItemThumbnailPosition(SOLA_WIDGET_ITEM* me, int x, int y)
{
    me->thumbnailGeometry.startX = x;
    me->thumbnailGeometry.startY = y;
    me->thumbnailGeometry.endX = x + 2 - 1;
    me->thumbnailGeometry.endY = y + 8 - 1;
    
    return 0;
}

int SetSolaWidgetItemChecked(SOLA_WIDGET_ITEM* me, int checked)
{
    if (checked != me->isChecked)
    {
        me->isChecked = checked;
        const char* iconBmpFile = GetSolaWidgetItemImageFile(me->isPassed, checked, me->typeName);
        SetPictureBitmap((char*)iconBmpFile, me->icon);
    }
    return 0;
}


int SetSolaWidgetItemPassed(SOLA_WIDGET_ITEM* me, int passed)
{   
    if (passed != me->isPassed)
    {
        me->isPassed = passed;
        const char* iconBmpFile = GetSolaWidgetItemImageFile(passed, me->isChecked, me->typeName);

        SetPictureBitmap((char*)iconBmpFile, me->icon);
    }
    return 0;
}

int SetSolaWidgetItemType(SOLA_WIDGET_ITEM* me, const char* typeName)
{
    if (strcmp(typeName, me->typeName))
    {
        me->typeName = (char*)typeName;
        const char* iconBmpFile = GetSolaWidgetItemImageFile(me->isPassed, me->isChecked, typeName);
        SetPictureBitmap((char*)iconBmpFile, me->icon);
    }
    return 0;
}


int SetSolaWidgetItemText(SOLA_WIDGET_ITEM* me, GUICHAR* text)
{
    if (!text)
        return -1;

    SetLabelText(text, me->label);
    free(text);
    
    return 0;
}


int SolaWidgetItemMove(SOLA_WIDGET_ITEM* me, int xOffset, int yOffset)
{
    int x = me->geometry.startX + xOffset;
    int y = me->geometry.startY + yOffset;

    return SetSolaWidgetItemPosition(me, x, y);
}



SOLA_WIDGET* CreateSolaWidget(int x, int y, int w, int h, ITEM_CHECKED_HANDLE handle, GUIWINDOW* window)
{
    if (!window)
        return NULL;

    SOLA_WIDGET* me = (SOLA_WIDGET*)malloc(sizeof(SOLA_WIDGET));
    //基本信息赋值
    me->visible.Area.Start.x = x;
    me->visible.Area.Start.y = y;
    me->visible.Area.End.x = x + w - 1;
    me->visible.Area.End.y = y + h - 1;
    me->visible.iEnable = 1;
    me->visible.iLayer = 0;
    me->visible.iFocus = 0;
    me->visible.iCursor = 0;
    me->itemCount = 0;
    me->checkedIndex = 0;
    me->items = NULL;
    me->window = window;
    me->leftArrowEnable = 0;
    me->rightArrowEnable = 0;
    me->itemCheckedHandle = handle;
    me->visible.fnDisplay = (DISPFUNC)DisplaySolaWidget;
    me->visible.fnDestroy = (DESTFUNC)DestroySolaWidget;

    //主显示区域的排布控件排布 依次为：左边距空白-左边箭头-显示区域-右边箭头-右边距空白
    int width = w - SW_ITEM_WIDTH*2 - SW_ARROW_X_OFF/2;
    me->displayArea.startX = x + SW_ITEM_WIDTH + SW_ARROW_X_OFF;
    me->displayArea.startY = y + SW_ARROW_Y_OFF;
    me->displayArea.endX = me->displayArea.startX + width - 1;
    me->displayArea.endY = me->displayArea.startY + SW_ITEM_HEIGHT - 1;

    //缩略图区域的绝对坐标
    me->thumbnailArea.startX = x + 39;
    me->thumbnailArea.startY = y + 34;
    me->thumbnailArea.endX = me->thumbnailArea.startX + w - 174;
    me->thumbnailArea.endY = me->thumbnailArea.startY + 3;

    //创建GUI资源
	if (isFont24())
	{
		me->distanceFont = CreateFont(FNTFILE_BIG, 24, 24, 0xFFFFFF, COLOR_TRANS);
	}
	else
	{
		me->distanceFont = CreateFont(FNTFILE_STD, 16, 16, 0xFFFFFF, COLOR_TRANS);
	}
    me->distanceText = TransString(" ");
    me->distanceLabel = CreateLabel(
        me->thumbnailArea.endX + 16, y + 29, 80, 24,
        me->distanceText
        );
    SetLabelAlign(2, me->distanceLabel);
    me->startIcon = CreatePicture(
        x, y + 5, 28, 28,
        BmpFileDirectory"start_icon.bmp"
        );
    me->background = CreatePicture(
        x, y, w, h, 
        BmpFileDirectory"sola_background.bmp"
        );
    me->leftArrow = CreatePicture(
        x + SW_ARROW_X_OFF + 243, y + SW_ARROW_Y_OFF + 109, 
        SW_ARROW_WIDTH, SW_ARROW_HEIGHT,
        BmpFileDirectory"sola_arrow_disable.bmp"
        );
    me->rightArrow = CreatePicture(
        x + SW_ARROW_X_OFF + 350, y + SW_ARROW_Y_OFF + 109,
        SW_ARROW_WIDTH, SW_ARROW_HEIGHT,
        BmpFileDirectory"sola_arrow_disable1.bmp"
        );
    me->leftBg = CreatePicture(
        x + SW_ARROW_X_OFF, y + SW_ARROW_Y_OFF, 
        SW_ITEM_WIDTH, SW_ITEM_HEIGHT,
        BmpFileDirectory"sola_left_arrow.bmp"
        );
    me->rightBg = CreatePicture(
        x + width + SW_ITEM_WIDTH + SW_ARROW_X_OFF + 1, y + SW_ARROW_Y_OFF+1,
        SW_ITEM_WIDTH, SW_ITEM_HEIGHT,
        BmpFileDirectory"sola_right_arrow.bmp"
        );
    SetLabelFont(me->distanceFont, me->distanceLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, me->distanceLabel);
    
    //设置左右按钮的图层为10，左右按钮一直处于最高图层
    me->leftArrow->Visible.iLayer = 10;
    me->rightArrow->Visible.iLayer = 10;
    me->leftBg->Visible.iLayer = 10;
    me->rightBg->Visible.iLayer = 10;

    //添加左右按钮到窗体的控件队列，并注册按钮处理函数
    AddWindowComp(
        OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
        me->leftArrow, window
        );
    AddWindowComp(
        OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
        me->rightArrow, window
        );
    AddWindowComp(
        OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
        me->leftBg, window
        );
    AddWindowComp(
        OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
        me->rightBg, window
        );

    GUIMESSAGE* currMesg = GetCurrMessage();
    LoginMessageReg(
        GUIMESSAGE_TCH_DOWN, me->leftArrow, 
        SolaWidgetLeftArrow_Down, 
        (void*)me, sizeof(me), currMesg
        );
    LoginMessageReg(
        GUIMESSAGE_TCH_UP, me->leftArrow, 
        SolaWidgetLeftArrow_Up, 
        (void*)me, sizeof(me), currMesg
        );
    LoginMessageReg(
        GUIMESSAGE_TCH_DOWN, me->rightArrow, 
        SolaWidgetRightArrow_Down,
        (void*)me, sizeof(me), currMesg
        );
    LoginMessageReg(
        GUIMESSAGE_TCH_UP, me->rightArrow, 
        SolaWidgetRightArrow_Up,
        (void*)me, sizeof(me), currMesg
        );

    return me;
}

int DisplaySolaWidget(SOLA_WIDGET* me)
{
    int i = 0;

    DisplayPicture(me->background);
    DisplayPicture(me->startIcon);

    DrawWidgetThumbnailDistanceLine(me);
    DrawWidgetDistanceLine(me);
    for (i = 0; i < me->itemCount; ++i)
    {
        DisplaySolaWidgetItem(me->items[i]);
    }
    DisplayPicture(me->leftArrow);
    DisplayPicture(me->rightArrow);
    DisplayPicture(me->leftBg);
    DisplayPicture(me->rightBg);
    DisplayLabel(me->distanceLabel);

    return 0;
}

int DestroySolaWidget(SOLA_WIDGET** me)
{
    GUIMESSAGE* currMesg = GetCurrMessage();
    
    //删除窗体队列中自身的所有控件
    DelWindowComp((*me)->leftArrow, (*me)->window);
    DelWindowComp((*me)->rightArrow, (*me)->window);
    DelWindowComp((*me)->leftBg, (*me)->window);
    DelWindowComp((*me)->rightBg, (*me)->window);

    //清除注册的按钮消息处理函数
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, (*me)->leftArrow, currMesg);
    LogoutMessageReg(GUIMESSAGE_TCH_UP, (*me)->leftArrow, currMesg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, (*me)->rightArrow, currMesg);
    LogoutMessageReg(GUIMESSAGE_TCH_UP, (*me)->rightArrow, currMesg);

    //清除可能的Items
    ClearSolaWidgetItems(*me);
    
    //销毁GUI资源
    DestroyPicture(&((*me)->background));
    DestroyPicture(&((*me)->startIcon));
    DestroyPicture(&((*me)->leftArrow));
    DestroyPicture(&((*me)->rightArrow));
    DestroyPicture(&((*me)->leftBg));
    DestroyPicture(&((*me)->rightBg));
    DestroyLabel(&((*me)->distanceLabel));
    DestroyFont(&((*me)->distanceFont));
    free((*me)->distanceText);

    //释放自身所占用的资源
    free(*me);
    *me = NULL;

    return  0;
}

int SetSolaWidgetEvent(SOLA_WIDGET* me, SolaEvents* solaEvent)
{
    if (!solaEvent)
        return -1;

    //清除可能的Items
    ClearSolaWidgetItems(me);

    me->solaEvents = solaEvent;
    
    //处理eventList
    int iErr = handleEventList(me, solaEvent);
    if (iErr < 0)
    {
        iErr = -2;
    }

    return iErr;
}


void ClearSolaWidgetItems(SOLA_WIDGET* me)
{
    //销毁可能已经存在的items
    if (me->items && me->itemCount > 0)
    {
        int i;
        for (i = 0; i < me->itemCount; ++i)
        {
            DestroySolaWidgetItem(&(me->items[i]));
        }
        free(me->items);
        free(me->distanceText);
        me->distanceText = TransString(" ");
        SetLabelText(me->distanceText, me->distanceLabel);
    }
    
    //清除关键数据的值为初始值
    me->items = NULL;
    me->itemCount = 0;
    me->checkedIndex = 0;
    me->distance = 0.0f;
    me->distancePerPixel = 0.0f;
    me->thumbnailDistancePerPixel = 0.0f;
    
    SetSolaWidgetLeftArrowEnable(me, 0);
    SetSolaWidgetRightArrowEnable(me, 0);
}


void SolaWidgetItemsFocusPrev(SOLA_WIDGET* me)
{
    int currIndex = me->checkedIndex;
    if (currIndex > 0)
    {
        currIndex--;
        SetSolaWidgetCheckIndex(me, currIndex);
        DisplaySolaWidget(me);
        SyncCurrFbmap();
    }
}


void SolaWidgetItemsFocusNext(SOLA_WIDGET* me)
{
    int currIndex = me->checkedIndex;
    if (currIndex < (me->itemCount - 1))
    {
        currIndex++;
        SetSolaWidgetCheckIndex(me, currIndex);
        DisplaySolaWidget(me);
        SyncCurrFbmap();    
    }
}

void SolaWidgetOnEventTypeChanged(SOLA_WIDGET* me)
{
    if (!me->items || me->itemCount <= 0)
    {
        return;
    }
    const char* typeName = SolaEvents_GetEventTypeName(me->solaEvents, me->checkedIndex);
    int passed = IsSolaEventPassed(me->solaEvents, me->checkedIndex);
    /*
    if (passed >= 0)
        passed = (passed == 0) ? 1 : 0;
    else 
        passed = -1;
    */
    SetSolaWidgetItemPassed(me->items[me->checkedIndex], passed);
    SetSolaWidgetItemType(me->items[me->checkedIndex], typeName);
    DisplaySolaWidgetItem(me->items[me->checkedIndex]);

}

/****************************
* 所有静态功能函数的具体实现
****************************/
static const char* GetSolaWidgetItemImageFile(int isPassed, int isChecked, const char* typeName)
{
    static const char* itemCheckedString[] = 
    {
        "unchecked",
        "checked"    
    };

    static const char* itemPassedString[] = 
    {
        "unpassed",
        "passed"    
    };

    static char fileName[PATH_MAX];
    
    const char* passedString = NULL;
    if (isPassed == 2) {
        passedString = "unknow";
    }
    else if((isPassed == 1) || (isPassed == 0)){
        passedString = itemPassedString[isPassed];
    }
    
    sprintf(
        fileName,BmpFileDirectory"type_%s_%s_%s.bmp",
        typeName,
        itemCheckedString[isChecked],
        passedString
    );
    return fileName;
}

static int DrawWidgetItemThumbnail(SOLA_WIDGET_ITEM* me)
{
    GUIPEN* pen = GetCurrPen();
    COLOR oldColor = pen->uiPenColor;
    //pen->uiPenColor = 0xBDBDBD;
    if(me->isPassed == 1)
    {
        pen->uiPenColor = 0x49E765;
    }
    else if(me->isPassed == 0)
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
        me->thumbnailGeometry.startY + me->parent->thumbnailArea.startY - 2,
        me->thumbnailGeometry.endX + me->parent->thumbnailArea.startX,
        me->thumbnailGeometry.endY + me->parent->thumbnailArea.startY - 2
        );
    
    //如果是选中的则绘制选中框
    if (!iErr && me->isChecked)
    {
        pen->uiPenColor = 0x1A64B7;
        iErr = DrawRectangle(
            me->thumbnailGeometry.startX + me->parent->thumbnailArea.startX - 2,
            me->thumbnailGeometry.startY + me->parent->thumbnailArea.startY - 4,
            me->thumbnailGeometry.endX + me->parent->thumbnailArea.startX + 2,
            me->thumbnailGeometry.endY + me->parent->thumbnailArea.startY - 2 + 2
            );
    }
    pen->uiPenColor = oldColor;

    return iErr;
}


static void SolaWidgetItemClicked(SOLA_WIDGET* me, int index)
{
    if (me->checkedIndex != index)
    {
        SetSolaWidgetCheckIndex(me, index);
        DisplaySolaWidget(me);
        SyncCurrFbmap();
    }
}

static int DrawWidgetThumbnailDistanceLine(SOLA_WIDGET* me)
{
    GUIPEN* pen = GetCurrPen();
    COLOR oldColor = pen->uiPenColor;
    pen->uiPenColor = 0x000000;

    int iErr = DrawBlock(
        me->thumbnailArea.startX, me->thumbnailArea.startY,
        me->thumbnailArea.endX, me->thumbnailArea.endY
        );
    pen->uiPenColor = oldColor;

    return iErr;
}

static int DrawWidgetDistanceLine(SOLA_WIDGET* me)
{
    if (me->itemCount <= 0)
    {
        return -1;
    }
    
    int startX = me->displayArea.startX > me->items[0]->iconArea.startX ?
        me->displayArea.startX : me->items[0]->iconArea.startX;
    int endX = me->displayArea.endX < me->items[me->itemCount - 1]->iconArea.startX ?
        me->displayArea.endX : me->items[me->itemCount - 1]->iconArea.startX;

    int startY = me->displayArea.startY + 43;
    int endY = startY + 2 - 1;

    if ((startX >= me->displayArea.startX) 
        && (endX <= me->displayArea.endX)
        && (startX < endX))
    {
        GUIPEN* pen = GetCurrPen();
        COLOR oldColor = pen->uiPenColor;
        pen->uiPenColor = 0x000000;
        int iErr = DrawBlock(startX, startY, endX, endY);
        pen->uiPenColor = oldColor;
        
        return iErr;
    }

    return -1;
}


float SolaWidgetFindMinDistanceBetweenItems(SOLA_WIDGET* me)
{
    float minValue = 0.0f;

    if (me->itemCount > 2)
    {   
        minValue = me->items[1]->value - me->items[0]->value;
        int i;
        for (i = 2; i < me->itemCount; ++i)
        {
            float value = me->items[i]->value - me->items[i - 1]->value;
            if (minValue > value)
            {
                minValue = value;
            }
        }
    }

    return minValue;
}

static void AdjustSolaItemsPostion(SOLA_WIDGET* me)
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
        if (inerval < SW_ITEM_WIDTH)
        {
            inerval = SW_ITEM_WIDTH;
        }
        pInterval[i] = inerval;
    }

    int x = 0;
    for (i = 0; i < me->itemCount; ++i)
    {
        SetSolaWidgetItemPosition(me->items[i], x, 0);
        x += pInterval[i];
    }

    free(pInterval);
}

static int handleEventList(SOLA_WIDGET* me, SolaEvents* solaEvent)
{
    int eventCount = SolaEvents_Count(solaEvent);
    if (eventCount <= 0)
    {
        LOG(LOG_ERROR, "Event list has no event\n");
        return -1;
    }
    
    //为items分配内存
    me->items = (SOLA_WIDGET_ITEM**)malloc(eventCount * sizeof(SOLA_WIDGET_ITEM*));
    me->itemCount = eventCount;

    //获得事件的总距离
    me->distance = SolaEvents_GetTotalDistance(solaEvent);
    
    //计算距离标签显示的内容
    char buffer[16];
    float SpanDistance = me->distance;
    if(solaEvent->EventTable.iRecvFiberIndex > 0)
    {
        SpanDistance = solaEvent->EventTable.EventsInfo[solaEvent->EventTable.iRecvFiberIndex].fEventsPosition;
    }
    
    if(solaEvent->EventTable.iLaunchFiberIndex > 0)
    {
        SpanDistance -= solaEvent->EventTable.EventsInfo[solaEvent->EventTable.iLaunchFiberIndex].fEventsPosition;
    }
    //切换单位
    free(me->distanceText);
    me->distanceText = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_SOLA, SpanDistance, 1);
    SetLabelText(me->distanceText, me->distanceLabel);
    
    //判定实际链路长度范围
    judgeDistance(me);
    
    //计算在缩略图区域中，每个像素的所表示的实际光纤的距离
    int width = me->thumbnailArea.endX - me->thumbnailArea.startX + 1;
    me->thumbnailDistancePerPixel = me->distance / (float)width;
   
    //计算显示区域中每个像素所表示的实际光线的距离
    width = me->displayArea.endX - me->displayArea.startX + 1;
    width -= SW_ITEM_WIDTH;
    me->distancePerPixel = me->distance / (float)width;

    //计算每个事件的距离，类型，以及缩略图的位置等
    int i;
    int launchFiberIndex = solaEvent->EventTable.iLaunchFiberIndex;
    int recvFiberIndex = solaEvent->EventTable.iRecvFiberIndex;
    float launchFiberPosition = solaEvent->EventTable.EventsInfo[launchFiberIndex].fEventsPosition;//solaEvent->EventTable.fLaunchFiberPosition;
    for (i = 0; i < eventCount; ++i)
    {
        const EVENTS_INFO* event = SolaEvents_GetEvent(solaEvent, i);
        //计算事件实际距离，如果距离大于1000m则使用'km'作为单位否则使用'm'作为单位
        float distance = event->fEventsPosition;
        if(launchFiberIndex > 0)
        {
            distance -= launchFiberPosition;
        }   
        //切换单位
        char* temp = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_SOLA, distance, 1);
        sprintf(buffer, "%s", temp);
        GuiMemFree(temp);
        char marker[5] = {0};
        //起点
        if((launchFiberIndex > 0 && launchFiberIndex == i)
            || (launchFiberIndex <= 0 && i == 0))   
        {
            strcat(buffer, "(A)");
            strcpy(marker, "(A)");
        }
        //终点
        else if((recvFiberIndex > 0 && recvFiberIndex == i)
            || (recvFiberIndex <= 0 && eventCount > 1 &&i == (eventCount - 1)))
        {
            strcat(buffer, "(B)");
            strcpy(marker, "(B)");
        }
        //为当前事件创建一个对应的SOLA_WIDGET_ITEM
        SOLA_WIDGET_ITEM* item = CreateSolaWidgetItem(SolaEvents_GetEventTypeName(solaEvent, i), i, TransString(buffer), me);
        item->value = distance;
        strcpy(item->marker, marker);
        //计算当前事件对应的SOLA_WIDGET_ITEM缩略图在缩略图显示区域中的位置
        int x = FLOATEQZERO(me->thumbnailDistancePerPixel) ? 0 : (distance+launchFiberPosition) / me->thumbnailDistancePerPixel;
        SetSolaWidgetItemThumbnailPosition(item, x, 0);


        //判断当前事件损耗是否通过 第0个默认通过
        int passed = IsSolaEventPassed(solaEvent,i);//(me->solaEvents, me->checkedIndex);
        SetSolaWidgetItemPassed(item, passed);
        me->items[i] = item;
    }

    //根据事件之间的距离自适应每个事件图标的位置
    AdjustSolaItemsPostion(me);
    SetSolaWidgetCheckIndex(me, launchFiberIndex);

    return 0;
}

static void SetSolaWidgetCheckIndex(SOLA_WIDGET* me, int index)
{
    if (!me->items || me->itemCount <= 0)
    {
        return;
    }

    SetSolaWidgetItemChecked(me->items[me->checkedIndex], 0);
    SetSolaWidgetItemChecked(me->items[index], 1);
    me->checkedIndex = index;

    if (me->checkedIndex == 0)
    {
        SetSolaWidgetLeftArrowEnable(me, 0);
    }
    else
    {
        SetSolaWidgetLeftArrowEnable(me, 1);
    }
    if (me->checkedIndex == (me->itemCount - 1))
    {
        SetSolaWidgetRightArrowEnable(me, 0);
    }
    else
    {
        SetSolaWidgetRightArrowEnable(me, 1);
    }

    //调整显示位置
    int xOffset = GetSolaItemXOffsetInDisplayArea(me->items[index]);
    if (xOffset != 0)
    {
        MoveSolaItemsPostion(me, xOffset, 0);
    }

    if (me->itemCheckedHandle)
    {
        me->itemCheckedHandle(index);
    }
}


static void SetSolaWidgetLeftArrowEnable(SOLA_WIDGET* me, int enable)
{
    if (me->leftArrowEnable != enable)
    {
        if (!enable)
        {
            SetPictureBitmap(
                BmpFileDirectory"sola_arrow_disable.bmp", 
                me->leftArrow
                );
        }
        else 
        {
            SetPictureBitmap(
                BmpFileDirectory"sola_left_arrow_nomal.bmp", 
                me->leftArrow
                );
        }
        me->leftArrowEnable = enable;
    }
}

static void SetSolaWidgetRightArrowEnable(SOLA_WIDGET* me, int enable)
{
    if (me->rightArrowEnable != enable)
    {
        if (!enable)
        {
            SetPictureBitmap(
                BmpFileDirectory"sola_arrow_disable1.bmp", 
                me->rightArrow
                );
        }
        else 
        {
            SetPictureBitmap(
                BmpFileDirectory"sola_right_arrow_nomal.bmp", 
                me->rightArrow
                );
        }
        me->rightArrowEnable = enable;
    }
}

static int GetSolaItemXOffsetInDisplayArea(SOLA_WIDGET_ITEM* me)
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

static void MoveSolaItemsPostion(SOLA_WIDGET* me, int xOffset, int yOffset)
{
    int i;
    for (i = 0; i < me->itemCount; ++i)
    {
        SolaWidgetItemMove(me->items[i], xOffset, yOffset);
    }
}

/****************************
* 所有按钮处理函数的具体实现
****************************/
static int SolaWidgetItem_Down(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    )
{
    return 0;
}

static int SolaWidgetItem_Up(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    )
{
    SOLA_WIDGET_ITEM* me = (SOLA_WIDGET_ITEM*)pOutArg;
    SolaWidgetItemClicked(me->parent, me->index);
    return 0;
}

static int SolaWidgetLeftArrow_Down(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    )
{
    SOLA_WIDGET* me = (SOLA_WIDGET*)pOutArg;

    if (me->leftArrowEnable)
    {
        SetPictureBitmap(
            BmpFileDirectory"sola_left_arrow_press.bmp",
            me->leftArrow
            );
        DisplayPicture(me->leftArrow);
        SyncCurrFbmap();
    }
    return 0;
}

static int SolaWidgetLeftArrow_Up(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    )
{
    SOLA_WIDGET* me = (SOLA_WIDGET*)pOutArg;

    if (me->leftArrowEnable)
    {
        SetPictureBitmap(
            BmpFileDirectory"sola_left_arrow_nomal.bmp",
            me->leftArrow
            );
        SolaWidgetItemsFocusPrev(me);
    }
    return 0;
}

static int SolaWidgetRightArrow_Down(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    )
{
    SOLA_WIDGET* me = (SOLA_WIDGET*)pOutArg;
    if (me->rightArrowEnable)
    {
        SetPictureBitmap(
            BmpFileDirectory"sola_right_arrow_press.bmp",
            me->rightArrow
            );
        DisplayPicture(me->rightArrow);
        SyncCurrFbmap();
    }
    return 0;
}

static int SolaWidgetRightArrow_Up(
    void *pInArg, int iInLen,
    void *pOutArg, int iOutLen
    )
{
    SOLA_WIDGET* me = (SOLA_WIDGET*)pOutArg;

    if (me->rightArrowEnable)
    {
        SetPictureBitmap(
            BmpFileDirectory"sola_right_arrow_nomal.bmp",
            me->rightArrow
            );
        SolaWidgetItemsFocusNext(me);
    }
    return 0;
}
static void judgeDistance(SOLA_WIDGET* me)
{
    LINK_ISPASS *linkIsPass = &me->solaEvents->EventTable.LinkIsPass;
    SOLA_MAX_MIN_SET *linkLengthThreshold = &pSolaMessager->linePassThreshold.lineLength;
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), me->distanceLabel);
    switch((int)linkLengthThreshold->type)
    {
     case 0:
        disPassed = 2;
        break;
     case 1:
        if(linkLengthThreshold->min > (me->distance/1000))
        {
            SetLabelFont(getGlobalFnt(EN_FONT_RED), me->distanceLabel);
            disPassed = 0;
        }
        else
        {
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), me->distanceLabel);
            disPassed = 1;
        }
        break;
     case 2:
        if(linkLengthThreshold->max < (me->distance/1000))
        {
            SetLabelFont(getGlobalFnt(EN_FONT_RED), me->distanceLabel);
            disPassed = 0;
        }
        else
        {
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), me->distanceLabel);
            disPassed = 1;
        }
        break;
     case 3:
        if((linkLengthThreshold->min > (me->distance/1000))
            || (linkLengthThreshold->max < (me->distance/1000)))
        {
            SetLabelFont(getGlobalFnt(EN_FONT_RED), me->distanceLabel);
            disPassed = 0;
        }
        else
        {
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), me->distanceLabel);
            disPassed = 1;
        }
        break;
    }
    linkIsPass->iIsRegionLengthPass = disPassed;
}