#define MAXNUM 80000
#define DEFAULTCALIBRATIONFIBERLEN 5
#define DEFAULTREFRACTIVE 1.46
#define F216BLINDWUDTH 22
/*
 *返回下标以及下标对应的值
*/
typedef struct _index_value
{
    int index;
    float value;
}INDEX_VALUE;

/*
 *返回下标以及下标对应的值
*/
typedef struct index_list
{
    INDEX_VALUE indexValue[MAXNUM];
    int num;
}INDEX_LIST;

/*
 *寻找局部最大值
*/
INDEX_LIST* FindPeak(float *data, int len, int minPeakDistance, float thresh);

/*
 *寻找满足条件的结果
*/
INDEX_LIST* Find(int *data, int len, int(*f)(const void *, const void *), int referValue);

/*
 *对数表
*/
void LogTableCreat();

/*
 *对数转换
*/
void IntTransferLog(unsigned short *idata, int len, float *flogdata, int referValue);

