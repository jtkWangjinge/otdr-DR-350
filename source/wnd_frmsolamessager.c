/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsloamessager.c
* 摘    要：  声明主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.1.0
* 作    者：  wjg
* 完成日期：  2014-12-04
*
* 取代版本：  v1.0.2
* 原 作 者：  wbq
* 完成日期：  2017-1-04
*******************************************************************************/

#include "wnd_frmsolamessager.h"
#include "guibase.h"
#include "wnd_global.h"
#include "app_unitconverter.h"


#define FVALUE1310NM pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1310]
#define FVALUE1550NM pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1550]
#define FVALUE1625NM pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1625]

SOLA_IDENTIFY_TABLE* CreateSolaIdentifyTable(
    int x, int y, int w, int h,SOLA_MESSAGER* pSolaMessager,
    GUIWINDOW* parent
    )
{
    if(!parent)
    return NULL;


    SOLA_IDENTIFY_TABLE* table = (SOLA_IDENTIFY_TABLE*)malloc(sizeof(SOLA_IDENTIFY_TABLE));
    if(NULL == table)
        return NULL;
    table->visible.Area.Start.x = x;
    table->visible.Area.Start.y = y;
    table->visible.Area.End.x = x + w - 1;
    table->visible.Area.End.y = y + h - 1;
    table->visible.iEnable = 1;
    table->visible.iLayer = 0;
    table->visible.iFocus = 0;
    table->visible.iCursor = 0;
    table->parent = parent;
    //横向表头
    table->identifyTitleText= TransString("SOLA_IDENTITY");
    table->identifyTitleLabel = CreateLabel(x, y-4, 190, 24, table->identifyTitleText);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->identifyTitleLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->identifyTitleLabel);

    table->auto_incrementTitleText= TransString("SOLA_AUTOINCREMENT");
    table->auto_incrementTitleLabel = CreateLabel(x+191, y-4, 115, 24, table->auto_incrementTitleText);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->auto_incrementTitleLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->auto_incrementTitleLabel);

    table->startTitleText= TransString("SOLA_START");
    table->startTitleLabel = CreateLabel(x+191+113 , y-4, 103, 24, table->startTitleText);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->startTitleLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->startTitleLabel);

    table->stopTitleText= TransString("SOLA_STOP");
    table->stopTitleLabel = CreateLabel(x+191+113+103, y-4, 103, 24, table->stopTitleText);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->stopTitleLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->stopTitleLabel);

    table->setpTitleText=  TransString("SOLA_STEP");
    table->setpTitleLabel = CreateLabel(x+191+110+103+103, y-4, 103, 24, table->setpTitleText);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->setpTitleLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->setpTitleLabel);

    //纵向表头
    table->CableIDText= TransString("SOLA_MANAGE_CABLEID");
    table->CableIDLabel = CreateLabel(x, y+32, 191, 24, table->CableIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->CableIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->CableIDLabel);

    table->FiberIDText= TransString("SOLA_MANAGE_FIBERID");
    table->FiberIDLabel = CreateLabel(x, y+32+43, 191, 24, table->FiberIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->FiberIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->FiberIDLabel);

    table->LocationAText= TransString("SOLA_MANAGE_LOCATIONA");
    table->LocationALabel = CreateLabel(x, y+32+43*2, 191, 24, table->LocationAText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->LocationALabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->LocationALabel);
    
    table->LocationBText= TransString("SOLA_MANAGE_LOCATIONB");
    table->LocationBLabel = CreateLabel(x, y+32+43*3, 191, 24, table->LocationBText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->LocationBLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->LocationBLabel);

    table->NULLText= TransString("SOLA_NONE");
    table->NULLLabel = CreateLabel(x, y+32+43*4, 191, 24, table->NULLText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->NULLLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->NULLLabel);

    //CableID属性
    char string1[10] = {0};
    char string2[10] = {0};
    char string3[10] = {0};
    if(pSolaMessager->identify.cableID.auto_increment){
        table->auto_incrementCableIDPicture = CreatePicture(x+191+40,y+27,30,30,BmpFileDirectory"btn_otdr_exit_press.bmp");
        sprintf(string1,"%d",pSolaMessager->identify.cableID.start);
        table->startCableIDText = TransString(string1);
        sprintf(string2,"%d",pSolaMessager->identify.cableID.stop);
        table->stopCableIDText = TransString(string2);
        sprintf(string3,"%d",pSolaMessager->identify.cableID.setp);
        table->setpCableIDText = TransString(string3);
    }
    else{
        table->auto_incrementCableIDPicture = CreatePicture(x+191+40,y+27,30,30,BmpFileDirectory"btn_key_press.bmp");
        table->startCableIDText = TransString("--");
        table->stopCableIDText = TransString("--");
        table->setpCableIDText = TransString("--");
    }
    table->startCableIDLabel = CreateLabel(x+191+110,y+32,100,24,table->startCableIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->startCableIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->startCableIDLabel);

    table->stopCableIDLabel = CreateLabel(x+191+110+103,y+32,100,24,table->stopCableIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->stopCableIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->stopCableIDLabel);

    table->setpCableIDLabel = CreateLabel(x+191+110+103*2,y+32,100,24,table->setpCableIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->setpCableIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->setpCableIDLabel);

    //FiberID属性
    char string4[10] = {0};
    char string5[10] = {0};
    char string6[10] = {0};
    if(pSolaMessager->identify.fiberID.auto_increment){
       table->auto_incrementFiberIDPicture = CreatePicture(x+191+40,y+27+43,30,30,BmpFileDirectory"btn_otdr_exit_press.bmp");
       sprintf(string4,"%d",pSolaMessager->identify.fiberID.start);
       table->startFiberIDText = TransString(string4);
       sprintf(string5,"%d",pSolaMessager->identify.fiberID.stop);
       table->stopFiberIDText = TransString(string5);
       sprintf(string6,"%d",pSolaMessager->identify.fiberID.setp);
       table->setpFiberIDText = TransString(string6);
    }
    else{
       table->auto_incrementFiberIDPicture = CreatePicture(x+191+40,y+27+43,30,30,BmpFileDirectory"btn_key_press.bmp");
       table->startFiberIDText = TransString("--");
       table->stopFiberIDText = TransString("--");
       table->setpFiberIDText = TransString("--");
        }
    table->startFiberIDLabel = CreateLabel(x+191+110,y+32+43,100,24,table->startFiberIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->startFiberIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->startFiberIDLabel);

    table->stopFiberIDLabel = CreateLabel(x+191+110+103,y+32+43,100,24,table->stopFiberIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->stopFiberIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->stopFiberIDLabel);

    table->setpFiberIDLabel = CreateLabel(x+191+110+103*2,y+32+43,100,24,table->setpFiberIDText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->setpFiberIDLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->setpFiberIDLabel);

    //LocationA属性
    char string7[10] = {0};
    char string8[10] = {0};
    char string9[10] = {0};
    if(pSolaMessager->identify.locationA.auto_increment){
        table->auto_incrementLocationAPicture = CreatePicture(x+191+40,y+27+43*2,30,30,BmpFileDirectory"btn_otdr_exit_press.bmp");
        sprintf(string7,"%d",pSolaMessager->identify.locationA.start);
        table->startLocationAText = TransString(string7);
        sprintf(string8,"%d",pSolaMessager->identify.locationA.stop);
        table->stopLocationAText = TransString(string8);
        sprintf(string9,"%d",pSolaMessager->identify.locationA.setp);
        table->setpLocationAText = TransString(string9);
    }
    else{
        table->auto_incrementLocationAPicture = CreatePicture(x+191+40,y+27+43*2,30,30,BmpFileDirectory"btn_key_press.bmp");
        table->startLocationAText = TransString("--");
        table->stopLocationAText = TransString("--");
        table->setpLocationAText = TransString("--");
    }
    
    table->startLocationALabel = CreateLabel(x+191+110,y+32+43*2,100,24,table->startLocationAText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->startLocationALabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->startLocationALabel);

    table->stopLocationALabel = CreateLabel(x+191+110+103,y+32+43*2,100,24,table->stopLocationAText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->stopLocationALabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->stopLocationALabel);

    table->setpLocationALabel = CreateLabel(x+191+110+103*2,y+32+43*2,100,24,table->setpLocationAText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->setpLocationALabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->setpLocationALabel);
    
    //LocationB属性
    char string10[10] = {0};
    char string11[10] = {0};
    char string12[10] = {0};
    if(pSolaMessager->identify.locationB.auto_increment){
        table->auto_incrementLocationBPicture = CreatePicture(x+191+40,y+27+43*3,30,30,BmpFileDirectory"btn_otdr_exit_press.bmp");
        sprintf(string10,"%d",pSolaMessager->identify.locationB.start);
        table->startLocationBText = TransString(string10);
        sprintf(string11,"%d",pSolaMessager->identify.locationB.stop);
        table->stopLocationBText = TransString(string11);
        sprintf(string12,"%d",pSolaMessager->identify.locationB.setp);
        table->setpLocationBText = TransString(string12);
    
    }
    else{
        table->auto_incrementLocationBPicture = CreatePicture(x+191+40,y+27+43*3,30,30,BmpFileDirectory"btn_key_press.bmp");
        table->startLocationBText = TransString("--");
        table->stopLocationBText = TransString("--");
        table->setpLocationBText = TransString("--");
    }
    
    table->startLocationBLabel = CreateLabel(x+191+110,y+32+43*3,100,24,table->startLocationBText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->startLocationBLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->startLocationBLabel);

    table->stopLocationBLabel = CreateLabel(x+191+110+103,y+32+43*3,100,24,table->stopLocationBText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->stopLocationBLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->stopLocationBLabel);

    table->setpLocationBLabel = CreateLabel(x+191+110+103*2,y+32+43*3,100,24,table->setpLocationBText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->setpLocationBLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->setpLocationBLabel);
    
    //NULL属性
    table->auto_incrementNULLPicture = CreatePicture(x+191+40,y+27+43*4,30,30,BmpFileDirectory"btn_key_press.bmp");

    table->startNULLText = TransString("--");
    table->startNULLLabel = CreateLabel(x+191+110,y+32+43*4,100,24,table->startNULLText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->startNULLLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->startNULLLabel);

    table->stopNULLText = TransString("--");
    table->stopNULLLabel = CreateLabel(x+191+110+103,y+32+43*4,100,24,table->stopNULLText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->stopNULLLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->stopNULLLabel);

    table->setpNULLText = TransString("--");
    table->setpNULLLabel = CreateLabel(x+191+110+103*2,y+32+43*4,100,24,table->setpNULLText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->setpNULLLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, table->setpNULLLabel);

    return table;
}


