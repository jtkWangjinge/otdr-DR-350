#include "app_solafile.h"
#include "iniparser.h"
#include <time.h>
#include "app_frmfilebrowse.h"
#define SOLA_FILE_DEBUG                     1
#define TOTAL_SOLA_EVENT_MAGIC              0x00545345  //"TSEn"
#define WAVE_SOLA_EVENT_MAGIC               0x00575345  //"WSE"
#define VERSIONED_MAGIC(_magic, _version)   ((_magic) | ((_version) << 24))

#define SOLA_EVENT_INFO_KEY(_key, _section, _name,_index)   sprintf(_key, "%s:%s_%d", _section, _name, _index)
#define SOLA_NORMAL_KEY(_key, _section, _name)              sprintf(_key, "%s:%s", _section, _name)



#define SOLA_VER_10    10     //1.0版本
#define SOLA_VER_11    11     //1.1版本

#define SOLA_SECTION_TOTAL       "Total"
#define SOLA_SECTION_MAIN        "Main"

#define SOLA_ITEM_VERSION        "Version"
#define SOLA_ITEM_TIME           "Time"
#define SOLA_ITEM_TESTTIME       "TestTime"
#define SOLA_ITEM_COUNT          "Count"
#define SOLA_ITEM_BEGIN          "Begin"
#define SOLA_ITEM_END            "End"
#define SOLA_ITEM_STYLE          "Style"
#define SOLA_ITEM_PULSE          "Pulse"
#define SOLA_ITEM_EVTPOS         "EventsPosition"
#define SOLA_ITEM_EVTENDPOS      "EventsEndPosition"
#define SOLA_ITEM_REFLECT        "Reflect"
#define SOLA_ITEM_ATTENUATION    "Attenuation"
#define SOLA_ITEM_LOSS           "Loss"
#define SOLA_ITEM_TOTALLOSS      "TotalLoss"
#define SOLA_ITEM_EVTRTNLOSS     "EventReturnLoss"
#define SOLA_ITEM_CONTINUELENGTH "ContinueLength"
#define SOLA_ITEM_WAVECNT        "waveLengthListSize"
#define SOLA_ITEM_WAVELIST       "waveLengthList"
#define SOLA_ITEM_RETURNLOSS     "returnLoss"
#define SOLA_ITEM_LINKLOSS       "totalLoss"
#define SOLA_ITEM_LAUNCHFIBER    "LaunchFiberIndex"
#define SOLA_ITEM_RECVFIBER      "RecvFiberIndex"


#define SOLA_ITEM_START          "Start"
#define SOLA_ITEM_CONTINUEATTENUA "ContinueAttenuation"
#define SOLA_ITEM_DELTA          "Delta"
#define SOLA_ITEM_WAVESYMBOL     "WaveSymbol"
#define SOLA_ITME_SMALLREFLECTFLG "SmallReflectFlag"
#define SOLA_ITME_SMALLLOSSFLG   "SmallLossFlag"



static const char* gWaveSection[WAVE_NUM] = { "Wave1310", "Wave1550", "Wave1625" };


extern SOLA_MESSAGER*       pSolaMessager;

//读取主section
static int ReadMainSection(dictionary* dict, time_t *testTime);
//读取事件信息
static int ReadEventInfo(dictionary* dict, const char* section, int index, EVENTS_INFO* pEvent, int ver);
//读取时间表
static int ReadEventTable(dictionary* dict, const char* section, EVENTS_TABLE* pEventTable, int ver);
//读取总的事件表
static int ReadTotalEventTable(dictionary* dict, SolaEvents* pSolaEvent);
//读取各个波长的事件表
static int ReadWaveEventTable(dictionary* dict, SolaEvents* pSolaEvent);

//创建并写入main section
static void WriteMainSection(FILE* out, time_t testTime);
//写入事件信息
static void WriteEventInfo(FILE* out, int index, const EVENTS_INFO* pEvent);
//写入事件表section
static void WriteEventTable(FILE* out, const char* section, const EVENTS_TABLE* pEventTable);
//写入总事件表section
static void WriteTotalSection(FILE* out, const SolaEvents* pSolaEvent);
//写入各个波长的section
static void WriteWaveEventTable(FILE* out, const SolaEvents* pSolaEvent);
//判断内容是否是sola文件
static int isSolaFile(const char* pszPath);


