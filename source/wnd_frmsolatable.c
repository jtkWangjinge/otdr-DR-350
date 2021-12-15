/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsloatable.c
* 摘    要：  声明主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.1.0
* 作    者：  wjg
* 完成日期：  2014-12-04
*
* 取代版本：  v1.0.2
* 原 作 者：  
* 完成日期：  2016-11-07
*******************************************************************************/

#include "wnd_frmsolatable.h"
#include "guibase.h"
#include "wnd_global.h"
#include "wnd_frmsolaconfig.h"
#include <limits.h>
#include "app_frminit.h"
#include "guiphoto.h"
#include "wnd_frmsolamessager.h"
#include "app_unitconverter.h"
/*********************
* 显示区域相关的宏定义
*********************/
#define Y_OFFSET 5
static GUIPICTURE* pSolaTableBgLoss1310 = NULL;
static GUIPICTURE* pSolaTableBgLoss1550 = NULL;
static GUIPICTURE* pSolaTableBgIOR1310 = NULL;
static GUIPICTURE* pSolaTableBgIOR1550 = NULL;

static GUIPICTURE* pSolaTableLineLoss1310 = NULL;
static GUIPICTURE* pSolaTableLineLoss1550 = NULL;
static GUIPICTURE* pSolaTableLineIOR1310 = NULL;
static GUIPICTURE* pSolaTableLineIOR1550 = NULL;
/************************
* 内部使用的静态函资源定义
************************/
static void setSolaLineTableWaveLength(SOLA_TABLE_LINE* pThis, int wave);
static void setSolaLineTableTotalLoss(SOLA_TABLE_LINE* pThis, SolaEvents* pEndEvent);

static void setSolaEventTableWaveLength(SOLA_TABLE_EVENT_ITEM* pThis, int wave);
static void setSolaEventItem(SOLA_TABLE_EVENT_ITEM* pThis, SolaEvents* pEvent, int eventIndex);
static void setSolaEventType(SOLA_TABLE_EVENT_ITEM* pThis, SolaEvents* pEvent, 
                             int eventIndex, int direction);
typedef int (*SOLAISPASSED)(const EVENTS_INFO*, int, int, int);
void setSolaIsPassed(struct _sola_table_pass* pThis, SolaEvents* events);
static void judgePass(SOLAISPASSED pFUN,int waveLen,const EVENTS_INFO* solaEvent,int splitter_type,int otdr_type,GUIPICTURE* ico);
static int judgeLinePara(SOLA_TABLE_LINE* solaTableline,int currWave,float totalLoss,SolaEvents* pEvent,GUIPICTURE* ico1,GUIPICTURE* ico2);
/***********************
* 内部使用的功能静态函数
***********************/
static int linePassed = 0;
extern int disPassed;
extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//?????????
extern SOLA_MESSAGER* pSolaMessager;

/***********************
* 所有公有函数的具体实现
***********************/
SOLA_TABLE_LINE* CreateSolaLineTable(
    int x, int y, int w, int h,
    GUIWINDOW* parent
    )
{
    if(!parent)
        return NULL;
        
    SOLA_TABLE_LINE* line = (SOLA_TABLE_LINE*)malloc(sizeof(SOLA_TABLE_LINE));
    if(NULL == line)
        return NULL;
    line->visible.Area.Start.x = x;
    line->visible.Area.Start.y = y;
    line->visible.Area.End.x = x + w - 1;
    line->visible.Area.End.y = y + h - 1;
    line->visible.iEnable = 1;
    line->visible.iLayer = 0;
    line->visible.iFocus = 0;
    line->visible.iCursor = 0;
    line->parent = parent;
    line->wavelen = 1310;
    line->setWaveLen = setSolaLineTableWaveLength;
    line->setTotalLoss = setSolaLineTableTotalLoss;
    line->background = CreatePhoto("sola_table_line");
    
    line->TitleText = TransString("SOLA");
    line->TitleLabel = CreateLabel(x, y+Y_OFFSET, 100, 24, line->TitleText);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->TitleLabel);

    line->Title1310Text = TransString("1310 nm");
    line->Title1550Text = TransString("1550 nm");

    line->Title1310Label = CreateLabel(x+100, y+Y_OFFSET, 100, 24, line->Title1310Text);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->Title1310Label);
    line->Title1550Label  = CreateLabel(x+100+100, y+Y_OFFSET, 100, 24, line->Title1550Text);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->Title1550Label);

    line->TitleLineLossText = TransString("SOLA_LINE_LOSS");
    line->TitleLineReturnText = TransString("SOLA_LINE_RETURN_LOSS");

    line->TitleLineLossLabel = CreateLabel(x, y+23+Y_OFFSET, 100, 24, line->TitleLineLossText);
    line->TitleLineReturnLabel = CreateLabel(x, y+23+25+Y_OFFSET, 100, 24, line->TitleLineReturnText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), line->TitleLineLossLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), line->TitleLineReturnLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->TitleLineLossLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->TitleLineReturnLabel);

    line->Line1310LossText = TransString("--");
    line->Line1310ReturnText = TransString("--");
    line->Line1310LossLabel = CreateLabel(x+100, y+23+Y_OFFSET, 100, 24, line->Line1310LossText);
    
    line->Line1310ReturnLossLabel = CreateLabel(x+100, y+23+25+Y_OFFSET, 100, 24, line->Line1310ReturnText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), line->Line1310LossLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), line->Line1310ReturnLossLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->Line1310LossLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->Line1310ReturnLossLabel);

    line->Line1550LossText = TransString("--");
    line->Line1550ReturnText = TransString("--");
    line->Line1550LossLabel = CreateLabel(x+100+100, y+23+Y_OFFSET, 100, 24, line->Line1550LossText);
    line->Line1550ReturnLossLabel = CreateLabel(x+100+100, y+23+25+Y_OFFSET, 100, 24, line->Line1550ReturnText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), line->Line1550LossLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), line->Line1550ReturnLossLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->Line1550LossLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, line->Line1550ReturnLossLabel);
    
    return line;
}