void DisplaySolaIdentifyTable(SOLA_IDENTIFY_TABLE* table)
{
/*
    //表格外框
    DrawRectangle(0,40,680,275);
    //表格横线
    int i;
    for(i=76;i<=236;i+=40){
        DrawLine(0,i,680,i);
    }
    //表格纵线
    DrawLine(302,40,302,275);
    DrawLine(453,40,453,275);
    DrawLine(529,40,529,275);
    DrawLine(604,40,604,275);
*/
    DisplayLabel(table->identifyTitleLabel);
    DisplayLabel(table->auto_incrementTitleLabel);
    DisplayLabel(table->startTitleLabel);
    DisplayLabel(table->stopTitleLabel);
    DisplayLabel(table->setpTitleLabel);

    DisplayLabel(table->CableIDLabel);
    DisplayLabel(table->FiberIDLabel);
    DisplayLabel(table->LocationALabel);
    DisplayLabel(table->LocationBLabel);
    //DisplayLabel(table->NULLLabel);

    DisplayPicture(table->auto_incrementCableIDPicture);
    DisplayLabel(table->startCableIDLabel);
    DisplayLabel(table->stopCableIDLabel);
    DisplayLabel(table->setpCableIDLabel);
    
    DisplayPicture(table->auto_incrementFiberIDPicture);
    DisplayLabel(table->startFiberIDLabel);
    DisplayLabel(table->stopFiberIDLabel);
    DisplayLabel(table->setpFiberIDLabel);
    
    DisplayPicture(table->auto_incrementLocationAPicture);
    DisplayLabel(table->startLocationALabel);
    DisplayLabel(table->stopLocationALabel);
    DisplayLabel(table->setpLocationALabel);
    
    DisplayPicture(table->auto_incrementLocationBPicture);
    DisplayLabel(table->startLocationBLabel);
    DisplayLabel(table->stopLocationBLabel);
    DisplayLabel(table->setpLocationBLabel);
    
    //DisplayPicture(table->auto_incrementNULLPicture);
    //DisplayLabel(table->startNULLLabel);
    //DisplayLabel(table->stopNULLLabel);
    //DisplayLabel(table->setpNULLLabel);
    
}
void DestroySolaIdentifyTable(SOLA_IDENTIFY_TABLE* table)
{
    DestroyLabel(&(table->identifyTitleLabel));
    DestroyLabel(&(table->auto_incrementTitleLabel));
    DestroyLabel(&(table->startTitleLabel));
    DestroyLabel(&(table->stopTitleLabel));
    DestroyLabel(&(table->setpTitleLabel));
    DestroyLabel(&(table->CableIDLabel));
    DestroyLabel(&(table->FiberIDLabel));
    DestroyLabel(&(table->LocationALabel));
    DestroyLabel(&(table->LocationBLabel));
    DestroyLabel(&(table->NULLLabel));

    GuiMemFree(table->identifyTitleText);
    GuiMemFree(table->auto_incrementTitleText);
    GuiMemFree(table->startTitleText);
    GuiMemFree(table->stopTitleText);
    GuiMemFree(table->setpTitleText);
    GuiMemFree(table->CableIDText);
    GuiMemFree(table->FiberIDText);
    GuiMemFree(table->LocationAText);
    GuiMemFree(table->LocationBText);
    GuiMemFree(table->NULLText);

    DestroyPicture(&(table->auto_incrementCableIDPicture));
    DestroyLabel(&(table->startCableIDLabel));
    GuiMemFree(table->startCableIDText);
    DestroyLabel(&(table->stopCableIDLabel));
    GuiMemFree(table->stopCableIDText);
    DestroyLabel(&(table->setpCableIDLabel));
    GuiMemFree(table->setpCableIDText);
    
    
    DestroyPicture(&(table->auto_incrementFiberIDPicture));
    DestroyLabel(&(table->startFiberIDLabel));
    GuiMemFree(table->startFiberIDText);
    DestroyLabel(&(table->stopFiberIDLabel));
    GuiMemFree(table->stopFiberIDText);
    DestroyLabel(&(table->setpFiberIDLabel));
    GuiMemFree(table->setpFiberIDText);
    
    DestroyPicture(&(table->auto_incrementLocationAPicture));
    DestroyLabel(&(table->startLocationALabel));
    GuiMemFree(table->startLocationAText);
    DestroyLabel(&(table->stopLocationALabel));
    GuiMemFree(table->stopLocationAText);
    DestroyLabel(&(table->setpLocationALabel));
    GuiMemFree(table->setpLocationAText);
    
    DestroyPicture(&(table->auto_incrementLocationBPicture));
    DestroyLabel(&(table->startLocationBLabel));
    GuiMemFree(table->startLocationBText);
    DestroyLabel(&(table->stopLocationBLabel));
    GuiMemFree(table->stopLocationBText);
    DestroyLabel(&(table->setpLocationBLabel));
    GuiMemFree(table->setpLocationBText);
    
    DestroyPicture(&(table->auto_incrementNULLPicture));
    DestroyLabel(&(table->startNULLLabel));
    GuiMemFree(table->startNULLText);
    DestroyLabel(&(table->stopNULLLabel));
    GuiMemFree(table->stopNULLText);
    DestroyLabel(&(table->setpNULLLabel));
    GuiMemFree(table->setpNULLText);

    GuiMemFree(table);
    
}


