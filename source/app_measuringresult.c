/****************************************Copyright (c)****************************************************
**
**                           捷太科仪器(苏州)有限公司上海研发部
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  app_measuringresult.c
** Latest Version:             V1.0.0
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               
** Descriptions:               保存测量结果信息，给测试和工程使用
** 
*********************************************************************************************************/

#include "app_measuringresult.h"
#include "wnd_frmsola.h"
#include "app_frminit.h"
#include "app_cjson.h"
#include "app_unitconverter.h"


#define MEASURINGMENT_PATH	MntUsbDirectory"/"	// 保存路径是在U盘下面

#define RESULT_BUF_MAX 128

static int SaveEnable = 0; // 保存使能

static char *StrWaveLenght[] = {"1310", "1550", "1625"};

static char *StrPulse[] = {
"AUTO",
"5 ns",
"10 ns",
"20 ns",
#ifdef MINI2
"30 ns",
#endif
"50 ns",
"100 ns",
"200 ns",
"500 ns",
"1 us",
"2 us",
"10 us",
"20 us",
};

static char *RangeKm[] = {
"AUTO",
"1.3 km",
"2.5 km",
"5 km",
"10 km",
"20 km",
"40 km",
"80 km",
"120 km",
"160 km",
"260 km",
};

typedef enum TYPE
{
    OTDR_RESULT = 1,
    SOLA_RESULT
}MEASURING_RESULT_TYPE;


static int solaGetSpiltterEvent(int splitterEvent, char *cEventStyle)
{
	int ret = 0;
	if ((splitterEvent < SPLITTER1_2) || (splitterEvent > SPLITTER1_128))
	{
		snprintf(cEventStyle, RESULT_BUF_MAX, "1");
		ret = -1;
	}
	else
	{
		snprintf(cEventStyle, RESULT_BUF_MAX, "Splitter%d", (0x1 << splitterEvent));
		ret = 0;
	}
	return ret;
}

/**
 *****************************************************************************
 * @brief	从事件的style，得到其数据类型
 * 			
 *返回值字符串： 		0:区段， 1：接头，2:连接器，3：宏弯曲，4：光功率不足，5：组合事件，
 *          	SplitterN:分光器1：N，---：---
 * 
 * @param 	事件style
 * @param 	对应的事件类别字符串，保持与F225统一
 * @return	static int 暂时无效
******************************************************************************/
static int solaGetEventType(int iEventStyle, char *cEventStyle)
{
    int normalEvent = iEventStyle & 0x0000ffff;
    int splitterEvent = (iEventStyle & 0xffff0000) >> 16;
	if (0 == solaGetSpiltterEvent(splitterEvent, cEventStyle))
	{
		return 0;
	}

    switch(normalEvent)
    {
    case 0:
    case 1:
    case 10:
    case 11:
		snprintf(cEventStyle, RESULT_BUF_MAX, "1");
    	return 0;
    case 2:
    case 3:
    case 4:
    case 12:
    case 13:
    case 14:
		snprintf(cEventStyle, RESULT_BUF_MAX, "2");
    	return 0;
    case 5:
    case 15:
		snprintf(cEventStyle, RESULT_BUF_MAX, "3");
    	return 0;
    case 21:
		snprintf(cEventStyle, RESULT_BUF_MAX, "4");
    	return 0;
    default:
		snprintf(cEventStyle, RESULT_BUF_MAX, "1");
    	break;
    }
    
    return -2;
}


//得到事件类型 返回0 熔接事件 返回1 连接器事件
static int getConnectorEventType(int type)
{
    int iRet = 0;
    
    switch(type)
    {
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
        case 14:
            iRet = 1;
            break; 
        default:
        	break;
     }
     
     return iRet;
}

//获得事件类型

