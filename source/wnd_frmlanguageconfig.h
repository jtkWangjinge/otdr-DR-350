/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlanguageconfig.h
* 摘    要：  声明烧录工厂配置文件功能相关函数
*            
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-27
*
*******************************************************************************/

#ifndef _WND_FRMLANGUAGECONFIG_H
#define _WND_FRMLANGUAGECONFIG_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "app_factoryfirmware.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmfactoryfirmware的初始化函数，建立窗体控件、注册消息处理
int FrmLanguageConfigInit(void *pWndObj);
//窗体frmfactoryfirmware的退出函数，释放所有资源
int FrmLanguageConfigExit(void *pWndObj);
//窗体frmfactoryfirmware的绘制函数，绘制整个窗体
int FrmLanguageConfigPaint(void *pWndObj);
//窗体frmfactoryfirmware的循环函数，进行窗体循环
int FrmLanguageConfigLoop(void *pWndObj);
//窗体frmfactoryfirmware的挂起函数，进行窗体挂起前预处理
int FrmLanguageConfigPause(void *pWndObj);
//窗体frmfactoryfirmware的恢复函数，进行窗体恢复前预处理
int FrmLanguageConfigResume(void *pWndObj);


int LanguageConfigViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);


//以下是语言列表选项的操作
int isBitSetted(unsigned long long lw, int bit);
void setBit(unsigned long long* lw, int bit);
void clearBit(unsigned long long* lw, int bit);
//获取选中的语言总数
int GetLanguageCount(void);
//获取选中的语言列表
int* GetSelectedLanguageList(void);
//获取选中语言列表的index
int* GetSelectedLanguageListIndex(void);
//获取选中项的下标值
int GetSelectedIndex(int languageIndex);
//语言列表按键回调函数
void LanguageKeyCallBack(int iOption);
#endif


