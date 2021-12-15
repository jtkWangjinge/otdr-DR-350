#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h>  
#include <sys/wait.h>

#define SERVER_MSG	1
#define CLIENT_MSG	2
#define BUFSIZE     1024
 
struct msg_st  
{  
    long int msg_type;  
    char text[BUFSIZE];  
};  

static int msgid = -1;  
static struct msg_st g_Message;

static int InitMsgQueue(void)
{
	int iErr = 0;

	//建立消息队列  
    msgid = msgget((key_t)2222, 0666 | IPC_CREAT);  
	if(msgid == -1)  
    {  
        fprintf(stderr, "msgget failed with error: %d\n", errno);  
        iErr = -1;  
    }  
	return iErr;
}

static int GetMsg(struct msg_st *pMsg)
{
	int iErr = 0;	

	if(msgrcv(msgid, (void*)&g_Message, BUFSIZE, CLIENT_MSG, 0) == -1)  
	{  
		iErr = -1; 
	} 
	else
	{
		memcpy(pMsg, &g_Message, sizeof(struct msg_st));
	}

	return iErr;
}


static int SendMsg(int iErrNo)
{
	int iErr = 0;

	g_Message.msg_type = SERVER_MSG;
	if (iErrNo == 0) {
		strcpy(g_Message.text, "OK");
	}
	else {
		strcpy(g_Message.text, "ERROR");
	}
	//printf("server ----------------- send message : %s\n",g_Message.text);
	if(msgsnd(msgid, (void*)&g_Message, 128, 0) == -1)  
	{  
		iErr = -1; 
	} 

	return iErr;
}

static void pr_exit(int status)
{	
	FILE *Fp = NULL;

	Fp = fopen("/mnt/sdcard/exit.log", "w+");

	if(WIFEXITED(status))
	{
		fprintf(Fp, "normal termination, exit status = %d\n",
				WEXITSTATUS(status));
	}
	else if(WIFSIGNALED(status))

	{
		fprintf(Fp, "abnormal termination, signal number = %d%s\n",
				WTERMSIG(status),
				#ifdef WCOREDUMP
					WCOREDUMP(status)?"(core file generated)" : "");
				#else
					"");
				#endif
	}
	else if(WIFSTOPPED(status))
	{
		fprintf(Fp, "child stopped, signal number = %d\n",
				WSTOPSIG(status));
	}
}
  
//  
static int mysystem(char *pCmd)
{
	int iReturn = 0;
	int iStatus = 0;
	FILE *Fp = NULL;
	char cResultBuff[1024];

	if(NULL == pCmd)
	{
		printf("pCmd is NULL\n");
		return -1;
	}

	/* 执行命令 */
	Fp = popen(pCmd, "r");
	/* 命令执行失败 */
	if(NULL == Fp)
	{
		printf("popen error: %s\n", strerror(errno));
		return -1;
	}

	//printf("%s\n", pCmd);

	/* 打印命令执行结果 */
    while(fgets(cResultBuff, sizeof(cResultBuff), Fp) != NULL)
    {
        printf("%s", cResultBuff);
    }	

    /*等待命令执行完毕并关闭管道及文件指针*/
    iStatus = pclose(Fp);
    if(-1 == iStatus)
    {
        printf("pclose fail!\n");
        return -2;
    }
    else
    {
       //printf("iStatus = %d\n", iStatus);
	   //printf("WEXITSTATUS(iStatus) = %d\n", WEXITSTATUS(iStatus));
	   iReturn = WEXITSTATUS(iStatus);
    }	

	return iReturn;
}


static int MsecSleep(unsigned long ulMsec)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
 
    struct timeval tv;

    tv.tv_sec = ulMsec / 1000;
    tv.tv_usec = ulMsec % 1000 * 1000;
    iReturn = select(0, NULL, NULL, NULL, &tv);

    return iReturn;
}

int main()  
{  
    int iStatus = 0;
    struct msg_st data;  
    pid_t pid = -1;

    InitMsgQueue();
   
reboot:
    if((pid=fork()) < 0)
	{
		return -1;
	}
	else if(pid == 0)
	{
        //子进程 
        int iErr = 0;
        while(1)  
        {  
            GetMsg(&data);
            //printf("copy: %s\n",data.text);  
            iErr = mysystem(data.text);
            if(iErr < 0)
            {
                printf("iErr = %d\n", iErr);
            }
            
            SendMsg(iErr);
        }  
    }
	else
	{
		wait(&iStatus);
		printf("popen ---------------------------------------------------------------------------------------------------------break!!\n");
		SendMsg(1);
		pr_exit(iStatus);	
		goto reboot;
	}
    
    return 0;
}  