static int otdrGetEventTypeName(EVENTS_TABLE Events, int index, char *typeName)
{
	int displaySectionEvent = pOtdrTopSettings->pUser_Setting->sCommonSetting.iDisplaySectionEvent ? 1 : 0;
	
    if(typeName && index < (Events.iEventsNumber * (1 + displaySectionEvent) - displaySectionEvent))
    {
    	if(displaySectionEvent)
        {
            if(index % 2)
            {
                snprintf(typeName, RESULT_BUF_MAX, "9");	// 和F225保持一致
                return 0;
            }
            index /= 2;
        }
		
        if((Events.iEventsStart != 0 && Events.iEventsStart == index) || 
           (Events.iLaunchFiberIndex != 0 && Events.iLaunchFiberIndex == index) ||
           (Events.iLaunchFiberIndex == 0 && Events.iEventsStart == 0 && index == 0))
        {
			snprintf(typeName, RESULT_BUF_MAX, "7");	// 和F225保持一致
            return 0;
        }

        if((Events.iRecvFiberIndex != 0 && Events.iRecvFiberIndex == index) ||
            (Events.iRecvFiberIndex == 0 && (Events.iEventsNumber - 1) == index))
        {
			snprintf(typeName, RESULT_BUF_MAX, "4");
            return 0;
        }

        switch(Events.EventsInfo[index].iStyle)
        {
            case 0:
            case 5:
				snprintf(typeName, RESULT_BUF_MAX, "0");
            	break;
            case 1:
				snprintf(typeName, RESULT_BUF_MAX, "1");
            	break;
            case 3:
				snprintf(typeName, RESULT_BUF_MAX, "3");
            	break;
            case 2:
            case 4:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
				snprintf(typeName, RESULT_BUF_MAX, "2");
            	break;
            case 20:
				snprintf(typeName, RESULT_BUF_MAX, "5");
            	break;
            case 21:
				snprintf(typeName, RESULT_BUF_MAX, "6");
            	break;
            default:
            	return -1;
        }
        return 0;
    }
    return -1;
}


