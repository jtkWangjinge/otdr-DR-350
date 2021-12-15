/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guitext.c
* 摘    要：  实现GUI的文本类型及操作，及基本的文本输出操作函数，为GUI图形控件的
*             实现提供基础
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guitext.h"


/************************************
* 为实现GUITEXT而需要引用的其他头文件
************************************/
#include "guiimage.h"
#include "wnd_global.h"

//阿拉伯语独立字符
static unsigned short ArabicIsolated[] = {0xFE80, 0xFE81, 0xFE83, 0xFE85, 0xFE87, 0xFE89, 0xFE8D, 0xFE8F, 0xFE93, 0xFE95, 0xFE99, 0xFE9D, 0xFEA1, 0xFEA5,
            	  		 0xFEA9, 0xFEAB, 0xFEAD, 0xFEAF, 0xFEB1, 0xFEB5, 0xFEB9, 0xFEBD, 0xFEC1, 0xFEC5, 0xFEC9, 0xFECD, 0x0640, 0xFED1,
            	  		 0xFED5, 0xFED9, 0xFEDD, 0xFEE1, 0xFEE5, 0xFEE9, 0xFEED, 0xFEEF, 0xFEF1, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F,
            	  		 0x0650, 0x0651, 0x0652, 0x0660, 0x0661, 0x0662, 0x0663, 0x0665, 0x0665, 0x0666, 0x0667, 0x0668, 0x0669, 0x065E,
            	  		 0x065F, 0xFB50, 0xFB66, 0xFB5E, 0xFB52, 0xFB56, 0xFB62, 0xFB5A, 0xFB76, 0xFB72, 0xFB7A, 0xFB7E, 0xFB88, 0xFB84,
            	  		 0xFB82, 0xFB86, 0xFB8C, 0xFB8A, 0xFB6A, 0xFB6E, 0xFB8E, 0xFBD3, 0xFB92, 0xFB9A, 0xFB96, 0xFB9E, 0xFBA0, 0xFBAA,
            	  		 0xFBA4, 0xFBA6, 0xFBE0, 0xFBD9, 0xFBD7, 0xFBDB, 0xFBE2, 0xFBDE, 0xFBFC, 0xFBE4, 0xFBAE, 0xFBB0, 0xFEF5, 0xFEF7,
            	  		 0xFEF9, 0xFEFB};
//阿拉伯语词头
/*static unsigned short ArabicInitial[] = {0xFE8B, 0xFE91, 0xFE97, 0xFE9B, 0xFE9F, 0xFEA3, 0xFEA7, 0xFEB3, 0xFEB7, 0xFEBB, 0xFEBF, 0xFEC3,
           			    0xFEC7, 0xFECB, 0xFECF, 0xFED3, 0xFED7, 0xFEDB, 0xFEDF, 0xFEE3, 0xFEE7, 0xFEEB, 0xFBE8, 0xFEF3,
           			    0xFB68, 0xFB60, 0xFB54, 0xFB58, 0xFB64, 0xFB5C, 0xFB78, 0xFB74, 0xFB7C, 0xFB80, 0x069F, 0xFB6C,
           			    0xFB70, 0xFB90, 0xFBD5, 0xFB94, 0xFB9C, 0xFB98, 0xFBA2, 0xFBAC, 0xFBA8, 0xFBFE, 0xFBE6};*/
//阿拉伯语词中
static unsigned short ArabicMedial[] = {0xFE8C, 0xFE92, 0xFE98, 0xFE9C, 0xFEA0, 0xFEA4, 0xFEA8, 0xFEB4, 0xFEB8, 0xFEBC, 0xFEC0, 0xFEC4, 0xFEC8,
          			   0xFECC, 0xFED0, 0xFED4, 0xFED8, 0xFEDC, 0xFEE0, 0xFEE4, 0xFEE8, 0xFEEC, 0xFBE9, 0xFEF4, 0xFB69, 0xFB61,
          			   0xFB55, 0xFB59, 0xFB65, 0xFB5D, 0xFB79, 0xFB75, 0xFB7D, 0xFB81, 0xFB6D, 0xFB71, 0xFED4, 0xFB6D, 0xFB91,
          			   0xFBD6, 0xFB95, 0xFB9D, 0xFB99, 0xFBA3, 0xFE9C, 0xFBAD, 0xFBA9, 0xFBFF, 0xFBE7};
