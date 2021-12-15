/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：app_filesor.c  
* 摘    要：定义对sor文件各个数据块的读写操作
*
* 当前版本：v1.1.0 
* 作    者：
* 完成日期：

* 取代版本： v1.0.0 
* 原 作 者： 
* 完成日期： 
*******************************************************************************/

#include "app_filesor.h"
#include "app_global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>

//定义各块的字符串标识
#define MAP_ID "Map"
#define GEN_ID "GenParams"
#define SUP_ID "SupParams"
#define FXD_ID "FxdParams"
#define ENT_ID "KeyEvents"
#define LNK_ID "LnkParams"
#define DAT_ID "DataPts"
#define SPC_ID "DH"
#define CHK_ID "Cksum"

#define VERSION_SOR	200

#define BUF_SIZE 1024

/*******************************************************************************
***                 为实现app_frmotdr而定义的静态全局变量                    ***
*******************************************************************************/
//定义SOR文件中各块的结构体指针
static SORMAP *pSorMap = NULL;
static SORGEN *pSorGen = NULL;
static SORSUP *pSorSup = NULL;
static SORFXD *pSorFxd = NULL;
static SOREVT *pSorEvent = NULL;
static SORLNK *pSorLnk = NULL;
static SORDAT *pSorData = NULL;
static SORSPC *pSorSpec = NULL;
static SORCHK *pSorCheck = NULL;


/*******************************************************************************
***                     为实现app_frmotdr而定义内部函数                      ***
*******************************************************************************/


//把map块的数据读写到新文件中去
static int WriteMap(int fd, SORMAP *map);

//把datapoint块的数据读写到新文件中去    
static int WriteDatapoint(int fd, SORMAP *map, SORDAT *datapoint);

//把key块的数据读写到新文件中去    
static int WriteKeyevent(int fd, SORMAP *map, SOREVT *keyevent);

//把fixed块的数据读写到新文件中去
static int WriteFixed(int fd, SORMAP *map, SORFXD *fixed);

//把supplier块的数据读写到新文件中去    
static int WriteSupplier(int fd, SORMAP *map, SORSUP *supplier);

//把general块的数据读写到新文件中去 
static int WriteGeneral(int fd, SORMAP *map, SORGEN *general);

//把special块的数据读写到新文件中去 
int WriteSpecial(int fd, SORMAP *map, SORSPC *special);

//把checksum块的数据读写到新文件中去 
int WriteChecksum(int fd, SORMAP *map, SORCHK *checksum);


//把文件中map块的数据读到map结构中去  
static SORMAP * ReadMapBlock(int fd, SORMAP *map);

//把文件中checksum块的数据读到checksum结构中去 
static SORCHK *ReadChecksum(int fd, SORMAP *map, SORCHK *checksum);

//把文件中special块的数据读到special结构中去    
static SORSPC *ReadSpecial(int fd, SORMAP *map, SORSPC *special);

//把文件中datapoint块的数据读到datapoint结构中去  
static SORDAT * ReadDatapoints(int fd, SORMAP *map, SORDAT    *datapoint);

//把文件中link块的数据读到link结构中去
static SORLNK * ReadLink(int fd, SORMAP *map, SORLNK *link);

//把文件中keyevents块的数据读到keyevents结构中去  
static SOREVT * ReadKeyevent(int fd, SORMAP *map, SOREVT* keyevent);

//把文件中fixed块的数据读到fixed结构中去    
static SORFXD * ReadFixed(int fd, SORMAP *map, SORFXD *fixed);

//把文件中supplier块的数据读到supplier结构中去
static SORSUP * ReadSupplier(int fd, SORMAP *map, SORSUP *supplier);

//把文件中general块的数据读到general结构中去   
static SORGEN * ReadGeneral(int fd, SORMAP *map, SORGEN *general);

// char类型转int类型(针对EXFO)
static int char_to_int(char *p);

// char类型转int类型(大豪)
static int char_to_int_self(char *p);

//从buf中拷贝字符串
static int GetString(char **dest, char **buf);

//开辟对应大小的数据缓冲区空间,把文件中对应的数据读到buf中去    
static char *ReadGetBlockSize(int fd, SORMAP *map, char *str);

//用于写操作时把数据块中的字符串拷贝到缓冲区buf中，并改变buf指针的位置
static int  GetStringTobuf(char *str, char **buf);

//开始写数据的准备工作:定位文件指针,找到对应数据块的信息   
static long WriteGetBlockSize(int fd, SORMAP *map, char *str);

//修改每个比例因子下的各个数据点的值
static SORDAT *ChangeDataValue(SORDAT *datapoint);

//保存测试数据到SOR文件之前的初始化工作
static int InitWriteToSor(PSOR_DATA pSor);

//操作SOR文件之后的结束工作，资源释放
static void ReleaseMemSor();

//保存测试数据到SOR文件之前的初始化工作
static int InitReadFromSor();

//把从SOR文件读取到的数据，设置到数据结构中
static int SorData2pSor(char *FileName, PSOR_DATA pSor);

//crc16位冗余校验
static unsigned short crc16(unsigned char *buf,int len);

static long GetMaxUint16_index(unsigned short *pSignal, long iSignalLength); 

//将传入的值按三位小数精度四舍五入截断后返回
static double Float2FloatBy3Precision(double data);

static long GetMaxUint16_index(unsigned short *pSignal, long iSignalLength)
{
	unsigned short fMaxNumber = 0;
	int i = 0;
	int index=0;

	for (i=0; i<iSignalLength; i++)
	{
		if (pSignal[i]>fMaxNumber)
		{
			fMaxNumber = pSignal[i];
			index=i;
		}
	}
	return (long)index;
}
/***
  * 功能：
  		查看从文件中读取到数据块ID是否支持
  * 参数：
  		1、char *string:	从文件中读取到数据块的ID
  * 返回：
        成功返回1，失败返回零.
 ***/
static int IsSupportBlockID(char* string)
{
    int iReturn = 0; 
    
    if (string == NULL)
        return 0;

    const char* str[9] = {MAP_ID, GEN_ID, SUP_ID, FXD_ID, 
                         ENT_ID, LNK_ID, DAT_ID, SPC_ID, CHK_ID
                        };
    int i;
    for (i = 0; i < 9; ++i)
    {
        if (strcmp(str[i], string) == 0)
        {
            iReturn = 1;
            break;
        }
    }
    return iReturn;
}

/***
 * 功能：
        把文件中map块的数据读到map结构中去    
 * 参数：
        1.int fd:           打开的文件的文件描述符
        2.SORMAP *map:      用来接收数据的结构体指针
 * 返回：
         成功返回Map块结构体指针，失败返回NULL
 * 备注：
 ***/
static SORMAP * ReadMapBlock(int fd, SORMAP *map)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    int iErr = 0;    //错误标志
    int len = 0;    //字符串长度
    int iRetValue = 0;


    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //分配buf内存空间
        buf = (char *)calloc(1,BUF_SIZE);
        if (NULL == buf)
        {
            //申请内存失败
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        ptmp = buf;    //保存缓冲区指针
    }

    if (0 == iErr)
    {
        //读取文件数据到缓冲区中
        iRetValue = read(fd, buf, BUF_SIZE);
        if (-1 == iRetValue)
        {
            //map块读取失败
            iErr = -4;
        }
    }

    //从缓冲区中获取对应结构的数据
    if (0 == iErr)
    {
        //读取map块id
        len = GetString(&map->strMapId, &buf);    
        if (len < 0)
        {
            //读取map id 失败
            iErr = -5;
        }
    }

    if (0 == iErr)
    {
        buf += len;    //缓冲区指针向后移动字符串长度
    }

    if (0 == iErr)
    {
        //读取 map 版本号    
        memcpy(&map->usMapRevisionNu, buf , 2);
        buf += 2 ;
    }

    if (0 == iErr)
    {
        //读取 map 块大小
        memcpy(&map->lMapSize, buf, 4);
        buf += 4;    
    }

    if (0 == iErr)
    {
        //读取文件中数据块的数量
        memcpy(&map->sNuBlock, buf, 2);
        buf += 2;
    }
	
    if (0 == iErr)
    {
    	int i;
        int j = 0;
        char* temp = NULL;//(char *)calloc(1, 256);
        //读取map块中其他块信息    
        for (i = 0; i < map->sNuBlock-1; i++)
        {
            len = GetString(&temp, &buf);
            
            if (len < 0)
            {
                //ID 获取失败,释放对应内存空间
                iErr = -6;
                break;
            }

            buf += len;
            
            if (IsSupportBlockID(temp))
            {
                //读取文件其他数据块的ID
                GetString(&map->pBlockInfo[j].strBlockId, &temp);
                //读取块版本号
                memcpy(&map->pBlockInfo[j].usRevisionNum, buf, 2);
                buf += 2;
                //读取块大小
                memcpy(&map->pBlockInfo[j].lBlockSize, buf, 4);
                j++;
            }
            else
            {
                buf += 2;
            }
            buf += 4;
            free(temp);
            temp = NULL;
        }
        map->sNuBlock = j;
    }

    //错误处理
    switch (iErr)
    {
        case -6:
        case -5:
            free(map->strMapId);
			map->strMapId = NULL;
        case -4:
            free(ptmp);
			ptmp = NULL;
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            return NULL;
        default:
            break;
    }

	if(ptmp)
	{
		free(ptmp);
		ptmp = NULL;
	}

    return map;
}


