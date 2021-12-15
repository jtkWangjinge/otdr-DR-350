/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfilebrowse.h
* 摘    要：  声明窗体wnd_frmfilebrowse(文件浏览)的窗体处理线程及相关操作函数
*			  该窗体提供了文件浏览功能。        
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-9-3
*
* 取代版本：
* 原 作 者：
* 完成日期：
*/
/*******************************************************************************/
#ifndef _WND_FRMFILEBROWSE1_H
#define _WND_FRMFILEBROWSE1_H

/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "app_frmfilebrowse.h"

#include "guiglobal.h"

#include "wnd_global.h"

typedef void (*FILEBROWSECALLLBACK)(void);
extern int isInSmallFileBrowse;									//是否在小的浏览器窗口

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
/***
  * 功能：
        窗体frmfilebrowse的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1、unsigned int uiPlaceX:		控件的X坐标    
        2、unsigned int uiPlaceY:		控件的Y坐标
		3、unsigned int uiMaxFileNum :	每页最大显示文件个数
        4、CALLLBACKWINDOW func :		重绘前一个窗体
        5、void *pWndObj		:		指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFileBrowseInit1(unsigned int uiPlaceX, unsigned int uiPlaceY, unsigned int uiMaxFileNum,
                       CALLLBACKWINDOW func, void *pWndObj);
//窗体frmfilebrowse的退出函数，释放所有资源
int FrmFileBrowseExit1(void *pWndObj);
//窗体frmfilebrowse的绘制函数，绘制整个窗体
int FrmFileBrowsePaint1(void *pWndObj);


/***
  * 功能：
		删除显示路径缓存中的/mnt
  * 参数：
		1、char *pBuff:		待处理的路径
  * 返回：
		无
  * 备注：	
***/
void DeletePathTitleBuff(char *pBuff);


/***
  * 功能：
        设置当前文件浏览器的操作类型
  * 参数：
        1、FILEOPERATION enFileOperation:	将要设置的文件操作类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetFileOperationType(unsigned int enFileOperation);

/***
  * 功能：
        设置当前文件浏览器的文件选择模式
  * 参数：
        1、FILE_SELECT_MODE enSelectMode:	将要设置的文件选择模式
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetFileSelectMode(FILE_SELECT_MODE enSelectMode);

/***
  * 功能：
        设置首次进入文件浏览器应该打开的目录
  * 参数：
        1、const char *pPath:	将要打开的目录
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetFirstOpenFolder(const char *pPath);


/***
  * 功能：
        设置多选状态发生变化时，用户控件状态更新的回调函数
  * 参数：
        1、FILEBROWSECALLLBACK pFuncCallBack:	更新回调函数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int SetWidgetUpdateCallBack(FILEBROWSECALLLBACK pFuncCallBack);


/***
  * 功能：
        获取当前选中的文件名
  * 参数：
        1、char *pFileName:	存贮当前选中的文件名
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		用户自己保证pFileName足够大
***/
int GetSelectFileName(char *pFileName);


/***
  * 功能：
        打开目录或者系统支持的文件
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		此函数不刷新帧缓冲
***/
int OpenFileAndFolder(const char *pFileName);

/***
  * 功能：
		文件浏览器窗体按键的down事件
  * 参数：
		1.int iValue:	按键的键值
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FileBrowseKeyDown(unsigned int uiValue);


/***
  * 功能：
		文件浏览器窗体按键的up事件
  * 参数：
		1.int iValue:	按键的键值
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FileBrowseKeyUp(unsigned int uiValue);

/***
  * 功能：
        更新全选显示
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		此函数不刷新帧缓冲
***/
int UpdateFullSelect(void);

/***
  * 功能：
        设置打开文件夹标志为0
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：

***/
int ResetOpenFolderFlag(void);

/***
  * 功能：
        设置当前的文件过滤类型
  * 参数：
        1.FILETYPE enFilter:	将要设置的文件过滤类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int __SetCurFileType(FILETYPE enFilter);

/***
  * 功能：
        复制选中的文件到剪切板上
  * 参数：
        无
  * 返回：
        无
  * 备注：

***/
void CopyFileToClipboard(void);
/***
  * 功能：
        复制剪切板上的内容到指定目录
  * 参数：
        1、const char* pDestDirectory:  指定目录
        2、int iPasteFlag:              是否直接粘贴 
  * 返回：
        成功返回零，失败返回非零值
  * 备注：

***/
int PasteFromClipboard(const char* pDestDirectory, int iPasteFlag);
/***
  * 功能：
        显示剪切板上的内容作测试使用
  * 参数：
        无
  * 返回:
  * 备注：

***/
void ShowClipboard(void);
/***
  * 功能：
        重新绘制右侧菜单栏
  * 参数：
        CALLLBACKWINDOW1 func
  * 返回：
        无
  * 备注：

***/
void RepaintWnd(CALLLBACKWINDOW1 func);
/***
  * 功能：
        设置文件类型以及全选按钮的显示
  * 参数：
        int flag(0:不显示，1:显示)
  * 返回：
        无
  * 备注：

***/
void SetDisplayFileTypeAndSelectAll(int flag);

//选择所有文件/文件夹,成功返回0，失败（顶层目录）返回-1
int SelectAllFile(void);

//根据索引打开文件或文件夹
void OpenAtIndex(unsigned int index);

//获取当前选中文件的索引
int GetCurrFileIndex(void);

//设置文件类型的使能
int SetFileTypeEnabled(int iEnabled);

//设置当前文件类型的文本
void SetCurrentFileTypeLabel(char* fileType);

//按键响应回调函数
void KeyBoardCallBack1(int iSelected);

//设置背景选中
void RefreshFileBrowseSelectedFile(void);

//设置文件类型
void SelectFileType(void);

#endif 