//老版本的读取函数
int ReadSola(const char* fileName, SolaEvents* sola)
{
    int iErr = SolaAccessFailed;
    
    FILE* fp = fopen(fileName, "r");
    if (fp)
    {
        //判断当前是否能读取sola结构
        (void)fseek(fp, 0l, SEEK_END);
        long size = ftell(fp);
        if (size != sizeof(SolaEvents)) {
            fprintf(stderr, "Read SOLA file(%s) Error:SOLA file version not matched\n", fileName); 
            return SolaBadVersion;
        }

        //读取数据
        int ret = fread(sola, 1, sizeof(SolaEvents)-sizeof(sola->absolutePath), fp);
        if (ret != sizeof(SolaEvents))
        {
            iErr = SolaUnknownError;   
        }
        else
        {
            iErr = SolaOk;  
        }
        fclose(fp);
    }
    return iErr;
}


static void ReadSolaFileData_V11(dictionary* dict, SolaEvents* pSolaEvent)
{
    char key[64] = {0};
    int cnt = 0;
    int i = 0;
    int j = 0;
    char* pTmpStr = NULL;

    SOLA_NORMAL_KEY(key, SOLA_SECTION_TOTAL, SOLA_ITEM_LAUNCHFIBER);
    pSolaEvent->EventTable.iLaunchFiberIndex = iniparser_getint(dict, key, 0);
    SOLA_NORMAL_KEY(key, SOLA_SECTION_TOTAL, SOLA_ITEM_RECVFIBER);
    pSolaEvent->EventTable.iRecvFiberIndex = iniparser_getint(dict, key, 0);
    
    
    SOLA_NORMAL_KEY(key, SOLA_SECTION_TOTAL, SOLA_ITEM_WAVECNT);
    cnt = iniparser_getint(dict, key, 0);
    for(i = 0; i < cnt; i++)
    {
        SOLA_EVENT_INFO_KEY(key, SOLA_SECTION_TOTAL, SOLA_ITEM_WAVELIST,i);
        pTmpStr = (char*)iniparser_getstring(dict, key, "NULL");
        for(j=0;j<WAVE_NUM-1;j++)
        {
            if(0 == strcmp(pTmpStr, gWaveSection[j]+4))
            {
                pSolaEvent->WaveIsSetted[j] = 1;
                sprintf(key, "%s:%s_%s", SOLA_SECTION_TOTAL, SOLA_ITEM_RETURNLOSS, gWaveSection[j]+4);
                pSolaEvent->TotalReturnLoss[j] = (float)iniparser_getdouble(dict, key, 0);
                pSolaEvent->WaveEventTable[j].iLaunchFiberIndex = pSolaEvent->EventTable.fLaunchFiberPosition;
                pSolaEvent->WaveEventTable[j].iRecvFiberIndex   = pSolaEvent->EventTable.fRecvFiberPosition;  
                ReadEventTable(dict, gWaveSection[j], &pSolaEvent->WaveEventTable[j], SOLA_VER_11);
            }
        }
    }
    
    ReadEventTable(dict, SOLA_SECTION_TOTAL, &pSolaEvent->EventTable, SOLA_VER_11);
    
    return;
}


/** 
 * 载入一个SOLA文件
 * @param[in]  pszPath      SOLA文件路径
 * @param[out] pSolaEvent   将载入的SOLA文件内容保存到指定的SOLA事件表结构体
 * @return     int          错误码（SOLA_FILE_ERROR）
 */