/***
  * 功能：
        把文件中general块的数据读到general结构中去    
  * 参数：
        1.int fd:                打开的文件的文件描述符
        2.SORMAP *map:            包含general块信息的map结构体指针
        3.SORGEN *general:        用来接收general块信息的结构体指针    
  * 返回：
        成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SORGEN * ReadGeneral(int fd, SORMAP *map, SORGEN *general)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    int iErr = 0;    //错误标志
    int len = 0; 

    //用来匹配数据块的 数据块ID
    char *str = GEN_ID;

    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == general || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        ptmp = buf;    //保存缓冲区指针
    }

    //将文件general块数据读到general数据快中
    if (0 == iErr)
    {
        //获取general 块Id
        len = GetString(&general->strBlockId, &buf); 
        if (len < 0)
        {
            iErr = -4;
        }
        //缓冲区指针向后移动字符串长度
        buf += len;
    }
    
    if (0 == iErr)
    {
        //获取语言码
        memcpy(general->pLanguageCode, buf, 2); 
        buf += 2;
    }

    if (0 == iErr)
    {
        //获取电缆ID
        len = GetString(&general->strCableId, &buf); 
        if ( len < 0)
        {
            iErr = -5;    
        }
        
        //缓冲区指针向后移动字符串长度
        buf += len;
    }

    if (0 == iErr)
    {
        //获取光纤ID
        len = GetString(&general->strFiberId, &buf);
        if ( len < 0)
        {
            iErr = -6;    
        }

        //缓冲区指针向后移动字符串长度
        buf += len;
    }

    if (0 == iErr)
    {
        //读取光纤类型
        memcpy(&general->sFiberType, buf, 2); 
        buf += 2;
        
        //读取OTDR波长
        memcpy(&general->sWaveLength, buf, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        //获取测量起始位置信息
        len = GetString(&general->strOriginatingLoca, &buf);
        if ( len < 0)
        {
            iErr = -7;    
        }

        //缓冲区指针向后移动字符串长度
        buf += len;
    }

    if (0 == iErr)
    {
        //获取测量结束位置信息
        len = GetString(&general->strTerminatingLoca, &buf);
        if ( len < 0)
        {
            iErr = -8;    
        }

        //缓冲区指针向后移动字符串长度
        buf += len;
    }

    if (0 == iErr)
    {
        //读取电缆编码
        len = GetString(&general->strCableCode , &buf);
        if ( len < 0)
        {
            iErr = -9;    
        }

        //缓冲区指针向后移动字符串长度
        buf += len;
    }
    
    if (0 == iErr)
    {
        //读取光纤测量状态
        memcpy(general->pDataFlag, buf, 2);
        buf += 2;
        
        //读取测量物理距离（时间）
        memcpy(&general->lUserOffset, buf, 4);
        buf += 4;
        
        //读取测量物理距离
        memcpy(&general->lUserDist, buf, 4);
        buf += 4;
    }
    
    if (0 == iErr)
    {
        //获取OTDR 操作标识
        len = GetString(&general->strOperator, &buf);
        if ( len < 0)
        {
            iErr = -10;    
        }

        //缓冲区指针向后移动字符串长度
        buf += len;
    }
    
    if (0 == iErr)
    {
        len = GetString(&general->strComment, &buf);
        if ( len < 0)
        {
            iErr = -11;    
        }
    }

    //错误处理
    switch (iErr)
    {
        case -11:
            free(general->strOperator);        
            general->strOperator = NULL;        
        case -10:
            free(general->strCableCode);
            general->strCableCode = NULL;
        case -9:
            free(general->strTerminatingLoca);
            general->strTerminatingLoca = NULL;
        case -8:
            free(general->strOriginatingLoca);
            general->strOriginatingLoca = NULL;
        case -7:
            free(general->strFiberId);
            general->strFiberId = NULL;
        case -6:
            free(general->strCableId); 
            general->strCableId = NULL; 
        case -5:
            free(general->strBlockId); 
            general->strBlockId = NULL; 
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default:
            break;
            
    }

	if(ptmp)
	{
		free(ptmp);    //释放buf空间
		ptmp = NULL;
	}
    
    return general;
}


/***
  * 功能：
        把文件中supplier块的数据读到supplier结构中去    
  * 参数：
        1.int fd:             打开的文件的文件描述符
        2.SORMAP *map:        包含supplier块信息的map结构体指针
        3.SORSUP *supplier:    用来接收supplier块信息的结构体指针    
  * 返回：
        成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SORSUP * ReadSupplier(int fd, SORMAP *map, SORSUP *supplier)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    int iErr = 0;
    int len = 0;

    //用来匹配对应数据块的 数据快ID
    char *str = SUP_ID;

    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == supplier || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }

        ptmp = buf;    //保存缓冲区指针
    }

    //把文件中supplier块数据读到supplier数据快中
    if (0 == iErr)
    {
        //获取供应商ID
        len = GetString(&supplier->strBlockId, &buf);
        if (len < 0)
        {
            iErr = -4;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //获取供应商名
        len = GetString(&supplier->strSupplierName, &buf);
        if (len < 0)
        {
            iErr = -5;
        }
        buf += len;
    }
    
    if (0 == iErr)
    {
        //获取OTDR主机 ID
        len = GetString(&supplier->strMainframeId, &buf);
        if (len < 0)
        {
            iErr = -6;
        }
        buf += len;
    }
    
    if (0 == iErr)
    {
        //获取OTDR主机序列号
        len = GetString(&supplier->strMainframeSN, &buf);
        if (len < 0)
        {
            iErr = -7;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //获取OTDR插件模块ID
        len = GetString(&supplier->strOpticalId, &buf);
        if (len < 0)
        {
            iErr = -8;
        }
        buf += len;
    }
    
    if (0 == iErr)
    {
        //获取OTDR插件模块序列号
        len = GetString(&supplier->strOpticalSN, &buf);
        if (len < 0)
        {
            iErr = -9;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //获取软件版本号
        len = GetString(&supplier->strSoftRevision, &buf);
        if (len < 0)
        {
            iErr = -10;
        }
        buf += len;
    }
    
    if (0 == iErr)
    {
        //获取供应商其他信息
        len = GetString(&supplier->strOther, &buf);
        if (len < 0)
        {
            iErr = -11;
        }
        buf += len;
    }

    //错误处理
    switch (iErr)
    {
        case -11:
            free(supplier->strSoftRevision);
            supplier->strSoftRevision = NULL;
        case -10:
            free(supplier->strOpticalSN);
            supplier->strOpticalSN = NULL;
        case -9:
            free(supplier->strOpticalId);
            supplier->strOpticalId = NULL;
        case -8:
            free(supplier->strMainframeSN);
            supplier->strMainframeSN = NULL;
        case -7:
            free(supplier->strMainframeId);
            supplier->strMainframeId = NULL;
        case -6:
            free(supplier->strSupplierName);
            supplier->strSupplierName = NULL;
        case -5:
            free(supplier->strBlockId);
            supplier->strBlockId = NULL;
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default :
            break;
    }

	if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	}  

    return supplier;
}


/***
  * 功能：
        把文件中fixed块的数据读到fixed结构中去    
  * 参数：
        1.int fd:            打开的文件的文件描述符
        2.SORMAP *map:        包含fixed块信息的map结构体指针
        3.SORFXD *fixed:      用来接收fixed块信息的结构体指针    
  * 返回：
         成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SORFXD * ReadFixed(int fd, SORMAP *map, SORFXD *fixed)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    int iErr = 0;    //错误标志

    //用来匹配对应数据块的 数据快ID
    char *str = FXD_ID;

    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == fixed || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }
        ptmp = buf;    //保存缓冲区指针
    }

    
    //把文件中fixed块数据读到fixed块中
    if (0 == iErr)
    {
    	int len = 0;
        //获取fixed 块ID
        len = GetString(&fixed->strBlockId, &buf);    
        if (len < 0)
        {
            iErr = -4;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //读取光纤测量的时间戳
        memcpy(&fixed->ulDTS, buf, 4);
        buf += 4;
        //读取测量的距离单位
        memcpy(&fixed->pDistUints, buf, 2);
        buf += 2;
        //读取实际波长
        memcpy(&fixed->sWaveLength, buf, 2);
        buf += 2;
        //读取激光器到第一个数据的时间
        memcpy(&fixed->lAcquisitionOffset, buf, 4);
        buf += 4;
        //读取激光器到第一个数据的距离
        memcpy(&fixed->lOffsetDist, buf, 4);
        buf += 4;
        //读取是用的脉冲宽度总数
        memcpy(&fixed->sPulesTotal, buf ,2);
        buf += 2;
    }

    if (0 == iErr)
    {
        //分配脉冲宽度内存空间
        fixed->pPulseWidth = (short *)calloc(1,2 * fixed->sPulesTotal);
        if (NULL == fixed->pPulseWidth)
        {
            //内存分分配失败
            iErr = -5;
        }
    }

    if (0 == iErr)
    {
        //读取各个脉冲宽度的值
        memcpy(fixed->pPulseWidth, buf, (2 * fixed->sPulesTotal));
        buf += (2 * fixed->sPulesTotal);
    }

    if (0 == iErr)
    {
        //分配数据空间指针内存空间
        fixed->pDataSpacing = (long *)calloc(1,4 * fixed->sPulesTotal);
        if (NULL == fixed->pDataSpacing)
        {
            //内存分分配失败
            iErr = -6;
        }
    }

    if (0 == iErr)
    {
        //读取数据间距
        memcpy(fixed->pDataSpacing, buf, (4 * fixed->sPulesTotal));
        buf += (4 * fixed->sPulesTotal);
    }

    if (0 == iErr)
    {
        //分配数据点量内存空间
        fixed->pPointsNumber = (long *)calloc(1,4 * fixed->sPulesTotal);
        if (NULL == fixed->pPointsNumber)
        {
            //内存分分配失败
            iErr = -7;
        }
    }

    if (0 == iErr)
    {
        //读取每个脉冲宽度所产生的数据量
        memcpy(fixed->pPointsNumber, buf, (4 * fixed->sPulesTotal));
        buf += (4 * fixed->sPulesTotal);
        //读取群折射律
        memcpy(&fixed->lGroupIndex, buf ,4);
        buf += 4;
        //读取反向散射系数
        memcpy(&fixed->sBackscatterCode, buf ,2);
        buf += 2;
        //读取平均次数
        memcpy(&fixed->lAveragesNumber, buf, 4);
        buf += 4;
        //读取平均时间
        memcpy(&fixed->usAverageTime, buf, 2);
        buf += 2;
        //读取OTDR测量单程总长度（时间）
        memcpy(&fixed->lAcquisitionRange, buf, 4);
        buf += 4;
        //读取OTDR测量单程总长度（距离）
        memcpy(&fixed->lAcquisitionDist, buf, 4);
        buf += 4;
        //读取前面板偏移
        memcpy(&fixed->lFrontoffset, buf, 4);
        buf += 4;
        //读取噪声基值
        memcpy(&fixed->usNoiseFloor, buf, 2);
        buf += 2;
        //读取噪声基值比例因子
        memcpy(&fixed->sNoiseScale, buf, 2);
        buf += 2;
        //读取衰减值
        memcpy(&fixed->usPowerOffset, buf, 2);
        buf += 2;
        //读取损耗阈值
        memcpy(&fixed->usLossThreshold, buf, 2);
        buf += 2;
        //读取反射阈值
        memcpy(&fixed->usRefThreshold, buf, 2);
        buf += 2;
        //读取末端阈值
        memcpy(&fixed->usEndThreshold, buf, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        //读取波形类型
        memcpy(fixed->strTraceType, buf, 2); 
        buf += 2;
    }

    //读取窗口坐标
    if (0 == iErr)
    {
        memcpy(&fixed->WndCoordinate.lTopLeftX, buf, 4);
        buf += 4;
        memcpy(&fixed->WndCoordinate.lTopLeftY, buf, 4);
        buf += 4;
        memcpy(&fixed->WndCoordinate.lBotRightX, buf, 4);
        buf += 4;
        memcpy(&fixed->WndCoordinate.lBotRightY, buf, 4);
    }
    
    //错误处理
    switch (iErr)
    {
        case -8:
            free(fixed->pPointsNumber);
            fixed->pPointsNumber = NULL;
        case -7:
            free(fixed->pDataSpacing);
            fixed->pDataSpacing = NULL;
        case -6:
            free(fixed->pPulseWidth);
            fixed->pPulseWidth = NULL;
        case -5:
            free(fixed->strBlockId);
            fixed->strBlockId  = NULL;
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default :
            break;
    }

    if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	} 

    return fixed;
}


/***
  * 功能：
        把文件中keyevents块的数据读到keyevents结构中去    
  * 参数：
        1.int fd:                打开的文件的文件描述符
        2.SORMAP *map:            包含keyevents块信息的map结构体指针
        3.SOREVT *keyevent:        用来接收keyevent块信息的结构体指针
  * 返回：
        成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SOREVT * ReadKeyevent(int fd, SORMAP *map, SOREVT* keyevent)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    int i;
    int len = 0;
    int iErr = 0;    //错误标志

    //用来保存事件结构体指针的临时指针变量
    SOREVT_EVENT *ptr = NULL;    
    
    //用来匹配对应数据块的 数据快ID
    char *str = ENT_ID;
    
    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == keyevent || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }
        ptmp = buf;    //保存缓冲区指针
    }

    //把文件中keyevent块数据读到keyevent中
    if (0 == iErr)
    {
        //获取事件数据块ID
        len = GetString(&keyevent->strBlockId, &buf);
        if (len < 0)
        {
            iErr = -4;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //读取关键事件的数量
        memcpy(&keyevent->sEventTotal, buf, 2);
        buf += 2;
    }    

    if (0 == iErr)
    {
        //开辟事件结构体空间
        keyevent->pEventInfo = (SOREVT_EVENT *)calloc(1,sizeof(SOREVT_EVENT)*keyevent->sEventTotal);
        if (NULL == keyevent->pEventInfo)
        {
            iErr = -5;
        }
        
        //保存事件信息结构体指针
        ptr = keyevent->pEventInfo;
    }

	if (0 == iErr)
    {
	    //读取每个事件的信息
	    for(i = keyevent->sEventTotal; i>0; i--) 
	    {
	        //读取事件编号
	        memcpy(&keyevent->pEventInfo->sEventNum, buf, 2);
	        buf += 2;
	        //度链路起点到事件的时间
	        memcpy(&keyevent->pEventInfo->lPropagationTime, buf, 4);
	        buf += 4;
	        //读取衰减系数
	        memcpy(&keyevent->pEventInfo->sAttenuationCoef, buf, 2);
	        buf += 2;
	        //读取事件损耗
	        memcpy(&keyevent->pEventInfo->sEventLoss, buf, 2);
	        buf += 2;
	        //读取事件反射
	        memcpy(&keyevent->pEventInfo->lEventReflectance, buf, 4);
	        buf += 4;
	        //读取事件描述
	        memcpy(&keyevent->pEventInfo->pEventCode, buf, 6);
	        buf += 6;
	        //读取损耗测量方法
	        memcpy(&keyevent->pEventInfo->pLossMeasurement, buf, 2);
	        buf += 2;
	        //读取事件标记
	        memcpy(&keyevent->pEventInfo->lMarkLoca1, buf, 4);
	        buf += 4;
	        memcpy(&keyevent->pEventInfo->lMarkLoca2, buf, 4);
	        buf += 4;
	        memcpy(&keyevent->pEventInfo->lMarkLoca3, buf, 4);
	        buf += 4;
	        memcpy(&keyevent->pEventInfo->lMarkLoca4, buf, 4);
	        buf += 4;
	        memcpy(&keyevent->pEventInfo->lMarkLoca5, buf, 4);
	        buf += 4;

	        len = GetString(&keyevent->pEventInfo->strComment, &buf);
	        
	        if (len < 0)
	        {
	            iErr = -6;
	            //错误处理
	            while(i <= keyevent->sEventTotal )
	            {
	                free(keyevent->pEventInfo->strComment);
	                keyevent->pEventInfo->strComment = NULL;

	                keyevent->pEventInfo--;
	                i++;
	            }
	            break;
	        }

	        buf += len;

	        keyevent->pEventInfo++;    //读取下一个事件信息
	    }
	}
	
    if (0 == iErr)
    {
        keyevent->pEventInfo = ptr;    //恢复事件结构体指针
    }

    if (0 == iErr)
    {
        //读取链路端到端损耗
        memcpy(&keyevent->lEndtoendLoss, buf, 4);
        buf += 4;
    }

    //读取端到端的标记位置
    if (0 == iErr)
    {
        //起始标记位置
        memcpy(&keyevent->EndtoendMarker.lStartPosition, buf, 4);
        buf += 4;
        //结束标记位置
        memcpy(&keyevent->EndtoendMarker.lFinishPosition, buf, 4);
        buf += 4;
    }

    if (0 == iErr)
    {
        //读取回波损耗
        memcpy(&keyevent->usReturnLoss, buf, 2);
        buf += 2;
    }
    
    //读取回波损耗的标记位置
    if (0 == iErr)
    {
        //开始标记位置
        memcpy(&keyevent->ReturnMarker.lStartPosition, buf, 4);
        buf += 4;
        //结束标记位置
        memcpy(&keyevent->ReturnMarker.lFinishPosition, buf, 4);
    }

    //错误处理
    switch (iErr)
    {
        case -6:
            free(keyevent->pEventInfo);
            keyevent->pEventInfo = NULL;
        case -5:
            free(keyevent->strBlockId);
            keyevent->strBlockId = NULL;
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default:
            break;
    }

    if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	} 
	
    return keyevent;
}



/***
  * 功能：
        把文件中link块的数据读到link结构中去    
  * 参数：
        1.int fd:            打开的文件的文件描述符
        2.SORMAP *map:        包含link块信息的map结构体指针
        3.SORLNK *link:        用来接收link块信息的结构体指针
  * 返回：
        成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SORLNK * ReadLink(int fd, SORMAP *map, SORLNK *link)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    SORLNK_LANDMARK *ptr;    //用于保存地标信息结构体指针的临时变量
    int iErr = 0;
    int len = 0;

    //用来匹配link块的数据块ID
    char *str = LNK_ID;
    
    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == link || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }
        ptmp = buf;    //保存缓冲区指针
    }

    //把文件中link块数据读到link块中
    if (0 == iErr)
    {
        //读取link块ID
        len = GetString(&link->strBlockId, &buf);
        if (len < 0)
        {
            //ID读取失败
            iErr = -4;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //读取地标总数量
        memcpy(&link->sLandmarksTotal, buf, 2);
        buf += 2;
    }    

    if (0 == iErr)
    {
        //开辟地标信息内存空间
        link->pLandmarkInfo=(SORLNK_LANDMARK *)calloc(1,sizeof(SORLNK_LANDMARK)*link->sLandmarksTotal);
        if (NULL == link->pLandmarkInfo)
        {
            //内存分配失败
            iErr = -5;
        }
    }
    
    if (0 == iErr)
    {
        memset(link->pLandmarkInfo, 0, sizeof(SORLNK_LANDMARK)*link->sLandmarksTotal);

        ptr = link->pLandmarkInfo;    //保存地标结构体指针
    }

    if (0 == iErr)
    {
    	int i;
        //读取链路地标信息
        for(i = link->sLandmarksTotal; i > 0; i--)
        {
            //读取地标编号
            memcpy(&link->pLandmarkInfo->sLandmarkNum, buf, 2);
            buf += 2;
            //读取地标类型
            memcpy(&link->pLandmarkInfo->pLandmarkCode, buf, 2);
            buf += 2;
            //读取链路开始到该地标的光单程距离
            memcpy(&link->pLandmarkInfo->lLandmarkLoca, buf, 4);
            buf += 4;
            //读取与该地标相关的事件编号
            memcpy(&link->pLandmarkInfo->sEventNum, buf, 2);
            buf += 2;
            //读取GPS经度
            memcpy(&link->pLandmarkInfo->lGpsLongitude, buf, 4);
            buf += 4;
            //读取GPS纬度
            memcpy(&link->pLandmarkInfo->lGpsLatitude, buf, 4);
            buf += 4;
            //读取光路长度于电缆包层长度的百分比
            memcpy(&link->pLandmarkInfo->sFCI, buf, 2);
            buf += 2;
            //读取 地标设定前的电缆包层的编号
            memcpy(&link->pLandmarkInfo->lSMI, buf, 4);
            buf += 4;
            //读取 地标设定后的电缆包层的编号
            memcpy(&link->pLandmarkInfo->lSML, buf, 4);
            buf += 4;
            //读取电缆包层标记的距离单位
            memcpy(&link->pLandmarkInfo->pDistUints, buf ,2);
            buf += 2;
            //读取光纤的模场直径 
            memcpy(&link->pLandmarkInfo->sMFD, buf, 2);
            buf += 2;

            len = GetString(&link->pLandmarkInfo->strComment, &buf);
            if (len < 0)
            {    
                iErr = -6;        
                //错误处理
                while (i <= link->sLandmarksTotal)
                {
                    free(link->pLandmarkInfo->strComment);
                    link->pLandmarkInfo->strComment = NULL;

                    link->pLandmarkInfo++; 
                    i++;
                }
                break;
            }
            //缓冲区指针向后移动字符串的长度
            buf += len;

            link->pLandmarkInfo++; //读取下一个地标信息
        }    
    }

    if (0 == iErr)
    {
        link->pLandmarkInfo = ptr;    //恢复地标信息结构体指针
    }

    //错误处理
    switch (iErr)
    {
        case -6:
            free(link->pLandmarkInfo);
            link->pLandmarkInfo = NULL;
        case -5:
            free(link->strBlockId);
            link->strBlockId = NULL;
        case -4:
            free(ptmp);
            ptmp = NULL;    
        case -3:
        case -2:
        case -1:
            return NULL;
        default :
            break;
    }

    if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	} 

    return link;
}


/***
  * 功能：
        把文件中datapoint块的数据读到datapoint结构中去    
  * 参数：
        1.int fd:                打开的文件的文件描述符
        2.SORMAP *map:              包含datapoint块信息的map结构体指针
        3.SORDAT *datapoint:        用来接收datapoint块信息的结构体指针
  * 返回：
        成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SORDAT * ReadDatapoints(int fd, SORMAP *map, SORDAT    *datapoint)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
    int iErr = 0; //错误标志

    SORDAT_SCALEFACTOR *ptr; //保存比例因子信息结构体指针的临时变量
    char *str = DAT_ID;    //用来匹配datapoint块的数据快ID

    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == datapoint || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }

        ptmp = buf;    //保存缓冲区指针
    }

    //把文件中datapoint中的数据读到datapoint中去
    if (0 == iErr)
    {
    	int len;
        //读取datapoint数据块ID
        len = GetString(&datapoint->strBlockId, &buf);
        if (len < 0)
        {
            //ID读取失败
            iErr = -4;
        }
        
        buf += len;
    }

    if (0 == iErr)
    {
        //读取文件中数据点的总数
        memcpy(&datapoint->lPointsTotal, buf, 4);
        buf += 4;
        
        //读取使用的比列因子的总数
        memcpy(&datapoint->sFactorsTotal, buf, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        //开辟比例因子信息内存空间
        datapoint->pFactorInfo = (SORDAT_SCALEFACTOR *)calloc(1, sizeof(SORDAT_SCALEFACTOR)*(datapoint->sFactorsTotal));
        if (NULL == datapoint->pFactorInfo)
        {
            //内存分配失败
            iErr = -5;
        }

        ptr = datapoint->pFactorInfo;    //保存比例因子结构体指针
    }

    if (0 == iErr)
    {
    	int i;
        //读取每个比例因子下的数据
        for (i = datapoint->sFactorsTotal; i>0; i--)
        { 
            //读取该比例因子下的数据点的总数
            memcpy(&datapoint->pFactorInfo->lPointsTotal, buf, 4);
            buf += 4;
            //读取该比例因子的值
            memcpy(&datapoint->pFactorInfo->sScaleFactor, buf, 2);
            buf += 2;
            //开辟每个比例因子下的数据点的内存空间
            datapoint->pFactorInfo->pDataPoints=(unsigned short *)calloc(1,2 * datapoint->pFactorInfo->lPointsTotal);
            if (NULL == datapoint->pFactorInfo->pDataPoints)
            {
                //内存分配失败
                iErr = -6;
                
                //错误处理
                while (i <= datapoint->sFactorsTotal)
                {
                    free(datapoint->pFactorInfo->pDataPoints);
                    datapoint->pFactorInfo->pDataPoints = NULL;

                    datapoint->pFactorInfo--;    
                    i++;
                }
                break;
            }

            //获取数据信息
            memcpy(datapoint->pFactorInfo->pDataPoints, buf, 2*datapoint->pFactorInfo->lPointsTotal);    
            buf = buf + datapoint->pFactorInfo->lPointsTotal;
            //读取下一个比例因子的信息

            datapoint->pFactorInfo++;    
        }
    }

    if (0 == iErr)
    {
        datapoint->pFactorInfo = ptr;    //恢复比例因子结构体指针
    }

    //错误处理
    switch (iErr)
    {
        case -6:
            free(datapoint->pFactorInfo);
            datapoint->pFactorInfo = NULL;
        case -5:
            free(datapoint->strBlockId);
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default :
            break;
    }

    if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	} 

    return datapoint;
}


/***
  * 功能：
        把文件中special块的数据读到special结构中去    
  * 参数：
        1.int fd:           打开的文件的文件描述符
        2.SORMAP *map:      包含special块信息的map结构体指针
        3.SORSPC *special:  用来接收special块信息的结构体指针
  * 返回：
        成功返回结构体指针，失败返回NULL
  * 备注：
 ***/
