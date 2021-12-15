#include "fct_algorithm.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

float LogSigTable[65536]={0};

/*
 *������
*/
void LogTableCreat()
{
	int i;
	LogSigTable[0] = 0.0f;
	
	for(i = 1;i < 65536;i++)
	{
		LogSigTable[i] = 5.0f*log10f((float)i);
	}
}

/*
 *���ֵ
*/
int GetMaxInt(int *pSignal,int iSignalLength)  
{ 
    int i = 0; 
    int MaxNumber = 0; 
    for (i = 0; i<iSignalLength; i++) 
    { 
        if (pSignal[i]>MaxNumber) 
        { 
             MaxNumber = pSignal[i]; 
        } 
    } 

return MaxNumber; 
}

/*
 *����ת��
*/
void IntTransferLog(unsigned short *idata, int len, float *flogdata, int referValue)
{
	//����ת�� 
	int  i = 0;

	for(i = 0;i < len;i++)
	{
		int Temp = referValue - (int)idata[i];
		
		if ((Temp > 0) && (Temp < 65536))
		{
			flogdata[i] = LogSigTable[Temp+1];
		}
		else if((Temp > -65536) && (Temp < 0))
		{
			flogdata[i] = - LogSigTable[1-Temp];
		}		
		else
		{
			flogdata[i] = 0;
		}
	}
}

/*
 *�ȽϺ���
*/
int CompareFloat(const void *a, const void *b)
{
    return (*((float*)a)> *((float*)b));
}

/*
 *�ж��Ƿ����
*/
int EqualInt(const void *a, const void *b)
{
    return ((*(int*)a) == (*(int*)b));
}

/*
 *Ѱ�Ҽ���ֵ
*/
INDEX_LIST* FindPeak(float *data, int len, int minPeakDistance, float thresh)
{
    int i = 0;

    // һ�׵���
    int *dy = (int*)malloc(sizeof(int)*len);
    memset(dy, 0, sizeof(int)*len);
    for (i = 1; i < len; i++)
    {
        dy[i] = ((data[i]>data[i - 1]) ? 1 : ((data[i] == data[i - 1]) ? 0 : (-1)));
    }

    int referValue = 0;
    INDEX_LIST *p = Find(dy, len, EqualInt, referValue);
    printf("------------------num=%d------------------\n", p->num);

    // פ��
    for (i = 0; i < p->num; i++)
    {
        if (p->indexValue[i].index + 1 >= len)
        {
            break;
        }
        else
        {
            if (p->indexValue[i].index + 1>=0)
            {
                dy[p->indexValue[i].index] = 1;
            }
            else
            {
                dy[p->indexValue[i].index] = -1;
            }
        }
    }
    free(p);
    p = NULL;

    // ���׵���
    int *dy2 = (int*)malloc(sizeof(int)*len);
    memset(dy2, 0, sizeof(int)*len);
    for (i = 2; i < len; i++)
    {
        dy2[i] = dy[i] - dy[i - 1];
    }


    referValue = -2;
    p = Find(dy2, len, EqualInt, referValue);


    int indexArray[10] = { 0 };
    int cnt = 0;
    // ɾ��С����ֵ�ļ�ֵ��
    for (i = 0; i < p->num; i++)
    {
        if (cnt>=10)
        {
        	printf("��ֵ���ù�С��������·�лز��϶�!!!");
            break;
        }
        if (data[p->indexValue[i].index]>thresh)
        {
            indexArray[cnt] = p->indexValue[i].index;
            cnt++;
        }
    }

     // ɾ��С�ڼ�ֵ����С����ĵ�
    int num = 0;
    int finalMaxValue[10] = { 0 };
    int j = 0;
    for (i = 0; i < cnt;)
    {
        int sameCount = 1;
        for (j = i + 1; j < cnt; j++)
        {
            if (indexArray[j] - indexArray[i] < minPeakDistance)
            {
                sameCount++;
            }
            else
            {
                break;
            }
        }
        finalMaxValue[num++] = GetMaxInt(indexArray + i, sameCount);
        if (sameCount == 1)
        {
            i++;
        }
        else
        {
            i = sameCount + i;
        }
    }
    printf("num=%d\n", num);
    for (i = 0; i < num; i++)
    {
        printf("finalMaxValue[%d]=%d\n", i, finalMaxValue[i]);
    }


    // ���¸�ֵ
    for (i = 0; i < num; i++)
    {
        p->indexValue[i].index = finalMaxValue[i];
        p->indexValue[i].value = data[p->indexValue[i].index];
    }

    p->num = num;

    for (i = 0; i < p->num; i++)
    {
        printf("p->indexValue[%d].index=%d\n", i, p->indexValue[i].index);
    }
    
    free(dy);
    dy = NULL;
    return p;
}

/*
 *Ѱ�����������Ľ��
 *...
 *@return   �±��Լ��±��Ӧ��ֵ
 *@note     ��Ҫ�ֶ��ͷ��ڴ�
*/
INDEX_LIST* Find(int *data, int len, int(*f)(const void *, const void *), int referValue)
{
    int i = 0, cnt = 0;

    INDEX_LIST *indexList = (INDEX_LIST*)malloc(sizeof(INDEX_LIST));
    memset(indexList, 0, sizeof(INDEX_LIST));
    
    INDEX_VALUE *indexValue = (INDEX_VALUE*)malloc(sizeof(INDEX_VALUE)*MAXNUM);
    memset(indexValue, 0, sizeof(INDEX_VALUE)*MAXNUM);
    for (i = 0; i < len; i++)
    {
        if (f(&data[i], &referValue) == 1)
        {
            indexValue[cnt].index = i;
            indexValue[cnt].value = data[i];
            cnt++;
        }
    }
    memcpy(&indexList->indexValue, indexValue, sizeof(INDEX_VALUE)*cnt);
    indexList->num = cnt;
    free(indexValue);
    return indexList;
}
