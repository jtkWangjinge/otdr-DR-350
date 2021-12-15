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

#ifndef _ALGORITHM_ANALYSIS_H
#define _ALGORITHM_ANALYSIS_H


//#include "guiglobal.h"
//#include "app_global.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define maxf(a,b) ((a)>(b)?(a):(b))
#define minf(a,b) ((a)<(b)?(a):(b))
#define PI 3.141593f
#define RGB_Y 480
#define RGB_X 640
#define IMAGE_MID_VAL 128

#define THOUSAND    1000                 //!< 定点计算

#define ArangeMinR 11                   //!< A区域内径
#define ArangeMaxR 18                   //!< A区域外径
#define BrangeMinR 19                   //!< B区域内径
#define BrangeMaxR 75                   //!< B区域外径	
#define CrangeMinR 76                   //!< C区域内径
#define CrangeMaxR 95                   //!< C区域外径	
#define DrangeMinR 96                   //!< D区域内径
#define DrangeMaxR 171                  //!< D区域外径

#define EDGE_UP 40                      //!< 端面临界边缘(上)
#define EDGE_DOWN 40                    //!< 端面临界边缘(下)
#define EDGE_LEFT 40                    //!< 端面临界边缘(左)
#define EDGE_RIGHT 40                   //!< 端面临界边缘(右)
#define MAXFLAWNUMBER  250              //!< 最大缺陷个数
#define MAXFLAWCONSISTNUM 2000          //!< 每个缺陷组成点的最大计数
#define MAXIMAGEGRAY   255              //!< 最大灰度值
#define CIRCLEFIPOSIYIONSUCCESS 30      //!< 成功定位圆心标志

typedef unsigned char* IMG;

typedef struct _CIRCLE
{
    int x;                                             //圆心横坐标
    int y;                                             //圆心纵坐标
    int r;                                             //圆的半径
    int c;                                             //圆的周长
}CIRCLE;

typedef struct _ENDFACEIMFORMATION
{
    CIRCLE CirCenter;                                   //!< 目标端面信息
    int FlawTotalNum;                                   //!< 缺陷的总数目
    int Style[250];                                     //!< 缺陷的类型0为纤芯，1为划痕，2为污点
    int Circum[MAXFLAWNUMBER];                          //!< 缺陷区域的周长，即区域的边界像素个数
    int AisPassed;                                      //!< 0:合格 >0：不合格
    int BisPassed;                                      //!< 0:合格 >0：不合格
    int CisPassed;                                      //!< 0:合格 >0：不合格
    int DisPassed;                                      //!< 0:合格 >0：不合格
}ENDFACEIMFORMATION;

typedef struct _COORDINATE
{
	int x;								//!< 点的横坐标
	int y;								//!< 点的纵坐标
}COORDINATE1;

typedef struct _SIZED					//!< 图像大小
{
	int cx;
	int cy;
}SIZED;

typedef struct _RESULT
{
	COORDINATE1 Cir;

	int ARegionScracthNum;				//!< A区域划痕数量
	int ARegionDefectNum;				//!< A区域缺陷数量

	int BRegionSmallScracthNum; 		//!< B区域划痕数量
	int BRegionLargeScracthNum; 		//!< B区域划痕数量
	int BRegionSmallDefectNum;			//!< B区域缺陷数量
	int BRegionMiddleDefectNum; 		//!< B区域缺陷数量
	int BRegionLargeDefectNum;			//!< B区域缺陷数量

	int DRegionScracthNum;				//!< D区域划痕数量
	int DRegionSmallDefectNum;			//!< D区域缺陷数量
	int DRegionLargeDefectNum;			//!< D区域缺陷数量

}RESULT;

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
void SetPoints(IMG pResult, SIZED sz, COORDINATE1 *pPoint[250], ENDFACEIMFORMATION *EndFaceImf);

/*
*缺陷信息计算(获取)
*@para[in] prt        不同灰度缺陷计数
*@para[in] sz         图像大小
*@para[in] Cir        端面圆心信息
*@para[in] pRetPoint  缺陷(集合)
*@para[in] pResult    缺陷标识
*@para[in] count      缺陷个数
*@para[in] iA         A区域合格标志    0:不合格 1:合格
*@para[in] iB         B区域合格标志    0:不合格 1:合格
*@para[in] iC         C区域合格标志    0:不合格 1:合格
*@return   None
*@note 	None
*/
void GetProperty(IMG pResult, SIZED sz, ENDFACEIMFORMATION *EndFaceImf, RESULT *AnalyseResult);

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
int FlawPointCount(COORDINATE1 *pPoint, long int iNum, COORDINATE1 pCirCent, int iMinR, int iMaxR);

/*
*计算缺陷最大直径
*@para[in] pPoint 缺陷标识
*@para[in] iNum   该缺陷的点个数
*@return   缺陷最大直径
*@note     None
*/
int CalculateFlawDiameter(COORDINATE1 *pPoint, long int iNum, COORDINATE1 pCirCent, int iMinR, int iMaxR);

/*
*缺陷类型判断
*@para[in] circum   周长
*@para[in] diameter 缺陷直径
*@return   缺陷类型  1：划痕 2：污点
*@note     判断依据：周长除以直径<=5
*/
int FlawStyleJudge(int circum, int diameter);

/*
*高斯滤波器
*@para[in] pGray     原始灰度图像
*@para[in] sz        图像大小
*@para[in] pResult   高斯图像
*@para[in] sigma     高斯滤波器theata(标准差)
*@renturn  None
*@note     None
*/
void GaussianSmooth(IMG pGray, SIZED sz, IMG pResult, float sigma);