static void setSolaLineTableWaveLength(SOLA_TABLE_LINE* pThis, int wave)
{
    pThis->wavelen = wave;
}

static void setSolaLineTableTotalLoss(SOLA_TABLE_LINE* pThis, SolaEvents* pEvent)
{
    const EVENTS_INFO* solaEvent = NULL;
    
    int wave1310Passed = 0;
    int wave1550Passed = 0;
    char buffTotal[32] = {0};
    char buffReturn[32] = {0};
    GuiMemFree(pThis->Line1310LossText);
    GuiMemFree(pThis->Line1310ReturnText);
    GuiMemFree(pThis->Line1550LossText);
    GuiMemFree(pThis->Line1550ReturnText);

    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableLineLoss1310);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableLineLoss1550);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableLineIOR1310);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableLineIOR1550);
    
    solaEvent = SolaEvents_GetEndEvent(pEvent, WAVELEN_1310);
    if(solaEvent)
    {
        float totalLoss = SolaEvents_GetTotalLoss(pEvent, WAVELEN_1310);
        char* pBuffTotal = Float2StringUnit(LOSS_PRECISION, totalLoss, " dB");
        float totalReturnLoss = fabsf(pEvent->TotalReturnLoss[WAVELEN_1310]);
        char* pBuffReturn = Float2StringUnit(ORL_PRECISION, totalReturnLoss, " dB");
        strcpy(buffTotal, pBuffTotal);
        strcpy(buffReturn, pBuffReturn);
            
        wave1310Passed = judgeLinePara(pThis,WAVELEN_1310,totalLoss,pEvent,pSolaTableLineLoss1310,pSolaTableLineIOR1310);
        GuiMemFree(pBuffTotal);
        GuiMemFree(pBuffReturn);
    }
    else
    {
        sprintf(buffTotal, "--");
        sprintf(buffReturn, "--");
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1310);
        wave1310Passed = 2;
    }
    pThis->Line1310LossText = TransString(buffTotal);
    pThis->Line1310ReturnText = TransString(buffReturn);
    
    solaEvent = SolaEvents_GetEndEvent(pEvent, WAVELEN_1550);
    if(solaEvent)
    {
        float totalLoss = SolaEvents_GetTotalLoss(pEvent, WAVELEN_1550);
        char* pBuffTotal = Float2StringUnit(LOSS_PRECISION, totalLoss, " dB");
        float totalReturnLoss = fabsf(pEvent->TotalReturnLoss[WAVELEN_1550]);
        char* pBuffReturn = Float2StringUnit(ORL_PRECISION, totalReturnLoss, " dB");
        strcpy(buffTotal, pBuffTotal);
        strcpy(buffReturn, pBuffReturn);
        
        //????t·???Χ
        wave1550Passed = judgeLinePara(pThis,WAVELEN_1550,totalLoss,pEvent,pSolaTableLineLoss1550,pSolaTableLineIOR1550);
        GuiMemFree(pBuffTotal);
        GuiMemFree(pBuffReturn);
    }
    else
    {
        sprintf(buffTotal, "--");
        sprintf(buffReturn, "--");
        wave1550Passed = 2;
    }
    pThis->Line1550LossText = TransString(buffTotal);
    pThis->Line1550ReturnText = TransString(buffReturn);
    SetLabelText(pThis->Line1310LossText, pThis->Line1310LossLabel);
    SetLabelText(pThis->Line1310ReturnText, pThis->Line1310ReturnLossLabel);
    SetLabelText(pThis->Line1550LossText, pThis->Line1550LossLabel);
    SetLabelText(pThis->Line1550ReturnText, pThis->Line1550ReturnLossLabel);

    if((wave1310Passed == 2) && (wave1550Passed == 2))
    {
        linePassed = 2;
    }
    if((wave1310Passed == 2) && (wave1550Passed != 2))
    {
        linePassed = wave1550Passed;
    }
    if((wave1310Passed != 2) && (wave1550Passed == 2))
    {
        linePassed = wave1310Passed;
    }
    if((wave1310Passed != 2) && (wave1550Passed != 2))
    {
        linePassed = wave1310Passed && wave1550Passed;
    }
    DisplaySolaLineTable(pThis);
}

