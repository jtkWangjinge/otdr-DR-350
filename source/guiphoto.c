/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiphoto.c
* 摘    要：  
*
* 当前版本：  
* 作    者：  
* 完成日期：  
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/
#include "run/iniparser/include/Skin.h"
#include "guiphoto.h"
#include "guiconfig.h"

#include "app_global.h"

extern HSKIN hSkin;
/***
  * 功能：
        根据指定的图片名直接建立图形框对象
  * 参数：
        1.const char* pszPictureObjectName:加载图片的名称
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIPICTURE *CreatePhoto(const char* pszPictureObjectName)
{
    HPICTURE pPicInfo = SkinGetPicture(hSkin, pszPictureObjectName);
	skinset* skin = (skinset*)pPicInfo;
    //错误标志、返回值定义
    int iErr = 0;
    GUIPICTURE *pPicObj = NULL;
    char buff[128];
   	strcpy(buff, BmpFileDirectory);

    if (!iErr)
    {   
        if(NULL != skin->path)
        {
            strcat(buff, skin->path);
        }
        else
        {
            LOG(LOG_ERROR, "The PictureObjectName and picturename in ini is not match\n");
        }
        pPicObj = CreatePicture(skin->x, skin->y, skin->w, skin->h, buff);
        if(NULL == pPicObj)
        {
            iErr = -1;
        }
    }
    //SkinDestroy(hSkin, pPicInfo, NULL, NULL);
    //错误处理
    switch (iErr)
    {
    case -1:
        //SkinDestroy(hSkin, pPicInfo, NULL, NULL);
        //no break
    default:
        break;
    }

    return pPicObj;
}
/***
  * 功能：
        根据指定的信息直接建立图形框对象
  * 参数：
        1.GUIPICTURE** ppPicObj:            所要加载图片对象名
        2.const char* pszPictureObjectName: 加载图片的名称
        3.int breakPoint:                   生成图片对象分列的转折点
        4.int startPoint:                   循环的起始点
        5.int count:                        循环的次数
        
  * 返回：
        无返回值
  * 备注：
***/
void CreateLoopPhoto(GUIPICTURE** ppPicObj,const char* pszPictureObjectName, 
                    int breakPoint, int startPoint, int count)
{
    //hSkin = SkinCreate(SKININIPATH);
    HPICTURE pPicInfo = SkinGetLoopPicture(hSkin, pszPictureObjectName);
	skinset* skin = (skinset*)pPicInfo;
    //错误标志、返回值定义
    int iErr = 0;
    char buff[128];
   	strcpy(buff, BmpFileDirectory);
    strcat(buff, skin->path);
    int x_offset = skin->x_offset;
    int y_offset = skin->y_offset;
    int i = 0;
    if (!iErr)
    {   
        if(0 == breakPoint)
        {
            for(i = startPoint; i < count; ++i)
            {
                ppPicObj[i] = CreatePicture(skin->x+i*x_offset, skin->y+ i*y_offset, skin->w, skin->h, buff);
            } 
        }
        else
        {
            for(i = startPoint; i < count; ++i)
            {
                if(i < breakPoint)
                {
                    ppPicObj[i] = CreatePicture(skin->x, skin->y+ i*y_offset, skin->w, skin->h, buff);
                }
                else
                {
                    ppPicObj[i] = CreatePicture(skin->x + x_offset, skin->y+(i-breakPoint)*y_offset, skin->w, skin->h, buff);
                }
            }
        }
        if(NULL == ppPicObj)
        {
            iErr = -1;
        }
    }
    //SkinDestroy(hSkin, pPicInfo, NULL, NULL);
    //错误处理
    switch (iErr)
    {
    case -1:
        //SkinDestroy(hSkin, pPicInfo, NULL, NULL);
        //no break
    default:
        break;
    }
}
void CreateLoopPhoto1(GUIPICTURE * * ppPicObj, const char * pszPictureObjectName, int breakPoint, int startPoint, int count)
{
    //HSKIN hSkin = SkinCreate(SKININIPATH);
    HPICTURE pPicInfo = SkinGetLoopPicture(hSkin, pszPictureObjectName);
	skinset* skin = (skinset*)pPicInfo;
     //错误标志、返回值定义
    int iErr = 0;
    char buff[128];
    int x_offset = skin->x_offset;
    int y_offset = skin->y_offset;
    int i;
    if (!iErr)
    {
        if(breakPoint == 0)
        {
            for(i = startPoint; i < count; ++i)
            {
                sprintf(buff, "%s%s%d%s", BmpFileDirectory, pszPictureObjectName, i, ".bmp");
                ppPicObj[i] = CreatePicture(skin->x + i*x_offset, skin->y+ i*y_offset, skin->w, skin->h, buff);
            } 
        }
        else
        {
            for(i = startPoint; i < count; ++i)
            {
                if(i < breakPoint)
                {
                    ppPicObj[i] = CreatePicture(skin->x, skin->y+ i*y_offset, skin->w, skin->h, buff);
                }
                else
                {
                    ppPicObj[i] = CreatePicture(skin->x + x_offset, skin->y+(i-breakPoint)*y_offset, skin->w, skin->h, buff);
                }
            }

        }
        if(NULL == ppPicObj)
        {
            iErr = -1;
        }
    }
    //SkinDestroy(hSkin, pPicInfo, NULL, NULL);
    //错误处理
    switch (iErr)
    {
    case -1:
        //SkinDestroy(hSkin, pPicInfo, NULL, NULL);
        //no break
    default:
        break;
    }
}
//删除图形框对象
int DestroyPhoto(GUIPICTURE **ppPicObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    iErr = DestroyPicture(ppPicObj);
    return iErr;
}