SOLA_ITEMPASS_TABLE* CreateSolaItemPassTable(
    int x, int y, int w, int h,SOLA_MESSAGER* pSolaMessager,
    GUIWINDOW* parent
    )
{
      if(!parent)
      return NULL;

      SOLA_ITEMPASS_TABLE* table = (SOLA_ITEMPASS_TABLE*)malloc(sizeof(SOLA_ITEMPASS_TABLE));
      if(NULL == table)
          return NULL;
      table->visible.Area.Start.x = x;
      table->visible.Area.Start.y = y;
      table->visible.Area.End.x = x + w - 1;
      table->visible.Area.End.y = y + h - 1;
      table->visible.iEnable = 1;
      table->visible.iLayer = 0;
      table->visible.iFocus = 0;
      table->visible.iCursor = 0;
      table->parent = parent;

      //第一列
      int i;
      for(i = 0;i<12;i++)
      {
          table->textOfTable1[i] = TransString("Loss");
          table->labelOfTable1[i] = CreateLabel(x, y+33*i, 250, 24, table->textOfTable1[i]);
          if(i == 0){
              SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->labelOfTable1[i]);
          }
          else{
              SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable1[i]);
          }
          SetLabelAlign(GUILABEL_ALIGN_LEFT, table->labelOfTable1[i]);
      }
      //第二列
      table->textOfTable2[0] = TransString("1310nm");
      table->labelOfTable2[0] = CreateLabel(x+251, y, w*2/5, h, table->textOfTable2[0]);
      
      //char string[11] = {0};       
      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1310]);
      //table->textOfTable2[1] = TransString(string);
      table->textOfTable2[1] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1310]);
      table->labelOfTable2[1] = CreateLabel(x+251, y+33*1, w*2/5, h, table->textOfTable2[1]);
      
      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1310]);
      //table->textOfTable2[2] = TransString(string);
      table->textOfTable2[2] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1310]);
      table->labelOfTable2[2] = CreateLabel(x+251, y+33*2, w*2/5, h, table->textOfTable2[2]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1310]);
      //table->textOfTable2[3] = TransString(string);
      table->textOfTable2[3] = Float2GuiString(REFLECTANCE_PRECISION, 
                               pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1310]);
      table->labelOfTable2[3] = CreateLabel(x+251, y+33*3, w*2/5, h, table->textOfTable2[3]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[4] = TransString(string);
      table->textOfTable2[4] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[4] = CreateLabel(x+251, y+33*4, w*2/5, h, table->textOfTable2[4]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[5] = TransString(string);
      table->textOfTable2[5] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[5] = CreateLabel(x+251, y+33*5, w*2/5, h, table->textOfTable2[5]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[6] = TransString(string);
      table->textOfTable2[6] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[6] = CreateLabel(x+251, y+33*6, w*2/5, h, table->textOfTable2[6]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[7] = TransString(string);
      table->textOfTable2[7] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[7] = CreateLabel(x+251, y+33*7, w*2/5, h, table->textOfTable2[7]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[8] = TransString(string);
      table->textOfTable2[8] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[8] = CreateLabel(x+251, y+33*8, w*2/5, h, table->textOfTable2[8]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[9] = TransString(string);
      table->textOfTable2[9] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[9] = CreateLabel(x+251, y+33*9, w*2/5, h, table->textOfTable2[9]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[WAVELEN_1310]);
      //table->textOfTable2[10] = TransString(string);
      table->textOfTable2[10] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[WAVELEN_1310]);
      table->labelOfTable2[10] = CreateLabel(x+251, y+33*10, w*2/5, h, table->textOfTable2[10]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[WAVELEN_1310]);
      //table->textOfTable2[11] = TransString(string);
      table->textOfTable2[11] = Float2GuiString(REFLECTANCE_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[WAVELEN_1310]);
      table->labelOfTable2[11] = CreateLabel(x+251, y+33*11, w*2/5, h, table->textOfTable2[11]);
      
    //第三列
      table->textOfTable3[0] = TransString("1550nm");
      table->labelOfTable3[0] = CreateLabel(x+251+124, y, w*2/5, h, table->textOfTable3[0]);
      
      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1550]);
      //table->textOfTable3[1] = TransString(string);
      table->textOfTable3[1] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1550]);
      table->labelOfTable3[1] = CreateLabel(x+251+124, y+33*1, w*2/5, h, table->textOfTable3[1]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1550]);
      //table->textOfTable3[2] = TransString(string);
      table->textOfTable3[2] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1550]);
      table->labelOfTable3[2] = CreateLabel(x+251+124, y+33*2, w*2/5, h, table->textOfTable3[2]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1550]);
      //table->textOfTable3[3] = TransString(string);
      table->textOfTable3[3] = Float2GuiString(REFLECTANCE_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1550]);
      table->labelOfTable3[3] = CreateLabel(x+251+124, y+33*3, w*2/5, h, table->textOfTable3[3]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[4] = TransString(string);
      table->textOfTable3[4] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[4] = CreateLabel(x+251+124, y+33*4, w*2/5, h, table->textOfTable3[4]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[5] = TransString(string);
      table->textOfTable3[5] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[5] = CreateLabel(x+251+124, y+33*5, w*2/5, h, table->textOfTable3[5]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[6] = TransString(string);
      table->textOfTable3[6] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[6] = CreateLabel(x+251+124, y+33*6, w*2/5, h, table->textOfTable3[6]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[7] = TransString(string);
      table->textOfTable3[7] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[7] = CreateLabel(x+251+124, y+33*7, w*2/5, h, table->textOfTable3[7]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[8] = TransString(string);
      table->textOfTable3[8] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[8] = CreateLabel(x+251+124, y+33*8, w*2/5, h, table->textOfTable3[8]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[9] = TransString(string);
      table->textOfTable3[9] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[9] = CreateLabel(x+251+124, y+33*9, w*2/5, h, table->textOfTable3[9]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[WAVELEN_1550]);
      //table->textOfTable3[10] = TransString(string);
      table->textOfTable3[10] = Float2GuiString(LOSS_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[WAVELEN_1550]);
      table->labelOfTable3[10] = CreateLabel(x+251+124, y+33*10, w*2/5, h, table->textOfTable3[10]);

      //sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[WAVELEN_1550]);
      //table->textOfTable3[11] = TransString(string);
      table->textOfTable3[11] = Float2GuiString(REFLECTANCE_PRECISION, 
                                pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[WAVELEN_1550]);
      table->labelOfTable3[11] = CreateLabel(x+251+124, y+33*11, w*2/5, h, table->textOfTable3[11]);
      #if 0
      for(i = 0;i<12;i++)
      {
          if(i == 0){
                SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->labelOfTable2[i]);
                SetLabelFont(getGlobalFnt(EN_FONT_WHITE), table->labelOfTable3[i]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, table->labelOfTable2[i]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, table->labelOfTable3[i]);
          }
          else{
                if(pSolaMessager->itemPassThreshold.isUsed){
                      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable2[i]);
                      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable3[i]);
                      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable2[i]);
                      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable3[i]);
                }
                else{
                      SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), table->labelOfTable2[i]);
                      SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), table->labelOfTable3[i]);
                      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable2[i]);
                      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable3[i]);
                }
          }
      }
      #endif
      //第四列
      /*table->textOfTable4[0] = TransString("1625nm");
      table->labelOfTable4[0] = CreateLabel(x+272+136+136, y, w/2, h, table->textOfTable4[0]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[0]);
      SetLabelAlign(GUILABEL_ALIGN_LEFT, table->labelOfTable4[0]);
      */
      /*for(i=1;i<12;i++){
      sprintf(string,"%5.3f", *rename1625[i-1]);
      table->textOfTable4[i] = TransString(string);
      table->labelOfTable4[i] = CreateLabel(x+272+136+136, y+26*i, w/40*21, h, table->textOfTable4[i]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[i]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[i]);
      }*/
        /*
      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue1625nm);
      table->textOfTable4[1] = TransString(string);
      table->labelOfTable4[1] = CreateLabel(x+272+136+136, y+26*1, w/40*21, h, table->textOfTable4[1]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[1]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[1]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue1625nm);
      table->textOfTable4[2] = TransString(string);
      table->labelOfTable4[2] = CreateLabel(x+272+136+136, y+26*2, w/40*21, h, table->textOfTable4[2]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[2]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[2]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue1625nm);
      table->textOfTable4[3] = TransString(string);
      table->labelOfTable4[3] = CreateLabel(x+272+136+136, y+26*3, w/40*21, h, table->textOfTable4[3]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[3]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[3]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue1625nm);
      table->textOfTable4[4] = TransString(string);
      table->labelOfTable4[4] = CreateLabel(x+272+136+136, y+26*4, w/40*21, h, table->textOfTable4[4]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[4]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[4]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue1625nm);
      table->textOfTable4[5] = TransString(string);
      table->labelOfTable4[5] = CreateLabel(x+272+136+136, y+26*5, w/40*21, h, table->textOfTable4[5]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[5]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[5]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue1625nm);
      table->textOfTable4[6] = TransString(string);
      table->labelOfTable4[6] = CreateLabel(x+272+136+136, y+26*6, w/40*21, h, table->textOfTable4[6]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[6]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[6]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue1625nm);
      table->textOfTable4[7] = TransString(string);
      table->labelOfTable4[7] = CreateLabel(x+272+136+136, y+26*7, w/40*21, h, table->textOfTable4[7]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[7]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[7]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue1625nm);
      table->textOfTable4[8] = TransString(string);
      table->labelOfTable4[8] = CreateLabel(x+272+136+136, y+26*8, w/40*21, h, table->textOfTable4[8]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[8]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[8]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue1625nm);
      table->textOfTable4[9] = TransString(string);
      table->labelOfTable4[9] = CreateLabel(x+272+136+136, y+26*9, w/40*21, h, table->textOfTable4[9]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[9]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[9]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue1625nm);
      table->textOfTable4[10] = TransString(string);
      table->labelOfTable4[10] = CreateLabel(x+272+136+136, y+26*10, w/40*21, h, table->textOfTable4[10]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[10]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[10]);

      sprintf(string,"%5.3f", pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue1625nm);
      table->textOfTable4[11] = TransString(string);
      table->labelOfTable4[11] = CreateLabel(x+272+136+136, y+26*11, w/40*21, h, table->textOfTable4[11]);
      SetLabelFont(getGlobalFnt(EN_FONT_BLACK), table->labelOfTable4[11]);
      SetLabelAlign(GUILABEL_ALIGN_RIGHT, table->labelOfTable4[11]);
      */
      return table;
}