static void insertOTDREvent(int iCurve, cJSON * json)
{
	EVENTS_TABLE Events = pOtdrTopSettings->pDisplayInfo->pCurve[iCurve]->Events;

	int displaySectionEvent = pOtdrTopSettings->pUser_Setting->sCommonSetting.iDisplaySectionEvent ? 1 : 0;
    int eventCount = (Events.iEventsNumber > 1 && displaySectionEvent) ? Events.iEventsNumber * 2 - 1 : Events.iEventsNumber;
	
	int iStartIndex = (Events.iLaunchFiberIndex > 0) ? Events.iLaunchFiberIndex : Events.iEventsStart;
    int iEndIndex = (Events.iRecvFiberIndex > 0) ? Events.iRecvFiberIndex : Events.iEventsNumber - 1;
	float fStartDistance = Events.EventsInfo[iStartIndex].fEventsPosition;
    float fStartTotalLoss = Events.EventsInfo[iStartIndex].fTotalLoss;

	if(displaySectionEvent)
    {
        iStartIndex *= 2;
        iEndIndex *= 2;
    }
	
	cJSON* eventtable = cJSON_CreateObject();
	char buf[RESULT_BUF_MAX] = {0};
	snprintf(buf, RESULT_BUF_MAX, "%d", eventCount);
	cJSON_AddStringToObject(eventtable, "EventCount", buf);
	snprintf(buf, RESULT_BUF_MAX, "%d", displaySectionEvent);
	cJSON_AddStringToObject(eventtable, "EnableSection", buf);

	int i = 0;
	for (i = 0; i < eventCount; i++)
	{
		cJSON* eventinfo = cJSON_CreateObject();
		
		EVENTS_INFO event = Events.EventsInfo[i/(1+displaySectionEvent)];
        EVENTS_ISPASS result = Events.EventsIsPass[i/(1+displaySectionEvent)];
		//事件的距离和累积损耗以起始点为准(如果存在起始点)
		float fDistanceOrLength = event.fEventsPosition;
		float fTLoss = event.fTotalLoss;
		double fSectionEvtLoss = 0.0f;

        if(displaySectionEvent)
        {
            double fAttenuation = Float2Float(PRECISION_3, event.fAttenuation);
            double SectionLen = Events.EventsInfo[i/2+1].fEventsPosition
                 - event.fEventsPosition;
            fSectionEvtLoss = fAttenuation * (SectionLen / 1000);
        }
		
		if (iStartIndex != 0)
		{
			fDistanceOrLength -= fStartDistance;
			fTLoss = event.fTotalLoss - fStartTotalLoss;
		}
		
		if(i > 0 && i < eventCount - 1)
		{
            fTLoss += Float2Float(PRECISION_3, event.fLoss);
		}

		if((i % 2) && displaySectionEvent)
		{
            fDistanceOrLength = Events.EventsInfo[(i/(1+displaySectionEvent)) + 1].fEventsPosition - event.fEventsPosition;
            fTLoss += fSectionEvtLoss;
		}
		
		// eventtype
		otdrGetEventTypeName(Events, i, buf);//同 F225 的事件类型
		cJSON_AddStringToObject(eventinfo, "Type", buf);
		
		// Num
		if((i % 2) && displaySectionEvent)
        {
			snprintf(buf, RESULT_BUF_MAX, " ");
        }
        else if (i > iStartIndex && i < iEndIndex)
        {
		    snprintf(buf, RESULT_BUF_MAX, "%d", (i - iStartIndex) / (displaySectionEvent + 1));
		}
		else if(i == iStartIndex)
		{
            if (Events.iEventsStart > 0) 
            {
                snprintf(buf, RESULT_BUF_MAX, "R");
            }
            else
            {
                if(iStartIndex == iEndIndex)
                {
                    snprintf(buf, RESULT_BUF_MAX, "S/E");
                }
                else
                {
                    snprintf(buf, RESULT_BUF_MAX, "S");
                }
            }
		}
		else if(i == iEndIndex)
		{
            snprintf(buf, RESULT_BUF_MAX, "E");
		}
        else
        {
            snprintf(buf, RESULT_BUF_MAX, "*");
        }
		cJSON_AddStringToObject(eventinfo, "Num", buf);
		
		//position
		snprintf(buf, RESULT_BUF_MAX, "%.1f", fDistanceOrLength);
		cJSON_AddStringToObject(eventinfo, "Pos", buf);

		// loss
		if((i % 2) && displaySectionEvent)
        {
			snprintf(buf, RESULT_BUF_MAX, "%.3f", fSectionEvtLoss);
			cJSON_AddStringToObject(eventinfo, "Loss", buf);
			cJSON_AddStringToObject(eventinfo, "LossPass", "1");
        }
		else
		{
			if (i != 0 && i != eventCount - 1)
	        {
	            snprintf(buf, RESULT_BUF_MAX, "%d", result.iIsEventLossPass);
				cJSON_AddStringToObject(eventinfo, "LossPass", buf);
	        	snprintf(buf, RESULT_BUF_MAX, "%.3f", event.fLoss);
				cJSON_AddStringToObject(eventinfo, "Loss", buf);
	        }  
	        else
	        {
	        	cJSON_AddStringToObject(eventinfo, "LossPass", "1");
				cJSON_AddStringToObject(eventinfo, "Loss", " ");
	        }
		}
		

		// Reflect
		if((i % 2) && displaySectionEvent)
        {
            cJSON_AddStringToObject(eventinfo, "ReflectPass", "1");
			cJSON_AddStringToObject(eventinfo, "Reflect", " ");
        }
		else
		{
			if (getConnectorEventType(event.iStyle))
	        {
	            snprintf(buf, RESULT_BUF_MAX, "%d", result.iIsReflectPass);
				cJSON_AddStringToObject(eventinfo, "ReflectPass", buf);
	        	snprintf(buf, RESULT_BUF_MAX, "%.1f", event.fReflect);
				cJSON_AddStringToObject(eventinfo, "Reflect", buf);
	        }
	        else
	        {
	            cJSON_AddStringToObject(eventinfo, "ReflectPass", "1");
				cJSON_AddStringToObject(eventinfo, "Reflect", " ");
	        }
		}
		

		// attenuation
		if(!displaySectionEvent || ((i % 2) == 0) || (i == eventCount - 1))
        {
            cJSON_AddStringToObject(eventinfo, "AttenPass", "1");
			cJSON_AddStringToObject(eventinfo, "Atten", " ");
        }
		else
		{
			snprintf(buf, RESULT_BUF_MAX, "%d", result.iIsAttenuationPass);
			cJSON_AddStringToObject(eventinfo, "AttenPass", buf);
        	snprintf(buf, RESULT_BUF_MAX, "%.3f", event.fAttenuation);
			cJSON_AddStringToObject(eventinfo, "Atten", buf);
		}

		// TotalLoss
		if(i == 0 || i < iStartIndex || (i > iEndIndex && iEndIndex > 0))
        {
			cJSON_AddStringToObject(eventinfo, "TotalLoss", " ");
        }
		else if(i == 0)
		{
			cJSON_AddStringToObject(eventinfo, "TotalLoss", "0.000");
		}
        else
        {
			snprintf(buf, RESULT_BUF_MAX, "%.3f", fTLoss);
			cJSON_AddStringToObject(eventinfo, "TotalLoss", buf);
        }

		snprintf(buf, RESULT_BUF_MAX, "%d", i);
		cJSON_AddItemToObject(eventtable, buf, eventinfo);
	}

	cJSON_AddItemToObject(json, "EventTable", eventtable);
}