void DisplaySolaLineTable(SOLA_TABLE_LINE* line)
{
    DisplayPicture(line->background);
    DisplayPicture(pSolaTableLineLoss1310 );
    DisplayPicture(pSolaTableLineLoss1550 );
    DisplayPicture(pSolaTableLineIOR1310 );
    DisplayPicture(pSolaTableLineIOR1550 );
    
    //Table Title Display
    DisplayLabel(line->TitleLabel);
    DisplayLabel(line->Title1310Label);
    DisplayLabel(line->Title1550Label);
    DisplayLabel(line->TitleLineLossLabel);
    DisplayLabel(line->TitleLineReturnLabel);

    //Table Loss Value Display
    DisplayLabel(line->Line1310LossLabel);   
    DisplayLabel(line->Line1310ReturnLossLabel);
    DisplayLabel(line->Line1550LossLabel);
    DisplayLabel(line->Line1550ReturnLossLabel);
}

void DestroySolaLineTable(SOLA_TABLE_LINE* line)
{
    DestroyPicture(&(line->background));
    DestroyPicture(&pSolaTableLineLoss1310 );
    DestroyPicture(&pSolaTableLineLoss1550 );
    DestroyPicture(&pSolaTableLineIOR1310 );
    DestroyPicture(&pSolaTableLineIOR1550 );
    
    DestroyLabel(&(line->TitleLabel));
    DestroyLabel(&(line->Title1310Label));
    DestroyLabel(&(line->Title1550Label));
    DestroyLabel(&(line->TitleLineLossLabel));
    DestroyLabel(&(line->TitleLineReturnLabel));

    DestroyLabel(&(line->Line1310LossLabel));
    DestroyLabel(&(line->Line1310ReturnLossLabel));
    DestroyLabel(&(line->Line1550LossLabel));
    DestroyLabel(&(line->Line1550ReturnLossLabel));
    
    GuiMemFree(line->TitleText);
    GuiMemFree(line->Title1310Text);
    GuiMemFree(line->Title1550Text);
    GuiMemFree(line->TitleLineLossText);
    GuiMemFree(line->TitleLineReturnText);
    
    GuiMemFree(line->Line1310LossText);
    GuiMemFree(line->Line1310ReturnText);
    GuiMemFree(line->Line1550LossText);
    GuiMemFree(line->Line1550ReturnText);

    GuiMemFree(line);
}