static SORSPC *ReadSpecial(int fd, SORMAP *map, SORSPC *special)
{
    //临时变量
    char *buf = NULL;    
    char *ptmp = NULL;
    int len = 0;
    int iErr = 0;    //错误标志
    
    //用来匹配special块的数据块ID
    char *str = SPC_ID;

    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == special || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }
        ptmp = buf;    //保存缓冲区指针
    }

    if (0 == iErr)
    {
        //获取special ID
        len = GetString(&special->strBlockId, &buf);
        if (len < 0)
        {
            //ID 获取失败
            iErr =-4;
        }

        buf += len;
    }

	 if (0 == iErr)
    {
        //获取special ID
        len = GetString(&special->strInfo, &buf);
        if (len < 0)
        {
            //ID 获取失败
            iErr =-4;
        }
    }
    //错误处理
    switch (iErr)
    {
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default :
            break;
    }

    if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	} 

    return special;
}


/***
 * 功能：
        把文件中checksum块的数据读到checksum结构中去    
 * 参数：
        1.int fd:                打开的文件的文件描述符
        2.SORMAP *map:            包含checksum块信息的map结构体指针
        3.SORCHK *checksum:        用来接收checksum块信息的结构体指针    
 * 返回：
        成功返回结构体指针，失败返回NULL
 * 备注：
 ***/
static SORCHK *ReadChecksum(int fd, SORMAP *map, SORCHK *checksum)
{
    //临时变量
    char *buf = NULL;
    char *ptmp = NULL;
   
    int iErr = 0;    //错误标志
    
    //用来匹配checksum块的数据块ID
    char *str = CHK_ID;

    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || NULL == checksum || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //定位文件指针到文件开头
        iErr = lseek(fd, 0, SEEK_SET);
        if (-1 == iErr)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //读数据前的准备
        buf = ReadGetBlockSize(fd, map, str);
        if (NULL == buf)
        {
            iErr = -3;
        }
        ptmp = buf;    //保存缓冲区指针
    }

    if (0 == iErr)
    {
    	int len = 0; 
        //读取checksum块ID
        len = GetString(&checksum->strBlockId, &buf);
        if (len < 0)
        {
            //ID 读取失败
            iErr = -4;
        }

        buf += len;
    }

    if (0 == iErr)
    {
            //读取校验和
        memcpy(&checksum->usCheckSum, buf, 2);

    }

    //错误处理
    switch (iErr)
    {
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            return NULL;
        default :
            break;
    }

    if(ptmp)
	{
		free(ptmp);    //释放缓冲区内存
    	ptmp = NULL;
	} 

    return checksum;
}