static int getFileName(char *FileName, MEASURING_RESULT_TYPE type)
{
	time_t t = 0;
	struct tm *p = NULL;

	if(NULL == FileName)
	{
		return -1;
	}
	
	if (0 > (t = time(NULL)))
	{
		return -2;
	}
	
	if (NULL == (p = gmtime(&t))) 
	{
		return -3;
	}

	if (type == OTDR_RESULT)
	{
		sprintf(FileName, MEASURINGMENT_PATH"OTDR_%.4d_%.2d_%.2d_%.2d_%.2d_%.2d.json", 
			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, 
			p->tm_hour, p->tm_min, p->tm_sec);
	}
	else
	{
		sprintf(FileName, MEASURINGMENT_PATH"SOLA_%.4d_%.2d_%.2d_%.2d_%.2d_%.2d.json", 
			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, 
			p->tm_hour, p->tm_min, p->tm_sec);
	}
	
	return 0;
}


static void saveFile(MEASURING_RESULT_TYPE type, cJSON * json)
{
	char filename[NAME_MAX] = {0};
	/* 不存在U盘或者json为空，不保存             */
	if ((json == NULL) || (0 != access(MEASURINGMENT_PATH, F_OK)) || (getFileName(filename, type)))
		return;

	//
	char *data = cJSON_Print(json);
	FILE *fp;
	fp = fopen(filename, "w");
	if (fp == NULL)
	{
		perror("open measuringment result file error!");
		return;
	}

	fwrite(data, strlen(data), 1, fp);
	fclose(fp);
	free(data);// 注意释放data，因为cJSON_Print(json)，会申请内存的
}

static void getSolaPass(int isPass, char *buf)
{
	if (1 == isPass)
	{
		snprintf(buf, RESULT_BUF_MAX, "1");
	}
	else if (0 == isPass)
	{
		snprintf(buf, RESULT_BUF_MAX, "0");
	}
	else
	{
		snprintf(buf, RESULT_BUF_MAX, "-1");
	}
}

static void getLossAndReflect(int normalEvent, int *loss, int *reflect)
{
	switch(normalEvent)
    {
	    case 0:
	    case 1:
			*loss = 1;
			*reflect = 0;
			break;
	    case 2:
	    case 3:
	    case 4:
			*loss = 1;
			*reflect = 1;
			break;
	    case 5:
			*loss = 1;
			*reflect = 0;
			break;
	    case 10:
	    case 11:
	    case 15:
	        *loss = 0;
			*reflect = 0;
			break;
	    case 12:
	    case 13:
	    case 14:
	        *loss = 0;
			*reflect = 1;
			break;
	    case 21:
	        *loss = 0;
			*reflect = 0;
			break;
	    default:
			*loss = 0;
			*reflect = 0;
			break;
    }
}

static void getSolaLossAndReflect(cJSON *json, EVENTS_INFO *event, EVENTS_ISPASS *eventIsPass, int loss, int reflect)
{
	char buf[RESULT_BUF_MAX] = {0};
	if (1 == loss)
	{
		snprintf(buf, RESULT_BUF_MAX, "%.3f", event->fLoss);
		cJSON_AddStringToObject(json, "Loss", buf);
		getSolaPass(eventIsPass->iIsEventLossPass, buf);
		cJSON_AddStringToObject(json, "LossPass", buf);
		cJSON_AddStringToObject(json, "LossShow", "1");
	}
	else
	{
		cJSON_AddStringToObject(json, "Loss", "--");
		cJSON_AddStringToObject(json, "LossPass", "1");
		cJSON_AddStringToObject(json, "LossShow", "0");
	}

	if (1 == reflect)
	{
		snprintf(buf, RESULT_BUF_MAX, "%.1f", event->fReflect);
		cJSON_AddStringToObject(json, "Reflect", buf);
		getSolaPass(eventIsPass->iIsReflectPass, buf);
		cJSON_AddStringToObject(json, "ReflectPass", buf);
		cJSON_AddStringToObject(json, "ReflectShow", "1");
	}
	else
	{
		cJSON_AddStringToObject(json, "Reflect", "--");
		cJSON_AddStringToObject(json, "ReflectPass", "1");
		cJSON_AddStringToObject(json, "ReflectShow", "0");
	}
}