SOLA_TABLE_EVENT_ITEM* CreateSolaEventItemTable(
    int x, int y, int w, int h,
    GUIWINDOW* parent
    )
{
    SOLA_TABLE_EVENT_ITEM* item = (SOLA_TABLE_EVENT_ITEM*)malloc(sizeof(SOLA_TABLE_EVENT_ITEM));
    if(NULL == item)
        return NULL;
    item->visible.Area.Start.x = x;
    item->visible.Area.Start.y = y;
    item->visible.Area.End.x = x + w - 1;
    item->visible.Area.End.y = y + h - 1;
    item->visible.iEnable = 1;
    item->visible.iLayer = 0;
    item->visible.iFocus = 0;
    item->visible.iCursor = 0;
    item->parent = parent;
    item->wavelen = 1310;
    item->eventSelectedIndex = -1;
    item->setWaveLen = setSolaEventTableWaveLength;
    item->setEventItem = setSolaEventItem;
    item->setEventItemType = setSolaEventType;
    item->background = CreatePhoto("sola_table_item");

    item->typeTitle = TransString("SOLA_EVENT_TYPE");
    item->typeTitleLabel = CreateLabel(x, y+Y_OFFSET, 111, 26, item->typeTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->typeTitleLabel);

    item->distanceTitle = TransString("SOLA_EVENT_DIST");
    item->distanceTitleLabel = CreateLabel(x+108, y+Y_OFFSET, 100, 26, item->distanceTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->distanceTitleLabel);

    item->loss1310Title = TransString("SOLA_EVENT_LOSS");
    item->loss1310TitleLabel = CreateLabel(x+112+100, y+Y_OFFSET, 100, 26, item->loss1310Title);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->loss1310TitleLabel);

    item->IOR1310Title = TransString("SOLA_EVENT_IOR");
    item->IOR1310TitleLabel = CreateLabel(x+112+100+100, y+Y_OFFSET, 100, 26, item->IOR1310Title);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->IOR1310TitleLabel);

    item->loss1550Title = TransString("SOLA_EVENT_LOSS");
    item->loss1550TitleLabel = CreateLabel(x+112+100+100+102, y+Y_OFFSET, 100, 26, item->loss1550Title);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->loss1550TitleLabel);

    item->IOR1550Title = TransString("SOLA_EVENT_IOR");
    item->IOR1550TitleLabel = CreateLabel(x+112+100+100+100+104, y+Y_OFFSET, 100, 26, item->IOR1550Title);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->IOR1550TitleLabel);

    item->wavelen1310Title1 = TransString("1310 nm");
    item->wavelen1310Label1 = CreateLabel(x+112+100, y+26+Y_OFFSET, 100, 26, item->wavelen1310Title1);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->wavelen1310Label1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->wavelen1310Label1);

    item->wavelen1310Title2 = TransString("1310 nm");
    item->wavelen1310Label2 = CreateLabel(x+112+100+100, y+26+Y_OFFSET, 100, 26, item->wavelen1310Title2);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->wavelen1310Label2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->wavelen1310Label2);

    item->wavelen1550Title1 = TransString("1550 nm");
    item->wavelen1550Label1 = CreateLabel(x+112+100+100+100, y+26+Y_OFFSET, 100, 26, item->wavelen1550Title1);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->wavelen1550Label1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->wavelen1550Label1);

    item->wavelen1550Title2 = TransString("1550 nm");
    item->wavelen1550Label2 = CreateLabel(x+112+100+100+100+100, y+26+Y_OFFSET, 100, 26, item->wavelen1550Title2);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->wavelen1550Label2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->wavelen1550Label2);

    item->eventIcon= CreatePicture(x+30, y+55, 47, 19, BmpFileDirectory"sola_table_item_null.bmp");
    item->upButton = CreatePicture(x+2, y+50, 24, 24, BmpFileDirectory"btn_sola_type_up.bmp");
    item->downButton = CreatePicture(x+78, y+50, 24, 24, BmpFileDirectory"btn_sola_type_down.bmp");
    item->upButtonTouch = CreatePicture(x+2-12, y+50-12, 48, 48, NULL);
    item->downButtonTouch = CreatePicture(x+78-12, y+50-12, 48, 48, NULL);
    
    item->distanceText = TransString("");
    item->distanceLabel = CreateLabel(x+105, y+26+26+Y_OFFSET, 100, 26, item->distanceText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->distanceLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->distanceLabel);

    item->loss1310 = TransString("");
    item->loss1310Label = CreateLabel(x+112+100, y+26+26+Y_OFFSET, 100, 26, item->loss1310);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->loss1310Label);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->loss1310Label);

    item->IOR1310 = TransString("");
    item->IOR1310Label = CreateLabel(x+112+100+100, y+26+26+Y_OFFSET, 100, 26, item->IOR1310);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->IOR1310Label);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->IOR1310Label);
    
    pSolaTableBgLoss1310 = CreatePicture(x + 104*2, 423, 100, 24, NULL);
    pSolaTableBgIOR1310 = CreatePicture(x + 104*3-1, 423, 100, 24, NULL);
    pSolaTableBgLoss1550 = CreatePicture(x + 104*4-2, 423, 100, 24, NULL);
    pSolaTableBgIOR1550 = CreatePicture(x + 104*5-3, 423, 100, 24, NULL);

    pSolaTableLineLoss1310 = CreatePicture(133, 312, 100, 24, NULL);
    pSolaTableLineLoss1550 = CreatePicture(236, 312, 100, 24, NULL);
    pSolaTableLineIOR1310 = CreatePicture(133, 339, 100, 23, NULL);
    pSolaTableLineIOR1550 = CreatePicture(236, 339, 100, 23, NULL);
    
    item->loss1550 = TransString("");
    item->loss1550Label = CreateLabel(x+112+100+100+100, y+26+26+Y_OFFSET, 100, 26, item->loss1550);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->loss1550Label);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->loss1550Label);

    item->IOR1550 = TransString("");
    item->IOR1550Label = CreateLabel(x+112+100+100+100+100, y+26+26+Y_OFFSET, 100, 26, item->IOR1550);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), item->IOR1550Label);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, item->IOR1550Label);

    return item;
}

static void setSolaEventTableWaveLength(SOLA_TABLE_EVENT_ITEM* pThis, int wave)
{
    pThis->wavelen = wave;
}

