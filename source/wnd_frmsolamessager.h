/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsloamessager.h
* 摘    要：  声明主窗体frmsolatable的窗体处理线程及相关操作函数
*
* 当前版本：  v1.1.0
* 作    者：  wjg
* 完成日期：  2014-12-04
*
* 取代版本：  v1.0.2
* 原 作 者：  wbq
* 完成日期：  2016-1-04
*******************************************************************************/
#ifndef _WND_FRMSOLAMESSAGER_H
#define _WND_FRMSOLAMESSAGER_H

#include "guiglobal.h"
#include "app_frmsolamessager.h"


//标识界面表格结构体
typedef struct _sola_identify_table
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    GUIPICTURE* background;     //背景图片资源

    GUILABEL* identifyTitleLabel;   //标识标签
    GUICHAR* identifyTitleText;     //标识文本

    GUILABEL* auto_incrementTitleLabel;   //自动递增标签
    GUICHAR*  auto_incrementTitleText;    //自动递增文本
    GUILABEL* startTitleLabel;  //开始标签
    GUICHAR*  startTitleText;   //开始文本
    GUILABEL* stopTitleLabel;   //终止标签
    GUICHAR*  stopTitleText;    //终止文本
    GUILABEL* setpTitleLabel;   //分步标签
    GUICHAR*  setpTitleText;    //分步文本

    GUILABEL* CableIDLabel;    //CableID标签
    GUICHAR*  CableIDText;     //CableID文本
    GUILABEL* FiberIDLabel;    //FiberID标签
    GUICHAR*  FiberIDText;     //FiberID文本
    GUILABEL* LocationALabel;  //LocationA标签
    GUICHAR*  LocationAText;   //LocationA文本
    GUILABEL* LocationBLabel;  //LocationB标签
    GUICHAR*  LocationBText;   //LocationB文本
    GUILABEL* NULLLabel;       //NULL标签
    GUICHAR*  NULLText;        //NULL文本

    GUIPICTURE* auto_incrementCableIDPicture;//CableID对应属性   
    GUILABEL* startCableIDLabel;  
    GUICHAR*  startCableIDText;   
    GUILABEL* stopCableIDLabel;   
    GUICHAR*  stopCableIDText;    
    GUILABEL* setpCableIDLabel;   
    GUICHAR*  setpCableIDText;        

    GUIPICTURE* auto_incrementFiberIDPicture;//FiberID对应属性   
    GUILABEL* startFiberIDLabel;  
    GUICHAR*  startFiberIDText;   
    GUILABEL* stopFiberIDLabel;   
    GUICHAR*  stopFiberIDText;    
    GUILABEL* setpFiberIDLabel;   
    GUICHAR*  setpFiberIDText;       

    GUIPICTURE* auto_incrementLocationAPicture;//LocationA对应属性   
    GUILABEL* startLocationALabel;  
    GUICHAR*  startLocationAText;   
    GUILABEL* stopLocationALabel;   
    GUICHAR*  stopLocationAText;    
    GUILABEL* setpLocationALabel;   
    GUICHAR*  setpLocationAText;         

    GUIPICTURE* auto_incrementLocationBPicture;//LocationB对应属性      
    GUILABEL* startLocationBLabel;  
    GUICHAR*  startLocationBText;   
    GUILABEL* stopLocationBLabel;   
    GUICHAR*  stopLocationBText;    
    GUILABEL* setpLocationBLabel;   
    GUICHAR*  setpLocationBText;         

    GUIPICTURE* auto_incrementNULLPicture;//NULL对应属性      
    GUILABEL* startNULLLabel;  
    GUICHAR*  startNULLText;   
    GUILABEL* stopNULLLabel;   
    GUICHAR*  stopNULLText;    
    GUILABEL* setpNULLLabel;   
    GUICHAR*  setpNULLText;         


    GUIWINDOW* parent;          //所属的窗体


} SOLA_IDENTIFY_TABLE;



typedef struct _sola_itemPass_table
{
    GUIVISIBLE visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    GUIPICTURE* background;     //背景图片资源
    
    GUILABEL* labelOfTable1[12];
    GUICHAR*  textOfTable1[12];

    GUILABEL* labelOfTable2[12];
    GUICHAR*  textOfTable2[12];

    GUILABEL* labelOfTable3[12];
    GUICHAR*  textOfTable3[12];
    
    GUILABEL* labelOfTable4[12];
    GUICHAR*  textOfTable4[12];

    GUIWINDOW* parent;          //所属的窗体
} SOLA_ITEMPASS_TABLE;

//Identify表格
SOLA_IDENTIFY_TABLE* CreateSolaIdentifyTable(
    int x, int y, int w, int h,SOLA_MESSAGER* pSolaMessager,
    GUIWINDOW* parent
    );
void DisplaySolaIdentifyTable(SOLA_IDENTIFY_TABLE* table);
void DestroySolaIdentifyTable(SOLA_IDENTIFY_TABLE* table);


//ItemPass表格
SOLA_ITEMPASS_TABLE* CreateSolaItemPassTable(
    int x, int y, int w, int h,SOLA_MESSAGER* pSolaMessager,
    GUIWINDOW* parent
    );
void DisplaySolaItemPassTable(SOLA_ITEMPASS_TABLE* table);
void DestroySolaItemPassTable(SOLA_ITEMPASS_TABLE* table);

#endif  //_WND_FRMSOLATABLE_H