/***
  * 功能：
        把map块的数据读写到新文件中去    
  * 参数：
        1.int fd:            打开的用于写的文件的文件描述符
        2.SORMAP *map:        用于写入文件的map结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int WriteMap(int fd, SORMAP *map)
{
    //临时变量定义
    char *ptmp = NULL;
    char *buf = NULL;
    int len = 0;
    int iErr = 0;    //错误标志,返回值
	int iRetValue;
	
    if (0 == iErr)
    {
        //参数的检查
        if (NULL == map || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {    
        //定位文件指针到文件开始位置

        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue )
        {
            //文件指针定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //申请缓冲区内存空间
        buf = (char *)calloc(1,map->lMapSize);
        if (NULL == buf)
        {
            //内存申请失败
            iErr = -3;
        } 
    }

    if (0 == iErr)
    {
    	memset(buf , 0, map->lMapSize);
        ptmp = buf;    //保存缓冲区指针
    }
    
    //将map块的各个字段数据写到buf中去
    if (0 == iErr)
    {
        //把map ID 写到buf中
        len = GetStringTobuf(map->strMapId, &buf);        
        if (len < 0)
        {
            //ID 写失败
            iErr = -4;
        }
        
        buf += len; 
    }

    if (0 == iErr)
    {
    	int i;
        //写入map块版本号
        memcpy(buf, &map->usMapRevisionNu, 2);    
        buf += 2;
        //写入map块大小
        memcpy(buf, &map->lMapSize, 4);
        buf += 4;    
        //写入文件中数据快的总数
        memcpy(buf, &map->sNuBlock, 2);
        buf += 2;
        //把其他块信息写到buf中
        for(i=0; i< map->sNuBlock-1; i++)
        {
            //写入ID    
            len = GetStringTobuf(map->pBlockInfo[i].strBlockId, &buf);
            if (len < 0)
            {
                //ID写入失败
                iErr = -5;
                break;
            }

            buf += len;

            //写入版本号
            memcpy(buf, &map->pBlockInfo[i].usRevisionNum, 2);
            buf += 2;
            //写入块大小
            memcpy(buf, &map->pBlockInfo[i].lBlockSize, 4);
            buf += 4;
        }
    }

    if (0 == iErr)
    {
        //把map块的数据写到新文件中去
        iRetValue = write(fd, ptmp, map->lMapSize);    
        if (-1 == iRetValue)
        {
            //数据块写失败
            iErr = -6;
        }
    }

    //错误处理
    switch (iErr)
    {
        case -6:
        case -5:
        case -4:
            free(ptmp);
            ptmp = NULL;
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    }

    if (0 == iErr)
    {
        //释放缓冲区空间
        free(ptmp);
        ptmp = NULL;
    }

    return iErr;
}


/***
  * 功能：
        把general块的数据读写到新文件中去    
  * 参数：
        1.int fd:                打开的用于写的文件的文件描述符
        2.SORMAP *map:             包含general块信息的map结构体指针
        3.SORGEN *general:        用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int WriteGeneral(int fd, SORMAP *map, SORGEN *general)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp = NULL;
    char *str = GEN_ID;    //用来匹配数据块的ID字符串
    int iErr = 0; //错误标志，返回值
    int len = 0;
    long blocksize;        
	int iRetValue = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == general || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue)
        {
            //文件指针定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
             iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        } 
    }

    if (0 == iErr)
    {
    	memset(buf, 0, blocksize);
        ptmp = buf;    //保存缓冲区指针
    }

    //开始把general中各个字段数据写到buf中
    if (0 == iErr)
    {
        //写入general块ID 
        len = GetStringTobuf(general->strBlockId, &buf);
        if (len < 0)
        {
            //ID 写入失败
            iErr = -5;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入语言码
        memcpy(buf, general->pLanguageCode, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        //写入电缆ID
        len = GetStringTobuf(general->strCableId, &buf);
        if (len < 0)
        {
            //电缆ID 写入失败
            iErr = -6;    
        }
        
        buf += len;
    }

    if (0 == iErr)
    {
        //写入光纤ID
        len = GetStringTobuf(general->strFiberId, &buf);
        if (len < 0)
        {
            //光纤ID 写入失败
            iErr = -7;    
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入光纤类型
        memcpy(buf, &general->sFiberType, 2);
        buf += 2;
        //写入OTDR波长
        memcpy(buf, &general->sWaveLength, 2); 
        buf += 2;
    }

    if (0 == iErr)
    {
        //写入测量开始位置信息
        len = GetStringTobuf(general->strOriginatingLoca, &buf);
        if (len < 0)
        {
            //测量开始位置信息写入失败
            iErr = -8;    
        }
         
        buf += len;
    }

    if (0 == iErr)
    {
        //写入测量结束位置信息
        len = GetStringTobuf(general->strTerminatingLoca, &buf);
        if (len < 0)
        {
            //测量结束位置信息写入失败
            iErr = -9;    
        }
        
        buf += len;
    }

    if (0 == iErr)
    {
        //写电缆编码信息 到buf
        len = GetStringTobuf(general->strCableCode, &buf);
        if (len < 0)
        {
            //电缆编码信息写入失败
            iErr = -10;    
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入光纤测量状态
        memcpy(buf, general->pDataFlag, 2); 
        buf += 2;
        
        //写入测量物理距离（时间）
        memcpy(buf, &general->lUserOffset, 4);    //获取AO距离
        buf += 4;
        
        //写入测量物理距离（距离）
        memcpy(buf, &general->lUserDist, 4); //获取AOD距离
        buf += 4;
    }

    if (0 == iErr)
    {
        //写入OTDR操作
        len = GetStringTobuf(general->strOperator, &buf);
        if (len < 0)
        {
            iErr = -11;
        }

        buf += len;
    }    

    if (0 == iErr)
    {
        len = GetStringTobuf(general->strComment, &buf);
        if (len < 0)
        {
            iErr = -12;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //把数据读到新文件中去
        iRetValue = write(fd, ptmp, blocksize);
        if (iRetValue < 0)
        {
            //数据写入失败
            iErr = -13;
        }
    }

    if (0 == iErr)
    {
        //释放buf空间
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch (iErr)
    {
        case -12:    
        case -11:    
        case -10:    
        case -9:    
        case -8:    
        case -7:    
        case -6:    
        case -5:    
            free(ptmp);
            ptmp = NULL;
        case -4:    
        case -3:    
        case -2:    
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    
    }

    return iErr;
}



/***
  * 功能：
        把supplier块的数据读写到新文件中去    
 * 参数：
        1.int fd:                打开的用于写的文件的文件描述符
        2.SORMAP *map:              包含supplier块信息的map结构体指针
        3.SORSUP *supplier:        用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int WriteSupplier(int fd, SORMAP *map, SORSUP *supplier)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp =NULL;
    char *str = SUP_ID;    //用来匹配数据块的ID字符串
    int iErr = 0;    //错误标志，返回值定义
    int len = 0;
    long blocksize; 
	int iRetValue = 0;

    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == supplier || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue)
        {
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        }
    }

	if (0 == iErr)
    {
    	memset(buf, 0, blocksize);
        ptmp = buf;    //保存缓冲区指针
    }
	 
    //开始把supplier中各个字段数据写到buf中
    if (0 == iErr)
    {
        //写入supplier块ID 
        len = GetStringTobuf(supplier->strBlockId, &buf);
        if (len < 0)
        {
            //ID写入失败
            iErr = -5;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入供应商名
        len = GetStringTobuf(supplier->strSupplierName, &buf);
        if (len < 0)
        {
            //供应商名写入失败
            iErr = -6;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入OTDR主机ID
        len = GetStringTobuf(supplier->strMainframeId, &buf);
        if (len < 0)
        {
            //OTDR主机ID写入失败
            iErr = -7;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入OTDR主机序列号
        len = GetStringTobuf(supplier->strMainframeSN, &buf);
        if (len < 0)
        {
            //OTDR主机序列号写入失败
            iErr = -8;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入OTDR插件模块ID
        len = GetStringTobuf(supplier->strOpticalId, &buf);
        if (len < 0)
        {
            //OTDR插件模块ID写入失败
            iErr = -9;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入OTDR插件模块序列号
        len = GetStringTobuf(supplier->strOpticalSN, &buf);
        if (len < 0)
        {
            //OTDR插件模块序列号写入失败
            iErr = -10;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //写入软件版本
        len = GetStringTobuf(supplier->strSoftRevision, &buf);
        if (len < 0)
        {
            //软件版本写入失败
            iErr = -11;
        }

        buf += len;
    }
    
    if (0 == iErr)
    {
        len = GetStringTobuf(supplier->strOther, &buf);
        if (len < 0)
        {
            iErr = -12;
        }

        buf += len;
    }

    if (0 == iErr)
    {
        //把数据写到文件中
        iRetValue = write(fd, ptmp, blocksize);
        if (iRetValue < 0)
        {
            //数据写入文件失败
            iErr = -13;
        }

    }

    if (0 == iErr)
    {
        //释放buf空间
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch (iErr)
    {
        case -13:
        case -12:
        case -11:
        case -10:
        case -9:
        case -8:
        case -7:
        case -6:
        case -5:
            free(ptmp);
            ptmp = NULL;
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    }

    return iErr;
}



/***
  * 功能：
        把fixed块的数据读写到新文件中去    
  * 参数：
        1.int fd:           打开的用于写的文件的文件描述符
        2.SORMAP *map:      包含fixed块信息的map结构体指针
        3.SORFXD *fixed:    用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int WriteFixed(int fd, SORMAP *map, SORFXD *fixed)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp =NULL;
    char *str = FXD_ID;    //用来匹配数据块的ID字符串
    long blocksize;        
    int iErr = 0;    //错误标志，返回值定义
	int iRetValue = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == fixed || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (iRetValue < 0)
        {
            iErr = -2;
        }
    }    

    if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        }
    }

	if (0 == iErr)
    {
    	memset(buf, 0, blocksize);
        ptmp = buf;    //保存缓冲区指针
    }
	 
    //开始把fixed各个字段数据写到buf中
    if (0 == iErr)
    {
        int len = 0;
        //写入fixed块ID
        len = GetStringTobuf(fixed->strBlockId, &buf);
        if (len < 0)
        {
            //写入失败
            iErr = -5;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //写入光纤测量的时间戳
        memcpy(buf, &fixed->ulDTS, 4);
        buf += 4;
        //写入测量的距离单位
        memcpy(buf,&fixed->pDistUints, 2);
        buf += 2;
        //写入实际波长
        memcpy(buf, &fixed->sWaveLength, 2);
        buf += 2;
        //写入激光器到第一个数据的时间
        memcpy(buf, &fixed->lAcquisitionOffset, 4);
        buf += 4;
        //写入激光器到第一个数据的距离
        memcpy(buf, &fixed->lOffsetDist, 4);
        buf += 4;
        //写入使用的脉宽总数
        memcpy(buf, &fixed->sPulesTotal, 2);
        buf += 2;
        //写入各个脉冲宽度的值
        memcpy(buf, fixed->pPulseWidth, 2*fixed->sPulesTotal);
        buf += 2*fixed->sPulesTotal;
        //写入数据间距
        memcpy(buf, fixed->pDataSpacing, 4*fixed->sPulesTotal);
        buf += 4*fixed->sPulesTotal;
        //写入每个脉冲宽度产生的数据量
        memcpy(buf,fixed->pPointsNumber, 4*fixed->sPulesTotal);
        buf += 4*fixed->sPulesTotal;
        //写入群折射律
        memcpy(buf, &fixed->lGroupIndex, 4);
        buf += 4;
        //写入反向散射系数
        memcpy(buf, &fixed->sBackscatterCode, 2);
        buf += 2;
        //写入平均次数
        memcpy(buf,&fixed->lAveragesNumber, 4);
        buf += 4;
        //写入平均时间
        memcpy(buf,&fixed->usAverageTime, 2);
        buf += 2;
        //写入OTDR测量的单程总长度（时间）
        memcpy(buf, &fixed->lAcquisitionRange, 4);
        buf += 4;
        //写入OTDR测量的单程总长度（距离）
        memcpy(buf, &fixed->lAcquisitionDist, 4);
        buf += 4;
        //写入前面板便宜
        memcpy(buf, &fixed->lFrontoffset, 4);
        buf += 4;
        //写入噪声基值
        memcpy(buf, &fixed->usNoiseFloor, 2);
        buf += 2;
        //写入噪声基值比例因子
        memcpy(buf, &fixed->sNoiseScale, 2);
        buf += 2;
        //写入衰减值
        memcpy(buf, &fixed->usPowerOffset, 2);
        buf += 2;
        //写入损耗阈值
        memcpy(buf, &fixed->usLossThreshold, 2);
        buf += 2;
        //写入反射阈值
        memcpy(buf, &fixed->usRefThreshold, 2);
        buf += 2;
        //写入末端阈值
        memcpy(buf, &fixed->usEndThreshold, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        //写入波形的类型
         memcpy(buf,&fixed->strTraceType, 2);
        //len = GetStringTobuf(fixed->strTraceType, &buf);
       // if (len < 0)
        //{
         //   iErr = -6;
        //}
        buf += 2;
    }

    //写入窗口坐标
    if (0 == iErr)
    {
        memcpy(buf, &fixed->WndCoordinate.lTopLeftX, 4);
        buf += 4;
        memcpy(buf, &fixed->WndCoordinate.lTopLeftY, 4);
        buf += 4;
        memcpy(buf, &fixed->WndCoordinate.lBotRightX, 4);
        buf += 4;
        memcpy(buf, &fixed->WndCoordinate.lBotRightY, 4);
    }

    if (0 == iErr)
    {
        //把数据写到文件中
        iRetValue = write(fd, ptmp, blocksize);
        if (iRetValue < 0)
        {
            iErr = -6;
        }
    }

    if (0 == iErr)
    {
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch(iErr)
    {
        case -6:
        case -5:
            free(ptmp);
            ptmp = NULL;
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default:
            break;
    }

    return iErr;
}



/***
  * 功能：
        把key块的数据读写到新文件中去    
  * 参数：
        1.int fd:               打开的用于写的文件的文件描述符
        2.SORMAP *map:          包含key块信息的map结构体指针
        3.SOREVT *keyevent:     用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int WriteKeyevent(int fd, SORMAP *map, SOREVT *keyevent)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp =NULL;
    char *str = ENT_ID;    //用来匹配数据块的ID字符串
    SOREVT_EVENT *ptr;    
    long blocksize;        
    int len = 0;
    int iErr = 0;    //错误标志，返回值定义
	int iRetValue = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == keyevent || fd < 0)
        {			
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        }
    }

    if (0 == iErr)
    {
    	memset(buf, 0, blocksize);
        ptmp = buf;    //保存缓冲区指针
        ptr = keyevent->pEventInfo;    //保存事件结构体指针
    }

    //开始把keyevent各个字段数据写到buf中
    if (0 == iErr)
    {
        //写入ID 
        len = GetStringTobuf(keyevent->strBlockId, &buf);
        if (len < 0)
        {
            //ID 写入失败
            iErr = -5;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //写入事件的数量
        memcpy(buf, &keyevent->sEventTotal, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        int i;
        //写入每个事件中各个字段的数据
        for(i = keyevent->sEventTotal; i>0; i--)
        {
            //写入事件编号
            memcpy(buf, &keyevent->pEventInfo->sEventNum, 2);
            buf += 2;
            //写入链路起始点到事件点的时间
            memcpy(buf, &keyevent->pEventInfo->lPropagationTime, 4);
            buf += 4;
            //写入衰减系数
            memcpy(buf, &keyevent->pEventInfo->sAttenuationCoef, 2);
            buf += 2;
            //写入事件损耗
            memcpy(buf, &keyevent->pEventInfo->sEventLoss, 2);
            buf += 2;
            //写入事件反射
            memcpy(buf, &keyevent->pEventInfo->lEventReflectance, 4);
            buf += 4;
            //写入事件描述
            memcpy(buf, &keyevent->pEventInfo->pEventCode, 6);
            buf += 6;
            //写入损耗测量方法
            memcpy(buf, &keyevent->pEventInfo->pLossMeasurement, 2);
            buf += 2;
            if (0 == iErr)
            {
                //写入事件标记
                memcpy(buf, &keyevent->pEventInfo->lMarkLoca1, 4);
                buf += 4;
                memcpy(buf, &keyevent->pEventInfo->lMarkLoca2, 4);
                buf += 4;
                memcpy(buf, &keyevent->pEventInfo->lMarkLoca3, 4);
                buf += 4;
                memcpy(buf, &keyevent->pEventInfo->lMarkLoca4, 4);
                buf += 4;
                memcpy(buf, &keyevent->pEventInfo->lMarkLoca5, 4);
                buf += 4;

            }
            if (0 == iErr)
            {
                len = GetStringTobuf(keyevent->pEventInfo->strComment, &buf);
                if (len < 0)
                {
                    iErr = -6;
                    break;
                }
                buf += len;
                keyevent->pEventInfo++;    //写下一个事件信息
            }
        }
    }

    if (0 == iErr)
    {
        //恢复事件结构体指针
        keyevent->pEventInfo = ptr;
    }

    if (0 == iErr)
    {
        //写入链路端到端损耗    
        memcpy(buf, &keyevent->lEndtoendLoss, 4);
        buf += 4;
    }

    //写入链路端到端损耗的标记位置    
    if (0 == iErr)
    {
        memcpy(buf, &keyevent->EndtoendMarker.lStartPosition, 4);
        buf += 4;
        memcpy(buf, &keyevent->EndtoendMarker.lFinishPosition, 4);
        buf += 4;
    }

    if (0 == iErr)
    {
        //写入回波损耗
        memcpy(buf, &keyevent->usReturnLoss, 2);
        buf += 2;
    }

    //写入回波损耗的标记位置
    if (0 == iErr)
    {
        memcpy(buf, &keyevent->ReturnMarker.lStartPosition, 4);
        buf += 4;
        memcpy(buf, &keyevent->ReturnMarker.lFinishPosition, 4);
    }

    if (0 == iErr)
    {
        //把数据写到文件中
        iRetValue = write(fd, ptmp, blocksize);
        if (iRetValue < 0)
        {
            //数据写入失败
            iErr = -7;
        }
    }

    if (0 == iErr)
    {
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch (iErr)
    {
        case -7:
        case -6:
        case -5:
            free(ptmp);
            ptmp = NULL;
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    }

    return iErr;
}


/***
  * 功能：
        把datapoint块的数据读写到新文件中去    
  * 参数：
        1.int fd:                   打开的用于写的文件的文件描述符
        2.SORMAP *map:              包含datapoint块信息的map结构体指针
        3.SORDAT *datapoint:        用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int WriteDatapoint(int fd, SORMAP *map, SORDAT *datapoint)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp =NULL;
    char *str = DAT_ID;    //用来匹配数据块的ID字符串
    SORDAT_SCALEFACTOR *ptr = NULL; //用来保存比列因子指针的临时变量
    long blocksize = 0;        
    int iErr = 0; //错误标志，返回值定义
	int iRetValue = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == datapoint || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue )
        {
            //文件指针定位失败
            iErr = -2;
        }
    }    

    if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        }
    }

    if (0 == iErr)
    {
    	memset(buf, 0, blocksize);
        //保存缓冲区指针
        ptmp = buf;
        
        //保存比例因子信息结构体指针
        ptr = datapoint->pFactorInfo;
    }

    //开始把datapoint数据快各个字段数据写到buf中
    if (0 == iErr)
    {
        int len = 0;
        //写入ID
        len = GetStringTobuf(datapoint->strBlockId, &buf);
        if (len < 0)
        {
            //ID写入失败
            iErr = -5;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //写入数据点的总数
        memcpy(buf, &datapoint->lPointsTotal, 4);
        buf += 4;
        //写入比例因子的总数
        memcpy(buf, &datapoint->sFactorsTotal, 2);
        buf += 2;
    }

    if (0 == iErr)
    {
        int i;
        //写每个比列因子下的数据
        for (i = datapoint->sFactorsTotal; i>0; i--)
        { 
            
            //写入该比例因子下的数据点总数
            memcpy(buf, &datapoint->pFactorInfo->lPointsTotal, 4);
            buf += 4;
            //写入该比例因子的值
            memcpy(buf, &datapoint->pFactorInfo->sScaleFactor, 2);
            buf += 2;
            //写入该比例因子下的数据点的数据值
            memcpy(buf, datapoint->pFactorInfo->pDataPoints,  2 * datapoint->pFactorInfo->lPointsTotal);    
            //下一个比列因子下的数据信息
            buf +=  datapoint->pFactorInfo->lPointsTotal;
            //写入下一个比例因子下的信息
            datapoint->pFactorInfo++;    
        }
        
        //恢复比例因子信息结构体指针
        datapoint->pFactorInfo = ptr;
    }

    if (0 == iErr)
    {
        //把数据写到文件中
        iRetValue = write(fd, ptmp, blocksize);
        if (iRetValue < 0)
        {
            //数据写入失败
            iErr = -6;
        }
    }

    if (0 == iErr)
    {
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch (iErr)
    {
        case -6:
        case -5:
            free(ptmp);
            ptmp = NULL;
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    }

    return iErr;
}


/***
  * 功能：
        把special块的数据写到新文件中去    
  * 参数：
        1.int fd:               打开的用于写的文件的文件描述符
        2.SORMAP *map:          包含special块信息的map结构体指针
        3.SORSPC *special:      用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
int WriteSpecial(int fd, SORMAP *map, SORSPC *special)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp =NULL;
    char *str = SPC_ID; //用来匹配数据块的ID字符串
    long blocksize = 0;    
    int iErr = 0;
	int iRetValue = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == special || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue)
        {
            //文件指针定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        }
    }

    if (0 == iErr)
    {
        memset(buf, 0, blocksize);
        ptmp = buf;    //保存缓冲区指针
    }

    if (0 == iErr)
    {
    	int len = 0;
        //写入ID 
        len = GetStringTobuf(special->strBlockId, &buf);
        if (len < 0)
        {
            iErr = -5;
        }
        buf += len;
    }

    if (0 == iErr)
    {
    	//
    	memcpy(buf, special->strInfo, strlen(special->strInfo)+1);
        //把数据写到文件中
        iRetValue = write(fd, ptmp, blocksize);
        if (-1 == iRetValue )
        {
            //数据写入失败
            iErr = -6;
        }
    }    

    if (0 == iErr)
    {
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch (iErr)
    {
        case -6:
        case -5:
            free(ptmp);
            ptmp = NULL;
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    }

    return iErr;
}




/***
  * 功能：
        把checksum块的数据读写到新文件中去    
  * 参数：
        1.int fd:               打开的用于写的文件的文件描述符
        2.SORMAP *map:          包含checksum块信息的map结构体指针
        3.CHECKSUM *checksum:   用于读入文件的数据结构体指针
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
int WriteChecksum(int fd, SORMAP *map, SORCHK *checksum)
{
    //临时变量定义
    char *buf = NULL;
    char *ptmp =NULL;
    char *str = CHK_ID; //用来匹配数据块的ID字符串
    long blocksize = 0;    
    int iErr = 0;   //错误标志
	int iRetValue = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == checksum || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //把文件指针指向文件开始
        iRetValue = lseek(fd, 0, SEEK_SET);
        if (-1 == iRetValue)
        {
            //文件指针定位失败
            iErr = -2;
        }
    }
	
	if (0 == iErr)
    {
        //写操作前的准备
        blocksize = WriteGetBlockSize(fd, map, str);
        if (blocksize < 0)
        {
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //开辟对应大小的数据缓冲区空间
        buf = (char *)calloc(1,blocksize);
        if (NULL == buf)
        {
            iErr = -4;
        }
    }

    if (0 == iErr)
    {
        memset(buf, 0, blocksize);
        ptmp = buf;    //保存缓冲区指针
    }

    if (0 == iErr)
    {
        int len = 0;
        //写入ID 
        len = GetStringTobuf(checksum->strBlockId, &buf);
        if (len < 0)
        {
            //ID写入失败
            iErr = -5;
        }
        buf += len;
    }

    if (0 == iErr)
    {
        //写入校验和的值
        memcpy(buf, &checksum->usCheckSum, 2);
        
        //把checksum数据写到文件中
        iRetValue = write(fd, ptmp, blocksize);
        if (iRetValue < 0)
        {
            //数据写入失败
            iErr = -6;
        }
    }

    if (0 == iErr)
    {
        //释放buf内存空间
        free(ptmp);
        ptmp = NULL;
    }

    //错误处理
    switch (iErr)
    {
        case -6:
        case -5:
            free(ptmp);
            ptmp = NULL;
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            break;
        default :
            break;
    }

    return iErr;
}



/***
  * 功能：
        从buf中拷贝字符串
  * 参数：
        1.char **dest:        用来接收字符串的指针的指针
        2.char **buf:         数据缓冲区指针的指针
  * 返回：
        成功返回字符串的长度，失败返回错误标记值
  * 备注：
        该函数会对dest参数内容做修改，请注意防止内存泄露。
 ***/
static int GetString(char **dest, char **buf)
{
    //临时变量，用来记录字符串长度
    int len;
    char *str = NULL;
    //错误标志
    int iErr = 0;
    
    if (0 == iErr)
    {
        //检查指针是否有效 
        if (NULL == buf || NULL == dest)
        {
            iErr = -1;
        }
    }
    
    if (0 == iErr)
    {
        //检查指针指向的是否是有效指针
        if (NULL == *buf)
        {
            iErr =  -2;
        }
    }

    if (0 ==  iErr)
    {
        //求出字符串长度
        len = strlen(*buf) +1 ;
    }

    if (0 == iErr)
    {
        //开辟接收字符串的内存空
        str = (char *)calloc(1,len);    
        if (NULL == str)
        {
            LOG(LOG_ERROR, "----GetString: calloc memory failed---\n");
            iErr = -3;
        }
    }

    if (0 == iErr)
    {
        //拷贝字符串
        strcpy(str, *buf); 

        *dest = str;
    }

    //错误处理    
    switch (iErr)
    {
        case -3:
        case -2:
        case -1:
            return iErr;
        default:
            break;
    }

    return len;
}

/***
  * 功能：
        用于写操作时把数据块中的字符串拷贝到缓冲区buf中，并改变buf指针的位置
  * 参数：
        1.char *str:    数据快中的字符串
        2.char **buf:   用于接收数据快中的字符串的缓冲区指针的地址
  * 返回：
        成功返回字符串长度，失败返回-1 
  * 备注：
 ***/
static int  GetStringTobuf(char *str, char **buf)
{
    //定义临时变量
    int len = 0;

    //参数检查
    if (NULL == str || NULL == buf)
    {
        //参数错误
        return -1;
    }

    strcpy(*buf , str);    
    len = strlen(str) + 1;
    
    return len;
}



/***
  * 功能：
        开始读数据的准备工作:
        1.定位文件指针,
        2.开辟对应大小的数据缓冲区空间,
        3.把文件中对应的数据读到buf中去    
  * 参数：
        1.int fd:           打开的用于读数据的文件的文件描述符
        2.SORMAP *map:      包含数据块信息的结构体指针
        3.char *str:        用来匹配对应的数据块的ID字符串
  * 返回：
         成功返回数据缓冲区指针，失败返回NULL
  * 备注：
 ***/