//改变sola事件类型，
//direction = 1 ---->next;
//direction = -1 ---->previous
static void setSolaEventType(SOLA_TABLE_EVENT_ITEM* pThis, SolaEvents* pEvent, 
                                int eventIndex, int direction)
{
    int solaType = 1;
    const EVENTS_INFO* solaEvent = NULL;
    solaEvent = SolaEvents_GetEvent(pEvent, eventIndex);
    if(!solaEvent)
       return;
    solaType = SolaEvents_GetEventType(pEvent, eventIndex, Splitter_eventType);
    
    int tmpLoss = (int)(pEvent->EventTable.EventsInfo[eventIndex].fLoss / 3 + 1);

    if (tmpLoss > 7)
     tmpLoss = 7;

    if(solaType >= 0 && solaType <= tmpLoss)
    {  
        solaType += direction;
    }
    if(solaType < 0 || solaType > tmpLoss)
    {  
        solaType -= direction;
    }

    SolaEvents_SetEventType(pEvent, eventIndex, Splitter_eventType, solaType);

    IsSolaEventPassed(pEvent, eventIndex);
}

static void setSolaEventItem(SOLA_TABLE_EVENT_ITEM* pThis, SolaEvents* pEvent, int eventIndex)
{
    const EVENTS_INFO* solaEvent = NULL;
    const EVENTS_INFO* otdrEvent = NULL;

    char bmpPath[PATH_MAX];
    char lossBuff[32];
    char IORBuff[32];
    char distBuff[32];
    int splitter_type = 0;
    int otdr_type = -1;
    int launchFiberIndex = pEvent->EventTable.iLaunchFiberIndex;
    int recvFiberIndex = pEvent->EventTable.iRecvFiberIndex;
    float launchFiberPosition = pEvent->EventTable.EventsInfo[launchFiberIndex].fEventsPosition;

    splitter_type = SolaEvents_GetEventType(pEvent, eventIndex, Splitter_eventType);//= (info->iStyle & 0xffff0000) >> 16;
    otdr_type = SolaEvents_GetEventType(pEvent, eventIndex, Ordinary_eventType);//info->iStyle & 0x0000ffff;

    GuiMemFree(pThis->distanceText);
    solaEvent = SolaEvents_GetEvent(pEvent, eventIndex);
    
    if(solaEvent)
        pThis->eventSelectedIndex = eventIndex;

    if(solaEvent)
    {
        //???λ
        float fTemp1 = 0.0f;
        if(pEvent->EventTable.iLaunchFiberIndex > 0)
        {
            fTemp1 = solaEvent->fEventsPosition - launchFiberPosition;
        }
        else
        {
            fTemp1 = solaEvent->fEventsPosition;
        }
        
        char* dist = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_SOLA, fTemp1, 1);
        sprintf(distBuff, "%s", dist);
        GuiMemFree(dist);
        //起点
        if((launchFiberIndex > 0 && launchFiberIndex == eventIndex)
            || (launchFiberIndex <= 0 && eventIndex == 0))   
        {
            strcat(distBuff, "(A)");
        }
        //终点
        else if((recvFiberIndex > 0 && recvFiberIndex == eventIndex)
            || (recvFiberIndex <= 0 
            && pEvent->EventTable.iEventsNumber > 1 
            && eventIndex == (pEvent->EventTable.iEventsNumber - 1)))
        {
            strcat(distBuff, "(B)");
        }
        //使用统一的接口来获得事件类型的名字作为文件名的索引
        const char* typeName = SolaEvents_GetEventTypeName(pEvent, eventIndex);
        sprintf(bmpPath,BmpFileDirectory"type_%s_icon.bmp",
                typeName);
    }
    else
    {
        sprintf(distBuff, "--");
        sprintf(bmpPath,BmpFileDirectory"sola_table_item_null.bmp");

    }
    pThis->distanceText = TransString(distBuff);
    SetLabelText(pThis->distanceText, pThis->distanceLabel);
    SetPictureBitmap(bmpPath, pThis->eventIcon);

    GuiMemFree(pThis->loss1310);
    GuiMemFree(pThis->IOR1310);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pThis->loss1310Label);        
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pThis->IOR1310Label);

    otdrEvent = SolaEvents_GetWaveEvent(pEvent, eventIndex, WAVELEN_1310);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1310);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgIOR1310);

    if(otdrEvent)
    {
        char* pLoss = Float2String(LOSS_PRECISION, otdrEvent->fLoss);
        sprintf(lossBuff, "%s", pLoss);
        GuiMemFree(pLoss);
        judgePass(IsSolaLossPassed,WAVELEN_1310,otdrEvent,splitter_type,otdr_type,pSolaTableBgLoss1310);
        judgePass(IsSolaIORPassed,WAVELEN_1310,otdrEvent,splitter_type,otdr_type,pSolaTableBgIOR1310);

        int iOtdrType = solaEvent->iStyle & 0x0000ffff;

        //第一个事件最后一个事件或分析结束事件不显示损耗
        if(eventIndex+1 == SolaEvents_Count(pEvent) || eventIndex == 0 || iOtdrType == 21)
        {
            sprintf(lossBuff, "--");
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pThis->loss1310Label);   
            SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1310);
        }
        
        //只有反射事件、回波事件和复合事件显示反射率
        if( (iOtdrType == 2) || (iOtdrType == 3) || (iOtdrType == 4) ||
            (iOtdrType == 12) || (iOtdrType == 13) || (iOtdrType == 14) )
        {
            char* pIOR = Float2String(REFLECTANCE_PRECISION, otdrEvent->fReflect);
            sprintf(IORBuff, "%s", pIOR);
            GuiMemFree(pIOR);
        }
        else
        {
            sprintf(IORBuff, "--");
            SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgIOR1310);
        }
    }
    else
    {
        sprintf(lossBuff, "--");
        sprintf(IORBuff, "--");
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1310);
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgIOR1310);
    }
    pThis->loss1310 = TransString(lossBuff);
    SetLabelText(pThis->loss1310, pThis->loss1310Label);
    pThis->IOR1310 = TransString(IORBuff);
    SetLabelText(pThis->IOR1310, pThis->IOR1310Label);
    
    GuiMemFree(pThis->loss1550);
    GuiMemFree(pThis->IOR1550);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pThis->loss1550Label);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pThis->IOR1550Label);

    otdrEvent = SolaEvents_GetWaveEvent(pEvent, eventIndex, WAVELEN_1550);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1550);
    SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgIOR1550);

    if(otdrEvent)
    {
        char* pLoss = Float2String(LOSS_PRECISION, otdrEvent->fLoss);
        sprintf(lossBuff, "%s", pLoss);
        GuiMemFree(pLoss);
        
        judgePass(IsSolaLossPassed,WAVELEN_1550,otdrEvent,splitter_type,otdr_type,pSolaTableBgLoss1550);
        judgePass(IsSolaIORPassed,WAVELEN_1550,otdrEvent,splitter_type,otdr_type,pSolaTableBgIOR1550);
        
        int iOtdrType = solaEvent->iStyle & 0x0000ffff;
        //第一个事件最后一个事件或分析结束事件不显示损耗
        if(eventIndex+1 == SolaEvents_Count(pEvent) || eventIndex == 0 || iOtdrType == 21)
        {
            sprintf(lossBuff, "--");
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pThis->loss1550Label);
            SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1550);
        }
        //只有反射事件、回波事件和复合事件显示反射率
        if( (iOtdrType == 2) || (iOtdrType == 3) || (iOtdrType == 4) ||
            (iOtdrType == 12) || (iOtdrType == 13) || (iOtdrType == 14))
        {
            char* pIOR = Float2String(REFLECTANCE_PRECISION, otdrEvent->fReflect);
            sprintf(IORBuff, "%s", pIOR);
            GuiMemFree(pIOR);
        }
        else
        {
            sprintf(IORBuff, "--");
            SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgIOR1550);
        }

    }
    else
    {
        sprintf(lossBuff, "--");
        sprintf(IORBuff, "--");
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgLoss1550);
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", pSolaTableBgIOR1550);
    }
    
    pThis->loss1550 = TransString(lossBuff);
    SetLabelText(pThis->loss1550, pThis->loss1550Label);
    pThis->IOR1550 = TransString(IORBuff);
    SetLabelText(pThis->IOR1550, pThis->IOR1550Label);
    
    DisplaySolaLossTable(pThis);
}