/*
*@Sobel算子
*@para[in] sz         图像大小
*@para[in] pGray      灰度图像
*@para[in] pGradX     X梯度
*@para[in] pGradY     Y梯度
*@para[in] pGradY     合成梯度
*@return   图像清晰度
*@note     使用Sobel算子求梯度，图像清晰度
*/
void SobelGrad(SIZED sz, IMG pGray, float *pGradX, float *pGradY, int *pMag);

/*
*@ 最大类间方差（大律法）
*@ para[in]    pMag 图像梯度
*@ para[in]    sz   图像大小
*@ return      图像分割阈值
*@ note        多用于数值大小单峰情况，前景和背景大小相差不大的情况，求得类间方差最大，类内方差最小
*/
float OSTU(int *pMag, SIZED sz, COORDINATE1 CirCore);

/*
*非极大值抑制
*@para[in] pMag      图像梯度
*@para[in] pGradX    图像X轴方向梯度
*@para[in] pGradY    图像Y轴方向梯度
*@para[in] sz        图像大小
*@para[in] pNSRst    图像二值
*/
void NonMaxSuppress(int *pMag, float *pGradX, float *pGradY, SIZED sz, IMG pNSRst);

/*
*边界查找(确定缺陷起始点)
*@para[in] pResult 图像标识
*@para[in] sz      图像大小
*@return   缺陷个数
*@note     缺陷个数最多不超过250
*/
int Hysteresis(IMG pResult, SIZED sz, COORDINATE1 CirCore);

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
void TraceEdge(int y, int x, IMG pResult, SIZED sz, int mark, int *Rec);

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
void HoughTransform(IMG pResult, ENDFACEIMFORMATION *EndFaceImf, SIZED sz);

/*
*图像亮度
*/
int CalculateImgBrightness(IMG OriImg, SIZED sz);

/*
*清晰度评价函数
*/
int ImageEvaluationClarity(IMG Gray, SIZED OriSize, COORDINATE1 CirCore);

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
//void ZoomImage(IMAGE *OriImg, IMAGE *RetImg, float Scale);

/*
*图像居中
*@para[in] Gray     原始图像
*@para[in] sz       原始图像大小
*@para[in] RetImg   居中图像（返回）
*@return   None
*@note     用于离焦状态下，居中目标图像
*/
//void AlignCenter(IMAGE *OriImg, IMAGE *RetImg);


/*
*图像居中
*@para[in] Gray     原始图像
*@para[in] sz       原始图像大小
*@para[in] RetImg   居中图像（返回）
*@return   None
*@note     用于离焦状态下，居中目标图像
*/
//void AlignCenter(IMG OriImg, SIZED OriSize, IMG RetImg, int Flag, COORDINATE1 *CirCore);
void AlignCenter(IMG OriImg, SIZED OriSize, IMG RetImg, int Flag, float Scale, COORDINATE1 *CirCore);
//void AlignCenter(IMG OriImg, SIZED OriSize, IMG RetImg, int Flag);

/*
*图像居中放大
*/
void SetPictureCenterZoom(IMG OriImg, SIZED sz, COORDINATE1 *CirCore, IMG RetImg);
//void SetPictureCenterZoom(IMG OriImg, SIZED sz, IMG RetImg);

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
void ZoomImage(IMG OriImg, SIZED OriSize, float Scale, IMG RetImg);

/*
*计算圆心（粗略）
*/
void CalCircleCore(IMG OriImg, SIZED sz, COORDINATE1 *CirCore);

/*
*计算圆心2（粗略）
*/
void CalCircleCore2(IMG OriImg, SIZED sz, COORDINATE1 *CirCore);



/*
*获取并设置端面（外部调用函数）
*/
//void GetResultPic(IMG OriImg, SIZED OriSize, IMG RetImg, int *brightness, int *sharpness);

/*
*检查光纤端面（外部调用函数）
*/
//int GetCheckPic(IMG OriImg, SIZED OriSize, IMG RetImg, RESULT *RetResult);

//void SetAutoCenter(int isCenter);

//void SetZoom(float Multiple);


//void savefata(IMG img, int size);

/*
*裁剪图像
*/
void CutImg(IMG OriImg, SIZED OriSize, SIZED GoalSize, IMG  RetImg);

/*
*获得经过调整后的图像
*@para[in]	OriImg		原始图像
*@para[in]	OriSize 	原始图像大小
*@para[in]	RetImg		输出图像
*@para[in]	brightness	图像亮度
*@para[in]	sharpness	原始清晰度
*@return	None
*@note		返回图像大小等于输入图像大小
*/
void GetResultPic(IMG OriImg, SIZED OriSize, IMG RetImg, int *brightness, int *sharpness);

/*
*获得经过调整后的图像
*@para[in]	OriImg		原始图像
*@para[in]	OriSize 	原始图像大小
*@para[in]	RetImg		输出图像
*@para[in]	RetResult	端面分析结果
*@return	返回是否找到端面 1：没找到 0：找到
*@note		None
*/
int GetCheckPic(IMG OriImg, SIZED OriSize, IMG RetImg, RESULT *RetResult);


/*
*获得经过调整后的图像
*@para[in]	isCenter	是否居中		  1：居中 0：不居中
*@return				返回是否设置成功  1：成功 0：失败
*@note		None
*/
int SetAutoCenter(int isCenter);

/*
*获得经过调整后的图像
*@para[in]	Multiple	放大倍数		  1：居中 0：不居中
*@return				返回是否设置成功  1：成功 0：失败
*@note		None
*/
int SetZoom(const float Multiple);

float GetZoomScale();


int getMeanandMaxInt(IMG img, int len, int *maxData);

void dealGradMsg(IMG GradMag, int len, int delta);

void getNewGradMsg(IMG OriImg, SIZED OriSize, IMG RetImg);

//extern "C" __declspec(dllexport)  //调用函数名。
#endif

