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

#include "app_matfile.h"
#include "app_global.h"

#include "matio.h"
#include "guiconfig.h"
#include <unistd.h>
#include <string.h>

const char *fieldnames[FIELDCOUNT] = {"Wave", "Pulse", "Range", "Blind", "SmpRatio",
									  "SampleInterval", "RefractiveIndex", "BackScatter", "DisplayLen", "ApdIndex",
									  "VddIndex", "Channel", "SpliceLossThr", "ReturnLossThr", "EndLossThr", "IsHighResolution",
									  "SignalData", "Offset", "AtteDbClass", "TestType"};

const char *MatfileStructType[2] = {"OtdrData_Group", "SolaData_Group"};

static mat_t *matio = NULL;
static matvar_t *structMatvar = NULL;
static matvar_t *matvar[FIELDCOUNT+1] = {NULL};//必须多一个，否则死机。matio明确说明的。
static int fieldMap[FIELDCOUNT] = {0};

static int matinit = 0;
static int structNum = 0;
static MATFILE g_matfile = {0};


typedef struct list_node
{
	int wave;
	int count;
	struct list_node *next;
}ListNode;
static ListNode *wavelist = NULL; 

static int  addListNode(int wave);
static int  getFileName(char *name, int wave);

static void closeMatFile();
static int  addMatStruct(PMATFILE mat);
static int  addMatFile(PMATFILE mat, int wave);
static void setSave(PMATFILE mat, int save);
static void setWave(PMATFILE mat, int wave);
static void setPulse(PMATFILE mat, int pulse);
static void setRange(PMATFILE mat, int range);
static void setBlind(PMATFILE mat, int blind);
static void setSmpRation(PMATFILE mat, float smpRation);
static void setPoint2Distance(PMATFILE mat, float smpInterval);
static void setRefractiveIndex(PMATFILE mat, float refractiveIndex);
static void setBackScatter(PMATFILE mat, float backScatter);
static void setDisplayLen(PMATFILE mat, unsigned int displayLen);
static void setApd(PMATFILE mat, unsigned int apd);
static void setVdd(PMATFILE mat, unsigned int vdd);
static void setChannel(PMATFILE mat, unsigned int channel);
static void setSpliceLossThr(PMATFILE mat, float spliceLossThr);
static void setReturnLossThr(PMATFILE mat, float returnLossThr);
static void setEndLossThr(PMATFILE mat, float endLossThr);
static void setHighResolution(PMATFILE mat, int highResolution);
static void setData(PMATFILE mat, unsigned short *data, int dataLen);
static void setOffset(PMATFILE mat, int offset);
static void setAtteDbClass(PMATFILE mat, int iAtteDbClass);
static void setTestType(PMATFILE mat, MAT_STRUCT_TYPE iTestType);

static int addListNode(int wave)
{
	int ret = 0;
	int hasThisWave = 0;

	//先遍历
	if (wavelist)
	{
		ListNode *cur = wavelist;
	    //遍历链表
	    while(cur != NULL)
	    {
	    	if (cur->wave == wave)
	    	{
	    		ret = ++(cur->count);
				hasThisWave = 1;
				break;
	    	}
	        cur = cur->next;
	    }
	}

	if (!hasThisWave)
	{
		//没有则插入
		ListNode *node = NULL;
		node = (ListNode *)malloc(sizeof(ListNode));
		if(node == NULL)
		{
			LOG(LOG_ERROR, "malloc failed!\n");
			ret = -1;
		}
		else
		{
			memset(node,0,sizeof(ListNode));
			node->wave = wave;
			node->count = 1;
			node->next = NULL;

		    //插入链表
			if (wavelist)
			{
				ListNode *cur = wavelist;
		        //遍历链表，让cur指向最后一个元素
		        while(cur->next != NULL)
		        {
		            cur = cur->next;
		        }
				cur->next = node;
			}
			else
			{
				wavelist = node;
			}
			
			ret = 1;
		}
	}
	
	return ret;
}

