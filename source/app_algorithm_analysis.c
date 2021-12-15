/*******************************************************************************
* Copyright(c)2013，大豪信息技术(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  algorithm_analysis.h
* 摘    要：  得到图像的属性（大小，类型）
*
* 当前版本：  v1.0.0
* 作    者：  Shan Juntao
* 完成日期： 
*
* 修改版本：  v1.0.1 
* 作    者：  
* 完成日期：
*******************************************************************************/
#include "app_algorithm_analysis.h"
#include<time.h>


float ZoomFlag = 1.0f;
float DefaultZoom = 1.0f;
int AlignCenterFlag = 1;

int LowExposeFlag = 0;
int HighExposeFlag = 0;
int ExposeOffset = 0;

int BrightNess = 0;
#define F21X
#define BIT_32MAX 2147483647

/*
*@建立边缘点集
*@para[in] pResult 图像(缺陷标记)
*@para[in] sz      图像大小
*@para[in] prt     图像缺陷信息
*@para[in] pPoint  不同灰度值的缺陷像素计数
*@para[in] Cir  	光纤端面圆信息
*@return   None
*@Note     None
*/
void SetPoints(IMG pResult, SIZED sz, COORDINATE1 *pPoint[250], ENDFACEIMFORMATION *EndFaceImf)
{
    int i, j, k, pos;

    for (i = EndFaceImf->CirCenter.x - 171; i < EndFaceImf->CirCenter.x + 171; i++)
    {
        if ((i < 0) || (i >= RGB_X))
        {
            continue;
        }
        for (j = EndFaceImf->CirCenter.y - 171; j < EndFaceImf->CirCenter.y + 171; j++)
        {
            if ((j < 0) || (j >= RGB_Y))
            {
                continue;
            }

            pos = i * sz.cy + j;

            if (pResult[pos] > 0)
            {
                k = pResult[pos] - 1;

                pPoint[k][EndFaceImf->Circum[k]].x = i;
                pPoint[k][EndFaceImf->Circum[k]].y = j;
                if (EndFaceImf->Circum[k] < MAXFLAWCONSISTNUM - 1)
                {
                    EndFaceImf->Circum[k] += 1;
                }
                else
                {
                    continue;
                }
            }
        }
    }
}


/*
*霍夫变换
*@ para[in] pResult 二值图像
*@ para[in] prt     缺陷信息
*@ para[in] Cir     圆心坐标
*@ para[in] sz      原图大小
*@ para[in] count
*@ para[in] iAddrx
*@ para[in] iAddry
*@ return   None
*@ note     转换坐标系(霍夫空间)，寻找圆心以及圆半径
*/
void HoughTransform(IMG pResult, ENDFACEIMFORMATION *EndFaceImf, SIZED sz)
{
    printf("----hello HoughTransform-----\n");
    int i = 0, j = 0, k = 0;
    static int CircleCenterDistribute[RGB_X][RGB_Y][5];

    for (i = 0; i < RGB_X; i++)
    {
        for (j = 0; j < RGB_Y; j++)
        {
            for (k = 0; k < 5; k++)
            {
                CircleCenterDistribute[i][j][k] = 0;
            }
        }
    }

    int Rmin = 80;                      //!< 查找最小半径
    int Rmax = 88;                      //!< 查找最大半径 
    int Rstep = 2;                      //!< 半径步长
    int Xooordinate = 0;                //!< X坐标
    int Ycoordinate = 0;                //!< Y坐标
    float angle = 0.0f;                 //!< 初始角度
    float angleStep = 0.2f * PI;        //!< 角度步长 
    int Rearchrange = (Rmax - Rmin) / Rstep;

    // 霍夫变换
    for (k = 0; k < Rearchrange; k++)
    {
        for (i = EDGE_UP; i < sz.cx - EDGE_DOWN; i++)
        {
            for (j = EDGE_LEFT; j < sz.cy - EDGE_RIGHT; j++)
            {
                for (angle = 0; angle < 2 * PI; angle += angleStep)
                {
                    if (pResult[i*sz.cx + j] > 0)
                    {
                        Xooordinate = (int)roundf(i - (k*Rstep + Rmin)*cosf(angle));
                        Ycoordinate = (int)roundf(j - (k*Rstep + Rmin)*sinf(angle));
                        if ((Xooordinate >= 0) && (Xooordinate < sz.cx) && (Ycoordinate >= 0) && (Ycoordinate < sz.cy))
                        {
                            CircleCenterDistribute[Xooordinate][Ycoordinate][k] += 1;
                        }
                    }

                }
            }
        }
    }

    // 寻找圆心
    int MaxValue = 0;
    for (k = 0; k < Rearchrange; k++)
    {
        for (i = EDGE_UP; i < sz.cx - EDGE_DOWN; i++)
        {
            for (j = EDGE_LEFT; j < sz.cy - EDGE_RIGHT; j++)
            {
                if (CircleCenterDistribute[i][j][k] > MaxValue)
                {
                    MaxValue = CircleCenterDistribute[i][j][k];
                    EndFaceImf->CirCenter.x = i;
                    EndFaceImf->CirCenter.y = j;
                    EndFaceImf->CirCenter.r = Rmin + k*Rstep;
                }
            }
        }
    }

    // 计算圆周长
    for (i = 0; i < sz.cx * sz.cy; i++)
    {
        if (pResult[i]>0)
        {
            Xooordinate = i / sz.cy;
            Ycoordinate = i % sz.cy;
            if (((Xooordinate - EndFaceImf->CirCenter.x)*(Xooordinate - EndFaceImf->CirCenter.x)
                + (Ycoordinate - EndFaceImf->CirCenter.y)*(Ycoordinate - EndFaceImf->CirCenter.y) >= EndFaceImf->CirCenter.r * EndFaceImf->CirCenter.r - 10)
                && ((Xooordinate - EndFaceImf->CirCenter.x)*(Xooordinate - EndFaceImf->CirCenter.x)
                + (Ycoordinate - EndFaceImf->CirCenter.y)*(Ycoordinate - EndFaceImf->CirCenter.y) <= (EndFaceImf->CirCenter.r * EndFaceImf->CirCenter.r + 10)))
            {
                EndFaceImf->CirCenter.c += 1;
            }
        }

    }
    //printf("----center:x=%d,y=%d,r=%d,c=%d---\n", EndFaceImf->CirCenter.x, EndFaceImf->CirCenter.y, EndFaceImf->CirCenter.r, EndFaceImf->CirCenter.c);
}

/*
*缺陷点计数
*@para[in] pPoint      不同的缺陷辨识以及各个缺陷点组成
*@para[in] iNum        各个缺陷组成点的个数
*@para[in] pCirCent    端面圆心信息
*@para[in] iMinR       区域内径
*@para[in] iMinR       区域外径
*@return   该区域内满足条件的缺陷点个数
*@note     None
*/
int FlawPointCount(COORDINATE1 *pPoint, long int iNum, COORDINATE1 pCirCent, int iMinR, int iMaxR)
{
    int i, j;
    int iN = iNum;
    int iDistance = 0;

    for (i = 0; i < iNum; i++)
    {

        if (((pPoint[i].x - pCirCent.x)*(pPoint[i].x - pCirCent.x) + (pPoint[i].y - pCirCent.y)*(pPoint[i].y - pCirCent.y) >= iMaxR*iMaxR)
            || (((pPoint[i].x - pCirCent.x)*(pPoint[i].x - pCirCent.x) + (pPoint[i].y - pCirCent.y)*(pPoint[i].y - pCirCent.y) <= iMinR*iMinR)))
        {
            iN--;
            continue;
        }
        if ((pPoint[i].x <= 2) || (pPoint[i].y <= 2) || (pPoint[i].x >= 485) || (pPoint[i].y >= 485))
        {
            pPoint[i].x = -1;
            pPoint[i].y = -1;
            iN--;
            continue;
        }

        iDistance = (pPoint[i].x - pCirCent.x)*(pPoint[i].x - pCirCent.x) + (pPoint[i].y - pCirCent.y)*(pPoint[i].y - pCirCent.y);
        if ((iDistance >= BrangeMaxR*BrangeMaxR) && (iDistance <= DrangeMinR*DrangeMinR))
        {
            pPoint[i].x = -1;
            pPoint[i].y = -1;
            iN--;
        }
        else
        {
            if (iDistance <= ArangeMinR*ArangeMinR)
            {
                pPoint[i].x = -1;
                pPoint[i].y = -1;
                iN--;
            }
            else
            {
                if (iDistance > DrangeMaxR*DrangeMaxR)
                {
                    pPoint[i].x = -1;
                    pPoint[i].y = -1;
                    iN--;
                }
            }
        }
    }

    if ((iN == 0))
    {
        return 0;
    }

    for (i = 0; i<iNum - 1; i++)
    {
        if ((pPoint[i].x <= 2) || (pPoint[i].y <= 2) || (pPoint[i].x >= 485) || (pPoint[i].y >= 485))
        {
            for (j = i + 1; j<iNum; j++)
            {
                if ((pPoint[j].x > 2) && (pPoint[j].y > 2) && (pPoint[j].x < 485) && (pPoint[j].y < 485))
                {
                    pPoint[i].x = pPoint[j].x;
                    pPoint[i].y = pPoint[j].y;
                    pPoint[j].x = -1;
                    pPoint[j].y = -1;
                    break;
                }
            }
        }
    }
    return iN;

}