static void insertSOLAEvent(cJSON *eventtable, int enWave)
{
	char buf[RESULT_BUF_MAX] = {0};
	SOLA_WIDGET* solaWidget = getSolaWidget();
	SolaEvents* solaEvents = solaWidget->solaEvents;

	int launchFiberIndex = solaEvents->EventTable.iLaunchFiberIndex;
	float launchFiberPosition = solaEvents->EventTable.EventsInfo[launchFiberIndex].fEventsPosition;

	// 添加事件个数
	int eventcount = solaWidget->itemCount;;
	snprintf(buf, RESULT_BUF_MAX, "%d", eventcount);
	cJSON_AddStringToObject(eventtable, "EventCount", buf);

	int j = 0;
	for (j = 0; j < eventcount; j++)
	{
		cJSON *eventinfoJson = cJSON_CreateObject();	
		
		const EVENTS_INFO *eventInfo = SolaEvents_GetEvent(solaEvents, j);
		if (eventInfo)
		{
			// 添加事件距离
			if(launchFiberIndex > 0)
            {
				snprintf(buf, RESULT_BUF_MAX, "%.1f", eventInfo->fEventsPosition-launchFiberPosition);
            }
            else
            {
				snprintf(buf, RESULT_BUF_MAX, "%.1f", eventInfo->fEventsPosition);
            }
			cJSON_AddStringToObject(eventinfoJson, "Pos", buf);

			// 添加事件类型
			solaGetEventType(eventInfo->iStyle, buf);
			cJSON_AddStringToObject(eventinfoJson, "EventType", buf);

			// 添加 事件损耗和折射率
			EVENTS_INFO *event = SolaEvents_GetWaveEvent(solaEvents, j, enWave);
			if(event == NULL)
		    {
		    	getSolaLossAndReflect(eventinfoJson, event, NULL, 0, 0);
		    }
			else
			{
				EVENTS_ISPASS *eventIsPass = &solaEvents->WaveEventTable[enWave].EventsIsPass[j];
			    int normalEvent = eventInfo->iStyle & 0x0000ffff;
			    int splitterEvent = (eventInfo->iStyle & 0xffff0000) >> 16; 
			    
			    if((normalEvent < 10) && (solaEvents->EventTable.iEventsNumber == j + 1 || j == 0))
			    {
			        normalEvent += 10;   
			    }

				if ((splitterEvent > SPLITTER_None) && (splitterEvent < SPLITTER_COUNT))
				{
					if(normalEvent >= 10 && normalEvent < 20)
        			{
        				getSolaLossAndReflect(eventinfoJson, event, eventIsPass, 0, 1);
        			}
					else
					{
						getSolaLossAndReflect(eventinfoJson, event, eventIsPass, 1, 1);
					}
				}
                else
                {
                    int loss = 0;
                    int reflect = 0;
                    getLossAndReflect(normalEvent, &loss, &reflect);
                    getSolaLossAndReflect(eventinfoJson, event, eventIsPass, loss, reflect);
                }
			}
		}
		snprintf(buf, RESULT_BUF_MAX, "%d", j);
		cJSON_AddItemToObject(eventtable, buf, eventinfoJson);
	}
}