static char *ReadGetBlockSize(int fd, SORMAP *map, char *str)
{
    //临时变量
    int i = 0;
    int iRetValue = 0;
    char *buf =NULL;
    
    //错误标志
    int iErr = 0;
    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == str || fd < 0)
        {
            iErr = -1;
        }
    }

    if (0 == iErr)
    {
        //跳过文件中map块
        iRetValue = lseek(fd, map->lMapSize, SEEK_CUR);    
        if (-1 == iRetValue)
        {
            //文件定位失败
            iErr = -2;
        }
    }

    if (0 == iErr)
    {
        //查找对应的数据快，并改变文件指针的位置
        while (i <= (map->sNuBlock -1) && strcmp(map->pBlockInfo[i].strBlockId, str))
        {
            //定位文件指针
            iRetValue = lseek(fd, map->pBlockInfo[i].lBlockSize, SEEK_CUR);

            if (-1 == iRetValue)
            {
                //文件定位失败
                iErr = -3;
                break;
            }
            i++;
        }

        if (i >  (map->sNuBlock -1))
        {
            //文件中没有对应的数据快
            return NULL;
        }
    }

    if (0 == iErr)
    {
        //分配缓冲区内存空间
        buf = (char *)calloc(1,map->pBlockInfo[i].lBlockSize);
        if (NULL == buf)
        {
            //内存申请失败
            iErr = -4;
        }
    }
    
    if (0 == iErr)
    {
        memset(buf, 0, map->pBlockInfo[i].lBlockSize);
    }

    if (0 == iErr)
    {
        //把文件中对应数据块的数据读到缓冲区中
        iRetValue = read(fd, buf, map->pBlockInfo[i].lBlockSize);
        if (-1 == iRetValue)
        {
            //文件读取失败
            iErr = -5;
        }
    }

    //错误处理
    switch (iErr)
    {
        case -5:
            free(buf);
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            return NULL;
        default:
            break;
    }

    return buf;
}


/***
  * 功能：
        开始写数据的准备工作:
        1.定位文件指针
        2.找到对应数据块的信息        
  * 参数：
        1.int fd:               打开的文件的文件描述符、
        2.SORMAP *map:        包含数据块信息的结构体指针
        3.char *str:            用来匹配对应的数据块的ID字符串
  * 返回：
        成功返回对应数据块的大小，失败返回小于零的值
  * 备注：
 ***/