/*
*缺陷类型判断
*@para[in] circum   周长
*@para[in] diameter 缺陷直径
*@return   缺陷类型  1：划痕 2：污点
*@note     判断依据：周长除以直径<=5
*/
int FlawStyleJudge(int circum, int diameter)
{
    if (diameter == 0)
    {
        return 0;
    }
    int style = circum / diameter >= 20 ? 1 : 2;
    return style;
}

/*
*缺陷信息计算(获取)
*@para[in] pResult       缺陷标识
*@para[in] sz            图像大小
*@para[in] EndFaceImf    端面信息
*@para[in] AnalyseResult 输出结果
*@return   None
*@note                   A区出现2um以上的缺陷,0~2um的缺陷大于5个,不合格;
B区出现5um以上的划痕,2~5um的缺陷大于5个,不合格;
C区为过渡区，不检测，一直合格;
D区出现10um以上的缺陷，不合格，否则合格;
*/
//void GetProperty(PROPERTY *prt, SIZED sz, CIRCLE Cir, IMG pResult, int count, int &iA, int &iB, int &iD)
void GetProperty(IMG pResult, SIZED sz, ENDFACEIMFORMATION *EndFaceImf, RESULT *AnalyseResult)
{
    int i = 0, j = 0;

    COORDINATE1 *pPoint[MAXFLAWNUMBER];               //!< 缺陷边缘坐标
    for (j = 0; j < MAXFLAWNUMBER; j++)
    {
        pPoint[j] = (COORDINATE1 *)malloc(sizeof(COORDINATE1)*MAXFLAWCONSISTNUM);
        memset(pPoint[j], 0, sizeof(COORDINATE1)* MAXFLAWCONSISTNUM);
    }

    // 统计各个缺陷坐标
    SetPoints(pResult, sz, pPoint, EndFaceImf);


    for (i = 0; i < EndFaceImf->FlawTotalNum; i++)
    {
        int iRemainder = 0;                           //!< 删除缺陷的一些不在考虑范围内的点后的个数
        int Diameter = 0;                             //!< 缺陷最大直径
        int style = 0;                                //!< 缺陷类型
        AnalyseResult->Cir.x = EndFaceImf->CirCenter.x;
        AnalyseResult->Cir.y = EndFaceImf->CirCenter.y;

        // D区域
        iRemainder = FlawPointCount(pPoint[i], EndFaceImf->Circum[i], AnalyseResult->Cir, DrangeMinR, DrangeMaxR);
        if (iRemainder != 0)
        {
            // 计算缺陷最大直径
            Diameter = CalculateFlawDiameter(pPoint[i], EndFaceImf->Circum[i], AnalyseResult->Cir, DrangeMinR, DrangeMaxR);

            // 判断缺陷类型
            style = FlawStyleJudge(iRemainder, Diameter);

            if ((Diameter > 14) && (iRemainder > 20))
            {
                if (style == 1)
                {
                    AnalyseResult->DRegionScracthNum++;
                }
                else if (style == 2)
                {
                    AnalyseResult->DRegionLargeDefectNum++;
                }
                //EndFaceImf->DisPassed++;
            }
            else if (style != 0)
            {
                AnalyseResult->DRegionSmallDefectNum++;
            }
        }

        // B区域
        iRemainder = FlawPointCount(pPoint[i], EndFaceImf->Circum[i], AnalyseResult->Cir, BrangeMinR, BrangeMaxR);
        if (iRemainder != 0)
        {
            // 计算缺陷最大直径
            Diameter = CalculateFlawDiameter(pPoint[i], EndFaceImf->Circum[i], AnalyseResult->Cir, BrangeMinR, BrangeMaxR);

            // 判断缺陷类型
            style = FlawStyleJudge(iRemainder, Diameter);
            if ((Diameter > 7) && (iRemainder > 7))                //!< 大划痕大缺陷
            {
                if (style == 1)
                {
                    AnalyseResult->BRegionLargeScracthNum++;
                }
                else if (style == 2)
                {
                    AnalyseResult->BRegionLargeDefectNum++;
                }
            }
            else if ((Diameter > 4) && (iRemainder > 4))           //!< 中划痕中缺陷
            {
                if (style == 2)
                {
                    AnalyseResult->BRegionMiddleDefectNum++;
                }
            }
            else                                                    //!< 小划痕小缺陷
            {
                if (style == 1)
                {
                    AnalyseResult->BRegionSmallScracthNum++;
                }
                else if (style == 2)
                {
                    AnalyseResult->BRegionSmallDefectNum++;
                }
            }
        }

        // A区域
        iRemainder = FlawPointCount(pPoint[i], EndFaceImf->Circum[i], AnalyseResult->Cir, ArangeMinR, ArangeMaxR);
        if (iRemainder != 0)
        {
            // 计算缺陷最大直径
            Diameter = CalculateFlawDiameter(pPoint[i], EndFaceImf->Circum[i], AnalyseResult->Cir, ArangeMinR, ArangeMaxR);

            // 判断缺陷类型
            style = FlawStyleJudge(iRemainder, Diameter);
            if ((Diameter > 3) && (iRemainder > 3))
            {
                if (style == 1)
                {
                    AnalyseResult->ARegionScracthNum++;
                }
                else if (style == 2)
                {
                    AnalyseResult->ARegionDefectNum++;
                }
            }
        }
    }

    for (j = 0; j < MAXFLAWNUMBER; j++)
    {
        free(pPoint[j]);
    }

    //printf("---GetProperty over2---\n");
}

/*
*计算缺陷最大直径
*@para[in] pPoint 缺陷标识
*@para[in] iNum   该缺陷的点个数
*@return   缺陷最大直径
*@note     None
*/
int CalculateFlawDiameter(COORDINATE1 *pPoint, long int iNum, COORDINATE1 pCirCent, int iMinR, int iMaxR)
{
    int i = 0;
    int MaxXcoordinate = 0;
    int MinXcoordinate = 500;
    int MaxYcoordinate = 0;
    int MinYcoordinate = 500;
    for (i = 0; i < iNum; i++)
    {
        if (((pPoint[i].x - pCirCent.x)*(pPoint[i].x - pCirCent.x) + (pPoint[i].y - pCirCent.y)*(pPoint[i].y - pCirCent.y) > iMinR*iMinR)
            && ((pPoint[i].x - pCirCent.x)*(pPoint[i].x - pCirCent.x) + (pPoint[i].y - pCirCent.y)*(pPoint[i].y - pCirCent.y) < iMaxR*iMaxR)
            && (pPoint[i].x != -1))
        {
            if (pPoint[i].x>MaxXcoordinate)
                MaxXcoordinate = pPoint[i].x;
            if (pPoint[i].y > MaxYcoordinate)
                MaxYcoordinate = pPoint[i].y;
            if (pPoint[i].x < MinXcoordinate)
                MinXcoordinate = pPoint[i].x;
            if (pPoint[i].y < MinYcoordinate)
                MinYcoordinate = pPoint[i].y;
        }
    }
    return maxf(MaxXcoordinate - MinXcoordinate, MaxYcoordinate - MinYcoordinate);
}