void DisplaySolaLossTable(SOLA_TABLE_EVENT_ITEM* item)
{
    DisplayPicture(item->background);
    DisplayPicture(pSolaTableBgLoss1310);
    DisplayPicture(pSolaTableBgLoss1550);
    DisplayPicture(pSolaTableBgIOR1310);
    DisplayPicture(pSolaTableBgIOR1550);
    
    DisplayLabel(item->typeTitleLabel);
    
    DisplayPicture(item->downButton);
    DisplayPicture(item->eventIcon);
    DisplayPicture(item->upButton);

    DisplayLabel(item->distanceTitleLabel);
    DisplayLabel(item->distanceLabel);

    DisplayLabel(item->loss1310TitleLabel);
    DisplayLabel(item->IOR1310TitleLabel);
    DisplayLabel(item->loss1550TitleLabel);
    DisplayLabel(item->IOR1550TitleLabel);

    DisplayLabel(item->wavelen1310Label1);
    DisplayLabel(item->wavelen1310Label2);
    DisplayLabel(item->loss1310Label);
    DisplayLabel(item->IOR1310Label);

    DisplayLabel(item->wavelen1550Label1);
    DisplayLabel(item->wavelen1550Label2);
    DisplayLabel(item->loss1550Label);
    DisplayLabel(item->IOR1550Label);

}