static long WriteGetBlockSize(int fd, SORMAP *map, char *str)
{
    //临时变量定义
    int iRetValue = 0; 
    int i = 0;
    int iErr = 0;    //错误标志

    if (0 == iErr)
    {
        //参数检查
        if (NULL == map || NULL == str || fd < 0)
        {
            //参数错误
            iErr = -1;
        }
    }

    if (0 == iErr)
    {    
        //跳过map 块
        iRetValue = lseek(fd, map->lMapSize, SEEK_CUR);    
        if (iRetValue < 0)
        {
            iErr = -2;    
        }    
    }    

    if (0 == iErr)
    {
        //查找对应的数据快，并改变文件指针的位置
        while (i < (map->sNuBlock -1) && strcmp(map->pBlockInfo[i].strBlockId, str))
        {
            iRetValue = lseek(fd, map->pBlockInfo[i].lBlockSize, SEEK_CUR);
            if (-1 == iRetValue)
            {
                //文件定位失败
                iErr = -3;
                
                break;
            }

            i++;
        }

        if (i >= (map->sNuBlock -1))
        {
            //没有对应的数据快
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
        case -4:
        case -3:
        case -2:
        case -1:
            lseek(fd, 0, SEEK_SET);
            return iErr;
        default :
            break;
    }

    return map->pBlockInfo[i].lBlockSize;
}


/***
  * 功能：
        修改每个比例因子下的各个数据点的值
  * 参数：
        1.SORDAT *datapoint:   数据点的数据块结构体指针
  * 返回：
        成功返回数据快结构体指针，失败返回NULL.
  * 备注：
 ***/
static SORDAT *ChangeDataValue(SORDAT *datapoint)
{
    //临时变量定义
    SORDAT_SCALEFACTOR *ptr = NULL;    //保存比例因子指针的临时变量
    unsigned short *ptmp = NULL; //保存数据点指针的临时变量
    int iErr = 0; //错误标志

    if (0 == iErr)
    {

        //参数的检查
        if (NULL == datapoint)
        {
            iErr = -1;
        }    
    }

    if (0 == iErr)
    {
        //保存比例因子指针
        ptr = datapoint->pFactorInfo; 
    }

    if (0 == iErr)
    {
        int i;
    	int j;
        // 数据的转换
        for (i = datapoint->sFactorsTotal; i > 0; i--)
        {
            ptmp = datapoint->pFactorInfo->pDataPoints; //保存数据点指针

            for (j = datapoint->pFactorInfo->lPointsTotal; j > 0; j --)
            {
                //转换每组比例因子下的数据点
                *datapoint->pFactorInfo->pDataPoints = 0xffff - *datapoint->pFactorInfo->pDataPoints;
                datapoint->pFactorInfo->pDataPoints++;
            }

            datapoint->pFactorInfo->pDataPoints = ptmp; //恢复数据点指针

            //转换下一个比例因子下的数据点
            datapoint->pFactorInfo++; 
        }
    }

    if (0 == iErr)
    {
        datapoint->pFactorInfo = ptr;  //恢复比例因子指针
    }

    //错误处理
    switch (iErr)
    {
        case -1:
            return NULL;
        default :
            break;
    
    }

    return datapoint;
}


/***
  * 功能：
		操作SOR文件之后的结束工作，资源释放
  * 参数：
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static void ReleaseMemSor()
{
	int i = 0;
	//check
	if(pSorCheck)
	{
		if(pSorCheck->strBlockId)
		{
			free(pSorCheck->strBlockId);
			pSorCheck->strBlockId = NULL;
		}
		free(pSorCheck);
		pSorCheck = NULL;
	}
	
	//special
	if(pSorSpec)
	{
		if(pSorSpec->strBlockId)
		{
			free(pSorSpec->strBlockId);
			pSorSpec->strBlockId = NULL;
		}
		free(pSorSpec);
		pSorSpec = NULL;
	}
	//data
	if(pSorData)
	{
		for (i = 0;i < pSorData->sFactorsTotal; i++)
		{
			if(pSorData->pFactorInfo[i].pDataPoints)
			{
				free(pSorData->pFactorInfo[i].pDataPoints);
				pSorData->pFactorInfo[i].pDataPoints = NULL;
			}
		}
		if(pSorData->pFactorInfo)
		{
			free(pSorData->pFactorInfo);
			pSorData->pFactorInfo = NULL;
		}
		if(pSorData->strBlockId)
		{
			free(pSorData->strBlockId);
			pSorData->strBlockId = NULL;
		}

		free(pSorData);
		pSorData = NULL;
	}
	//link
	if(pSorLnk)
	{
		for (i = 0;i < pSorLnk->sLandmarksTotal; i++)
		{
			if(pSorLnk->pLandmarkInfo[i].strComment)
			{
				free(pSorLnk->pLandmarkInfo[i].strComment);
				pSorLnk->pLandmarkInfo[i].strComment = NULL;
			}
		}
		if(pSorLnk->pLandmarkInfo)
		{
			free(pSorLnk->pLandmarkInfo);
			pSorLnk->pLandmarkInfo = NULL;
		}
		if(pSorLnk->strBlockId)
		{
			free(pSorLnk->strBlockId);
			pSorLnk->strBlockId = NULL;
		}

		free(pSorLnk);
		pSorLnk = NULL;
	}
	//event
	if(pSorEvent)
	{
		for(i = 0;i < pSorEvent->sEventTotal; i++)
		{
			if(pSorEvent->pEventInfo[i].strComment)
			{
				free(pSorEvent->pEventInfo[i].strComment);
				pSorEvent->pEventInfo[i].strComment = NULL;
			}
		}
		if(pSorEvent->pEventInfo)
		{
			free(pSorEvent->pEventInfo);
			pSorEvent->pEventInfo = NULL;
		}
		if(pSorEvent->strBlockId)
		{
			free(pSorEvent->strBlockId);
			pSorEvent->strBlockId = NULL;
		}

		free(pSorEvent);
		pSorEvent = NULL;
	}

	//fix
	if(pSorFxd)
	{
		if(pSorFxd->pPointsNumber)
		{
			free(pSorFxd->pPointsNumber);
			pSorFxd->pPointsNumber = NULL;
		}
		if(pSorFxd->pDataSpacing)
		{
			free(pSorFxd->pDataSpacing);
			pSorFxd->pDataSpacing = NULL;
		}
		if(pSorFxd->pPulseWidth)
		{
			free(pSorFxd->pPulseWidth);
			pSorFxd->pPulseWidth = NULL;
		}
		if(pSorFxd->strBlockId)
		{
			free(pSorFxd->strBlockId);
			pSorFxd->strBlockId  = NULL;
		}

		free(pSorFxd);
		pSorFxd = NULL;
	}
	//sup
	if(pSorSup)
	{
		if(pSorSup->strOther)
		{
			free(pSorSup->strOther);
			pSorSup->strOther = NULL;
		}
		if(pSorSup->strSoftRevision)
		{
			free(pSorSup->strSoftRevision);
			pSorSup->strSoftRevision = NULL;
		}
		if(pSorSup->strOpticalSN)
		{
			free(pSorSup->strOpticalSN);
	    	pSorSup->strOpticalSN = NULL;
		}
		if(pSorSup->strOpticalId)
		{
			free(pSorSup->strOpticalId);
	    	pSorSup->strOpticalId = NULL;
		}
		if(pSorSup->strMainframeSN)
		{
			free(pSorSup->strMainframeSN);
	    	pSorSup->strMainframeSN = NULL;
		}
		if(pSorSup->strMainframeId)
		{
			free(pSorSup->strMainframeId);
	    	pSorSup->strMainframeId = NULL;
		}
		if(pSorSup->strSupplierName)
		{
			free(pSorSup->strSupplierName);
	    	pSorSup->strSupplierName = NULL;
		}
		if(pSorSup->strBlockId)
		{
			free(pSorSup->strBlockId);
	    	pSorSup->strBlockId = NULL;
		}

		free(pSorSup);
		pSorSup = NULL;
	}

	//gen
	if(pSorGen)
	{
		if(pSorGen->strComment)
		{
			free(pSorGen->strComment);
			pSorGen->strComment = NULL;
		}
		if(pSorGen->strOperator)
		{
			free(pSorGen->strOperator);        
	    	pSorGen->strOperator = NULL;   
		}
		if(pSorGen->strCableCode)
		{
			free(pSorGen->strCableCode);
	    	pSorGen->strCableCode = NULL;
		}
		if(pSorGen->strTerminatingLoca)
		{
			free(pSorGen->strTerminatingLoca);
	    	pSorGen->strTerminatingLoca = NULL;
		}
		if(pSorGen->strOriginatingLoca)
		{
			free(pSorGen->strOriginatingLoca);
	    	pSorGen->strOriginatingLoca = NULL;
		}
		if(pSorGen->strFiberId)
		{
			free(pSorGen->strFiberId);
	    	pSorGen->strFiberId = NULL;
		}
		if(pSorGen->strCableId)
		{
			free(pSorGen->strCableId); 
	    	pSorGen->strCableId = NULL; 
		}
		if(pSorGen->strBlockId)
		{
			free(pSorGen->strBlockId); 
	    	pSorGen->strBlockId = NULL; 
		}

		free(pSorGen);
		pSorGen = NULL;
	}

	//map
	if(pSorMap)
	{
		for(i = 0;i < (pSorMap->sNuBlock - 1); i++)
		{
			if(pSorMap->pBlockInfo[i].strBlockId)
			{
				free(pSorMap->pBlockInfo[i].strBlockId);
				pSorMap->pBlockInfo[i].strBlockId = NULL;
			}
		}
		if(pSorMap->strMapId)
		{
			free(pSorMap->strMapId);
			pSorMap->strMapId =NULL;
		}

		free(pSorMap);
		pSorMap = NULL;
	}
}



/***
  * 功能：
		保存测试数据到SOR文件之前的初始化工作
  * 参数：
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int InitReadFromSor()
{
	//返回值
	int iRet = 0;

	pSorMap = (SORMAP *)calloc(1, sizeof(SORMAP));
	pSorGen = (SORGEN *)calloc(1, sizeof(SORGEN));
	pSorSup = (SORSUP *)calloc(1, sizeof(SORSUP));
	pSorFxd = (SORFXD *)calloc(1, sizeof(SORFXD));
	pSorEvent = (SOREVT *)calloc(1, sizeof(SOREVT));
	pSorLnk = (SORLNK *)calloc(1, sizeof(SORLNK));
	pSorData = (SORDAT *)calloc(1, sizeof(SORDAT));
	pSorSpec = (SORSPC *)calloc(1, sizeof(SORSPC));
	pSorCheck = (SORCHK *)calloc(1, sizeof(SORCHK));
	if ((pSorMap && pSorGen && pSorSup && pSorFxd && pSorEvent
	 	&& pSorLnk && pSorData && pSorSpec && pSorCheck) == 0)
	{
		iRet = -1;
		LOG(LOG_ERROR, "----ReadFromSor: InitReadFromSor: calloc Memory failed-------\n");
	}
	
	return iRet;
}


static unsigned short crc16(unsigned char *buf,int len)
{
	unsigned int i = 0;
	unsigned short crc = 0xffff;
	for(i = 0;i < len;i++)
	{
		crc = (unsigned short)((unsigned char)(crc >> 8) | (unsigned short)(crc << 8));
		crc ^= buf[i];
		crc ^= (unsigned char)(crc & 0xFF) >> 4;
		crc ^= (unsigned short)((unsigned short)(crc << 8) << 4);
		crc ^= (unsigned short)((unsigned short)((crc & 0xff) << 4) << 1);
	}

	crc = (crc >> 8) | (crc << 8);
	return crc;
}

static int char_to_int(char *p)
{
    int type = 2 ;
	if(!strncmp("0F9999",p,6))
		type = 0;//衰减事件‘增益事件
	else if(!strncmp("1F9999",p,6))
		type = 2;//反射事件
	else if(!strncmp("1E9999",p,6) || !strncmp("2E9999",p,6))
		type = 12;//反射末端事件
	else if (!strncmp("1O9999",p,6))
		type = 20;//连续光钎事件、分析结束事件
	else if (!strncmp("0E9999",p,6))
		type = 10;//衰减末端事件、增益末端事件
	else
		;
	return type;
}


static int char_to_int_self(char *p)
{
    int type = 0 ;
	if(!strncmp("0F9999",p,6))
		type = 0;//衰减事件
	else if(!strncmp("0F1999",p,6))
		type = 1;//增益事件
	else if(!strncmp("1F9999",p,6))
		type = 2;//反射事件
	else if(!strncmp("1F0999",p,6))
		type = 3;//回波事件
	else if(!strncmp("0E9999",p,6))
		type = 10;//衰减结束事件
	else if(!strncmp("0E1999",p,6))
		type = 11;//增益结束事件
	else if( !strncmp("1E9999",p,6))
		type = 12;//反射结束事件
	else if (!strncmp("1O9999",p,6))
		type = 20; //连续光钎事件
	else if (!strncmp("1O1999",p,6))
		type = 21;//分析结束事件
	else
		;
	return type;
}


/***
  * 功能：
		保存测试数据到SOR文件之前的初始化工作
  * 参数：
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int InitWriteToSor(PSOR_DATA pSor)
{
	//返回值
	int iRet = 0;
	
	int iBlkSize = 0;
	char *pBuff = NULL;
	double fPeriod = 250.0f / pSor->fSmpRatio;	//(float)1E4 / (float)ADC_CLOCK_FREQ;//单位100ps

	//初始化SORMAP
	if(iRet == 0)
	{
		iBlkSize = 0;
		pSorMap = (SORMAP *)calloc(1, sizeof(SORMAP));
		if (NULL == pSorMap)
		{
			iRet = -1;
			LOG(LOG_ERROR, "----InitWriteToSor: calloc pSorMap Failed!----\n");
		}
        else {
    		pBuff = MAP_ID;
    		GetString(&pSorMap->strMapId, &pBuff);
    		iBlkSize += strlen(pSorMap->strMapId) + 1;
    		pSorMap->usMapRevisionNu = VERSION_SOR;
    		iBlkSize += sizeof(pSorMap->usMapRevisionNu);

    		pBuff = GEN_ID;
    		GetString(&pSorMap->pBlockInfo[0].strBlockId, &pBuff);
    		pBuff = SUP_ID;
    		GetString(&pSorMap->pBlockInfo[1].strBlockId, &pBuff);
    		pBuff = FXD_ID;
    		GetString(&pSorMap->pBlockInfo[2].strBlockId, &pBuff);

    		if (pSor->iEventAnalysisFlag)
    		{
    			pBuff = ENT_ID;
    			GetString(&pSorMap->pBlockInfo[3].strBlockId, &pBuff);
    			pBuff = DAT_ID;
    			GetString(&pSorMap->pBlockInfo[4].strBlockId, &pBuff);
    			pBuff = CHK_ID;
    			GetString(&pSorMap->pBlockInfo[5].strBlockId, &pBuff);
    			pSorMap->sNuBlock = 7;
    		}
    		else
    		{
    			pBuff = DAT_ID;
    			GetString(&pSorMap->pBlockInfo[3].strBlockId, &pBuff);
    			pBuff = CHK_ID;
    			GetString(&pSorMap->pBlockInfo[4].strBlockId, &pBuff);
    			pSorMap->sNuBlock = 6;
    		}
    		iBlkSize += sizeof(pSorMap->sNuBlock);

    		int i = 0;
    		for(i = 0; i < (pSorMap->sNuBlock - 1); i++)
    		{
    			iBlkSize += strlen(pSorMap->pBlockInfo[i].strBlockId) + 1;
    			iBlkSize += sizeof(pSorMap->pBlockInfo[i].usRevisionNum);
    			iBlkSize += sizeof(pSorMap->pBlockInfo[i].lBlockSize);
    		}
    		
    		iBlkSize += sizeof(pSorMap->lMapSize);
    		pSorMap->lMapSize = iBlkSize;
        }
	}
	
	//初始化SORGEN
	if(iRet == 0)
	{
		iBlkSize = 0;
		pSorGen = (SORGEN *)calloc(1, sizeof(SORGEN));
		if (NULL == pSorGen)
		{
			iRet = -2;
			LOG(LOG_ERROR, "----InitWriteToSor: calloc pSorGen Failed!----\n");
		}
        else {
    		pBuff = pSorMap->pBlockInfo[0].strBlockId;
    		GetString(&pSorGen->strBlockId, &pBuff);
    		iBlkSize += strlen(pSorGen->strBlockId) + 1;
    		
    		pSorGen->pLanguageCode[0] = 'E';
    		pSorGen->pLanguageCode[1] = 'N';
    		iBlkSize += sizeof(pSorGen->pLanguageCode);
    		pBuff = pSor->generalInfo.pCableID;
    		GetString(&pSorGen->strCableId, &pBuff);
    		iBlkSize += strlen(pSorGen->strCableId) + 1;
    		pBuff = pSor->generalInfo.pFiberID;
    		GetString(&pSorGen->strFiberId, &pBuff);
    		iBlkSize += strlen(pSorGen->strFiberId) + 1;
    		pSorGen->sFiberType = pSor->generalInfo.sFiberType;
    		iBlkSize += sizeof(pSorGen->sFiberType);
    		pSorGen->sWaveLength = pSor->iWave;				//波长
    		iBlkSize += sizeof(pSorGen->sWaveLength);
    		pBuff = pSor->generalInfo.pStartPoint;
    		GetString(&pSorGen->strOriginatingLoca, &pBuff);
    		iBlkSize += strlen(pSorGen->strOriginatingLoca) + 1;
    		pBuff = pSor->generalInfo.pEndPoint;
    		GetString(&pSorGen->strTerminatingLoca, &pBuff);
    		iBlkSize += strlen(pSorGen->strTerminatingLoca) + 1;
    		pBuff = pSor->generalInfo.pCableCode;
    		GetString(&pSorGen->strCableCode, &pBuff);
    		iBlkSize += strlen(pSorGen->strCableCode) + 1;
    		pSorGen->pDataFlag[0] = pSor->generalInfo.pDataFlag[0];		//NC=as-bulit,new condition RC=as-repaired condition OT=other.
    		pSorGen->pDataFlag[1] = pSor->generalInfo.pDataFlag[1];
    		iBlkSize += sizeof(pSorGen->pDataFlag);
    		pSorGen->lUserOffset = 0;
    		iBlkSize += sizeof(pSorGen->lUserOffset);
    		pSorGen->lUserDist = 0;
    		iBlkSize += sizeof(pSorGen->lUserDist);
    		pBuff = pSor->generalInfo.pOperator;
    		GetString(&pSorGen->strOperator, &pBuff);
    		iBlkSize += strlen(pSorGen->strOperator) + 1;
    		pBuff = pSor->generalInfo.pComment;
    		GetString(&pSorGen->strComment, &pBuff);	
    		iBlkSize += strlen(pSorGen->strComment) + 1;
    		
    		pSorMap->pBlockInfo[0].usRevisionNum = VERSION_SOR;
    		pSorMap->pBlockInfo[0].lBlockSize = iBlkSize;
        }
	}

	//初始化SORSUP
	if(iRet == 0)
	{
		iBlkSize = 0;
		pSorSup = (SORSUP *)calloc(1, sizeof(SORSUP));
		if (NULL == pSorSup)
		{
			iRet = -3;
			LOG(LOG_ERROR, "----InitWriteToSor: calloc pSorSup Failed!----\n");
		}
        else {
    		pBuff = pSorMap->pBlockInfo[1].strBlockId;
    		GetString(&pSorSup->strBlockId, &pBuff);
    		pBuff = pSor->generalInfo.pCompanyName;
    		GetString(&pSorSup->strSupplierName, &pBuff);	//OTDR供应商
    		pBuff = MAINFRAME_NAME;
    		GetString(&pSorSup->strMainframeId, &pBuff);	//OTDR主机名
    		pBuff = MAINFRAME_SN;
    		GetString(&pSorSup->strMainframeSN, &pBuff);	//OTDR主机序列号
    		pBuff = OPTICAL_NAME;
    		GetString(&pSorSup->strOpticalId, &pBuff);		//OTDR光模块名
    		pBuff = OPTICAL_SN;
    		GetString(&pSorSup->strOpticalSN, &pBuff);		//ODTR光模块序列号
    		pBuff = FIRMWARE_VER;
    		GetString(&pSorSup->strSoftRevision, &pBuff);	//软件版本号
    		pBuff = __DATE__;
    		GetString(&pSorSup->strOther, &pBuff);			//其他添加的信息
    		
    		iBlkSize += strlen(pSorSup->strBlockId) + 1;
    		iBlkSize += strlen(pSorSup->strSupplierName) + 1;
    		iBlkSize += strlen(pSorSup->strMainframeId) + 1;
    		iBlkSize += strlen(pSorSup->strMainframeSN) + 1;
    		iBlkSize += strlen(pSorSup->strOpticalId) + 1;
    		iBlkSize += strlen(pSorSup->strOpticalSN) + 1;
    		iBlkSize += strlen(pSorSup->strSoftRevision) + 1;
    		iBlkSize += strlen(pSorSup->strOther) + 1;
    		
    		pSorMap->pBlockInfo[1].usRevisionNum = VERSION_SOR;
    		pSorMap->pBlockInfo[1].lBlockSize = iBlkSize;
        }
	}

	//初始化SORFXD
	if(iRet == 0)
	{
		iBlkSize = 0;
		pSorFxd = (SORFXD *)calloc(1, sizeof(SORFXD));
		if(NULL == pSorFxd)
		{
			iRet = -4;
			LOG(LOG_ERROR, "----InitWriteToSor: calloc pSorFxd Failed!----\n");
		}
        else {
    		pBuff = pSorMap->pBlockInfo[2].strBlockId;
    		GetString(&pSorFxd->strBlockId, &pBuff);
    		iBlkSize += strlen(pSorFxd->strBlockId) + 1;
    		
    		pSorFxd->ulDTS = time(NULL);
    		iBlkSize += sizeof(pSorFxd->ulDTS);

    		/*----因为我们的单位只有m没有km，所以就直接写mt----*/
    		{
    			pSorFxd->pDistUints[0] = 'm';
    			pSorFxd->pDistUints[1] = 't';
    		}
    		iBlkSize += sizeof(pSorFxd->pDistUints);
    		
    		pSorFxd->sWaveLength = pSor->iWave * 10; 	//定义要求需要放大十倍
    		iBlkSize += sizeof(pSorFxd->sWaveLength);
    		
    		pSorFxd->lAcquisitionOffset = 0;
    		iBlkSize += sizeof(pSorFxd->lAcquisitionOffset);
    		
    		pSorFxd->lOffsetDist = 0;
    		iBlkSize += sizeof(pSorFxd->lOffsetDist);
    		
    		pSorFxd->sPulesTotal = 1;	//所使用的脉宽总数
    		iBlkSize += sizeof(pSorFxd->sPulesTotal);
    		
    		pSorFxd->pPulseWidth = (short *)calloc(1, sizeof(short));
    		if(NULL == pSorFxd->pPulseWidth)
    		{
    			iRet = -5;
    			LOG(LOG_ERROR, "----InitWriteToSor: pSorFxd: calloc pSorFxd->pPulseWidth Failed!----\n");
    		}
    		pSorFxd->pPulseWidth[0] = pSor->iPulse;
    		iBlkSize += sizeof(pSorFxd->pPulseWidth[0]);
    		
    		pSorFxd->pDataSpacing = (long *)calloc(1, sizeof(long));
    		if(NULL == pSorFxd->pDataSpacing)
    		{
    			iRet = -6;
                LOG(LOG_ERROR, "----InitWriteToSor: pSorFxd: calloc pSorFxd->pDataSpacing Failed!----\n");
            }
    		pSorFxd->pDataSpacing[0] = (long)(fPeriod * 5000);//数据空间:  DS
    		iBlkSize += sizeof(pSorFxd->pDataSpacing[0]);
    		
    		pSorFxd->pPointsNumber = (long *)calloc(1, sizeof(long));
    		if(NULL == pSorFxd->pPointsNumber)
    		{
    			iRet = -7;
                LOG(LOG_ERROR, "----InitWriteToSor: pSorFxd: calloc pSorFxd->pPointsNumber Failed!----\n");
            }
    		pSorFxd->pPointsNumber[0] = pSor->iDataCount;   
    		iBlkSize += sizeof(pSorFxd->pPointsNumber[0]);

    		//群组折射率
    		pSorFxd->lGroupIndex = (long)(pSor->fRefractive * 100000);	//群折射
    		iBlkSize += sizeof(pSorFxd->lGroupIndex);
    		
    		//反向散射系数
    		pSorFxd->sBackscatterCode = (short)(pSor->fBackScattering * 10);
    		iBlkSize += sizeof(pSorFxd->sBackscatterCode);
    		
    		//平均次数
    		pSorFxd->lAveragesNumber = (long)(10000.0 * (pSor->iAverageTime * 1000.0 / pSor->lAcquisitionDist));
    		iBlkSize += sizeof(pSorFxd->lAveragesNumber);

    		//平均时间
    		pSorFxd->usAverageTime = pSor->iAverageTime * 10;	
    		iBlkSize += sizeof(pSorFxd->usAverageTime);

    		//单程总长度(100ps单位)
    		//pSorFxd->lAcquisitionRange = (long)(pSor->lAcquisitionDist * pSor->fRefractiveIndex / fVacuumSpeed * 1E+7);
    		pSorFxd->lAcquisitionRange = (long)(1000.0f * pSor->lAcquisitionDist
    										* pSor->fRefractive / LIGHTSPEED_1000KM_S * 10.0f);
			
    		iBlkSize += sizeof(pSorFxd->lAcquisitionRange);

    		//单程总长度(m)
    		pSorFxd->lAcquisitionDist = (long)(pSor->lAcquisitionDist * 10);//OTDR测量的单程总长度，以距离为单位
    		iBlkSize += sizeof(pSorFxd->lAcquisitionDist);

    		//前面板偏移
    		pSorFxd->lFrontoffset = 0;//前面板偏移，(以100皮秒为单位)
    		iBlkSize += sizeof(pSorFxd->lFrontoffset);
    		
    		pSorFxd->usNoiseFloor = 65535;
    		iBlkSize += sizeof(pSorFxd->usNoiseFloor);
    		
    		pSorFxd->sNoiseScale = 1000;//噪声基值比例因子
    		iBlkSize += sizeof(pSorFxd->sNoiseScale);
    		
    		pSorFxd->usPowerOffset = 0 ;//衰减
    		iBlkSize += sizeof(pSorFxd->usPowerOffset);

    	
    		pSorFxd->usLossThreshold = (unsigned short)(pSor->fSpliceLossThr * 1000);//损耗阈值
    		iBlkSize += sizeof(pSorFxd->usLossThreshold);

    		
    		pSorFxd->usRefThreshold = (unsigned short)((pSor->fReturnLossThr + 14.0f) * -1000);	//反射阈值,由于需保存在UINT16空间中，规定统一偏移14
    		iBlkSize += sizeof(pSorFxd->usRefThreshold);

    		
    		pSorFxd->usEndThreshold = (unsigned short)(pSor->fEndLossThr * 1000);//末端阈值
    		iBlkSize += sizeof(pSorFxd->usEndThreshold);

    		
    		pSorFxd->strTraceType[0] = 'S';	//波形的类型ST=standard trace ,single fiber
    		pSorFxd->strTraceType[1] = 'T';//		BT=Bidirectional trace。
    		iBlkSize += sizeof(pSorFxd->strTraceType);
    		
    		pSorFxd->WndCoordinate.lTopLeftX = 0;
    		pSorFxd->WndCoordinate.lTopLeftY = 65535;
    		pSorFxd->WndCoordinate.lBotRightX = 0;
    		pSorFxd->WndCoordinate.lBotRightY = 65535;
    		iBlkSize += sizeof(pSorFxd->WndCoordinate);
    		
    		pSorMap->pBlockInfo[2].usRevisionNum = VERSION_SOR;
    		pSorMap->pBlockInfo[2].lBlockSize = iBlkSize;
        }
	}

	//初始化SOREVT
	if((iRet == 0) && (pSor->iEventAnalysisFlag))
	{
		iBlkSize = 0;
		pSorEvent = (SOREVT *)calloc(1, sizeof(SOREVT));
		if(NULL == pSorEvent)
		{
			iRet = -8;
            LOG(LOG_ERROR, "----InitWriteToSor: calloc pSorEvent Failed!----\n");
        }
        else {
    		pBuff = pSorMap->pBlockInfo[3].strBlockId;
    		GetString(&pSorEvent->strBlockId, &pBuff);
    		iBlkSize += strlen(pSorEvent->strBlockId) + 1;

    		SOR_EVENTS_TABLE *pEventTable = &(pSor->eventTable);
    		pSorEvent->sEventTotal = pEventTable->iEventsNumber;//关键事件的数量
    		iBlkSize += sizeof(pSorEvent->sEventTotal);
            

    		pSorEvent->pEventInfo = (SOREVT_EVENT *)calloc(1, pSorEvent->sEventTotal*sizeof(SOREVT_EVENT));
    		if(NULL == pSorEvent->pEventInfo)
    		{
    			iRet = -9;
                LOG(LOG_ERROR, "----InitWriteToSor: pSorEvent: calloc pSorEvent->pEventInfo Failed!----\n");
            }
            else {
        		int i = 0;
        		SOREVT_EVENT * pEventInfo = NULL;
        		for (i = 0; i < pSorEvent->sEventTotal; i++)
        		{
        			pEventInfo = pSorEvent->pEventInfo + i;
        			
        			pEventInfo->sEventNum = i;
        			iBlkSize += sizeof(pEventInfo->sEventNum);
        			
        			pEventInfo->lPropagationTime = (long)((double)pEventTable->EventsInfo[i].iBegin * fPeriod / 2); 
        			iBlkSize += sizeof(pEventInfo->lPropagationTime);
        			
        		    pEventInfo->sAttenuationCoef = (short)(pEventTable->EventsInfo[i].fAttenuation * 1000);
        			iBlkSize += sizeof(pEventInfo->sAttenuationCoef);
        			
        			pEventInfo->sEventLoss = (short)(pEventTable->EventsInfo[i].fLoss * 1000);
        			iBlkSize += sizeof(pEventInfo->sEventLoss);
        			
        			pEventInfo->lEventReflectance = (long)(pEventTable->EventsInfo[i].fReflect * 1000);
        			iBlkSize += sizeof(pEventInfo->lEventReflectance);
        		
        			switch (pEventTable->EventsInfo[i].iStyle)
        			{
        				case 0:
        					memcpy(pEventInfo->pEventCode , "0F9999", 6);//衰减事件
        					break;
        				case 1:
        					memcpy(pEventInfo->pEventCode , "0F1999", 6);//增益事件
        					break;
        				case 2:
        					memcpy(pEventInfo->pEventCode , "1F9999", 6);//反射事件
        					break;
        				case 3:
        					memcpy(pEventInfo->pEventCode , "1F0999", 6);//回波事件
        					break;
        				case 10:
        					memcpy(pEventInfo->pEventCode , "0E9999", 6);//衰减事件末端
        					break;
        				case 11:
        					memcpy(pEventInfo->pEventCode , "0E1999", 6);//增益事件末端
        				    break;
        				case 12:
        					memcpy(pEventInfo->pEventCode , "1E9999", 6);//反射事件末端
        					break;
        				case 20:
        					memcpy(pEventInfo->pEventCode , "1O9999", 6);//连续事件
        					break;
        				case 21:
        					memcpy(pEventInfo->pEventCode , "1O1999", 6);//分析结束事件
        					break;
        				default:
        					break;	
        			}
        			iBlkSize += sizeof(pEventInfo->pEventCode);

        			if(pSor->iLossMethodMark == 0)
        			{
        				pEventInfo->pLossMeasurement[0] = 'L';
        				pEventInfo->pLossMeasurement[1] = 'S';
        			}
        			else
        			{
        				pEventInfo->pLossMeasurement[0] = '2';
        				pEventInfo->pLossMeasurement[1] = 'P';
        			}
        			iBlkSize += sizeof(pEventInfo->pLossMeasurement);
        			
        			if(i==0)
        			{
        				pEventInfo->lMarkLoca1 = (long)(pEventTable->EventsInfo[i].iBegin * fPeriod / 2);
        				pEventInfo->lMarkLoca2 = (long)(pEventTable->EventsInfo[i].iBegin * fPeriod / 2);
        				pEventInfo->lMarkLoca3 = (long)(pEventTable->EventsInfo[i].iEnd * fPeriod / 2);
        				pEventInfo->lMarkLoca4 = (long)(pEventTable->EventsInfo[i+1].iBegin* fPeriod / 2);
        				pEventInfo->lMarkLoca5 = GetMaxUint16_index(pSor->pData 
        				    + (long)pEventTable->EventsInfo[i].iBegin,(long)pEventTable->EventsInfo[i].iEnd 
        				    - (long)pEventTable->EventsInfo[i].iBegin)* fPeriod/2+pEventInfo->lMarkLoca2;
        			}
        			else if((i>0)&&(i<pSorEvent->sEventTotal-1))
        			{
        				pEventInfo->lMarkLoca1 = (long)(pEventTable->EventsInfo[i-1].iEnd* fPeriod/2);
        				pEventInfo->lMarkLoca2 = (long)(pEventTable->EventsInfo[i].iBegin* fPeriod/2);
        				pEventInfo->lMarkLoca3 = (long)(pEventTable->EventsInfo[i].iEnd * fPeriod/2);
        				pEventInfo->lMarkLoca4 = (long)(pEventTable->EventsInfo[i+1].iBegin* fPeriod/2);
        				pEventInfo->lMarkLoca5 = GetMaxUint16_index(pSor->pData 
        				    + (long)pEventTable->EventsInfo[i].iBegin,(long)pEventTable->EventsInfo[i].iEnd 
        				    - (long)pEventTable->EventsInfo[i].iBegin)* fPeriod/2+pEventInfo->lMarkLoca2;
        			}
        			else
        			{
        				pEventInfo->lMarkLoca1 = (long)(pEventTable->EventsInfo[i-1].iEnd* fPeriod/2);
        				pEventInfo->lMarkLoca2 = (long)(pEventTable->EventsInfo[i].iBegin* fPeriod/2);
        				pEventInfo->lMarkLoca3 = (long)(pEventTable->EventsInfo[i].iEnd * fPeriod/2);
        				pEventInfo->lMarkLoca4 = (long)(pEventTable->EventsInfo[i].iEnd * fPeriod/2);
        				pEventInfo->lMarkLoca5 = GetMaxUint16_index(pSor->pData 
        				    + (long)pEventTable->EventsInfo[i].iBegin,(long)pEventTable->EventsInfo[i].iEnd 
        				    - (long)pEventTable->EventsInfo[i].iBegin)* fPeriod/2+pEventInfo->lMarkLoca2;
        			}
        			iBlkSize += sizeof(pEventInfo->lMarkLoca4) * 5;

        			pBuff = "";
        			
                    if(pEventTable->iIsThereASpan == 1)
                    {
            			//起始位置
                        if((pEventTable->iLaunchFiberIndex <= 0 && i==0)
                            || (pEventTable->iLaunchFiberIndex > 0 && i==pEventTable->iLaunchFiberIndex))
                        {
            			    pBuff = "START";
            			}    
                        //终点位置
               			if((pEventTable->iRecvFiberIndex <= 0 && i == (pSorEvent->sEventTotal - 1))
                            || (pEventTable->iRecvFiberIndex > 0 && i == pEventTable->iRecvFiberIndex))
                        {
                            pBuff = "END";
                        }
                    }
                    else
                    {
                        if(pEventTable->iLaunchFiberIndex >= 0 && i==pEventTable->iLaunchFiberIndex)
                        {
                            pBuff = "START";
                        }

                        if(pEventTable->iLaunchFiberIndex < 0 && i == 0)
                        {
                            pBuff = "START";
                        }
                    }

        			GetString(&pEventInfo->strComment, &pBuff);

        			iBlkSize += strlen(pEventInfo->strComment) + 1;
        		}
            }
    		//youhua
    		pSorEvent->lEndtoendLoss = (long)(1 * 1000); //链路端到端损耗
    		iBlkSize += sizeof(pSorEvent->lEndtoendLoss);
    		
    		pSorEvent->EndtoendMarker.lStartPosition = (long)(300 * fPeriod / 2); //端到端损耗的标记位置
    		pSorEvent->EndtoendMarker.lFinishPosition = (long)(400 * fPeriod / 2);
    		iBlkSize += sizeof(pSorEvent->EndtoendMarker);
    		
    		pSorEvent->usReturnLoss = (unsigned short)(1 * 1000);	//回波损耗
    		iBlkSize += sizeof(pSorEvent->usReturnLoss);
    		
    		pSorEvent->ReturnMarker.lStartPosition = (long)(100 * fPeriod / 2); //回波损耗的标记位置
    		pSorEvent->ReturnMarker.lFinishPosition = (long)(200 * fPeriod / 2);
    		iBlkSize += sizeof(pSorEvent->EndtoendMarker);
        }
		pSorMap->pBlockInfo[3].usRevisionNum = VERSION_SOR;
		pSorMap->pBlockInfo[3].lBlockSize = iBlkSize;
	}
	else
	{
		iBlkSize = 0;
	}


	//初始化SORLNK
	if(iRet == 0)
	{
		
	}

	//初始化SORDAT
	if(iRet == 0)
	{
		int iDataPos = 0;
		iBlkSize = 0;
		pSorData = (SORDAT *)calloc(1, sizeof(SORDAT));
		if (NULL == pSorData)
		{	
			iRet = -10;
            LOG(LOG_ERROR, "----InitWriteToSor: pSorEvent: calloc pSorData Failed!----\n");
        }
        else {
    		if(pSor->iEventAnalysisFlag)
    		{
    			iDataPos = 4;
    		}
    		else
    		{
    			iDataPos = 3;
    		}

    		pBuff = pSorMap->pBlockInfo[iDataPos].strBlockId;
    		GetString(&pSorData->strBlockId, &pBuff);
    		iBlkSize += strlen(pSorData->strBlockId) + 1;

    		pSorData->lPointsTotal = pSor->iDataCount;//数据点的总数
    		iBlkSize += sizeof(pSorData->lPointsTotal);
    		
    		pSorData->sFactorsTotal = 1;//比列因子的总数
    		iBlkSize += sizeof(pSorData->sFactorsTotal);
    		
    		pSorData->pFactorInfo = (SORDAT_SCALEFACTOR *)calloc(pSorData->lPointsTotal, sizeof(SORDAT_SCALEFACTOR));//比例因子信息
    		if(NULL == pSorData->pFactorInfo)
    		{	
    			iRet = -11;
                LOG(LOG_ERROR, "----InitWriteToSor: pSorData: calloc pSorData->pFactorInfo Failed!----\n");
            }
    		pSorData->pFactorInfo->lPointsTotal = pSorData->lPointsTotal;
    		iBlkSize += sizeof(pSorData->pFactorInfo->lPointsTotal);
    		
    		pSorData->pFactorInfo->sScaleFactor = pSor->iScaleFactor;
    		iBlkSize += sizeof(pSorData->pFactorInfo->sScaleFactor);
    		
    		pSorData->pFactorInfo->pDataPoints = (unsigned short *)calloc(1, sizeof(unsigned short) * pSorData->lPointsTotal);
    		if(NULL == pSorData->pFactorInfo->pDataPoints)
    		{	
    			iRet = -12;
                LOG(LOG_ERROR, "----InitWriteToSor: pSorData: calloc pSorData->pFactorInfo->pDataPoints Failed!----\n");
            }

    		memcpy(pSorData->pFactorInfo->pDataPoints, pSor->pData, sizeof(unsigned short) * pSorData->lPointsTotal);

    		//数据翻转
    		ChangeDataValue(pSorData);
    		iBlkSize += sizeof(unsigned short) * pSorData->lPointsTotal;
        }
		pSorMap->pBlockInfo[iDataPos].usRevisionNum = VERSION_SOR;
		pSorMap->pBlockInfo[iDataPos].lBlockSize = iBlkSize;
	}

	//初始化SORSPC
	if(iRet == 0)
	{

	}

	//初始化SORCHK
	if(iRet == 0)
	{
		iBlkSize = 0;
		int iDataPos = 0;
		if (pSor->iEventAnalysisFlag)
		{
			iDataPos = 5;
		}
		else
		{
			iDataPos = 4;
		}
		pSorCheck = (SORCHK *)calloc(1, sizeof(SORCHK));
		if (NULL == pSorCheck)
		{
			iRet = -13;
            LOG(LOG_ERROR, "----InitWriteToSor: calloc pSorCheck Failed!----\n");
        }
		else
		{	
			pBuff = pSorMap->pBlockInfo[iDataPos].strBlockId;
			GetString(&pSorCheck->strBlockId, &pBuff);
			iBlkSize += strlen(pSorCheck->strBlockId) + 1;
			iBlkSize += 2;
	
			pSorMap->pBlockInfo[iDataPos].usRevisionNum = VERSION_SOR;
			pSorMap->pBlockInfo[iDataPos].lBlockSize = iBlkSize;
		}
	}
	
	if(iRet < 0)
		ReleaseMemSor();

	return iRet;
}