/*
*图像缩放
*@para[in] pGray  原始图像
*@para[in] Width  原始图像宽
*@para[in] Height 原始图像高
*@para[in] RetImg 缩放图像
*@para[in] Scale  缩放比例
*@return None
*@note            采用双线性插值算法进行图像缩放
*/
//void ZoomImage(IMAGE *OriImg, IMAGE *RetImg, float Scale)
#if 0
void ZoomImage(IMG OriImg, SIZED OriSize, IMG RetImg)
{
    SIZED RetSize;
    RetSize.cx = int(OriSize.cx*ZoomFlag);
    RetSize.cy = int(OriSize.cy*ZoomFlag);

    if (ZoomFlag == 1.0f)
    {
        memcpy(RetImg, OriImg, sizeof(unsigned char)*RetSize.cx*RetSize.cy);
        return;
    }
    //savefata(OriImg, OriSize.cx*OriSize.cy);

    int i = 0, j = 0;

    for (i = 0; i < RetSize.cx; i++)
    {
        for (j = 0; j < RetSize.cy; j++)
        {
            int xcoordinate = (int)floorf(i / ZoomFlag);
            int ycoordinate = (int)floorf(j / ZoomFlag);
            //printf("ycoordinate=%d\n", ycoordinate);
            if ((xcoordinate >= 0) && (ycoordinate >= 0) && (xcoordinate < OriSize.cx) && (ycoordinate < OriSize.cy))
            {
                float temp1 = OriImg[(xcoordinate + 1)*OriSize.cy + ycoordinate] * (i / ZoomFlag - xcoordinate)
                    + OriImg[xcoordinate*OriSize.cy + ycoordinate] * (1 - i / ZoomFlag + xcoordinate);
                float temp2 = OriImg[(xcoordinate + 1)*OriSize.cy + ycoordinate + 1] * (i / ZoomFlag - xcoordinate)
                    + OriImg[xcoordinate*OriSize.cy + ycoordinate + 1] * (1 - i / ZoomFlag + xcoordinate);
                RetImg[i*RetSize.cy + j] = (int)floorf(temp2*(j / ZoomFlag - ycoordinate) + temp1*(1 + ycoordinate - j / ZoomFlag));
                printf("%d   ", RetImg[i*RetSize.cy + j]);
            }
        }
        printf("\n");
    }
}
#endif
/*
*图像缩放
*@para[in] pGray  原始图像
*@para[in] Width  原始图像宽
*@para[in] Height 原始图像高
*@para[in] RetImg 缩放图像
*@para[in] Scale  缩放比例
*@return None
*@note            采用最近邻域插值算法进行图像缩放
*/
void ZoomImage(IMG OriImg, SIZED OriSize, float Scale, IMG RetImg)
{
    SIZED RetSize;
    RetSize.cx = (int)(OriSize.cx * Scale);
    RetSize.cy = (int)(OriSize.cy * Scale);

    if (Scale == 1.0f)
    {
        memcpy(RetImg, OriImg, sizeof(unsigned char) * RetSize.cx * RetSize.cy);
        return;
    }
    //savefata(OriImg, OriSize.cx*OriSize.cy);

    int i = 0, j = 0;

	int zoomIndex[2000] = { 0 };
	for (i = 0; i < 2000; ++i)
	{
		zoomIndex[i] = (int)(i / Scale);
	}

    for (i = 0; i < RetSize.cx; ++i)
    {
        //int xcoordinate = (int)(i / Scale);
		int xcoordinate = zoomIndex[i];
        int RetX = i * RetSize.cy;
        int OriX = xcoordinate * OriSize.cy;
        for (j = 0; j < RetSize.cy; ++j)
        {
            //RetImg[RetX + j] = OriImg[OriX + (int)(j / Scale)];
			RetImg[RetX + j] = OriImg[OriX + zoomIndex[j]];
        }
    }
	/*
	for (i = 0; i < RetSize.cy; i++)
	{
		int xcoordinate = (int)(i / Scale);
		int RetX = i * RetSize.cx;
		int OriX = xcoordinate * OriSize.cx;
		for (j = 0; j < RetSize.cx; j++)
		{
			RetImg[RetX + j] = OriImg[OriX + (int)(j / Scale)];
		}
	}
	*/
}

/*
*图像缩放2
*@para[in] OriImg  原始图像
*@para[in] OriSize 原始图像的大小
*@para[in] Scale   缩放比例
*@para[in] RetImg  返回的图像（显示区域不变，如果显示区域变大，需修改该函数）
*@return None
*@note            采用最近邻域插值算法进行图像缩放
*/
void ZoomImage2(IMG OriImg, SIZED OriSize, float Scale, IMG RetImg)
{
	if (Scale > 2.0f)
	{
		Scale = 2.0f;
	}

	SIZED tmpSize;
	tmpSize.cx = (int)(OriSize.cx * Scale);
	tmpSize.cy = (int)(OriSize.cy * Scale);
	IMG ZoomImg = (IMG)malloc(sizeof(unsigned char) * (tmpSize.cx * tmpSize.cy));

	ZoomImage(OriImg, OriSize, Scale, ZoomImg);

	CutImg(ZoomImg, tmpSize, OriSize, RetImg);
	
	free(ZoomImg);
}
/*
*计算圆心（粗略）
*/
void CalCircleCore(IMG OriImg, SIZED sz, COORDINATE1 *CirCore)
{
    float scale = 0.0625f * 2;
    //float scale = 1;
    SIZED RetSize;
    RetSize.cx = (int)(sz.cx*scale);
    RetSize.cy = (int)(sz.cy*scale);

    IMG ZoomImg = (IMG)malloc(sizeof(unsigned char)*(RetSize.cx*RetSize.cy));
    ZoomImage(OriImg, sz, scale, ZoomImg);

    int i = 0, j = 0;
    int Xcoordinate = 0;
    int Ycoordinate = 0;

    int Step = 1;                                     //!< 移动步长
    //int TemplateSize = (int)(CrangeMinR*scale);       //!< 模板大小（方形）
    int TemplateSize = (int)(CrangeMinR*scale);
    float MinMean = (float)BIT_32MAX;

    // 搜索目标区域
    int Xfocus = 0;
    int Yfocus = 0;

    //int offset = (int)(1.2*TemplateSize);
	int offset = (int)(2.2*TemplateSize);
    
    int ArrayNum = (RetSize.cy - 2 * TemplateSize) / Step;
    float* grayValue = (float*)malloc(sizeof(float) * ArrayNum);
    memset(grayValue, 0, sizeof(float) * ArrayNum);

    int count = 0;
    for (Xcoordinate = offset; Xcoordinate < RetSize.cx - offset; Xcoordinate += Step)
    {
        int k = 0;
        for (Ycoordinate = offset; Ycoordinate < RetSize.cy - offset; Ycoordinate += Step)
        {
            // 第一次计算（遍历模板大小）
            float sum = 0;
            //if ((Xcoordinate == TemplateSize) && (Ycoordinate == TemplateSize))
			if ((Xcoordinate == offset) && (Ycoordinate == offset))
            {
                for (i = Xcoordinate - TemplateSize; i < Xcoordinate + TemplateSize; i++)
                {
                    for (j = Ycoordinate - TemplateSize; j < Ycoordinate + TemplateSize; j++)
                    {
                        sum += ZoomImg[i*RetSize.cy + j];
                    }
                }
                //grayValue[k] = sum / (4 * TemplateSize *TemplateSize);
                grayValue[k] = sum;
                MinMean = grayValue[k];
                k++;
            }
            else
            {
                int jj = 0;
                float temp1 = 0.0f, temp2 = 0.0f;
                if (!count)
                {
                    temp1 = 0.0f; temp2 = 0.0f;
                    for (jj = Xcoordinate - TemplateSize; jj < Xcoordinate + TemplateSize; jj++)
                    {
                        temp1 += ZoomImg[(jj - 1)*RetSize.cy + Ycoordinate - TemplateSize - 1];
                        temp2 += ZoomImg[(jj - 1)*RetSize.cy + Ycoordinate + TemplateSize - 1];
                    }
                    //grayValue[k] = grayValue[k-1] - (temp1 - temp2) / (4 * TemplateSize *TemplateSize);
                    grayValue[k] = grayValue[k-1] - (temp1 - temp2);
                }
                else
                {
                    temp1 = 0.0f; temp2 = 0.0f;
                    for (jj = Ycoordinate - TemplateSize; jj < Ycoordinate + TemplateSize; jj++)
                    {
                        temp1 += ZoomImg[(Xcoordinate - TemplateSize - 1)*RetSize.cy + jj];
                        temp2 += ZoomImg[(Xcoordinate + TemplateSize - 1)*RetSize.cy + jj];
                    }
                    //grayValue[k] = grayValue[k] - (temp1 - temp2) / (4 * TemplateSize *TemplateSize);
                    grayValue[k] = grayValue[k] - (temp1 - temp2);

                }
                if (grayValue[k] < MinMean)
                {
                    MinMean = grayValue[k];
                    Xfocus = Xcoordinate;
                    Yfocus = Ycoordinate;
                }
                k++;
            }
            

        }
        count++;
    }

    CirCore->x = (int)(Xfocus / scale);
    CirCore->y = (int)(Yfocus / scale);
    free(ZoomImg);
    free(grayValue);
}