//阿拉伯语词尾
static unsigned short ArabicFinal[] = {0xFE82, 0xFE84, 0xFE86, 0xFE88, 0xFE8A, 0xFE8E, 0xFE90, 0xFE94, 0xFE96, 0xFE9A, 0xFE9E, 0xFEA2, 0xFEA6,
         			  0xFEAA, 0xFEAC, 0xFEAE, 0xFEB0, 0xFEB2, 0xFEB6, 0xFEBA, 0xFEBE, 0xFEC2, 0xFEC6, 0xFECA, 0xFECE, 0xFED2,
         			  0xFED6, 0xFEDA, 0xFEDE, 0xFEE2, 0xFEE6, 0xFEEA, 0xFEEE, 0xFEF0, 0xFEF2, 0xFB51, 0xFB67, 0xFB5F, 0xFB53,
         			  0xFB57, 0xFB63, 0xFB5B, 0xFB77, 0xFB73, 0xFB7B, 0xFB7F, 0xFB89, 0xFB85, 0xFB83, 0xFB87, 0xFB8D, 0xFB8B,
         			  0xFB6B, 0xFB6F, 0xFB8F, 0xFBD4, 0xFB93, 0xFB9B, 0xFB97, 0xFB9F, 0xFBA1, 0xFBAB, 0xFBA5, 0xFBA7, 0xFBE1,
         			  0xFBDA, 0xFBD8, 0xFBDC, 0xFBE3, 0xFBDF, 0xFBFD, 0xFBE5, 0xFBAF, 0xFBB1, 0xFEF6, 0xFEF8, 0xFEFA, 0xFEFC};
//阿拉伯语音调
static unsigned short ArabicPhonogram[] = {0x064B, 0xFE70, 0x064C, 0xFE72, 0x064D, 0xFE74, 0x064E, 0xFE76, 0x064F, 0xFE78, 0x0650, 0xFE7A,
             					   0x0651, 0xFE7C, 0x0652, 0xFE7E, 0xFC5E, 0xFC5F, 0xFC60, 0xFC61, 0xFC62, 0xFC63};
//泰语音调
static unsigned short ThaiPhonogram[] = {
	0x0E31, 0x0E34, 0x0E35, 0x0E36, 0x0E37, 0x0E38, 0x0E39, 0x0E3A, 0x0E47, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 
	0x0E4C, 0x0E4D, 0x0E4E, 0x0E5C, 0x0E5D, 0x0E5E, 0x0E5F, 0x0E60, 0x0E61, 0x0E62, 0x0E63, 0x0E64, 0x0E65, 0x0E66, 
	0x0E67, 0x0E68, 0x0E69, 0x0E6A, 0x0E6B, 0x0E6C, 0x0E6D, 0x0E6E, 0x0E6F, 0x0E70, 0x0E71, 0x0E72, 0x0E73, 0x0E74};

static int findValue(unsigned short *values, int len, unsigned short value);

static int findValue(unsigned short *values, int len, unsigned short value)
{
	int ret = 0;
	int i = 0;
	for (i = 0; i < len; ++i)
	{
		if (value == values[i])
		{
			ret = 1;
			break;
		}
	}
	return ret;
}

//获得阿拉伯语字符间隙
int getGap(unsigned short value)
{
	if(findValue(ArabicFinal, sizeof(ArabicFinal)/sizeof(unsigned short), value))
	{
		return 0;
	}

	if(findValue(ArabicMedial, sizeof(ArabicMedial)/sizeof(unsigned short), value))
	{
		return 0;
	}
	return 1;
}

// 是否清除左边空白
int isClearLeft(unsigned short value)
{
	if(findValue(ArabicFinal, sizeof(ArabicFinal)/sizeof(unsigned short), value))
	{
		return 0;
	}

	if(findValue(ArabicIsolated, sizeof(ArabicIsolated)/sizeof(unsigned short), value))
	{
		return 0;
	}
	return 1;
}

// 是否是阿拉伯语音标
int isArabicPhonogram(unsigned short value)
{	
	if(findValue(ArabicPhonogram, sizeof(ArabicPhonogram)/sizeof(unsigned short), value))
	{
		return 1;
	}
	return 0;
}