/***
  * 功能：
		把从SOR文件读取到的数据，设置到数据结构中
  * 参数：
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
 ***/
static int SorData2pSor(char *FileName, PSOR_DATA pSor)
{
	//返回值
	int iRet = 0;
	double fPeriod = 0.0f;
	char *suffix = NULL;
	
	pSor->iWave = pSorFxd->sWaveLength / 10;
	pSor->iPulse = pSorFxd->pPulseWidth[0];

	memset(pSor->cFileName, 0, FILE_NAME_MAX);
	strncpy(pSor->cFileName,FileName,strlen(FileName));
    suffix = strrchr(pSor->cFileName, '.');
    if(suffix && strcasecmp(suffix, ".SOR"))
    {
        *suffix = 0;
        strcat(pSor->cFileName, ".SOR");
    }
    
	pSor->fRefractive = (float)(pSorFxd->lGroupIndex / 100000.0f);
	pSor->fBackScattering = (float)(pSorFxd->sBackscatterCode / 10.0f);

	pSor->fSpliceLossThr = (float)(pSorFxd->usLossThreshold / 1000.0f);
	pSor->fReturnLossThr = (float)((pSorFxd->usRefThreshold / -1000.0f) - 14.0f);
	pSor->fEndLossThr = (float)(pSorFxd->usEndThreshold / 1000.0f);

	fPeriod = 2.0f * pSorFxd->pDataSpacing[0] / 1e+8;
	pSor->fSmpRatio = (1000 / (fPeriod*1000)/ 40);

    pSor->pDataSpacing = (long *)calloc(1, sizeof(long));
    pSor->pDataSpacing[0] = pSorFxd->pDataSpacing[0];

    LOG(LOG_INFO, "pSorFxd->lAcquisitionDist--- %ld \n", pSorFxd->lAcquisitionDist);
    LOG(LOG_INFO, "pSorFxd->lAcquisitionRange %ld \n", pSorFxd->lAcquisitionRange);
    LOG(LOG_INFO, "pSorFxd->pDataSpacing[0] %ld \n", pSorFxd->pDataSpacing[0]);
    LOG(LOG_INFO, "pSor->lAcquisitionDist--- %ld \n", pSor->lAcquisitionDist);

    /*
	if(pSorFxd->lAcquisitionDist == 0)
	{
		if(pSorFxd->lAcquisitionRange)
		{
			if(strncmp(pSorFxd->pDistUints, "mt",2) == 0)
			{
				pSorFxd->lAcquisitionDist = pSorFxd->lAcquisitionRange * LIGHTSPEED_1000KM_S / pSor->fRefractive;
			}
			else if(strncmp(pSorFxd->pDistUints, "km",2) == 0)
			{
				pSorFxd->lAcquisitionDist = pSorFxd->lAcquisitionRange * LIGHTSPEED_1000KM_S / pSor->fRefractive / 1000;
			}
			else
			{
				;
			}
		}
		else
		{
			printf("----pSor->lAcquisitionDist Data Error!!!----\n");
		}
	}

	pSor->lAcquisitionDist = pSorFxd->lAcquisitionDist / 10;
	*/

	if(pSorFxd->lAcquisitionRange > 0)
	{
		//防止计算误差小于10m，且影响量程的计算
		float fTmp = 1.0f * pSorFxd->lAcquisitionRange / 1e+4 * LIGHTSPEED_1000KM_S / pSor->fRefractive;
		long lTmp = (long)round(fTmp / 20.0f);
		pSor->lAcquisitionDist = lTmp * 20;
	}
	else
	{
		pSor->lAcquisitionDist = pSorFxd->lAcquisitionDist / 10;

	}
	
	if(pSorFxd->usAverageTime < 1)
	{
		if(pSorFxd->lAveragesNumber)
		{
			pSor->iAverageTime = (int)(pSorFxd->lAveragesNumber * pSor->lAcquisitionDist / 1e+7);	
		}
		else
		{
            LOG(LOG_ERROR, "----pSor->iAverageTime Data Error!!!----\n");
        }
	}
	else
	{
		pSor->iAverageTime = pSorFxd->usAverageTime / 10;
	}
	
		
	if(pSorData)
	{
		pSor->iDataCount = pSorData->lPointsTotal;
		pSor->iScaleFactor = pSorData->pFactorInfo->sScaleFactor;
		//数据翻转
		ChangeDataValue(pSorData);		
		memcpy(pSor->pData, pSorData->pFactorInfo->pDataPoints, sizeof(unsigned short) * pSorData->pFactorInfo->lPointsTotal);
	}
	else
	{
		pSor->iDataCount = pSorFxd->pPointsNumber[0];
	}
	
	//事件
	if(pSorEvent)
	{
		int i = 0;
		float fTotalLoss = 0.0;
		int iSelfFlag = 0;
		double ps_fperiod = fPeriod * 10000;
		
		if (!strncmp(pSorSup->strMainframeId,"F2",2)
		    || !strncmp(pSorSup->strMainframeId,"Vi",2)
		    || !strncmp(pSorSup->strMainframeId,"Mi",2)
		    || !strncmp(pSorSup->strMainframeId,"IN",2)
		   )
			iSelfFlag = 0;
		else
			iSelfFlag = 1;
			
		pSor->iEventAnalysisFlag = 1;
		pSor->eventTable.iEventsNumber = pSorEvent->sEventTotal;

        pSor->eventTable.iLaunchFiberIndex = 0;
        pSor->eventTable.iRecvFiberIndex = 0;
        int startIndex = -1;
        int endIndex = -1;
		for (i = 0; i < pSorEvent->sEventTotal; i++)
		{
		    float temp = 0.0;
			SOREVT_EVENT *pEventInfo = pSorEvent->pEventInfo + i;

			if (i==0)
			{
				pSor->eventTable.EventsInfo[i].iBegin = 0;
				pSor->eventTable.EventsInfo[i].fLoss = 0;
                pSor->eventTable.EventsInfo[i].fReflect = pEventInfo->lEventReflectance / 1000.0f;
				if(pSor->eventTable.EventsInfo[i].fReflect < -80)
				{
					pSor->eventTable.EventsInfo[i].fReflect = 0;
				}
				pSor->eventTable.EventsInfo[i].fAttenuation = pEventInfo->sAttenuationCoef / 1000.0;
				pSor->eventTable.EventsInfo[i].iStyle = char_to_int(pEventInfo->pEventCode);
				pSor->eventTable.EventsInfo[i].fTotalLoss = 0;
				fTotalLoss = 0;
			}
			else
			{
				pSor->eventTable.EventsInfo[i].iBegin = (int)((double)(pEventInfo->lPropagationTime*2 / ps_fperiod) + 0.5f);// change by zh
				pSor->eventTable.EventsInfo[i].fLoss = pEventInfo->sEventLoss / 1000.0f;
				pSor->eventTable.EventsInfo[i].fReflect = pEventInfo->lEventReflectance / 1000.0f;
				
				if(pSor->eventTable.EventsInfo[i].fReflect < -80)
				{
					pSor->eventTable.EventsInfo[i].fReflect = 0;
				}
				
				if(iSelfFlag == 0)
				{
					pSor->eventTable.EventsInfo[i].iStyle = char_to_int_self(pEventInfo->pEventCode);
				}
				else
				{
					pSor->eventTable.EventsInfo[i].iStyle = char_to_int(pEventInfo->pEventCode);
				}
				pSor->eventTable.EventsInfo[pSorEvent->sEventTotal-1].fLoss = 0.0f;
				pSor->eventTable.EventsInfo[i].iEnd = (int)(pEventInfo->lMarkLoca2 *2 / ps_fperiod);
				pSor->eventTable.EventsInfo[i].fAttenuation = pEventInfo->sAttenuationCoef / 1000.0;
				
                double f2d = pSor->pDataSpacing[0] / 10000.0f * LIGHTSPEED_M_S / pSor->fRefractive / 1e+10;
				temp = (float) (pSor->eventTable.EventsInfo[i].iBegin - pSor->eventTable.EventsInfo[i-1].iBegin)  * f2d / 1000;//* ps_fperiod * 0.015 / pSor->fRefractive / 1000.0 ;
                double fSectionLoss = Float2FloatBy3Precision(pSor->eventTable.EventsInfo[i-1].fAttenuation) * temp;
				if (iSelfFlag == 0)
					fTotalLoss = pSor->eventTable.EventsInfo[i-1].fLoss 
					    + Float2FloatBy3Precision(fSectionLoss) + fTotalLoss;
				else
					fTotalLoss = pSor->eventTable.EventsInfo[i-1].fLoss 
					    + (pSor->eventTable.EventsInfo[i].fAttenuation) * temp + fTotalLoss;

                fTotalLoss = Float2FloatBy3Precision(fTotalLoss);
				pSor->eventTable.EventsInfo[i].fTotalLoss = fTotalLoss;
				if (strncmp(pEventInfo->pLossMeasurement, "LS", 2) == 0)
				{
					pSor->iLossMethodMark = 0;
				}
				else if (strncmp(pEventInfo->pLossMeasurement, "2P", 2) == 0)
				{
					pSor->iLossMethodMark = 1;
				}
				else
				{
					pSor->iLossMethodMark = 0;
				}
				//需要重新计算（iBegin+pulse/10/fSmpIntval）
                pSor->eventTable.EventsInfo[i].iEnd = pSor->eventTable.EventsInfo[i].iBegin+pSor->iPulse/10/f2d;
			}
            
            if(strcmp(pEventInfo->strComment, "START") == 0)
            {
                if(i != 0)
                {
                    pSor->eventTable.iLaunchFiberIndex = i;
                }
                startIndex = i;
            }
            
            if(strcmp(pEventInfo->strComment, "END") == 0)
            {
                if(i != (pSorEvent->sEventTotal - 1))
                {
                    pSor->eventTable.iRecvFiberIndex = i;
                }
                endIndex = i;
            }

		}

		if((startIndex != -1 && endIndex == -1)
		    || (startIndex == -1 && endIndex != -1))
		{
            pSor->eventTable.iIsThereASpan = 0;
            pSor->eventTable.iLaunchFiberIndex = startIndex == -1 ? endIndex : startIndex;
            pSor->eventTable.iRecvFiberIndex = pSor->eventTable.iLaunchFiberIndex;
		}
		else
		{
            pSor->eventTable.iIsThereASpan = 1;
		}

		for (i = 1; i < pSorEvent->sEventTotal; i++)
		{
            if(pSor->eventTable.EventsInfo[i].iStyle >= 10)
            {
				break;
			}
		}

		for(i=i+1;i<pSorEvent->sEventTotal;i++)
		{
			pSor->eventTable.EventsInfo[i].iStyle = 3;
		}
	}

	strncpy(pSor->generalInfo.pComment,pSorGen->strComment,strlen(pSorGen->strComment));
	strncpy(pSor->generalInfo.pCompanyName,pSorSup->strSupplierName,strlen(pSorSup->strSupplierName));
	strncpy(pSor->generalInfo.pOperator,pSorGen->strOperator,strlen(pSorGen->strCableCode));
	strncpy(pSor->generalInfo.pCableID,pSorGen->strCableId,strlen(pSorGen->strCableCode));
	strncpy(pSor->generalInfo.pFiberID,pSorGen->strFiberId,strlen(pSorGen->strCableCode));
	strncpy(pSor->generalInfo.pCableCode,pSorGen->strCableCode,strlen(pSorGen->strCableCode));
	strncpy(pSor->generalInfo.pStartPoint,pSorGen->strOriginatingLoca,strlen(pSorGen->strCableCode));
	strncpy(pSor->generalInfo.pEndPoint,pSorGen->strTerminatingLoca,strlen(pSorGen->strCableCode));
	pSor->generalInfo.sFiberType = pSorGen->sFiberType;
	pSor->generalInfo.pDataFlag[0] = pSorGen->pDataFlag[0];
	pSor->generalInfo.pDataFlag[1] = pSorGen->pDataFlag[1];

	return iRet;
}