/*
*计算圆心2（粗略）
*/
void CalCircleCore2(IMG OriImg, SIZED sz, COORDINATE1 *CirCore)
{
	float scale = 0.0625f * 4;
	//float scale = 1;
	SIZED RetSize;
	RetSize.cx = (int)(sz.cx * scale);
	RetSize.cy = (int)(sz.cy * scale);

	IMG ZoomImg = (IMG)malloc(sizeof(unsigned char)*(RetSize.cx*RetSize.cy));
	ZoomImage(OriImg, sz, scale, ZoomImg);

	//char name[512] = "ZoomImage";
	//savePicture(ZoomImg, RetSize, name);

	int i = 0, j = 0;
	int Xcoordinate = 0;
	int Ycoordinate = 0;

	int Step = 2;                                     //!< 移动步长
	int TemplateSize = (int)(CrangeMinR * scale);
	//float MinMean = (float)BIT_32MAX;
	float MinMean = 0.0f;

	// 搜索目标区域
	int Xfocus = 0;
	int Yfocus = 0;

	//int offset = (int)(1.2f * TemplateSize);
	int offset = (int)(1.5 * TemplateSize);

	TemplateSize -= 3;

    int tmpP[2000] = { 0 };
    for (i = 0; i < 2000; ++i)
    {
        tmpP[i] = i * RetSize.cy;
    }

    int end1 = RetSize.cx - offset;
	int end2 = RetSize.cy - offset;
    
    int end3 = 0;
	int end4 = 0;
	int start1 = 0;
	int start2 = 0;
    float sum = 0;
    for (Xcoordinate = offset; Xcoordinate < end1; Xcoordinate += Step)
	{
		start1 = Xcoordinate - TemplateSize;
		for (Ycoordinate = offset; Ycoordinate < end2; Ycoordinate += Step)
		{
			// 第一次计算（遍历模板大小）
            sum = 0;
            end3 = Xcoordinate + TemplateSize;
			end4 = Ycoordinate + TemplateSize;
			start2 = Ycoordinate - TemplateSize;

			for (i = start1; i < end3; i++)
			{
				//tmpP = i * RetSize.cy;
				for (j = start2; j < end4; j++)
				{
                    sum += ZoomImg[tmpP[i] + j];
                }
			}
			if (sum > MinMean)
			{
				Xfocus = Xcoordinate;
				Yfocus = Ycoordinate;
				MinMean = sum;
			}
		}
	}

	/*
	for (Xcoordinate = offset; Xcoordinate < RetSize.cx - offset; Xcoordinate += Step)
	{
		for (Ycoordinate = offset; Ycoordinate < RetSize.cy - offset; Ycoordinate += Step)
		{
			// 第一次计算（遍历模板大小）
			float sum = 0;
			for (i = Xcoordinate - TemplateSize; i < Xcoordinate + TemplateSize; i++)
			{
				for (j = Ycoordinate - TemplateSize; j < Ycoordinate + TemplateSize; j++)
				{
					sum += (float)(ZoomImg[i * RetSize.cy + j]);
				}
			}
			if (sum > MinMean)
			{
				Xfocus = Xcoordinate;
				Yfocus = Ycoordinate;
				MinMean = sum;
			}
		}
	}
	*/

	CirCore->x = (int)(Xfocus / scale);
	CirCore->y = (int)(Yfocus / scale);
	free(ZoomImg);
}


/*
*图像居中
*@para[in] Gray     原始图像
*@para[in] sz       原始图像大小
*@para[in] RetImg   居中图像（返回）
*@para[in] Flag     居中方式 0：以当前图像大小居中 1：以原始图像大小进行居中
*@return   None
*@note     用于离焦状态下，居中目标图像
*/

//void AlignCenter(IMAGE *OriImg, IMAGE *RetImg)
void AlignCenter(IMG OriImg, SIZED OriSize, IMG RetImg, int Flag, float Scale, COORDINATE1 *CirCore)
{
    if (AlignCenterFlag == 0)
    {
        memcpy(RetImg, OriImg, sizeof(unsigned char)*OriSize.cx*OriSize.cy);
        return;
    }

    // 居中
    int CenterX = 0;
    int CenterY = 0;

    //是否以当前图像（目标图像）大小为标准进行居中
    if (Flag == 0)
    {
        CenterX = (int)(OriSize.cx / 2);
        CenterY = (int)(OriSize.cy / 2);
    }
    else
    {
        CenterX = (int)(OriSize.cx / Scale / 2);
        CenterY = (int)(OriSize.cy / Scale / 2);
       
    }
	
	if ((CirCore->x < CenterX) || (CirCore->y < CenterY) 
		|| (OriSize.cx - CenterX < CenterX) || (OriSize.cy - CenterY < CenterY)) 
	{ 
		memcpy(RetImg, OriImg, sizeof(unsigned char)*OriSize.cx*OriSize.cy); 
		return; 
	}

    int i = 0, j = 0;
    int CenterOffsetX = CirCore->x - CenterX;
    int CenterOffsetY = CirCore->y - CenterY;
    for (i = 0; i < OriSize.cx - CenterOffsetX; i++)
    {
        int OriX = (i + CenterOffsetX)*OriSize.cy;
        int RetX = i*OriSize.cy;
        for (j = 0; j < OriSize.cy - CenterOffsetY; j++)
        {
            RetImg[RetX + j] = OriImg[OriX + j + CenterOffsetY];
        }
    }

    // 圆心为中心
    CirCore->x = CenterX;
    CirCore->y = CenterY;
}




/*
*@Sobel算子
*@para[in] sz         图像大小
*@para[in] pGray      灰度图像
*@para[in] pGradX     X梯度
*@para[in] pGradY     Y梯度
*@para[in] pGradY     合成梯度
*@return   None
*@note     使用Sobel算子求梯度，图像清晰度
*/
void SobelGrad(SIZED sz, IMG pGray, float *pGradX, float *pGradY, int *pMag)
{
    int i = 0, j = 0;

    // Sobel算子
    float Gx[3][3] = { { 0.125f, 0.0f, -0.125f }, { 0.25f, 0.0f, -0.25f }, { 0.125f, 0.0f, -0.125f } };
    float Gy[3][3] = { { 0.125f, 0.25f, 0.125f }, { 0.0f, 0.0f, 0.0f }, { -0.125f, -0.25f, -0.125f } };

    for (i = 0; i < sz.cx; i++)
    {
        for (j = 0; j < sz.cy; j++)
        {
            if ((i == 0) || (i == sz.cx - 1) || (j == 0) || (j == sz.cy - 1))
            {
                pMag[i*sz.cy + j] = 0;
            }
            else
            {
                pGradX[i*sz.cy + j] = Gx[0][0] * pGray[(i - 1)*sz.cy + j - 1] + Gx[1][0] * pGray[i*sz.cy + j - 1] + Gx[2][0] * pGray[(i + 1)*sz.cy + j - 1]
                    + Gx[0][2] * pGray[(i - 1)*sz.cy + j + 1] + Gx[1][2] * pGray[i*sz.cy + j + 1] + Gx[2][2] * pGray[(i + 1)*sz.cy + j + 1];
                pGradY[i*sz.cy + j] = Gy[0][0] * pGray[(i - 1)*sz.cy + j - 1] + Gy[0][1] * pGray[(i - 1)*sz.cy + j] + Gy[0][2] * pGray[(i - 1)*sz.cy + j + 1]
                    + Gy[2][0] * pGray[(i + 1)*sz.cy + j - 1] + Gy[2][1] * pGray[(i + 1)*sz.cy + j] + Gy[2][2] * pGray[(i + 1)*sz.cy + j + 1];
                pMag[i*sz.cy + j] = (int)(pGradX[i*sz.cy + j] * pGradX[i*sz.cy + j] + pGradY[i*sz.cy + j] * pGradY[i*sz.cy + j]);

            }
        }
    }
}


