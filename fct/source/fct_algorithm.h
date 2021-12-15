#define MAXNUM 80000
#define DEFAULTCALIBRATIONFIBERLEN 5
#define DEFAULTREFRACTIVE 1.46
#define F216BLINDWUDTH 22
/*
 *�����±��Լ��±��Ӧ��ֵ
*/
typedef struct _index_value
{
    int index;
    float value;
}INDEX_VALUE;

/*
 *�����±��Լ��±��Ӧ��ֵ
*/
typedef struct index_list
{
    INDEX_VALUE indexValue[MAXNUM];
    int num;
}INDEX_LIST;

/*
 *Ѱ�Ҿֲ����ֵ
*/
INDEX_LIST* FindPeak(float *data, int len, int minPeakDistance, float thresh);

/*
 *Ѱ�����������Ľ��
*/
INDEX_LIST* Find(int *data, int len, int(*f)(const void *, const void *), int referValue);

/*
 *������
*/
void LogTableCreat();

/*
 *����ת��
*/
void IntTransferLog(unsigned short *idata, int len, float *flogdata, int referValue);