static void getSolaWaveTable(cJSON *wavetable, int waveCount)
{
	SOLA_WIDGET* solaWidget = getSolaWidget();
	
	char buf[RESULT_BUF_MAX] = {0};
	int waveLength = WAVELEN_1310;
	
	int i = 0;
	for (i = 0; i < waveCount; i++)
	{
		// 生成波形信息
		cJSON *waveinfo = cJSON_CreateObject();
		
		while(!solaWidget->solaEvents->WaveIsSetted[waveLength])
        {
            waveLength++;
        }

		// 添加波长
		cJSON_AddStringToObject(waveinfo, "Wave", StrWaveLenght[waveLength]);

		// 添加链路损耗
		float totalLoss = SolaEvents_GetTotalLoss(solaWidget->solaEvents, waveLength);
		snprintf(buf, RESULT_BUF_MAX, "%.3f", totalLoss);
		cJSON_AddStringToObject(waveinfo, "LinkLoss", buf);

		// 添加链路ORL
        float totalReturnLoss = solaWidget->solaEvents->TotalReturnLoss[waveLength]>0.0f
                                ?solaWidget->solaEvents->TotalReturnLoss[waveLength]
                                :solaWidget->solaEvents->TotalReturnLoss[waveLength]*(-1.0f);
		snprintf(buf, RESULT_BUF_MAX, "%.2f", totalReturnLoss);
		cJSON_AddStringToObject(waveinfo, "LinkORL", buf);
		
		// 添加 链路损耗和链路ORL 是否通过
		LINK_ISPASS *linkIsPass = &solaWidget->solaEvents->WaveEventTable[waveLength].LinkIsPass;
		getSolaPass(linkIsPass->iIsRegionLossPass, buf);
		cJSON_AddStringToObject(waveinfo, "LinkLossPass", buf);

		getSolaPass(linkIsPass->iIsRegionORLPass, buf);
		cJSON_AddStringToObject(waveinfo, "LinkORLPass", buf);


		// 添加事件表
		cJSON *eventtable = cJSON_CreateObject();

		// 添加事件
		insertSOLAEvent(eventtable, waveLength);
		
		cJSON_AddItemToObject(waveinfo, "EventTable", eventtable);
		snprintf(buf, RESULT_BUF_MAX, "%d", i);
		cJSON_AddItemToObject(wavetable, buf, waveinfo);

		waveLength++;
	}
}


void SetMeasuringResultEnable(int enable)
{
	if ((enable == 0) || (enable == 1))
	{
		SaveEnable = enable;
	}
}


void SaveOTDRMeasuringResult()
{
	if (SaveEnable != 1)
		return;
	
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	CURVE_PRARM Param;

	int iNum = GetCurveNum(pDisplay);
	int i;
	for (i = 0; i < iNum; i++)
	{
		GetCurvePara(i, pDisplay, &Param);
		
		cJSON *json = cJSON_CreateObject();
		
		cJSON_AddStringToObject(json, "Type", "OTDR");
		// 插入波长、脉宽、量程、分辨率
		cJSON_AddStringToObject(json, "Wave", StrWaveLenght[Param.enWave]);
		cJSON_AddStringToObject(json, "Pulse", StrPulse[Param.enPluse]);
		cJSON_AddStringToObject(json, "Range", RangeKm[Param.enRange]);
		cJSON_AddStringToObject(json, "SmpRation", pOtdrTopSettings->pUser_Setting->sSampleSetting.enSmpResolution ? "1":"0");

		// 保存事件列表信息
		insertOTDREvent(i, json);
			
		// 保存json到文件
		saveFile(OTDR_RESULT, json);
		// 释放内存
		cJSON_Delete(json);
	}	
}

void SaveSOLAMeasuringResult()
{
	if (SaveEnable != 1)
		return;
	
	char buf[RESULT_BUF_MAX] = {0};
	int waveCount = 0;
	SOLA_WIDGET* solaWidget = getSolaWidget();
	SolaEvents* solaEvents = solaWidget->solaEvents;

	// 添加类型 SOLA
	cJSON *json = cJSON_CreateObject();
	cJSON_AddStringToObject(json, "Type", "SOLA");

	// 生成json object WaveTable
	cJSON *wavetable = cJSON_CreateObject();
	
	// 得到波长个数
	int i = 0;
	for (i = 0; i < WAVE_NUM; i++)
    {
        if(solaEvents->WaveIsSetted[i] == 1)
        {
            waveCount++;
        }
    }
	// 在 WaveTable 中加入 WaveCount
	snprintf(buf, RESULT_BUF_MAX, "%d", waveCount);
	cJSON_AddStringToObject(wavetable, "WaveCount", buf);
	

	// 充实WaveTable
	getSolaWaveTable(wavetable, waveCount);

	// json 中加入 WaveTable
	cJSON_AddItemToObject(json, "WaveTable", wavetable);

	// 保存json到文件
	saveFile(SOLA_RESULT, json);
	// 释放内存
	cJSON_Delete(json);
}