int SolaLoad(const char* pszPath, SolaEvents* pSolaEvent)
{
    char* pVersion = NULL;
    char key[64] = {0};
    if(isDirectory(pszPath))
    {
        return SolaIsDirectory;
    }
    if(isSolaFile(pszPath))
        return SolaBadVersion;
    dictionary* dict = iniparser_load(pszPath);
    SolaEvents_Clear(pSolaEvent);
    setSolaName(pSolaEvent, pszPath);
    //如果加载失败，则尝试使用老版本的SOLA读取函数进行读取
    if (dict == NULL) {
        return ReadSola(pszPath, pSolaEvent);
    }

    //读取main section
    ReadMainSection(dict, &pSolaEvent->ulDTS);

    SOLA_NORMAL_KEY(key, SOLA_SECTION_MAIN, SOLA_ITEM_VERSION);
    pVersion  = (char*)iniparser_getstring(dict, key, "NULL");
    if(0 == strcmp(pVersion,"1.1"))
    {
        ReadSolaFileData_V11(dict, pSolaEvent);
    }
    else if(0 == strcmp(pVersion,"1.0"))
    {
        ReadTotalEventTable(dict, pSolaEvent);//V1.0接口
        ReadWaveEventTable(dict, pSolaEvent); //V1.0接口   
    }

    iniparser_freedict(dict);

    return SolaOk;
}


/** 
 * 保存一个SOLA文件
 * @param[in] pSolaEvent   将指定的SOLA事件表结构体的内容保存到SOLA文件
 * @param[in] absolutePath 存储的路径
 * @return     int          错误码（SOLA_FILE_ERROR）
 */
int SolaSave(const SolaEvents* pSolaEvent, char *absolutePath)
{
    FILE* out = NULL;  
    if(absolutePath)
    {
        out = fopen(absolutePath, "w");
    }
    
    if (out) 
    {
        //main section
        WriteMainSection(out, pSolaEvent->ulDTS);

        //total section
        WriteTotalSection(out, pSolaEvent);
        
        //各个波长的section
        WriteWaveEventTable(out, pSolaEvent);

        fclose(out);
        return SolaOk;
    }
    return SolaAccessFailed;
}

static int ReadMainSection(dictionary* dict, time_t *testTime)
{
    char key[64];
    (void)dict;
    
    if(testTime)
    {
        SOLA_NORMAL_KEY(key, SOLA_SECTION_MAIN, SOLA_ITEM_TESTTIME);
        *testTime = iniparser_getlongint(dict, key, time(NULL));
    }
    return SolaOk;
}

static int ReadEventInfo(dictionary* dict, const char* section, int index, EVENTS_INFO* pEvent, int ver)
{
    char key[64] = {0};
    
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_BEGIN, index);
    pEvent->iBegin = iniparser_getint(dict, key, pEvent->iBegin);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_END, index);
    pEvent->iEnd = iniparser_getint(dict, key, pEvent->iEnd);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_STYLE, index);
    pEvent->iStyle = iniparser_getint(dict, key, pEvent->iStyle);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_PULSE, index);
    pEvent->iPulse = iniparser_getint(dict, key, pEvent->iPulse);

    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_LOSS, index);
    pEvent->fLoss = (float)iniparser_getdouble(dict, key, (double)pEvent->fLoss);
    
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_REFLECT, index);
    pEvent->fReflect = (float)iniparser_getdouble(dict, key, (double)pEvent->fReflect);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_CONTINUELENGTH, index);
    pEvent->iContinueLength = iniparser_getint(dict, key, pEvent->iContinueLength);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_ATTENUATION, index);
    pEvent->fAttenuation = (float)iniparser_getdouble(dict, key, (double)pEvent->fAttenuation);
    
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_TOTALLOSS, index);
    pEvent->fTotalLoss = (float)iniparser_getdouble(dict, key, (double)pEvent->fTotalLoss);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_EVTPOS, index);
    pEvent->fEventsPosition = (float)iniparser_getdouble(dict, key, (double)pEvent->fEventsPosition);
    SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_EVTENDPOS, index);
    pEvent->fEventsEndPosition = (float)iniparser_getdouble(dict, key, (double)pEvent->fEventsEndPosition);
    

    if(SOLA_VER_10 == ver)
    {
        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_CONTINUEATTENUA, index);
        pEvent->fContinueAttenuation = (float)iniparser_getdouble(dict, key, (double)pEvent->fContinueAttenuation);
        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_EVTRTNLOSS, index);
        pEvent->fEventReturnLoss = (float)iniparser_getdouble(dict, key, (double)pEvent->fEventReturnLoss);
        
        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_DELTA, index);
        pEvent->fDelta = (float)iniparser_getdouble(dict, key, (double)pEvent->fDelta);

        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_WAVESYMBOL, index);
        pEvent->iWaveSymbol = iniparser_getint(dict, key, pEvent->iWaveSymbol);
        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITME_SMALLREFLECTFLG, index);
        pEvent->iSmallReflectFlag = iniparser_getint(dict, key, pEvent->iSmallReflectFlag);
        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITME_SMALLLOSSFLG, index);
        pEvent->iSmallLossFlag = iniparser_getint(dict, key, pEvent->iSmallLossFlag);
    }
    else if(SOLA_VER_11 == ver)
    {
        SOLA_EVENT_INFO_KEY(key, section, SOLA_ITEM_EVTRTNLOSS, index);
        pEvent->fContinueAttenuation = (float)iniparser_getdouble(dict, key, (double)pEvent->fContinueAttenuation);
        pEvent->fEventReturnLoss = 0;
    }
    
    return 0;
}