/***
  * 功能：
  		保存测试数据到SOR文件中
  * 参数：
  		1、char *pSorDirPath:	即将保存的完整路径名
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
		返回值错误描述:       	返回值
		传入指针为空:         	-1
		打开文件失败:         	-2
		InitWriteToSor:         -3 
  		WriteMap:				-4
  		WriteGeneral:			-5
  		WriteSupplier:			-6
  		WriteFixed:				-7
  		WriteKeyevent:			-8
  		WriteDatapoint:			-9
  		WriteChecksum:			-10
  		WriteCrc:				-11,-12,-13,-14,-15,-16
  		
 ***/
int WriteToSor(PSOR_DATA pSor)
{
	//返回值
	int iRet = 0;
	//临时变量
	int iSorFd = -1;

	if (iRet == 0)
	{
		//参数检查
		if (NULL == pSor)
		{
            LOG(LOG_ERROR, "----Sorry, pSor is NULL !!!---\n");
            iRet = -1;
		}
	}

	if (iRet == 0)
	{	
		//创建、打开文件
		iSorFd = open(pSor->cFileName, O_RDWR | O_CREAT, 0666);
		if (iSorFd < 0)
		{
            LOG(LOG_ERROR, "----Open|Create SorFile Error!--- pSor->cFileName= %s----\n", pSor->cFileName);
            iRet = -2;
		}
	}

	if (iRet == 0)
	{
		//设置各模块的成员
		iRet = InitWriteToSor(pSor);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "----InitWriteToSor Error!----iRet = %d------\n", iRet);
            iRet = -3;
		}
	}

	if (iRet == 0)
	{
		//写入MAP
		iRet = WriteMap(iSorFd, pSorMap);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "-----WriteMap Error!----iRet = %d-------\n", iRet);
            iRet = -4;
		}
	}

	if (iRet == 0)
	{
		//写入general块
		iRet = WriteGeneral(iSorFd, pSorMap, pSorGen);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "------WriteGeneral Error!----iRet = %d-------\n", iRet);
            iRet = -5;
		}
	}

	if (iRet == 0)
	{
		//写入supplier块
		iRet = WriteSupplier(iSorFd, pSorMap, pSorSup);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "------WriteSupplier Error!----iRet = %d-------\n", iRet);
            iRet = -6;
		}
	}

	if (iRet == 0)
	{
		//写入fixed块
		iRet = WriteFixed(iSorFd, pSorMap, pSorFxd);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "------WriteFixed Error!----iRet = %d-------\n", iRet);
            iRet = -7;
		}
	}

	if (iRet == 0)
	{
		//写入key块
		iRet = WriteKeyevent(iSorFd, pSorMap, pSorEvent);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "------WriteKeyevent Error!----iRet = %d-------\n", iRet);
            iRet = -8;
		}
	}

	if (iRet == 0)
	{
		//写入datapoint块
		iRet = WriteDatapoint(iSorFd, pSorMap, pSorData);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "------WriteDatapoint Error!----iRet = %d-------\n", iRet);
            iRet = -9;
		}
	}

	if (iRet == 0)
	{
		//写入CheckSum块
		iRet = WriteChecksum(iSorFd, pSorMap, pSorCheck);
		if (iRet < 0)
		{
            LOG(LOG_ERROR, "------WriteChecksum Error!----iRet = %d-------\n", iRet);
            iRet = -10;
		}
	}
	
	if ((iRet == 0) || (iRet < -3))
	{
		if (iSorFd > 0)
		{
			close(iSorFd);//关闭文件
		}
		
		ReleaseMemSor();//释放资源
	}

	if(iRet < -2)
	{
		//写的数据不完整或错误，要做clear
		int iErr = 0;
		iErr = remove(pSor->cFileName);
		if(iErr < 0)
			remove(pSor->cFileName);
        LOG(LOG_ERROR, "-------Write To Sor Failed!!! iRet = %d-----\n", iRet);
    }

	if(iRet ==  0)
	{
		unsigned char *buf = NULL;
		unsigned short crc = 0;
		int iErr = 0;
		long size = 0;
		struct stat SorFile;
		
		iSorFd = -1;

		iSorFd = open(pSor->cFileName, O_RDWR);
		if (iSorFd < 0)
		{
            LOG(LOG_ERROR, "---WriteCrc----Open SorFile Failed----\n");
            iRet = -11;
		}
		iErr = stat(pSor->cFileName,&SorFile);
		if (iErr < 0)
		{
            LOG(LOG_ERROR, "---WriteCrc----Stat SorFile Failed----\n");
            iRet = -12;
		}

		size = SorFile.st_size;
		buf = (unsigned char *)calloc(1,size);
		if(!buf)
		{
            LOG(LOG_ERROR, "----WriteCrc calloc buf failed!!!--\n");
            iRet = -13;
		}
		
		iErr = read(iSorFd, buf, size);
		if (-1 == iErr)
        {
            //文件读取失败
            LOG(LOG_ERROR, "---WriteCrc----Read SorFile Failed----\n");
            iRet = -14;
        }
		
		crc = crc16(buf,size-2);
		
		iErr = lseek(iSorFd, (size-2), SEEK_SET);
		if (-1 == iErr)
        {
            //文件指针定位失败
            LOG(LOG_ERROR, "---WriteCrc----lseek SorFile Failed----\n");
            iRet = -15;
        }
		iErr = write(iSorFd,&crc,2);
		if (-1 == iErr)
        {
            //文件写失败
            LOG(LOG_ERROR, "---WriteCrc----write crc checksum Failed----\n");
            iRet = -16;
        }
		
		close(iSorFd);
		
		free(buf);
		buf = NULL;

        LOG(LOG_INFO, "------Write SorFile Success...oh yeah!!!-------\n");
    }

	return iRet;
}


/***
  * 功能：
	  从SOR文件读取数据
  * 参数：
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
  		返回值错误描述:       		返回值
		打开文件失败:         		-1
		获取文件信息失败:         	-2
		文件size太小:               -3
		申请每个块的资源失败:       -4
		ReadMapBlock失败:			-5
		ReadGeneral失败:			-6
		ReadSupplier失败:			-7
		ReadFixed失败:				-8
		Event和Dada块都为空:		-9

 ***/
int ReadFromSor(char *FileName, PSOR_DATA pSor)
{
	//返回值
	int iRet = 0;
	//临时变量
	int iSorFd = -1;
    struct stat SorFileStatus;

	int iErr = 0;
	
	
	if (iRet == 0)
	{
		//创建、打开文件
		iSorFd = open(FileName, O_RDONLY);
		if (iSorFd < 0)
		{
            LOG(LOG_ERROR, "----ReadFromSor: open FileName Failed!!!  FileName = %s--\n", FileName);
            iRet = -1;
		}
	}

	iErr = stat(FileName, &SorFileStatus);
	if (iErr == -1)
	{
        LOG(LOG_ERROR, "----ReadFromSor: stat FileName Failed!!!  %s\n", strerror(errno));
        iRet = -2; 
	}
	else
	{
		//文件小于200字节,该文件失效
		if(SorFileStatus.st_size <= 200)
		{
            LOG(LOG_ERROR, "----ReadFromSor:  File size very  small!!!---size = %ld--\n", SorFileStatus.st_size);
            iRet = -3; 
		}
	}

	
	if (iRet == 0)
	{
		//分配资源
		iRet = InitReadFromSor();
		if (iRet < 0)
		{
			iRet = -4;
		}
	}

	if (iRet == 0)
	{
		//读取MAP块
		if (NULL == ReadMapBlock(iSorFd, pSorMap))
		{
			free(pSorMap);
			pSorMap = NULL;
            LOG(LOG_ERROR, "----ReadFromSor: ReadMapBlock:  Error!!!-----\n");
            iRet = -5;
		}
	}

	if (iRet == 0)
	{
		//读取general块
		if (NULL == ReadGeneral(iSorFd, pSorMap, pSorGen))
		{
			free(pSorGen);
			pSorGen = NULL;
            LOG(LOG_ERROR, "----ReadFromSor: ReadGeneral:  Error!!!-----\n");
            iRet = -6;
		}
	}

	if (iRet == 0)
	{
		//读取supplier块
		if (NULL == ReadSupplier(iSorFd, pSorMap, pSorSup))
		{
			free(pSorSup);
			pSorSup = NULL;
            LOG(LOG_ERROR, "----ReadFromSor: ReadSupplier:  Error!!!-----\n");
            iRet = -7;
		}
	}

	if (iRet == 0)
	{
		//读取fixed块
		if (NULL == ReadFixed(iSorFd, pSorMap, pSorFxd))
		{
			free(pSorFxd);
			pSorFxd = NULL;
            LOG(LOG_ERROR, "----ReadFromSor: ReadFixed:  Error!!!-----\n");
            iRet = -8;
		}
	}

	if (iRet == 0)
	{
		//读取keyevents块
		if (NULL == ReadKeyevent(iSorFd, pSorMap, pSorEvent))
		{
			free(pSorEvent);
			pSorEvent = NULL;
            LOG(LOG_ERROR, "----ReadFromSor: ReadKeyevent:  Error!!!-----\n");
        }
	}

	if (iRet == 0)
	{
		//读取link块 可选块
		if (NULL == ReadLink(iSorFd, pSorMap, pSorLnk))
		{
			free(pSorLnk);
			pSorLnk = NULL;
		}
	}

	if (iRet == 0)
	{
		//读取datapoint块
		if (NULL == ReadDatapoints(iSorFd, pSorMap, pSorData))
		{
			free(pSorData);
			pSorData = NULL;
            LOG(LOG_ERROR, "----ReadFromSor: ReadDatapoints:  Error!!!-----\n");
        }
	}

	if (iRet == 0)
	{
		//读取special块 可选块
		if (NULL == ReadSpecial(iSorFd, pSorMap, pSorSpec))
		{
			free(pSorSpec);
			pSorSpec = NULL;
		}
	}

	if (iRet == 0)
	{
		//读取checksum块 可选块
		if (NULL == ReadChecksum(iSorFd, pSorMap, pSorCheck))
		{
			free(pSorCheck);
			pSorCheck = NULL;
		}
	}

	if(!pSorEvent && !pSorData)
	{
		iRet = -9;
        LOG(LOG_ERROR, "----ReadFromSor:  EventBlock = NULL , SorData = NULL !!!-----\n");
    }

	if (iSorFd > 0)
	{
		//关闭文件
		close(iSorFd);
	}
	
	if(iRet == 0)
	{
		SorData2pSor(FileName, pSor);

        LOG(LOG_INFO, "Read from sor Success...oh yeah!\n");
    }

	ReleaseMemSor();

	return iRet;
}

//将传入的值按三位小数精度四舍五入截断后返回
static double Float2FloatBy3Precision(double data)
{
    double offset = 0.0005;
    double ret = 0.0f;
    
    if(data < 0)
    {
        ret = data - offset;
    }
    else
    {
        ret = data + offset;
    }

    int temp = ret * 1000;
    ret = (double)temp / 1000.0f;
    
    return ret;
}



