/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiphoto.h
* 摘    要：  定义GUI的图形框类型及操作，为实现图像显示重新封装。
*
* 当前版本：  
* 作    者：  
* 完成日期：  
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_PHOTO_H
#define _GUI_PHOTO_H

#include "guipicture.h"
//根据指定的信息直接建立图形框对象
GUIPICTURE* CreatePhoto(const char* pszPictureObjectName);
void CreateLoopPhoto(GUIPICTURE** ppPicObj,const char* pszPictureObjectName, 
                    int breakPoint, int startPoint, int count);
void CreateLoopPhoto1(GUIPICTURE * * ppPicObj, const char * pszPictureObjectName, int breakPoint, int startPoint, int count);
//删除图形框对象
int DestroyPhoto(GUIPICTURE **ppPicObj);
#endif