//获取测试次数
static int GetTestCount(int wave)
{
	//struct:{wave,{{pulse0,count}....{pulse10,count}}}
	static TEST_COUNT testCount[3] =
	{
		{1310, {{5, 0}, {10, 0}, {20, 0}, {50, 0}, {100, 0}, {200, 0}, {500, 0}, {1000, 0}, {2000, 0}, {10000, 0}, {20000, 0}}},
		{1550, {{5, 0}, {10, 0}, {20, 0}, {50, 0}, {100, 0}, {200, 0}, {500, 0}, {1000, 0}, {2000, 0}, {10000, 0}, {20000, 0}}},
		{1625, {{5, 0}, {10, 0}, {20, 0}, {50, 0}, {100, 0}, {200, 0}, {500, 0}, {1000, 0}, {2000, 0}, {10000, 0}, {20000, 0}}}
	};

	int i;
	int j;
	int count = 0;
	for (i = 0; i < 3; ++i)
	{
		for (j = 0; j < PULSE_NUM; ++j)
		{
			if ((testCount[i].wave == wave) && (testCount[i].pulseCount[j].iPulse == g_matfile.iPulse))
			{
				count = testCount[i].pulseCount[j].count;
				testCount[i].pulseCount[j].count++;
				break;
			}
		}
	}

	return count;
}

