/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_popselector.c
* 摘    要：  弹窗形式的选择框
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "wnd_popselector.h"

#include "wnd_global.h"

//记录偏移量
#define UP_OFFSET       24
#define DOWN_OFFSET     24
#define LEFT_OFFSET     20
#define RIGHT_OFFSET    20

#define ITEM_WIDTH      360
#define ITEM_HEIGHT     32
#define ITEM_GAP        8

#define WND_WIDTH       400

//记录上次焦点位置
static int iOldFocus = 0;
//选择框图片资源
static char *pSelectedBmp[2] = 
{
	BmpFileDirectory "btn_default_unchecked_unpress.bmp", 
	BmpFileDirectory "btn_default_checked_unpress.bmp"
};
static char* pSelectedBmpf[2] = 
{ 
	BmpFileDirectory"btn_default_unchecked_press.bmp", 
	BmpFileDirectory"btn_default_checked_press.bmp" 
};

//按键响应处理
static int PopLayerKey_Down(void *pInArg, int iInLen,
                             void *pOutArg, int iOutLen);
static int PopLayerKey_Up(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen);

/***
  * 功能：
     	创建弹窗控件
  * 参数：
  		1.int x: 起始点横坐标
  		2.int iNum: 列表数量
  		3.char **str1: 列表内容(英文)
		4.int *str2: 列表内容（多国语）
  		5.int iSelect:  焦点
  		6.DROPACKFUNC BackCall: 回调函数
  		7.GUIWINDOW *pWnd: 当前窗体
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/
POP_SELECTOR *CreatePopSelector(int x, int iNum, char **str1, int *str2,
                                int iSelect, POPBACKFUNC BackCall,
                                GUIWINDOW *pWnd)
{
    int iErr = 0;
    POP_SELECTOR *pPopPicker = NULL;
    GUIMESSAGE *pMsg;

    if (((str1 == NULL) && (str2 == NULL)) || pWnd == NULL)
        return NULL;

    if (!iErr)
    { 
        //分配资源
        pPopPicker = (POP_SELECTOR *)calloc(1, sizeof(POP_SELECTOR));
        if (NULL == pPopPicker)
        {
            LOG(LOG_ERROR, "pPopPicker is %p.\n", pPopPicker);
            iErr = -1;
        }
    }

    if (!iErr)
    {
        int i = 0;
        pPopPicker->iItemCnt = iNum;
        pPopPicker->iFocus = iSelect;
        iOldFocus = iSelect;
        pPopPicker->BackCallFunc = BackCall;
        pPopPicker->pWnd = pWnd;

        pPopPicker->pPopLayerLbl = CreateLabel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
        pPopPicker->pPopLayerLbl->Visible.iLayer = DROPLIST_LAYER - 1;
        pPopPicker->pPopLayerLbl->Visible.iCursor = 1;
        pPopPicker->pPopLayerLbl->Visible.iFocus = 1;
        //绘制背景
        int height = UP_OFFSET + DOWN_OFFSET + iNum * (ITEM_HEIGHT+ITEM_GAP);
        int y = (WINDOW_HEIGHT - height) / 2;
        pPopPicker->pPopBg = CreatePicture(x, y, WND_WIDTH, height, BmpFileDirectory "bg_pop_selector.bmp");

        for (i = 0; i < iNum; i++)
        {
            if (i != iSelect)
            {
                pPopPicker->pPopItemBg[i] = CreatePicture(x + LEFT_OFFSET, y + i * (ITEM_HEIGHT + ITEM_GAP) + UP_OFFSET,
                                                          ITEM_WIDTH, ITEM_HEIGHT, BmpFileDirectory "pop_list_unpress.bmp");
                pPopPicker->pPopItemBtn[i] = CreatePicture(x + LEFT_OFFSET+303, y + i * (ITEM_HEIGHT + ITEM_GAP) + UP_OFFSET+6,
                                                          20, 20, pSelectedBmp[0]);
            }

            else
            {
                pPopPicker->pPopItemBg[i] = CreatePicture(x + LEFT_OFFSET, y + i * (ITEM_HEIGHT + ITEM_GAP) + UP_OFFSET,
                                                          ITEM_WIDTH, ITEM_HEIGHT, BmpFileDirectory "pop_list_press.bmp");
                pPopPicker->pPopItemBtn[i] = CreatePicture(x + LEFT_OFFSET + 303, y + i * (ITEM_HEIGHT + ITEM_GAP) + UP_OFFSET + 6,
                                                           20, 20, pSelectedBmpf[1]);
            }
            if (!str2)
            {
                pPopPicker->pPopItemStr[i] = TransString(str1[i]);
            }
            else
            {
                //针对下拉框中出现多国语的情况
                pPopPicker->pPopItemStr[i] = GetCurrLanguageText(str2[i]);
            }

            pPopPicker->pPopItemLbl[i] = CreateLabel(x + LEFT_OFFSET + 37, y + i * (ITEM_HEIGHT + ITEM_GAP) + UP_OFFSET + 8,
                                                    100, 16, pPopPicker->pPopItemStr[i]);
            pPopPicker->pPopItemLbl[i]->Visible.iLayer = DROPLIST_LAYER;
            pPopPicker->pPopItemLbl[i]->Visible.iCursor = 1;
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pPopPicker->pPopItemLbl[i]);
        }

        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
                      pPopPicker->pPopLayerLbl, pWnd);
        for (i = 0; i < iNum; i++)
        {
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
                          pPopPicker->pPopItemLbl[i], pWnd);
        }
        //注册消息项
        pMsg = GetCurrMessage();
        LoginMessageReg(GUIMESSAGE_KEY_DOWN, pPopPicker->pPopLayerLbl,
                        PopLayerKey_Down, pPopPicker, 4, pMsg);
        LoginMessageReg(GUIMESSAGE_KEY_UP, pPopPicker->pPopLayerLbl,
                        PopLayerKey_Up, pPopPicker, 4, pMsg);
        //显示
        DisplayPicture(pPopPicker->pPopBg);

        for (i = 0; i < pPopPicker->iItemCnt; i++)
        {
            DisplayPicture(pPopPicker->pPopItemBg[i]);
            DisplayLabel(pPopPicker->pPopItemLbl[i]);
            DisplayPicture(pPopPicker->pPopItemBtn[i]);
        }

        //设置标题栏的电量、日期的刷新
        SetPowerEnable(2, 1);
        TouchChange(NULL, NULL, NULL, pPopPicker->pPopItemLbl[iSelect], 1);
        RefreshScreen(__FILE__, __func__, __LINE__);
    }

    return pPopPicker;
}

//销毁上拉列表控件
static int DestroyPopSelector(POP_SELECTOR **pPopPicker)
{
    int i = 0;
    GUIMESSAGE *pMsg;

    if (pPopPicker == NULL)
        return -1;

    pMsg = GetCurrMessage();
    DelWindowComp((*pPopPicker)->pPopLayerLbl, (*pPopPicker)->pWnd);

    for (i = 0; i < (*pPopPicker)->iItemCnt; i++)
    {
        DelWindowComp((*pPopPicker)->pPopItemLbl[i], (*pPopPicker)->pWnd);
    }

    LogoutMessageReg(GUIMESSAGE_KEY_DOWN, (*pPopPicker)->pPopLayerLbl, pMsg);
    LogoutMessageReg(GUIMESSAGE_KEY_UP, (*pPopPicker)->pPopLayerLbl, pMsg);
    DestroyLabel(&((*pPopPicker)->pPopLayerLbl));

    for (i = 0; i < (*pPopPicker)->iItemCnt; i++)
    {
        DestroyPicture(&((*pPopPicker)->pPopItemBg[i]));
        DestroyLabel(&((*pPopPicker)->pPopItemLbl[i]));
        GuiMemFree((*pPopPicker)->pPopItemStr[i]);
    }

    free((*pPopPicker));
    (*pPopPicker) = NULL;

    return 0;
}

//弹起函数
static int PopSelector_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    POP_SELECTOR *pPopPicker = (POP_SELECTOR *)pOutArg;
    int iTouch = iOutLen;
    pPopPicker->BackCallFunc(iTouch);
    DestroyPopSelector(&pPopPicker);

    return 0;
}

//中间layer层弹起函数
static int PopLayer_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    POP_SELECTOR *pPopPicker = (POP_SELECTOR *)pOutArg;
    pPopPicker->BackCallFunc(iOldFocus);
    DestroyPopSelector(&pPopPicker);

    return 0;
}

//刷新列表信息
static int RefreshPopList(POP_SELECTOR *pPopPicker)
{
    if (pPopPicker == NULL)
    {
        return -1;
    }

    int i;
    int iFocus = pPopPicker->iFocus;

    for (i = 0; i < pPopPicker->iItemCnt; ++i)
    {
        SetPictureBitmap(pSelectedBmp[0], pPopPicker->pPopItemBtn[i]);
        SetPictureBitmap(BmpFileDirectory "pop_list_unpress.bmp", pPopPicker->pPopItemBg[i]);
    }

    if (iFocus != -1)
    {
        SetPictureBitmap(pSelectedBmpf[1], pPopPicker->pPopItemBtn[iFocus]);
        SetPictureBitmap(BmpFileDirectory "pop_list_press.bmp", pPopPicker->pPopItemBg[iFocus]);
    }

    //刷新背景
    DisplayPicture(pPopPicker->pPopBg);
    for (i = 0; i < pPopPicker->iItemCnt; ++i)
    {
        DisplayPicture(pPopPicker->pPopItemBg[i]);
        DisplayLabel(pPopPicker->pPopItemLbl[i]);
        DisplayPicture(pPopPicker->pPopItemBtn[i]);
    }

    RefreshScreen(__FILE__, __func__, __LINE__);

    return 0;
}

static int PopLayerKey_Down(void *pInArg, int iInLen,
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_ESC:
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}

static int PopLayerKey_Up(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    POP_SELECTOR *pPopPicker = (POP_SELECTOR *)pOutArg;
    //检查参数
    if (!pPopPicker)
    {
        LOG(LOG_ERROR, "pPopPicker is %p.\n", pPopPicker);
        return -1;
    }

    switch (uiValue)
    {
    case KEYCODE_UP:
        if (pPopPicker->iFocus != 0)
            pPopPicker->iFocus -= 1;
        RefreshPopList(pPopPicker);
        break;
    case KEYCODE_DOWN:
        if (pPopPicker->iFocus != (pPopPicker->iItemCnt - 1))
            pPopPicker->iFocus += 1;
        RefreshPopList(pPopPicker);
        break;
    case KEYCODE_ENTER:
        PopSelector_Up(NULL, iInLen, pOutArg, pPopPicker->iFocus);
        break;
    case KEYCODE_ESC:
        PopLayer_Up(pInArg, iInLen, pOutArg, iOutLen);
        break;
    default:
        break;
    }

    return iReturn;
}