static int ReadEventTable(dictionary* dict, const char* section, EVENTS_TABLE* pEventTable, int ver)
{
    char key[64] = {0};
    int index;
    
    SOLA_NORMAL_KEY(key, section, SOLA_ITEM_COUNT);
    pEventTable->iEventsNumber = iniparser_getint(dict, key, 0);
    pEventTable->iEventsStart = 0;

    for (index = 0; index < pEventTable->iEventsNumber; ++index)
    {
        ReadEventInfo(dict, section, index, &pEventTable->EventsInfo[index], ver);
    }

    if(SOLA_VER_10 == ver)
    {
        SOLA_NORMAL_KEY(key, section, SOLA_ITEM_START);
        pEventTable->iEventsStart = iniparser_getint(dict, key, 0);
        SOLA_NORMAL_KEY(key, section, SOLA_ITEM_LAUNCHFIBER);
        pEventTable->iLaunchFiberIndex = iniparser_getint(dict, key, 0);
        SOLA_NORMAL_KEY(key, section, SOLA_ITEM_RECVFIBER);
        pEventTable->iRecvFiberIndex = iniparser_getint(dict, key, 0);
    }
    
    
#if (SOLA_FILE_DEBUG == 1)
    //PrintEventInfo(stdout, pEventTable);
#endif
    return 0;
}
//V1.0接口
static int ReadTotalEventTable(dictionary* dict, SolaEvents* pSolaEvent)
{
    return ReadEventTable(dict, SOLA_SECTION_TOTAL, &pSolaEvent->EventTable, SOLA_VER_10);
}

//读取各个波长的事件表，V1.0接口
static int ReadWaveEventTable(dictionary* dict, SolaEvents* pSolaEvent)
{
    char key[64] = {0};
    int i = 0;
    for (i = 0; i < WAVE_NUM; ++i) 
    {
        ReadEventTable(dict, gWaveSection[i], &pSolaEvent->WaveEventTable[i], SOLA_VER_10);
        SOLA_NORMAL_KEY(key, gWaveSection[i], "Selected");
        pSolaEvent->WaveIsSetted[i] = iniparser_getint(dict,  key, 0);
        SOLA_NORMAL_KEY(key, gWaveSection[i], "TotalReturnLoss");
        pSolaEvent->TotalReturnLoss[i] = (float)iniparser_getdouble(dict, key, (double)pSolaEvent->TotalReturnLoss[i]);
    }

    return 0;
}


//创建并写入main section
static void WriteMainSection(FILE* out, time_t testTime)
{
    //获取当前系统时间并格式化
    char time_buffer[32];
    time_t ct = time(NULL);
	struct tm* newtime = localtime(&ct);
	strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %T", newtime);

    //输出main section
    fprintf(out, "[Main]\n");
    fprintf(out, SOLA_ITEM_VERSION"=1.1\n");
    fprintf(out, SOLA_ITEM_TIME"=%s\n", time_buffer); 
    fprintf(out, SOLA_ITEM_TESTTIME"=%ld\n", testTime); 
    fprintf(out, "\n");
}