/*
@ 最大类间方差（大律法）
@ para[in]    pMag 图像梯度
@ para[in]    sz   图像大小
@ return      图像分割阈值
@ note        多用于数值大小单峰情况，前景和背景大小相差不大的情况，求得类间方差最大，类内方差最小
*/
float OSTU(int *pMag, SIZED sz, COORDINATE1 CirCore)
{
    int i = 0;
    int *pcount = (int*)malloc(sizeof(int)*(MAXIMAGEGRAY + 1));
    float *distribute = (float*)malloc(sizeof(float)*(MAXIMAGEGRAY + 1));

    memset(pcount, 0, sizeof(int)*(MAXIMAGEGRAY + 1));
    memset(distribute, 0, sizeof(float)*(MAXIMAGEGRAY + 1));


    // 概率分布
    for (i = 0; i < sz.cx*sz.cy; i++)
    {
        if (pMag[i]>MAXIMAGEGRAY)
        {
            pMag[i] = MAXIMAGEGRAY;
        }
        pcount[pMag[i]] += 1;
    }
#if 0
    for (i = 0; i < MAXIMAGEGRAY + 1; i++)
    {
        printf("pcount[%d] = %d\n", i, pcount[i]);
    }
#endif
    for (i = 0; i < MAXIMAGEGRAY + 1; i++)
    {
        distribute[i] = (float)pcount[i] / (sz.cx*sz.cy);
        //printf("distribute[%d] = %f\n", i, distribute[i]);
    }

    //求类间类内方差起始点
    int start = 0;
    for (i = 0; i < MAXIMAGEGRAY; i++)
    {
        if (distribute[i] != 0)
        {
            start = i;
            break;
        }
    }

    //求类间类内方差结束点
    int end = 255;
    for (i = MAXIMAGEGRAY; i >= 0; i--)
    {
        if ((distribute[i] != 0) || (i <= start))
        {
            end = i;
            break;
        }
    }

    //求图像在灰度i时的前景和背景图像的平均灰度和概率
    float *MeanGray = (float*)malloc(sizeof(float)*(MAXIMAGEGRAY + 1));
    float *SumDistribute = (float*)malloc(sizeof(float)*(MAXIMAGEGRAY + 1));
    memset(MeanGray, 0, sizeof(float)*(MAXIMAGEGRAY + 1));
    memset(SumDistribute, 0, sizeof(float)*(MAXIMAGEGRAY + 1));

    SumDistribute[0] = distribute[0];
    MeanGray[0] = distribute[0] * start;

    for (i = 1; i < end - start; i++)               //!< 像素值为255不纳入计算（边界点）
    {
        MeanGray[i] += distribute[i] * (start + i) + MeanGray[i - 1];
        SumDistribute[i] = distribute[i] + SumDistribute[i - 1];
    }

    // 防止分母出现0
    for (i = 0; i < MAXIMAGEGRAY + 1; i++)
    {
        if ((SumDistribute[i] == 0) || (SumDistribute[i] == 1))
        {
            SumDistribute[i] += 0.001f;
        }
    }

    // 前景和背景的类间方差
    int index = 0;
    float maxvar = 0;
    float *variance = (float*)malloc(sizeof(float)*(MAXIMAGEGRAY + 1));
    memset(variance, 0, sizeof(float)*(MAXIMAGEGRAY + 1));

    for (i = 0; i < MAXIMAGEGRAY + 1; i++)
    {
        variance[i] = powf((MeanGray[end - 1] * SumDistribute[i] - MeanGray[i]), 2) / (SumDistribute[i] * (1 - SumDistribute[i]));
        if (variance[i]>maxvar)
        {
            maxvar = variance[i];
            index = i;
        }
    }
    int j = 0;

    // 低曝光只针对于端面进行处理
    if ((ExposeOffset != 0))
    {
        for (i = 0; i < sz.cx; i++)
        {
            for (j = 0; j < sz.cy; j++)
            {
                //if ((i - CirCore.x)*(i - CirCore.x) + (j - CirCore.y)*(j - CirCore.y)<CrangeMinR*CrangeMinR)
                //{
                    if (pMag[i*sz.cy + j]>maxf(index - ExposeOffset, 1))
                    {
                        pMag[i*sz.cy + j] = MAXIMAGEGRAY;
                    }
                    else
                    {
                        pMag[i*sz.cy + j] = 0;
                    }
                //}
                    /*else
                    {
                    if (pMag[i*sz.cy + j] > index)
                    {
                    pMag[i*sz.cy + j] = MAXIMAGEGRAY;
                    }
                    else
                    {
                    pMag[i*sz.cy + j] = 0;
                    }
                    }*/
            }
        }
    }
    else
    {
        for (i = 0; i < sz.cx*sz.cy; i++)
        {
            if (pMag[i]>index)
            {
                pMag[i] = MAXIMAGEGRAY;
            }
            else
            {
                pMag[i] = 0;
            }
        }
    }

    free(pcount);
    free(distribute);
    free(MeanGray);
    free(SumDistribute);
    free(variance);
    return 0;
}

/*
*非极大值抑制
*@para[in] pMag      图像梯度
*@para[in] pGradX    图像X轴方向梯度
*@para[in] pGradY    图像Y轴方向梯度
*@para[in] sz        图像大小
*@para[in] pNSRst    图像二值
*/
void NonMaxSuppress(int *pMag, float *pGradX, float *pGradY, SIZED sz, IMG pNSRst)
{
    int x = 0, y = 0;

    for (x = 0; x < sz.cx; x++)
    {
        pNSRst[x] = 0;
        pNSRst[(sz.cy - 1)*sz.cx + x] = 0;
    }

    for (y = 0; y < sz.cy; y++)
    {
        pNSRst[y*sz.cx] = 0;
        pNSRst[(y + 1)*sz.cx - 1] = 0;
    }

    for (y = 1; y < sz.cy - 1; y++)
    {
        for (x = 1; x < sz.cx - 1; x++)
        {
            int iPos = y * sz.cx + x;
            if (pMag[iPos] == 0)     //当前像素梯度幅值为0,则不是边界点
            {
                pNSRst[iPos] = 0;
            }
            else
            {
                int ftemp1 = 0;
                int ftemp2 = 0;
                int ftemp = pMag[iPos];                       //!< 当前点的梯度幅值
                int igradx = (int)pGradX[iPos];                    //!< 当前点的X方向导数
                int igrady = (int)pGradY[iPos];                    //!< 当前点的Y方向导数
                int imag1 = 0, imag2 = 0, imag3 = 0, imag4 = 0;
                int weight = 0;
                //if (igrady != 0)
                if (igradx == 0)
                {
                    ftemp1 = (int)pGradX[iPos - sz.cx];
                    ftemp2 = (int)pGradX[iPos + sz.cx];
                }
                else if (igrady == 0)
                {
                    ftemp1 = (int)pGradX[iPos - 1];
                    ftemp2 = (int)pGradX[iPos + 1];
                }
                else if ((THOUSAND*igrady / igradx > 1000) || (THOUSAND*igrady / igradx < -1000))
                {
                    if (igradx / igrady <= 0)
                    {
                        weight = -THOUSAND*igradx / igrady;   //!< 计算插值比例
                    }
                    else
                    {
                        weight = THOUSAND*igradx / igrady;
                    }

                    imag2 = pMag[iPos - sz.cx];
                    imag4 = pMag[iPos + sz.cx];

                    if (igradx*igrady > 0)
                    {
                        imag1 = pMag[iPos - sz.cx - 1];
                        imag3 = pMag[iPos + sz.cx + 1];
                    }
                    else
                    {
                        imag1 = pMag[iPos - sz.cx + 1];
                        imag3 = pMag[iPos + sz.cx - 1];
                    }
                }
                else
                {
                    if (igrady / igradx <= 0)
                    {
                        weight = -THOUSAND*igrady / igradx;   //计算插值比例
                    }
                    else
                    {
                        weight = THOUSAND*igrady / igradx;   //计算插值比例
                    }
                    imag2 = pMag[iPos + 1];
                    imag4 = pMag[iPos - 1];

                    if (igradx*igrady > 0)
                    {
                        imag1 = pMag[iPos + sz.cx + 1];
                        imag3 = pMag[iPos - sz.cx - 1];
                    }
                    else
                    {
                        imag1 = pMag[iPos - sz.cx + 1];
                        imag3 = pMag[iPos + sz.cx - 1];
                    }
                }

                ftemp1 = (weight * imag1 + (THOUSAND - weight)*imag2) / THOUSAND;      //利用imag1,imag2,imag3,imag4对梯度进行插值
                ftemp2 = (weight * imag3 + (THOUSAND - weight)*imag4) / THOUSAND;

                if ((ftemp >= ftemp1) && (ftemp >= ftemp2))          //当前像素的梯度是局部的最大值，该点可能是边界点
                {
                    pNSRst[iPos] = IMAGE_MID_VAL;
                    //pNSRst[iPos] = IMAGE_MID_VAL;
                }
                else
                {
                    pNSRst[iPos] = 0;                            //不是边界点
                }
            }
        }
    }
}