void DisplaySolaItemPassTable(SOLA_ITEMPASS_TABLE* table)
{
    int i;
    for(i = 0;i<12;i++){
        DisplayLabel(table->labelOfTable1[i]);
    }
    DisplayLabel(table->labelOfTable2[0]);
    for(i = 1;i<12;i++){
        DisplayLabel(table->labelOfTable2[i]);
    }
    DisplayLabel(table->labelOfTable3[0]);
    for(i = 1;i<12;i++){
        DisplayLabel(table->labelOfTable3[i]);
    }
    /*DisplayLabel(table->labelOfTable4[0]);
    for(i = 1;i<12;i++){
        DisplayLabel(table->labelOfTable4[i]);
    }*/
}

void DestroySolaItemPassTable(SOLA_ITEMPASS_TABLE* table)
{
    int i;
    for(i = 0;i<12;i++){
       DestroyLabel(&(table->labelOfTable1[i]));
       GuiMemFree(table->textOfTable1[i]);
    }
    
    GuiMemFree(table->labelOfTable2[0]);
    for(i = 1;i<12;i++){
       DestroyLabel(&(table->labelOfTable2[i]));
       GuiMemFree(table->textOfTable2[i]);
    }

    GuiMemFree(table->labelOfTable3[0]);
    for(i = 1;i<12;i++){
       DestroyLabel(&(table->labelOfTable3[i]));
       GuiMemFree(table->textOfTable3[i]);
    }
    
    /*GuiMemFree(table->labelOfTable4[0]);
    for(i = 1;i<12;i++){
       DestroyLabel(&(table->labelOfTable4[i]));
       GuiMemFree(table->textOfTable4[i]);
    }
    */
    GuiMemFree(table);
    table = NULL;
}