//写入事件信息
static void WriteEventInfo(FILE* out, int index, const EVENTS_INFO* pEvent)
{
    fprintf(out, SOLA_ITEM_BEGIN"_%d=%d\n", index, pEvent->iBegin);
    fprintf(out, SOLA_ITEM_END"_%d=%d\n", index, pEvent->iEnd);
    fprintf(out, SOLA_ITEM_STYLE"_%d=%d\n", index, pEvent->iStyle);
    fprintf(out, SOLA_ITEM_PULSE"_%d=%d\n", index, pEvent->iPulse);
    fprintf(out, SOLA_ITEM_EVTPOS"_%d=%f\n", index, pEvent->fEventsPosition);
    fprintf(out, SOLA_ITEM_EVTENDPOS"_%d=%f\n", index, pEvent->fEventsEndPosition);
    fprintf(out, SOLA_ITEM_REFLECT"_%d=%f\n", index, pEvent->fReflect);
    fprintf(out, SOLA_ITEM_ATTENUATION"_%d=%f\n", index, pEvent->fAttenuation);
    fprintf(out, SOLA_ITEM_LOSS"_%d=%f\n", index, pEvent->fLoss);
    fprintf(out, SOLA_ITEM_TOTALLOSS"_%d=%f\n", index, pEvent->fTotalLoss);

    //fEventReturnLoss都为0，为与F225统一，将fContinueAttenuation保存在EventReturnLoss
    fprintf(out, SOLA_ITEM_EVTRTNLOSS"_%d=%f\n", index, pEvent->fContinueAttenuation);
    fprintf(out, SOLA_ITEM_CONTINUELENGTH"_%d=%d\n", index, pEvent->iContinueLength);
    
    //fprintf(out, "Delta_%d=%f\n", index, pEvent->fDelta);
    //fprintf(out, "ContinueAttenuation_%d=%f\n", index, pEvent->fContinueAttenuation);

    fprintf(out, "IsCombine_0=0\n");
    return;
}

//写入事件表section
static void WriteEventTable(FILE* out, const char* section, const EVENTS_TABLE* pEventTable)
{
    int index = 0;;
    fprintf(out, "[%s]\n", section);
    fprintf(out, SOLA_ITEM_COUNT"=%d\n", pEventTable->iEventsNumber);
    for (index = 0; index < pEventTable->iEventsNumber; ++index)
    {
        WriteEventInfo(out, index, &pEventTable->EventsInfo[index]);
    }
#if (SOLA_FILE_DEBUG == 1)
    //PrintEventInfo(stdout, pEventTable);
#endif
}

//写入总事件表section
static void WriteTotalSection(FILE* out, const SolaEvents* pSolaEvent)
{
    int i = 0;
    int index = 0;
    WriteEventTable(out, SOLA_SECTION_TOTAL, &pSolaEvent->EventTable);
    for(i = 0; i < WAVE_NUM; i++)
    {
        if(0 == pSolaEvent->WaveIsSetted[i])
        {
            continue;
        }
        fprintf(out, SOLA_ITEM_WAVELIST"_%d=%s\n",index, gWaveSection[i]+4);
        fprintf(out, SOLA_ITEM_LINKLOSS"_%s=%f\n",gWaveSection[i]+4, SolaEvents_GetTotalLoss(pSolaEvent, i));//链路损耗
        fprintf(out, SOLA_ITEM_RETURNLOSS"_%s=%f\n",gWaveSection[i]+4,  pSolaEvent->TotalReturnLoss[i]);//链路光回损
        index++;
    }
    
    fprintf(out, SOLA_ITEM_WAVECNT"=%d\n",index);
    fprintf(out, SOLA_ITEM_LAUNCHFIBER"=%d\n", pSolaEvent->EventTable.iLaunchFiberIndex);
    fprintf(out, SOLA_ITEM_RECVFIBER"=%d\n", pSolaEvent->EventTable.iRecvFiberIndex);
    fprintf(out, "\n");
    return;
}

//写入各个波长的section
static void WriteWaveEventTable(FILE* out, const SolaEvents* pSolaEvent)
{
    int i = 0;;
    for (i = 0; i < WAVE_NUM; ++i) 
    {
        if(0 == pSolaEvent->WaveIsSetted[i])
        {
            continue;
        }
        WriteEventTable(out, gWaveSection[i], &pSolaEvent->WaveEventTable[i]);
        /*fprintf(out, "TotalReturnLoss=%f\n", pSolaEvent->TotalReturnLoss[i]);
        fprintf(out, "LinkLoss=%f\n", SolaEvents_GetTotalLoss(pSolaEvent, i));
        fprintf(out, "LaunchFiberIndex=%d\n", pSolaEvent->WaveEventTable[i].iLaunchFiberIndex);
        fprintf(out, "RecvFiberIndex=%d\n", pSolaEvent->WaveEventTable[i].iRecvFiberIndex);*/
        fprintf(out, "\n");
    }
}

