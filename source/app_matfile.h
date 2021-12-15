/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_inno.h  
* 摘    要：  inno格式文件的加载和生成功能
*
* 当前版本：  V1.0.0 
* 作     者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _APP_MATFILE_H_
#define _APP_MATFILE_H_

#include <stdio.h>

#define OTDRNAME MODEL_NUMBER
#define FIELDCOUNT 20
#define MAXPOINT 100000

typedef enum _MAT_STRUCT_TYPE_
{
	MAT_STRUCT_OTDR = 0,
	MAT_STRUCT_SOLA = 1,
}MAT_STRUCT_TYPE;

typedef struct _MATFILE_
{
	int iIsSave;
    int iWave;
    int iPulse;
    int iRange;
    int iBlind;
    float fSmpRation;
    float fSmpInterval;
    float fRefractiveIndex;
    float fBackScatter;
    unsigned int uiDisplayLen;
    unsigned int uiApd;
    unsigned int uiVdd;
    unsigned int uiChannel;
    float fSpliceLossThr;
    float fReturnLossThr;
    float fEndLossThr;
    int iIsHighResolution;
    unsigned int data[MAXPOINT];
	int iOffset;
	int iAtteDbClass;
    int iTestType;//OTDR, SOLA

    void (*closeMatFile)();
    int  (*addMatStruct)(struct _MATFILE_ *mat);
    int  (*addMatFile)(struct _MATFILE_ *mat, int wave);
    void (*setSave)(struct _MATFILE_ *mat, int save);
    void (*setWave)(struct _MATFILE_ *mat, int wave);
    void (*setPulse)(struct _MATFILE_ *mat, int pulse);
    void (*setRange)(struct _MATFILE_ *mat, int range);
    void (*setBlind)(struct _MATFILE_ *mat, int blind);
    void (*setSmpRation)(struct _MATFILE_ *mat, float smpRation);
    void (*setPoint2Distance)(struct _MATFILE_ *mat, float smpInterval);
    void (*setRefractiveIndex)(struct _MATFILE_ *mat, float refractiveIndex);
    void (*setBackScatter)(struct _MATFILE_ *mat, float backScatter);
    void (*setDisplayLen)(struct _MATFILE_ *mat, unsigned int displayLen);
    void (*setApd)(struct _MATFILE_ *mat, unsigned int apd);
    void (*setVdd)(struct _MATFILE_ *mat, unsigned int vdd);
    void (*setChannel)(struct _MATFILE_ *mat, unsigned int channel);
    void (*setSpliceLossThr)(struct _MATFILE_ *mat, float spliceLossThr);
    void (*setReturnLossThr)(struct _MATFILE_ *mat, float returnLossThr);
    void (*setEndLossThr)(struct _MATFILE_ *mat, float endLossThr);
    void (*setHighResolution)(struct _MATFILE_ *mat, int highResolution);
    void (*setData)(struct _MATFILE_ *mat, unsigned short *data, int dataLen);
	void (*setOffset)(struct _MATFILE_ *mat, int ioffset);
	void (*setAtteDbClass)(struct _MATFILE_ *mat, int iAtteDbClass);
    void (*setTestType)(struct _MATFILE_ *mat, MAT_STRUCT_TYPE iTestType);
}MATFILE, *PMATFILE;

//统计同等波长、脉宽测量的次数
#define PULSE_NUM   11

typedef struct _pulse_count
{
    int iPulse;
    int count;
}PULSE_COUNT;

typedef struct _test_count
{
    int wave;
    PULSE_COUNT pulseCount[PULSE_NUM];
}TEST_COUNT;

PMATFILE GetMatFile();


#endif