void DestroySolaLossTable(SOLA_TABLE_EVENT_ITEM* item)
{
    DestroyPicture(&(item->background));  
    DestroyPicture(&pSolaTableBgLoss1310);
    DestroyPicture(&pSolaTableBgLoss1550);
    DestroyPicture(&pSolaTableBgIOR1310);
    DestroyPicture(&pSolaTableBgIOR1550);
    DestroyLabel(&(item->typeTitleLabel));    

    DestroyPicture(&(item->upButton));
    DestroyPicture(&(item->eventIcon));
    DestroyPicture(&(item->downButton));
    DestroyPicture(&(item->upButtonTouch));
    DestroyPicture(&(item->downButtonTouch));

    DestroyLabel(&(item->distanceTitleLabel));
    DestroyLabel(&(item->distanceLabel));
    
    DestroyLabel(&(item->loss1310TitleLabel));
    DestroyLabel(&(item->IOR1310TitleLabel));
    DestroyLabel(&(item->loss1550TitleLabel));
    DestroyLabel(&(item->IOR1550TitleLabel));
    
    DestroyLabel(&(item->wavelen1310Label1));
    DestroyLabel(&(item->wavelen1310Label2));

    DestroyLabel(&(item->loss1310Label));
    DestroyLabel(&(item->IOR1310Label));
    
    DestroyLabel(&(item->wavelen1550Label1));
    DestroyLabel(&(item->wavelen1550Label2));

    DestroyLabel(&(item->loss1550Label));
    DestroyLabel(&(item->IOR1550Label));

    GuiMemFree(item->typeTitle);
    GuiMemFree(item->distanceTitle);
    
    GuiMemFree(item->loss1310Title);
    GuiMemFree(item->IOR1310Title);
    GuiMemFree(item->loss1550Title);
    GuiMemFree(item->IOR1550Title);

    GuiMemFree(item->wavelen1310Title1);
    GuiMemFree(item->wavelen1310Title2);
    GuiMemFree(item->wavelen1550Title1);
    GuiMemFree(item->wavelen1550Title2);

    GuiMemFree(item->distanceText);
    GuiMemFree(item->loss1310);
    GuiMemFree(item->IOR1310);
    GuiMemFree(item->loss1550);
    GuiMemFree(item->IOR1550);
    
    GuiMemFree(item);
}


SOLA_TABLE_PASS* CreateSolaPassTable(
    int x, int y, int w, int h,
    GUIWINDOW* parent
    )
{
    if(!parent)
        return NULL;
    SOLA_TABLE_PASS* pass = (SOLA_TABLE_PASS*)malloc(sizeof(SOLA_TABLE_PASS));
    if(NULL == pass)
        return NULL;
    pass->visible.Area.Start.x = x;
    pass->visible.Area.Start.y = y;
    pass->visible.Area.End.x = x + w - 1;
    pass->visible.Area.End.y = y + h - 1;
    pass->visible.iEnable = 1;
    pass->visible.iLayer = 0;
    pass->visible.iFocus = 0;
    pass->visible.iCursor = 0;
    pass->parent = parent;
    pass->isPassed = 0;
    pass->setIsPassed = setSolaIsPassed;
    pass->background = CreatePhoto("sola_table_pass_bg");

    pass->passTitle = TransString("SOLA_PASS_RESULT");

    pass->passTitleLabel = CreateLabel(x, y+Y_OFFSET, 175, 24, pass->passTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pass->passTitleLabel);

    pass->passIcon = CreatePicture(x+70, y+25+Y_OFFSET, 34, 34, NULL);

    return pass;
}

void setSolaIsPassed(struct _sola_table_pass* pThis, SolaEvents* events)
{
    int i = 0;
    if(NULL == events)
        return;
    int eventCount = events->EventTable.iEventsNumber;
    if (eventCount <= 0)
    {
        LOG(LOG_ERROR, "Event list has no event\n");
        return;
    }
    pThis->isPassed = 1;
    int solaPass;
    for(i = 0; i < eventCount; ++i)
    {
        solaPass = IsSolaEventPassed(events, i);
        if((solaPass == 0) || (linePassed == 0) || (disPassed == 0))
        {
            pThis->isPassed = 0;
            break;
        }
        else if((solaPass == 1) || (linePassed == 1) || (disPassed == 1))
        {
            pThis->isPassed = 1;
        }
        else
        {
            pThis->isPassed = 2;
            break;
        }
    }
    if(pThis->isPassed == 0)
    {
        SetPictureBitmap(BmpFileDirectory"sola_table_no_pass.bmp", pThis->passIcon);
    }
    else if(pThis->isPassed == 1)
    {
        SetPictureBitmap(BmpFileDirectory"sola_table_pass.bmp", pThis->passIcon);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory"sola_table_nojudge.bmp", pThis->passIcon);
    }
    DisplaySolaPassTable(pThis);

}