/** 
 * 获得SOLA文件文件名
 * @param[in]  char *pDirPath SOLA文件存储目录
               int iNameMaxLength 文件名最大
               SOLA_FILE_NAME_SETTING *pAutoname 自动命名，根据此规则获得文件名
 * @param[out] char *pName 得到的SOLA文件名(绝对路径)
 * @return     int          错误码（0 错误   1 正确）
 */
int SolaNamed(char *pDirPath, char *pName, int iNameMaxLength, SOLA_FILE_NAME_SETTING *pAutoname)
{
    int iRet = 0;
    
    if (pDirPath == NULL || pName == NULL)
        return 0;
    
    if (access(MntDataDirectory, F_OK))
        return 0;

    if (pDirPath[strlen(pDirPath) - 1] == '/')
    {
        pDirPath[strlen(pDirPath) - 1] = 0;
    }
    
	if (access(pDirPath, F_OK) != 0)
	{
		iRet = mkdir(pDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(iRet)
		{
            return 0;
		}
	}
	
    char cTempNameBuf[MAXPATHNAME] = {0};
    char prefix[30] = {0};
    strcpy(prefix, pAutoname->prefix);
    int iSuffix = pAutoname->suffix;
    int iSuffixWidth = pAutoname->suffixWidth;
    memset(pName, 0, iNameMaxLength);

    int max = pow(10, iSuffixWidth);
    iSuffix = (pAutoname->suffix % max) ? (pAutoname->suffix % max) : 1;
    int i;
    for (i = 0; i < max; ++i)
    {
        int isGetName = 1;
        char temp[128] = {0};
        
        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd%s", prefix, iSuffixWidth, ".SOLA");
            sprintf(cTempNameBuf, temp, iSuffix);
            isGetName = !isFileExist(cTempNameBuf, pDirPath);
        }
        
        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd%s", prefix, iSuffixWidth, ".PDF");
            sprintf(cTempNameBuf, temp, iSuffix);
            isGetName = !isFileExist(cTempNameBuf, pDirPath);
        }
        
        sprintf(temp, "%s%%0%dd%s", prefix, iSuffixWidth, ".SOLA");
        sprintf(cTempNameBuf, temp, iSuffix);
        memcpy(pName, cTempNameBuf, iNameMaxLength);
        
        if(!isGetName)
        {
            if(pAutoname->suffixRule)
            {
                if(++iSuffix == max)
                {
                    iSuffix = 1;
                }
            }
            else
            {
                if(--iSuffix == 0)
                {
                    iSuffix = max - 1;
                }
            }

            if(iSuffix == pAutoname->suffix)
            {
                break;
            }
        }
        else
        {
            pSolaMessager->autoFilename.suffix = iSuffix;
            LOG(LOG_INFO, "sola file name : %s\n", pName);
            break;
        }
    }
    
    return 1;
}

/** 
 * 判断文件内容是否是sola文件
 * @param[in]  pszPath      SOLA文件路径
 * @return     int          错误码（SOLA_FILE_ERROR）
 */
static int isSolaFile(const char* pszPath)
{
    int iErr = 0;
    FILE *fp = fopen(pszPath, "r");
    char str[128]; //足够容纳最大长度的一行！
    if(fp == NULL)
        return -1;
    char* buff[4] = {"[Main]", SOLA_ITEM_VERSION, SOLA_ITEM_TIME, SOLA_ITEM_TESTTIME};
    int i;
    int count = 0;
    for(i = 0; i < 4; ++i)
    {
        fgets(str, sizeof(str), fp); 
        if(!strncmp(str, buff[i], strlen(buff[i])))
        {
            count++;
        }
    }
    
    iErr = (count == 4) ? 0 : -2;
    fclose(fp);
    
    return iErr;
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