// 是否是泰语音标
int isThaiPhonogram(unsigned short value)
{	
	if(findValue(ThaiPhonogram, sizeof(ThaiPhonogram)/sizeof(unsigned short), value))
	{
		return 1;
	}
	return 0;
}

            
/***
  * 功能：
        根据指定的信息直接建立文本对象
  * 参数：
        1.unsigned short *pTextData:    用于建立文本对象的文本内容(宽字符)
        2.unsigned int uiPlaceX:        文本水平放置位置，以左上角为基点
        3.unsigned int uiPlaceY:        文本垂直放置位置，以左上角为基点
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUITEXT* CreateText(unsigned short *pTextData, 
                    unsigned int uiPlaceX, unsigned int uiPlaceY)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUITEXT *pTextObj = NULL;
    //临时变量定义
    unsigned int uiSize;

    if (!iErr)
    {
        //判断pTextData是否为有效指针
        if (NULL == pTextData)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pTextObj = (GUITEXT *) malloc(sizeof(GUITEXT));
        if (NULL == pTextObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //得到文本长度
        pTextObj->uiTextLength = 0;
        pTextObj->pTextData = pTextData;
        while (*(pTextObj->pTextData))              //文本内容，以'\0'结尾
        {
            pTextObj->uiTextLength++;
            pTextObj->pTextData++;
        }
        //为保存文本内容分配内存资源
        uiSize = 2 * (pTextObj->uiTextLength + 1);  //文本长度不含'\0'
        pTextObj->pTextData = (unsigned short *) malloc(uiSize);
        if (NULL == pTextObj->pTextData)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //文本对象成员赋值
        memcpy(pTextObj->pTextData, pTextData, uiSize);
        pTextObj->uiPlaceX = uiPlaceX;
        pTextObj->uiPlaceY = uiPlaceY;
        pTextObj->uiViewWidth = 0;      //默认按文本实际宽度显示
        pTextObj->uiViewHeight = 0;     //默认按文本实际高度显示
        pTextObj->ppTextMatrix = NULL;
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pTextObj);
        //no break
    case -2:
    case -1:
        pTextObj = NULL;
        //no break
    default:
        break;
    }

    return pTextObj;
}


/***
  * 功能：
        删除文本对象
  * 参数：
        1.GUITEXT **ppTextObj:  指针的指针，指向需要销毁的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyText(GUITEXT **ppTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppTextObj是否为有效指针
        if (NULL == ppTextObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppTextObj所指向的是否为有效指针
        if (NULL == *ppTextObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //如果文本点阵资源已装载，尝试卸载文本点阵资源
        if (NULL != (*ppTextObj)->ppTextMatrix)
        {
            if (UnloadTextMatrix(*ppTextObj))
            {
                iErr = -3;
            }
        }
    }

    if (!iErr)
    {
        //释放资源
        free((*ppTextObj)->pTextData);
        //销毁文本对象，并将指针置空
        free(*ppTextObj);
        *ppTextObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        直接输出指定的文本
  * 参数：
        1.GUITEXT *pTextObj:    文本指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DisplayText(GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIFONT *pFont = NULL;
    GUIIMAGE *pImg = NULL;
    GUIFONT_MATRIX **ppMatrix = NULL;
    unsigned int uiX, uiY, uiTmp;

    if (!iErr)
    {
        //判断pTextObj是否为有效指针
        if (NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //若文本点阵资源未装载，尝试装载文本点阵资源
        if (NULL == pTextObj->ppTextMatrix)
        {
            if (LoadTextMatrix(pTextObj))
            {
                iErr = -2;
            }
        }
    }

    if (!iErr)
    {
        //得到当前字体对象并判断是否为有效指针
        pFont = GetCurrFont();
        if (NULL == pFont)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //输出文本中各字符所对应的点阵
        ppMatrix = pTextObj->ppTextMatrix;
        uiX = pTextObj->uiPlaceX;
        uiY = pTextObj->uiPlaceY;
        for (uiTmp = 0; uiTmp < pTextObj->uiTextLength; uiTmp++)
        {
            //判断是否有足够空间显示文本中的当前字符
            if ((uiX + ppMatrix[uiTmp]->uiRealWidth) > 
                (pTextObj->uiPlaceX + pTextObj->uiViewWidth))
            {
                uiX = pTextObj->uiPlaceX;
                uiY += ppMatrix[uiTmp]->uiRealHeight
                     + pFont->uiTopGap + pFont->uiBottomGap;
            }
            if ((uiY + ppMatrix[uiTmp]->uiRealHeight) > 
                (pTextObj->uiPlaceY + pTextObj->uiViewHeight))
            {
                iErr = -4;
                break;
            }
            //尝试为单个字体点阵建立1位色图像对象
            pImg = CreateImage(pFont->uiFontWidth, pFont->uiFontHeight, 1);
            if (NULL == pImg)
            {
                iErr = -5;
                break;
            }
            //设置图像资源、显示大小、位置与颜色，并显示图像(即字体点阵)
            memcpy(pImg->pImgData, 
                   ppMatrix[uiTmp]->pMatrixData, 
                   round_up(pFont->uiFontWidth, 8) / 8 * pFont->uiFontHeight);
            pImg->uiDispWidth = ppMatrix[uiTmp]->uiRealWidth;
            pImg->uiDispHeight = ppMatrix[uiTmp]->uiRealHeight;
            pImg->uiPlaceX = uiX;
            pImg->uiPlaceY = isFont24() ? uiY-4 : uiY;//uiY;
            pImg->uiFgColor = pFont->uiFgColor;
            pImg->uiBgColor = pFont->uiBgColor;
            if (DisplayImage(pImg))
            {
                iErr = -6;
                break;
            }
            //销毁图像资源
            DestroyImage(&pImg);
            //移动到下一个显示位置
            if (isArabic() || 0 == ppMatrix[uiTmp]->uiRealWidth)
            {
            	uiX += ppMatrix[uiTmp]->uiRealWidth;
            }
			else
			{
            	uiX += ppMatrix[uiTmp]->uiRealWidth + pFont->uiLeftGap + pFont->uiRightGap;
			}
        }
    }

    //错误处理
    switch (iErr)
    {
    case -6:
        DestroyImage(&pImg);
        //no break
    case -5:
    case -4:
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * 功能：
        设置文本的内容
  * 参数：
        1.unsigned short *pTextData:    文本内容(宽字符)，以'\0'结尾
        2.GUITEXT *pTextObj:            需要设置文本内容的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetTextData(unsigned short *pTextData, GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned short *pData;
    unsigned int uiLen, uiSize;

    if (!iErr)
    {
        //判断pTextData和pTextObj是否有效
        if (NULL == pTextData || NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //如果文本点阵资源已装载，尝试卸载文本点阵资源
        if (NULL != pTextObj->ppTextMatrix)
        {
            if (UnloadTextMatrix(pTextObj))
            {
                iErr = -2;
            }
        }
    }

    if (!iErr)
    {
        //得到文本长度
        pData = pTextData;
        uiLen = 0;
        while (*pData++)                //文本内容，以'\0'结尾
        {
            uiLen++;
        }
        //分配内存资源并保存文本内容
        uiSize = 2 * (uiLen + 1);       //文本长度不含'\0'
        pData = (unsigned short *) malloc(uiSize);
        if (NULL != pData)
        {
            memcpy(pData, pTextData, uiSize);
        }
        else
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //释放旧的文本内容
        free(pTextObj->pTextData);
        //重设文本长度和文本内容
        pTextObj->uiTextLength = uiLen;
        pTextObj->pTextData = pData;
        //重设文本的显示大小
        pTextObj->uiViewWidth = 0;
        pTextObj->uiViewHeight = 0;
    }

    return iErr;
}


/***
  * 功能：
        设置文本的放置位置
  * 参数：
        1.unsigned int uiPlaceX:    文本水平放置位置，以左上角为基点，绝对坐标
        2.unsigned int uiPlaceY:    文本垂直放置位置，以左上角为基点，绝对坐标
        3.GUITEXT *pTextObj:        需要设置放置位置的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetTextPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                  GUITEXT *pTextObj)
{
    //判断pTextObj是否有效
    if (NULL == pTextObj)
    {
        return -1;
    }

    pTextObj->uiPlaceX = uiPlaceX;
    pTextObj->uiPlaceY = uiPlaceY;

    return 0;
}


/***
  * 功能：
        设置文本的显示大小
  * 参数：
        1.unsigned int uiViewWidth:     显示宽度，>=0，初始化为0则按文本实际宽度
        2.unsigned int uiViewHeight:    显示高度，>=0，初始化为0则按文本实际高度
        3.GUITEXT *pTextObj:            需要设置显示大小的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetTextView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                GUITEXT *pTextObj)
{
    //判断pTextObj是否有效
    if (NULL == pTextObj)
    {
        return -1;
    }

    pTextObj->uiViewWidth = uiViewWidth;
    pTextObj->uiViewHeight = uiViewHeight;

    return 0;
}


/***
  * 功能：
        装载文本点阵资源
  * 参数：
        1.GUITEXT *pTextObj:    文本指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int LoadTextMatrix(GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiSize, uiWidth;
    GUIFONT *pFont = NULL;
    GUIFONT_MATRIX *pMatrix = NULL;

    if (!iErr)
    {
        //判断pTextObj是否为有效指针
        if (NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断点阵资源是否已加载
        if (NULL != pTextObj->ppTextMatrix)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //尝试为文本所对应的点阵资源分配内存，注意内存必须清0
        uiSize = pTextObj->uiTextLength * sizeof(GUIFONT_MATRIX **);
        pTextObj->ppTextMatrix = (GUIFONT_MATRIX **) calloc(uiSize, 1);
        if (NULL == pTextObj->ppTextMatrix)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //得到当前字体对象并判断是否为有效指针
        pFont = GetCurrFont();
        if (NULL == pFont)
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //逐个提取点阵资源，并计算文本实际宽度
        uiWidth = 0;
		int phonogramCount = 1;
        for (uiSize = 0; uiSize < pTextObj->uiTextLength; uiSize++)
        {
        	if (isArabic())
        	{
        		//音标连接到右边字符上
				if (isArabicPhonogram(pTextObj->pTextData[uiSize]))
				{
					pMatrix = ExtractFontMatrix(pTextObj->pTextData[uiSize], pFont);
					if (pMatrix)
		            {
		                pMatrix->uiRealWidth = 0;
						//pMatrix->uiRealWidth = 0;
		            }
					//统计音标的个数
					phonogramCount++;
				}
				else
				{
					if (phonogramCount > 1)//说明有音标
					{
						pMatrix = ExtractFontMatrixJoin(pTextObj->pTextData+uiSize, pFont, phonogramCount, 1);
						phonogramCount = 1;
					}
					else
					{
						pMatrix = ExtractFontMatrix(pTextObj->pTextData[uiSize], pFont);
					}
				}
        	}
			else if (isThai())
        	{
				//音标连接到左边字符上
				if (isThaiPhonogram(pTextObj->pTextData[uiSize]))
				{
					pMatrix = ExtractFontMatrix(pTextObj->pTextData[uiSize], pFont);
					if (pMatrix)
		            {
		                pMatrix->uiRealWidth = 0;
						//pMatrix->uiRealHeight = 0;
		            }
				}
				else
				{
					//统计音标的个数
					while (phonogramCount + uiSize < pTextObj->uiTextLength &&
						isThaiPhonogram(pTextObj->pTextData[phonogramCount + uiSize]))
					{
						phonogramCount++;
					}
					if (phonogramCount > 1)//说明有音标
					{
						pMatrix = ExtractFontMatrixJoin(pTextObj->pTextData+uiSize, pFont, phonogramCount, 0);
						phonogramCount = 1;
					}
					else
					{
						pMatrix = ExtractFontMatrix(pTextObj->pTextData[uiSize], pFont);
					}
				}
        	}
			else
			{
				pMatrix = ExtractFontMatrix(pTextObj->pTextData[uiSize], pFont);
			}

            if (NULL == pMatrix)
            {
                iErr = -5;
                break;
            }
			
            pTextObj->ppTextMatrix[uiSize] = pMatrix;
			if (isArabic() || isThaiPhonogram(pTextObj->pTextData[uiSize]))
        	{
				uiWidth += pMatrix->uiRealWidth;
			}
			else
			{
				uiWidth += pMatrix->uiRealWidth
                     + pFont->uiLeftGap + pFont->uiRightGap;
			}
        }
    }

    if (!iErr)
    {
        //确定文本要显示的宽度与高度
        if ((0 == pTextObj->uiViewWidth) ||     //为0按实际宽度显示
            (pTextObj->uiViewWidth > uiWidth))  //超出按实际宽度显示
        {
            pTextObj->uiViewWidth = uiWidth;
        }
        if (0 == pTextObj->uiViewHeight)        //为0按实际高度显示
        {
            pTextObj->uiViewHeight = pFont->uiFontHeight;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -5:
        for (uiSize = 0; uiSize < pTextObj->uiTextLength; uiSize++)
        {
            if (NULL != pTextObj->ppTextMatrix[uiSize])
            {
                free(pTextObj->ppTextMatrix[uiSize]->pMatrixData);
                free(pTextObj->ppTextMatrix[uiSize]);
                pTextObj->ppTextMatrix[uiSize] = NULL;
            }
        }
        //no break
    case -4:
		free(pTextObj->ppTextMatrix);
		pTextObj->ppTextMatrix = NULL;
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * 功能：
        卸载文本点阵资源
  * 参数：
        1.GUITEXT *pTextObj:    文本指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int UnloadTextMatrix(GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiLen;

    if (!iErr)
    {
        //判断pTextObj是否为有效指针
        if (NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断pTextObj的点阵资源是否已加载
        if (NULL == pTextObj->ppTextMatrix)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //逐个释放字体点阵
        for (uiLen = 0; uiLen < pTextObj->uiTextLength; uiLen++)
        {
            if (NULL != pTextObj->ppTextMatrix[uiLen])
            {
                free(pTextObj->ppTextMatrix[uiLen]->pMatrixData);
                free(pTextObj->ppTextMatrix[uiLen]);
            }
        }
        //卸载文本点阵资源，并将指针置空
        free(pTextObj->ppTextMatrix);
        pTextObj->ppTextMatrix = NULL;
    }

    return iErr;
}