/*
*边界查找
*@para[in] pResult 图像标识
*@para[in] sz      图像大小
*@return   缺陷个数
*@note     缺陷个数最多不超过250
*/
int Hysteresis(IMG pResult, SIZED sz, COORDINATE1 CirCore)
{
    int x = 0;
    int y = 0;
    int iPos = 0;
    int iRec = 0;    //!< 缺陷点统计
    int mark = 0;    //!< 缺陷标号


    for (y = 10; y < sz.cy - 10; y++)       //!< 从距离边界为10的范围内进行统计缺陷                
    {
        for (x = 10; x<sz.cx - 10; x++)
        {
            if (((y - CirCore.x)*(y - CirCore.x) + (x - CirCore.y) *(x - CirCore.y)>ArangeMinR*ArangeMinR)
                && ((y - CirCore.x)*(y - CirCore.x) + (x - CirCore.y) *(x - CirCore.y) < DrangeMaxR*DrangeMaxR))
            {
                iPos = y * sz.cx + x;

                // 如果该像素是边界点,该点可作为一个缺陷的起始点
                if (pResult[iPos] == IMAGE_MID_VAL)
                {
                    mark++;
                    if (mark > MAXFLAWNUMBER)
                    {
                        return mark;
                    }

                    if (mark == IMAGE_MID_VAL)
                    {
                        mark++;
                    }
                    pResult[iPos] = mark;

                    iRec = 0;
                    TraceEdge(y, x, pResult, sz, mark, &iRec);

                    if (iRec > MAXFLAWCONSISTNUM - 1)
                    {
                        continue;
                    }
                }
            }
        }
    }
    if (mark > IMAGE_MID_VAL)
    {
        mark = mark - 1;
    }

    for (y = 0; y < sz.cy; y++)                                    //!< 不是边界的点，像素值置为0
    {
        for (x = 0; x<sz.cx; x++)
        {
            iPos = y * sz.cx + x;

            if (pResult[iPos] == IMAGE_MID_VAL)
            {
                pResult[iPos] = 0;
            }

            if (pResult[iPos] > IMAGE_MID_VAL)                   //!< 将设置边界值漏掉的128补上
            {
                pResult[iPos] = pResult[iPos] - 1;
            }
        }
    }
    return mark;
}

/*
*边界查找(确定缺陷的组成)
*@para[in] y        缺陷起始坐标Y
*@para[in] x        缺陷起始坐标X
*@para[in] pResult  图像标识
*@para[in] sz       图像大小
*@para[in] mark     缺陷灰度
*@para[in] Rec      该缺陷组成点计数
*@return   None
*@note     每个缺陷点个数最多不超过2000
*/
void TraceEdge(int y, int x, IMG pResult, SIZED sz, int mark, int *Rec)
{
    int i = 0, j = 0, k = 0;
    // 搜寻的12邻域
    int xNum[12] = { 1, 1, 0, -1, -1, -1, 0, 1, 2, -2, 0, 0 };
    int yNum[12] = { 0, 1, 1, 1, 0, -1, -1, -1, 0, 0, 2, -2 };

    if (*Rec > MAXFLAWCONSISTNUM)
    {
        return;
    }

    for (k = 0; k < 8; k++)
    {
        i = y + yNum[k];
        j = x + xNum[k];

        if ((i < 10) || (i >= 475) || (j < 10) || (j >= 475))
        {
            continue;
        }

        if ((pResult[i*sz.cx + j] == IMAGE_MID_VAL))
        {
            pResult[i*sz.cx + j] = mark;                 //!< 该点设置为边界点
            *Rec += 1;
            TraceEdge(i, j, pResult, sz, mark, Rec);     //!< 以该点为中心，再进行追踪。递归调用TraceEdge函数
        }
    }
}

/*
*高斯滤波器
*@para[in] pGray     原始灰度图像
*@para[in] sz        图像大小
*@para[in] pResult   高斯图像
*@para[in] sigma     高斯滤波器theata(标准差)
*@renturn  None
*@note     None
*/
void GaussianSmooth(IMG pGray, SIZED sz, IMG pResult, float sigma)
{

    int x, y;
    int i;
    int iWindowSize = 5;                                            //高斯滤波器长度	(int)(1+2*ceil(3*sigma));                     
    int iLength = iWindowSize / 2;                                  //窗口长度	

    int fDotMul;                                                    //高斯系数与图像数据的点乘
    int *pTemp = (int*)malloc(sizeof(int)*sz.cx*sz.cy);
    int pKernel[5] = { 26, 1658, 6649, 1658, 26 };                  //一维高斯滤波器1/(sqrt(2*PI)*sigma)*exp(-x^2/(2*sigma^2))
    int fWeightSum = 10017;                                         //滤波系数总和

    for (y = 0; y < sz.cy; y++)										//x方向滤波
    {
        for (x = 0; x < sz.cx; x++)
        {
            fDotMul = 0;

            for (i = (-iLength); i <= iLength; i++)
            {

                if ((i + x >= 0) && (i + x < sz.cx))                        //判断是否在图像内部
                {
                    fDotMul += pGray[y*sz.cx + (i + x)] * pKernel[iLength + i];   //高斯系数与图像数据的点乘
                }
            }
            pTemp[y*sz.cx + x] = fDotMul / fWeightSum;
        }
    }

    for (x = 0; x < sz.cx; x++)                                         //y方向滤波
    {
        for (y = 0; y < sz.cy; y++)
        {
            fDotMul = 0;
            for (i = (-iLength); i <= iLength; i++)
            {
                if ((i + y >= 0) && (i + y < sz.cy))                        //判断是否在图像内部
                {
                    fDotMul += pTemp[(y + i)*sz.cx + x] * pKernel[iLength + i];
                }
            }
            pResult[y*sz.cx + x] = (unsigned char)(fDotMul / fWeightSum);     //滤波后的图像
        }
    }

    free(pTemp);
    pTemp = NULL;
}

/*
*图像亮度
*/
int CalculateImgBrightness(IMG OriImg, SIZED sz)
{
    int i = 0, j = 0;
    float Brightness = 0;
    for (i = 1; i < sz.cx - 1; i++)
    {
        int OriX = i*sz.cy;
        for (j = 1; j < sz.cy - 1; j++)
        {
            Brightness += OriImg[OriX + j];
        }
    }

    // 亮度和曝光值近似线性关系 y=20x-5.5;
    float BrightnessMean = Brightness / ((sz.cx - 2)*(sz.cy - 2));
    return ((int)(BrightnessMean));
}


/*
*清晰度评价函数
*/
int ImageEvaluationClarity(IMG Gray, SIZED OriSize, COORDINATE1 CirCore)
{
    float *GradMag = (float *)malloc(sizeof(float)*OriSize.cx*OriSize.cy);                   //!< 总的梯度
    memset(GradMag, 0, sizeof(float)*OriSize.cx*OriSize.cy);

    int i = 0, j = 0;

    int count = 0;
    float sum = 0.0f;
    for (i = 80; i < OriSize.cx - 80; i++)
    {
        for (j = 80; j < OriSize.cy - 80; j++)
        {
#ifdef F21X
            if (((i - CirCore.x)*(i - CirCore.x) + (j - CirCore.y)* (j - CirCore.y) > CrangeMinR * CrangeMinR)    //!< 目标区域
                && ((i - CirCore.x)*(i - CirCore.x) + (j - CirCore.y)* (j - CirCore.y) < CrangeMaxR * CrangeMaxR))
            {
#endif
                GradMag[i*OriSize.cy + j] = sqrtf((float)((Gray[i*OriSize.cy + j] - Gray[i*OriSize.cy + j + 1])*(Gray[i*OriSize.cy + j] - Gray[i*OriSize.cy + j + 1])
                    + (Gray[(i + 1)*OriSize.cy + j] - Gray[i*OriSize.cy + j])*(Gray[(i + 1)*OriSize.cy + j] - Gray[i*OriSize.cy + j])));
                sum += GradMag[i*OriSize.cy + j];
                count++;
#ifdef F21X
            }
#endif
        }
    }
    float MeanGray = sum / count;

    int UpCount = 0;
    float UpSum = 0.0f;
    int DownCount = 0;
    float DownSum = 0.0f;
    for (i = 80; i < OriSize.cx - 80; i++)
    {
        for (j = 80; j < OriSize.cy - 80; j++)
        {
#ifdef F21X

            if (((i - CirCore.x)*(i - CirCore.x) + (j - CirCore.y)* (j - CirCore.y) > CrangeMinR * CrangeMinR)    //!< 目标区域
                && ((i - CirCore.x)*(i - CirCore.x) + (j - CirCore.y)* (j - CirCore.y) < CrangeMaxR * CrangeMaxR))
            {
#endif
                if (GradMag[i*OriSize.cy + j]>MeanGray)
                {
                    UpCount++;
                    UpSum += GradMag[i*OriSize.cy + j];
                }
                else
                {
                    DownCount++;
                    DownSum += GradMag[i*OriSize.cy + j];
                }
#ifdef F21X

            }
#endif
        }
    }
    if ((UpCount == 0) || (UpSum == 0) || (DownCount == 0) || (UpSum / UpCount == 0))
    {
        free(GradMag);
        return 30;
    }
    else if (MeanGray < 2.5f)
    {
        free(GradMag);
        return 30;
    }
    else
    {
        free(GradMag);
		//printf("Clarity=%d\n",minf(maxf((int)(100 * (UpSum / UpCount - DownSum / DownCount) / (UpSum / UpCount)), 0), 100));
        return minf(maxf((int)(100 * (UpSum / UpCount - DownSum / DownCount) / (UpSum / UpCount)), 0), 100);
    }
}

