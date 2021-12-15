/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_popselector.h
* 摘    要：  声明弹窗形式的选择框
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*******************************************************************************/

#include "guipicture.h"
#include "guilabel.h"
#include "guiwindow.h"

#define POP_MAX 15

//上啦列表的回调函数
typedef void (*POPBACKFUNC)(int selected);

/*******************************************************************************
***                    定义wnd_droppicker.h结构定义                          ***
*******************************************************************************/
typedef struct _wnd_pop_selector
{
    GUIPICTURE *pPopBg;                 //整体背景
    GUIPICTURE *pPopItemBg[POP_MAX];    //背景
    GUIPICTURE *pPopItemBtn[POP_MAX];   //选中按钮
    GUILABEL *pPopItemLbl[POP_MAX];     //中心
    GUICHAR *pPopItemStr[POP_MAX];      //选项控件的图片

    GUILABEL *pPopLayerLbl;             //图层

    POPBACKFUNC BackCallFunc;           //回调函数
    GUIWINDOW *pWnd;

    int iEnable[POP_MAX];               //每个标签的使能状态
    int iItemCnt;                       //标签的个数
    int iFocus;                         //焦点的位置,传入-1 则不处理焦点
} POP_SELECTOR;

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
                                GUIWINDOW *pWnd);