static int  getFileName(char *name, int wave)
{
	int ret = 0;

	if (0 == access(MntUsbDirectory, F_OK))
	{
		int count = addListNode(wave);
		if (count > 0)
		{
			if (g_matfile.iTestType == MAT_STRUCT_OTDR)
				snprintf(name, 256, MntUsbDirectory"/data/%s-%d-%d-%d.mat", OTDRNAME, wave, g_matfile.iPulse, GetTestCount(wave));
			else if (g_matfile.iTestType == MAT_STRUCT_SOLA)
				snprintf(name, 256, MntUsbDirectory "/data/%s-%d-%d.mat", OTDRNAME, wave, count);
		}
		else
		{
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}
    
	return 0;
}


static void freeMatVar()
{
    int i = 0;
    for (i = 0; i < FIELDCOUNT; ++i)
	{
		if (fieldMap[i])
		{
			Mat_VarFree(matvar[i]);
			matvar[i] = NULL;
			fieldMap[i] = 0;
		}
	}
}

static void closeMatFile()
{
	structNum = 0;
	freeMatVar();
    if (matio)
    {
        Mat_Close(matio);
        matio = NULL;
    }
}

static int addMatStruct(PMATFILE mat)
{
	MAT_STRUCT_TYPE type = mat->iTestType;
	//检查参数
	if (type > MAT_STRUCT_SOLA || type < MAT_STRUCT_OTDR || (!mat->iIsSave))
	{
		freeMatVar();
		return -1;
	}

	int flag = 0;
    int i = 0;
    for (i = 0; i < FIELDCOUNT; ++i)
    {
        if (0 == fieldMap[i])
        {
            flag = 1;
            break;
        }
    }

	if (flag)
    {
        freeMatVar();
        return -1;
    }

	if (matio)
    {
        size_t dims[2] = {1, 1};

		char name[512] = {0};
		memset(name, 0, 512);
        snprintf(name, 512, "%s%d", MatfileStructType[type], ++structNum);

        structMatvar = NULL;

        matvar[FIELDCOUNT] = NULL;

        structMatvar = Mat_VarCreate(name, MAT_C_STRUCT, MAT_T_STRUCT, 2, dims, matvar, 0);

        if (structMatvar)
		{
            Mat_VarWrite(matio, structMatvar, MAT_COMPRESSION_NONE);
            Mat_VarFree(structMatvar);
            structMatvar = NULL;
            int i = 0;
            for (i = 0; i < FIELDCOUNT; i++)
            {
                matvar[i] = NULL;
                fieldMap[i] = 0;
            }

            return 0;
        }
    }
    return -1;
}

static int addMatFile(PMATFILE mat, int wave)
{
	int ret = 0;
	if (mat && mat->iIsSave)
	{
		closeMatFile();

		char name[256] = {0};
		if (0 == getFileName(name, wave))
		{
			matio = Mat_CreateVer(name, NULL, MAT_FT_DEFAULT);
		}
		ret = matio ? 0 : -1;
	}
	return ret;
}

static void setSave(PMATFILE mat, int save)
{
	if (mat)
	{
		mat->iIsSave = save;
	}
}

static void setWave(PMATFILE mat, int wave)
{
    if (mat && mat->iIsSave && !fieldMap[0])
    {
        size_t dims[2] = {1, 1};
        mat->iWave = wave;
        matvar[0] = Mat_VarCreate(fieldnames[0], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iWave, 0);
        fieldMap[0] = matvar[0] ? 1 : 0;
    }
}

static void setPulse(PMATFILE mat, int pulse)
{
    if (mat && mat->iIsSave && !fieldMap[1])
    {
        size_t dims[2] = {1, 1};
        mat->iPulse = pulse;
        matvar[1] = Mat_VarCreate(fieldnames[1], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iPulse, 0);
        fieldMap[1] = matvar[1] ? 1 : 0;
    }
}

static void setRange(PMATFILE mat, int range)
{
    if (mat && mat->iIsSave && !fieldMap[2])
    {
        size_t dims[2] = {1, 1};
        mat->iRange = range;
        matvar[2] = Mat_VarCreate(fieldnames[2], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iRange, 0);
        fieldMap[2] = 1;
    }
}

static void setBlind(PMATFILE mat, int blind)
{
    if (mat && mat->iIsSave && !fieldMap[3])
    {
        size_t dims[2] = {1, 1};
        mat->iBlind = blind;
        matvar[3] = Mat_VarCreate(fieldnames[3], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iBlind, 0);
        fieldMap[3] = 1;
    }
}

static void setSmpRation(PMATFILE mat, float smpRation)
{
    if (mat && mat->iIsSave && !fieldMap[4])
    {
        size_t dims[2] = {1, 1};
        mat->fSmpRation = smpRation;
        matvar[4] = Mat_VarCreate(fieldnames[4], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fSmpRation, 0);
        fieldMap[4] = 1;
    }
}

static void setPoint2Distance(PMATFILE mat, float smpInterval)
{
    if (mat && mat->iIsSave && !fieldMap[5])
    {
        size_t dims[2] = {1, 1};
        mat->fSmpInterval = smpInterval;
        matvar[5] = Mat_VarCreate(fieldnames[5], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fSmpInterval, 0);
        fieldMap[5] = 1;
    }
}

static void setRefractiveIndex(PMATFILE mat, float refractiveIndex)
{
    if (mat && mat->iIsSave && !fieldMap[6])
    {
        size_t dims[2] = {1, 1};
        mat->fRefractiveIndex = refractiveIndex;
        matvar[6] = Mat_VarCreate(fieldnames[6], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fRefractiveIndex, 0);
        fieldMap[6] = 1;
    }
}

static void setBackScatter(PMATFILE mat, float backScatter)
{
    if (mat && mat->iIsSave && !fieldMap[7])
    {
        size_t dims[2] = {1, 1};
        mat->fBackScatter = backScatter;
        matvar[7] = Mat_VarCreate(fieldnames[7], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fBackScatter, 0);
        fieldMap[7] = 1;
    }
}

static void setDisplayLen(PMATFILE mat, unsigned int displayLen)
{
    if (mat && mat->iIsSave && !fieldMap[8])
    {
        size_t dims[2] = {1, 1};
        mat->uiDisplayLen = displayLen;
        matvar[8] = Mat_VarCreate(fieldnames[8], MAT_C_UINT32, MAT_T_UINT32, 2, dims, &mat->uiDisplayLen, 0);
        fieldMap[8] = 1;
    }
}

static void setApd(PMATFILE mat, unsigned int apd)
{
    if (mat && mat->iIsSave && !fieldMap[9])
    {
        size_t dims[2] = {1, 1};
        mat->uiApd = apd;
        matvar[9] = Mat_VarCreate(fieldnames[9], MAT_C_UINT32, MAT_T_UINT32, 2, dims, &mat->uiApd, 0);
        fieldMap[9] = 1;
    }
}

static void setVdd(PMATFILE mat, unsigned int vdd)
{
    if (mat && mat->iIsSave && !fieldMap[10])
    {
        size_t dims[2] = {1, 1};
        mat->uiVdd = vdd;
        matvar[10] = Mat_VarCreate(fieldnames[10], MAT_C_UINT32, MAT_T_UINT32, 2, dims, &mat->uiVdd, 0);
        fieldMap[10] = 1;
    }
}

static void setChannel(PMATFILE mat, unsigned int channel)
{
    if (mat && mat->iIsSave && !fieldMap[11])
    {
        size_t dims[2] = {1, 1};
        mat->uiChannel = channel;
        matvar[11] = Mat_VarCreate(fieldnames[11], MAT_C_UINT32, MAT_T_UINT32, 2, dims, &mat->uiChannel, 0);
        fieldMap[11] = 1;
    }
}

static void setSpliceLossThr(PMATFILE mat, float spliceLossThr)
{
    if (mat && mat->iIsSave && !fieldMap[12])
    {
        size_t dims[2] = {1, 1};
        mat->fSpliceLossThr = spliceLossThr;
        matvar[12] = Mat_VarCreate(fieldnames[12], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fSpliceLossThr, 0);
        fieldMap[12] = 1;
    }
}

static void setReturnLossThr(PMATFILE mat, float returnLossThr)
{
    if (mat && mat->iIsSave && !fieldMap[13])
    {
        size_t dims[2] = {1, 1};
        mat->fReturnLossThr = returnLossThr;
        matvar[13] = Mat_VarCreate(fieldnames[13], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fReturnLossThr, 0);
        fieldMap[13] = 1;
    }
}

static void setEndLossThr(PMATFILE mat, float endLossThr)
{
    if (mat && mat->iIsSave && !fieldMap[14])
    {
        size_t dims[2] = {1, 1};
        mat->fEndLossThr = endLossThr;
        matvar[14] = Mat_VarCreate(fieldnames[14], MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, &mat->fEndLossThr, 0);
        fieldMap[14] = 1;
    }
}

static void setHighResolution(PMATFILE mat, int highResolution)
{
    if (mat && mat->iIsSave && !fieldMap[15])
    {
        size_t dims[2] = {1, 1};
        mat->iIsHighResolution = highResolution;
        matvar[15] = Mat_VarCreate(fieldnames[15], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iIsHighResolution, 0);
        fieldMap[15] = 1;
    }
}

static void setData(PMATFILE mat, unsigned short *data, int dataLen)
{
    if (mat && mat->iIsSave && !fieldMap[16])
    {
        memset(mat->data, 0, MAXPOINT);
		int i = 0;
		for(i = 0; i< dataLen; ++i)
		{
			mat->data[i] = data[i];
		}
        //memcpy(mat->data, data, dataLen*sizeof(unsigned int));
        size_t dims[2] = {MAXPOINT, 1};
        matvar[16] = Mat_VarCreate(fieldnames[16], MAT_C_UINT32, MAT_T_UINT32, 2, dims, mat->data, 0);
        fieldMap[16] = 1;
    }
}

static void setOffset(PMATFILE mat, int offset)
{
	if (mat && mat->iIsSave && !fieldMap[17])
	{
		size_t dims[2] = { 1, 1 };
		mat->iOffset = offset;
		matvar[17] = Mat_VarCreate(fieldnames[17], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iOffset, 0);
		fieldMap[17] = matvar[17] ? 1 : 0;
	}
}

static void setAtteDbClass(PMATFILE mat, int iAtteDbClass)
{
	if (mat && mat->iIsSave && !fieldMap[18])
	{
		size_t dims[2] = { 1, 1 };
		mat->iAtteDbClass = iAtteDbClass;
		matvar[18] = Mat_VarCreate(fieldnames[18], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iAtteDbClass, 0);
		fieldMap[18] = matvar[18] ? 1 : 0;
	}
}

static void setTestType(PMATFILE mat, MAT_STRUCT_TYPE iTestType)
{
	if (mat && mat->iIsSave && !fieldMap[19])
	{
		size_t dims[2] = {1, 1};
		mat->iTestType = iTestType;
		matvar[19] = Mat_VarCreate(fieldnames[19], MAT_C_INT32, MAT_T_INT32, 2, dims, &mat->iTestType, 0);
		fieldMap[19] = matvar[19] ? 1 : 0;
	}
}

PMATFILE GetMatFile()
{
	if (!matinit)
	{
        g_matfile.closeMatFile = closeMatFile;
		g_matfile.addMatStruct = addMatStruct;
		g_matfile.addMatFile = addMatFile;
        g_matfile.setSave = setSave;
		g_matfile.setWave = setWave;
		g_matfile.setPulse = setPulse;
		g_matfile.setRange = setRange;
		g_matfile.setBlind = setBlind;
		g_matfile.setSmpRation = setSmpRation;
		g_matfile.setPoint2Distance = setPoint2Distance;
		g_matfile.setRefractiveIndex = setRefractiveIndex;
		g_matfile.setBackScatter = setBackScatter;
		g_matfile.setDisplayLen = setDisplayLen;
		g_matfile.setApd = setApd;
		g_matfile.setVdd = setVdd;
		g_matfile.setChannel = setChannel;
		g_matfile.setSpliceLossThr = setSpliceLossThr;
		g_matfile.setReturnLossThr = setReturnLossThr;		
		g_matfile.setEndLossThr = setEndLossThr;		
		g_matfile.setHighResolution = setHighResolution;		
		g_matfile.setData = setData;

		g_matfile.setOffset = setOffset;
		g_matfile.setAtteDbClass = setAtteDbClass;
		g_matfile.setTestType = setTestType;

        g_matfile.iIsSave = 1;
        matinit = 1;
	}

	return &g_matfile;
}