/*
*端面基本信息（外部调用函数）
*/
void GetResultPic(IMG OriImg, SIZED OriSize, IMG RetImg, int *brightness, int *sharpness)
{
    BrightNess = 255;

	*brightness = 0;;
	*sharpness = 0;

    static int oldAlignState = 0;//保存居中状态
    if (oldAlignState != AlignCenterFlag)
        oldAlignState = AlignCenterFlag;
    else
    {
        //若已居中则直接返回，减少算法计算次数（只做临时使用，等待算法优化）
        if (AlignCenterFlag == 1)
            return;
    }

    if ((AlignCenterFlag == 0) && (ZoomFlag == 1.0f))
	{
		memcpy(RetImg, OriImg, sizeof(unsigned char) * OriSize.cx * OriSize.cy);
		return;
	}

	if (AlignCenterFlag == 0)
	{
		ZoomImage2(OriImg, OriSize, ZoomFlag, RetImg);
		return;
	}
	
    // 圆心坐标
    COORDINATE1 CirCenter;
    CirCenter.x = 0;
    CirCenter.y = 0;
	//struct timeval tv_start, tv_end;
	//gettimeofday(&tv_start, NULL);

	IMG ResultImg = (IMG)malloc(sizeof(unsigned char) * OriSize.cx * OriSize.cy);              //!< 经过边缘检测后的结果

	getNewGradMsg(OriImg, OriSize, ResultImg);

	int GradMagMax = 0;
	int len = (int)(OriSize.cx * OriSize.cy);
	int GradMagMean = getMeanandMaxInt(ResultImg, len, &GradMagMax);

	dealGradMsg(ResultImg, len, GradMagMean + 2);

	//char name[512] = "dealGradMsg";
	//savePicture(ResultImg, OriSize, name);

	//CalCircleCore(OriImg, OriSize, &CirCenter);
	CalCircleCore2(ResultImg, OriSize, &CirCenter);

	//gettimeofday(&tv_end, NULL);
	//printf("CalCircleCore---------------%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
	//gettimeofday(&tv_start, NULL);

	/*
    // 计算清晰度
    *sharpness = ImageEvaluationClarity(OriImg, OriSize, CirCenter);

	//gettimeofday(&tv_end, NULL);
	//printf("ImageEvaluationClarity---------------%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
	//gettimeofday(&tv_start, NULL);
    
	// 计算图像亮度
    *brightness = CalculateImgBrightness(OriImg, OriSize);
    BrightNess = *brightness;
    printf("*brightness = %d\n",*brightness);
	*/

	//图像居中放大
    SetPictureCenterZoom(OriImg, OriSize, &CirCenter, RetImg);

	//gettimeofday(&tv_end, NULL);
	//printf("SetPictureCenterZoom---------------%ld\n\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
    // 图像亮度
    //*brightness = 5;
    //*brightness = CalculateImgBrightness(OriImg, OriSize);
	free(ResultImg);
}


/*
*裁剪图像
*/
void CutImg(IMG OriImg, SIZED OriSize, SIZED GoalSize, IMG  RetImg)
{
    if ((GoalSize.cx > OriSize.cx) || (GoalSize.cy > OriSize.cy))
    {
        printf("Cut Image Fail!!!\n");
        return;
    }
    else
    {
        int i = 0, j = 0;
		/*
		for (i = 0; i < GoalSize.cy; i++)
		{
			int OriX = i * OriSize.cx;
			int RetX = i * GoalSize.cx;
			for (j = 0; j < GoalSize.cx; j++)
			{
				RetImg[RetX + j] = OriImg[OriX + j];
			}
		}
		*/
        for (i = 0; i < GoalSize.cx; ++i)
        {
            int OriX = i * OriSize.cy;
            int RetX = i * GoalSize.cy;
            for (j = 0; j < GoalSize.cy; ++j)
            {
                RetImg[RetX + j] = OriImg[OriX + j];
            }
        }
		
    }
}

/*
*图像居中放大
*/
void SetPictureCenterZoom(IMG OriImg, SIZED sz, COORDINATE1 *CirCore, IMG RetImg)
{
    DefaultZoom = 1.0f;
    SIZED RetSize = {0,0};
    // 圆心坐标
    int CircleCoreX = CirCore->x;
    int CircleCoreY = CirCore->y;
	//struct timeval tv_start, tv_end;
	//gettimeofday(&tv_start, NULL);

    //printf("------BrightNess=%d,AlignCenterFlag=%d------\n",BrightNess,AlignCenterFlag);
    float Scale = 1.0f;
    
    if (AlignCenterFlag == 1)                    //!< 图像居中(不放大)
    {
        int MinDistanceToBoundaryX = minf(sz.cx - CircleCoreX, CircleCoreX);
        int MinDistanceToBoundaryY = minf(sz.cy - CircleCoreY, CircleCoreY);
        float MagnificationFactor = (float)sz.cx / 2.0f / minf(MinDistanceToBoundaryY, MinDistanceToBoundaryX);
        MagnificationFactor = minf(MagnificationFactor, (float)sz.cy / 2.0f / minf(MinDistanceToBoundaryY, MinDistanceToBoundaryX)) + 0.1f;
        
		/*
        if(BrightNess < 30)
        {
            CirCore->y =(int)( sz.cy / 2);
            CirCore->x =(int)( sz.cx / 2);
            CircleCoreX =(int)( sz.cx / 2);
            CircleCoreY =(int)( sz.cy / 2);
            printf("------hello------\n");
        }
		*/
        if ((ZoomFlag == 1) || (MagnificationFactor > ZoomFlag))                       //!< 图像不放大
        {           
            // 设置默认放大倍数
            DefaultZoom = MagnificationFactor;
            // 放大倍数赋值
            Scale = MagnificationFactor;

			if (Scale > 2) 
			{ 
				memcpy(RetImg, OriImg, sizeof(unsigned char)*sz.cx*sz.cy);
            	return;	
			}

            RetSize.cx = (int)(sz.cx * Scale);
            RetSize.cy = (int)(sz.cy * Scale);
        }
        else                                     //!< 图像放大
        {
            Scale = ZoomFlag;
            #if 0
            if(MagnificationFactor>Scale)
            {
                CirCore->y =(int)( sz.cy / 2);
                CirCore->x =(int)( sz.cx / 2);
                CircleCoreX =(int)( sz.cx / 2);
                CircleCoreY =(int)( sz.cy / 2);
            }
            #endif
            RetSize.cx = (int)(sz.cx * Scale);
            RetSize.cy = (int)(sz.cy * Scale);
        }
		// 严格控制放大倍数，避免在接头插拔时出现卡顿现象 
		
    }
    else if (AlignCenterFlag == 0)               //!< 图像不居中
    {
        //SetZoom(1.0);
        if (ZoomFlag == 1)                       //!< 图像不放大
        {
            memcpy(RetImg, OriImg, sizeof(unsigned char)*sz.cx*sz.cy);
            return;
        }
        else
        {
            Scale = ZoomFlag;
            RetSize.cx = (int)(sz.cx * Scale);
            RetSize.cy = (int)(sz.cy * Scale);
        }
    }
	
	//gettimeofday(&tv_end, NULL);
	//printf("SetPictureCenterZoom---------------%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
	//gettimeofday(&tv_start, NULL);
    IMG ZoomImg = (IMG)malloc(sizeof(unsigned char)*(RetSize.cx*RetSize.cy));
    memset(ZoomImg, 0, sizeof(unsigned char)*(RetSize.cx*RetSize.cy));

    ZoomImage(OriImg, sz, Scale, ZoomImg);

	//char name[512] = "ZoomImage_2";
	//savePicture(ZoomImg, RetSize, name);

    //savefata(ZoomImg, RetSize.cx*RetSize.cy);
	//gettimeofday(&tv_end, NULL);
	//printf("SetPictureCenterZoom  ZoomImage---------------%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
	//gettimeofday(&tv_start, NULL);

    IMG CenterImg = (IMG)malloc(sizeof(unsigned char)*(RetSize.cx*RetSize.cy));
    memset(CenterImg, 0, sizeof(unsigned char)*(RetSize.cx*RetSize.cy));

    //COORDINATE1 CirCenter;
    //CalCircleCore(ZoomImg, RetSize, &CirCenter);
    //printf("CirCore1->x=%d    CirCore1->y=%d\n",CirCore->x,CirCore->y);
    CirCore->x = (int)((CirCore->x+1)*Scale);
    CirCore->y = (int)((CirCore->y+1)*Scale);
    printf("CirCore2->x=%d    CirCore2->y=%d\n",CirCore->x,CirCore->y);
    AlignCenter(ZoomImg, RetSize, CenterImg, 1, Scale, CirCore);
    //printf("CirCore3->x=%d    CirCore3->y=%d\n",CirCore->x,CirCore->y);
    //savefata(CenterImg, RetSize.cx*RetSize.cy);
	//gettimeofday(&tv_end, NULL);
	//printf("SetPictureCenterZoom  AlignCenter---------------%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
	//gettimeofday(&tv_start, NULL);

    // 裁剪图像
    CutImg(CenterImg, RetSize, sz, RetImg);
    //printf("CirCore4->x=%d    CirCore4->y=%d\n",CirCore->x,CirCore->y);
	//gettimeofday(&tv_end, NULL);
	//printf("SetPictureCenterZoom  CutImg---------------%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));

	//char name2[512] = "CutImg";
	//savePicture(RetImg, sz, name2);

    free(ZoomImg);
    free(CenterImg);
}