static void judgePass(SOLAISPASSED pFUN,int waveLen,const EVENTS_INFO* solaEvent,int splitter_type,int otdr_type,GUIPICTURE* ico)
{
    if(pFUN(solaEvent, waveLen, splitter_type, otdr_type) == 1)
    {
        SetPictureBitmap(BmpFileDirectory"bg_solatable_passselect.bmp", ico);
    }
    else if(pFUN(solaEvent, waveLen, splitter_type, otdr_type) == 0)
    {
        SetPictureBitmap(BmpFileDirectory"bg_solatable_select.bmp", ico);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", ico);
    }
}

static int judgeLinePara(SOLA_TABLE_LINE* solaTableline,int currWave,float totalLoss,SolaEvents* pEvent,GUIPICTURE* ico1,GUIPICTURE* ico2)
{
    int passed = 1;
    LINK_ISPASS *linkIsPass = &pEvent->WaveEventTable[currWave].LinkIsPass;
    switch((int)pSolaMessager->linePassThreshold.lineWave[currWave].lineLoss.type)
    {
     case 0:
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", ico1);
        linkIsPass->iIsRegionLossPass = 2;
        break;
     case 1:
        if(pSolaMessager->linePassThreshold.lineWave[currWave].lineLoss.min > totalLoss){
            SetPictureBitmap(BmpFileDirectory"bg_solatable_select.bmp", ico1);
            linkIsPass->iIsRegionLossPass = 0;
        }
        else{
            SetPictureBitmap(BmpFileDirectory"bg_solatable_passselect.bmp", ico1);
            linkIsPass->iIsRegionLossPass = 1;
        }
        break;
     case 2:
        if(pSolaMessager->linePassThreshold.lineWave[currWave].lineLoss.max < totalLoss){
            SetPictureBitmap(BmpFileDirectory"bg_solatable_select.bmp", ico1);
            linkIsPass->iIsRegionLossPass = 0;
        }
        else{
            SetPictureBitmap(BmpFileDirectory"bg_solatable_passselect.bmp", ico1);
            linkIsPass->iIsRegionLossPass = 1;
        }
        break;
     case 3:
        if((pSolaMessager->linePassThreshold.lineWave[currWave].lineLoss.min > totalLoss)
            || (pSolaMessager->linePassThreshold.lineWave[currWave].lineLoss.max < totalLoss)){
            SetPictureBitmap(BmpFileDirectory"bg_solatable_select.bmp", ico1);
            linkIsPass->iIsRegionLossPass = 0;
        }
        else{
            SetPictureBitmap(BmpFileDirectory"bg_solatable_passselect.bmp", ico1);
            linkIsPass->iIsRegionLossPass = 1;
        }
        break;
    }
        
    if((int)pSolaMessager->linePassThreshold.lineWave[currWave].lineReturnLoss.type){
        if(pSolaMessager->linePassThreshold.lineWave[currWave].lineReturnLoss.max >
            ((pEvent->TotalReturnLoss[currWave]>0.0f)?pEvent->TotalReturnLoss[currWave]: \
            (pEvent->TotalReturnLoss[currWave]*(-1.0f)))){
             SetPictureBitmap(BmpFileDirectory"bg_solatable_select.bmp", ico2 );
             linkIsPass->iIsRegionORLPass = 0;
        }
        else{
             SetPictureBitmap(BmpFileDirectory"bg_solatable_passselect.bmp", ico2 );
             linkIsPass->iIsRegionORLPass = 1;
        }
    }
    else{
        SetPictureBitmap(BmpFileDirectory"bg_solatable_unselect.bmp", ico2 );
        linkIsPass->iIsRegionORLPass = 2;
    }

    if((linkIsPass->iIsRegionLossPass == 2) && (linkIsPass->iIsRegionORLPass == 2))
    {
        passed = 2;
    }
    if((linkIsPass->iIsRegionLossPass == 2) && (linkIsPass->iIsRegionORLPass != 2))
    {
        passed = linkIsPass->iIsRegionORLPass;
    }
    if((linkIsPass->iIsRegionLossPass != 2) && (linkIsPass->iIsRegionORLPass == 2))
    {
        passed = linkIsPass->iIsRegionLossPass;
    }
    if((linkIsPass->iIsRegionLossPass != 2) && (linkIsPass->iIsRegionORLPass != 2))
    {
        passed = linkIsPass->iIsRegionLossPass && linkIsPass->iIsRegionORLPass;
    }
    return passed;
}

void DisplaySolaPassTable(SOLA_TABLE_PASS* pass)
{
    DisplayPicture(pass->background);
    DisplayLabel(pass->passTitleLabel);
    DisplayPicture(pass->passIcon);

}

void DestroySolaPassTable(SOLA_TABLE_PASS* pass)
{
    DestroyPicture(&(pass->background));
    DestroyLabel(&(pass->passTitleLabel));
    DestroyPicture(&(pass->passIcon));
    
    GuiMemFree(pass->passTitle);

    GuiMemFree(pass);
}