/*
*检查光纤端面
*/
int GetCheckPic(IMG OriImg, SIZED OriSize, IMG RetImg, RESULT *RetResult)
{
    //printf("GetCheckPic start!!!\n");
    IMG FilterGray = (IMG)malloc(sizeof(unsigned char)*OriSize.cx*OriSize.cy);          //!< 经过高斯滤波后的图像
    IMG ResultImg = (IMG)malloc(sizeof(unsigned char)*OriSize.cx*OriSize.cy);              //!< 经过边缘检测后的结果
    float *GradX = (float *)malloc(sizeof(float)*OriSize.cx*OriSize.cy);                      //!< X方向梯度	
    float *GradY = (float *)malloc(sizeof(float)*OriSize.cx*OriSize.cy);                      //!< Y方向梯度
    int *GradMag = (int *)malloc(sizeof(int)*OriSize.cx*OriSize.cy);                          //!< 总的梯度

    memset(FilterGray, 0, sizeof(unsigned char)*OriSize.cx*OriSize.cy);
    memset(ResultImg, 0, sizeof(unsigned char)*OriSize.cx*OriSize.cy);
    memset(GradX, 0, sizeof(float)*OriSize.cx*OriSize.cy);
    memset(GradY, 0, sizeof(float)*OriSize.cx*OriSize.cy);
    memset(GradMag, 0, sizeof(int)*OriSize.cx*OriSize.cy);

    COORDINATE1 CirCoreTemp;
    CirCoreTemp.x = 0;
    CirCoreTemp.y = 0;
    CalCircleCore(OriImg, OriSize, &CirCoreTemp);

    // 调整亮度
    int brightness = CalculateImgBrightness(OriImg, OriSize);

    brightness = ((int)((brightness + 5.5f) / 20.0f));

    // 曝光阈值步长
    ExposeOffset = (6 - brightness) * 7;

    /*if (brightness < 6)
    {
    LowExposeFlag = (6 - brightness) * 7;
    }
    if (brightness>6)
    {
    HighExposeFlag = (brightness - 6) * 15;
    }*/



    // 高斯滤波
    float sigma = 0.6f;                                                               //!< 图像平滑参数
    GaussianSmooth(OriImg, OriSize, FilterGray, sigma);
    //printf("GaussianSmooth over!!!\n");

    // Sobel算子求梯度
    SobelGrad(OriSize, FilterGray, GradX, GradY, GradMag);
    //printf("SobelGrad over!!!\n");

    // 最大类间方差
    OSTU(GradMag, OriSize, CirCoreTemp);
    //printf("OSTU over!!!\n");

    // 极大值抑制
    NonMaxSuppress(GradMag, GradX, GradY, OriSize, ResultImg);
    //printf("NonMaxSuppress over!!!\n");

    // 霍夫变换
    ENDFACEIMFORMATION EndFaceImformation;
    //EndFaceImformation.FlawTotalNum = count;
    EndFaceImformation.CirCenter.x = 0;
    EndFaceImformation.CirCenter.y = 0;
    EndFaceImformation.CirCenter.r = 0;
    EndFaceImformation.CirCenter.c = 0;
    memset(EndFaceImformation.Circum, 0, sizeof(int)*MAXFLAWNUMBER);
    memset(EndFaceImformation.Style, 0, sizeof(int)*MAXFLAWNUMBER);

    HoughTransform(ResultImg, &EndFaceImformation, OriSize);

    COORDINATE1 CirCenter;
    CirCenter.x = EndFaceImformation.CirCenter.x;
    CirCenter.y = EndFaceImformation.CirCenter.y;

    int MissCirCore = 0;
    if ((fabsf((float)CirCoreTemp.x - CirCenter.x) > 20) || (fabsf((float)CirCoreTemp.y - CirCenter.y) > 20))
    {
		printf("----fail!------\n");
        MissCirCore = 1;
    }

	printf("C=%d\n",EndFaceImformation.CirCenter.c);
    // 端面边缘周长低于阈值可认为找不到端面
    if ((EndFaceImformation.CirCenter.c < CIRCLEFIPOSIYIONSUCCESS) || (MissCirCore == 1))
    {
        free(FilterGray);
        free(ResultImg);
        free(GradX);
        free(GradY);
        free(GradMag);

        // 计算圆心
        CalCircleCore(OriImg, OriSize, &CirCenter);

        // 设置图像居中放大
        SetPictureCenterZoom(OriImg, OriSize, &CirCenter, RetImg);

        return 1;
    }

    // 边缘检测
    int count = Hysteresis(ResultImg, OriSize, CirCenter);
    //printf("Hysteresis over!!!\n");

    if (count > MAXFLAWNUMBER)
    {
        count = MAXFLAWNUMBER;
    }

    EndFaceImformation.FlawTotalNum = count;

    //获取端面信息
    GetProperty(ResultImg, OriSize, &EndFaceImformation, RetResult);

    int i = 0, j = 0;
    int iDistance = 0;
    for (i = 0; i < OriSize.cx; i++)
    {
        for (j = 0; j < OriSize.cy; j++)
        {
            iDistance = (i - RetResult->Cir.x)*(i - RetResult->Cir.x) + (j - RetResult->Cir.y)*(j - RetResult->Cir.y);
            if ((ResultImg[i*OriSize.cy + j]>0) && (((iDistance<BrangeMaxR*BrangeMaxR) || (iDistance>DrangeMinR*DrangeMinR)) && (iDistance>ArangeMaxR*ArangeMaxR)))
            {
                ResultImg[i*OriSize.cy + j] = 255;
            }
            else
            {
                ResultImg[i*OriSize.cy + j] = OriImg[i*OriSize.cy + j];
            }
        }
    }

    // 设置图像居中放大
    SetPictureCenterZoom(ResultImg, OriSize, &RetResult->Cir, RetImg);

    //savefata(RetImg, OriSize.cx*OriSize.cy);


    free(FilterGray);
    free(ResultImg);
    free(GradX);
    free(GradY);
    free(GradMag);
    //free(ImgHance);
    return 0;
}

/*
*居中
*/
//void SetAutoCenter(int isCenter)
int SetAutoCenter(int isCenter)
{
    AlignCenterFlag = isCenter;
    return 1;
}

/*
*放大
*/
//void SetZoom(float Multiple)
int SetZoom(const float Multiple)
{
    ZoomFlag = Multiple;
	
    return 1;
}

float GetZoomScale()
{
    if (DefaultZoom == 1.0f)
    {
        return ZoomFlag;
    }
    else
    {
        return DefaultZoom;
    }
}


int getMeanandMaxInt(IMG img, int len, int *dataMax)
{
	int i = 0;
	float sum = 0.0f;
	int maxImg = -10000000;
	for (i = 0; i < len; ++i)
	{
		sum += (float)(img[i]);
		if (img[i] > maxImg)
		{
			maxImg = img[i];
		}
	}

	*dataMax = maxImg;
	int ret = (int)(sum / len + 0.5f);
	return ret;
}

void dealGradMsg(IMG GradMag, int len, int delta)
{
	int i = 0;
	for (i = 0; i < len; ++i)
	{
		if (GradMag[i] > delta)
		{
			GradMag[i] = 255;
		}
		else
		{
			GradMag[i] = 0;
		}
	}
}


void getNewGradMsg(IMG OriImg, SIZED OriSize, IMG RetImg)
{
	int pos = 0;
	int i = 0, j = 0;
	for (i = 0; i < OriSize.cx; i++)
	{
		int tmpP = i * OriSize.cy;
		RetImg[tmpP] = 0;
		for (j = 1; j < OriSize.cy; j++)
		{
			pos = tmpP + j;
			if (OriImg[pos] > OriImg[pos - 1])
			{
				RetImg[pos] = OriImg[pos] - OriImg[pos - 1];
			}
			else
			{
				RetImg[pos] = OriImg[pos - 1] - OriImg[pos];
			}
		}
	}
}












