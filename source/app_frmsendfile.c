/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmsendfile.h
* 摘    要：  实现文件传输功能的实现和接口定义
*
* 取代版本：
* 原 作 者：
* 完成日期：
*
* 
*******************************************************************************/
#include "app_frmsendfile.h"
#include "app_global.h"

#include <sys/socket.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <netdb.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/stat.h> 
#include <arpa/inet.h>  
#include <ifaddrs.h> 
#include <netinet/tcp.h>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#define PORTNUMBER 3333

#define DATA_LENGTH 5000

pthread_t sendfile_main = -1;			//发送文件主线程，配置网络和监听pc端的连接

int inno_sockfd;						//服务器套接字描述符
int pc_sockfd;	   						//客户端套接字描述符

SEND_INFO FileSend_Info;				//文件发送信息
SELECT_FILE_INFO_SINGLE sendingFile;	//正在发送的文件

int connectState(int sock);

#define IDENTIFY        0x23            //标示符

#define FUN_FILECOUNT   0x01            //发送文件数量功能码
#define FUN_FILEINFO    0x02            //发送文件信息功能码
#define FUN_FILECONTENT 0x03            //发送文件内容功能码
#define FUN_HEARTBEAT   0x04            //发送心跳包

#define COT_ACT         0x03            //传输激活
#define COT_OVER        0x04            //传输停止
#define COT_CONFIRM     0x05            //肯定确认
#define COT_NEGATIVE    0x06            //否定确认
#define COT_SECTION     0x07            //段传输完成
#define COT_FILE        0x08            //文件传输完成

#define HEAD_LEN        0x08            //帧头长度

typedef struct _frame_head
{
	char identifyBegin;                 //开始标示符
	char function;						//功能码
	char reason;                        //传送原因
	char sectionNum;                    //段序号
	char partNum;                       //节序号
	char highDataLen;                   //文件内容高字节
	char lowDataLen;                    //文件内容低字节
	char identifyEnd;					//结束标示符
}__attribute__((packed))FRAME_HEAD;   //帧头定义

typedef struct _frame_cmd
{
	FRAME_HEAD head;				    //帧头
}__attribute__((packed))FRAME_CMD;    //没有内容的命令包

typedef struct _frame_response
{
	FRAME_HEAD head;				      //帧头
}__attribute__((packed))FRAME_RESPONSE;//帧内容响应包

typedef struct _frame_content
{
	FRAME_HEAD head;					//帧头
	char       data[DATA_LENGTH];				//数据内容
}__attribute__((packed))FRAME_CONTENT;//带数据内容的包

/***
  * 功能：
        释放存储文件列表申请的内存
  * 参数：

  * 返回：

  * 备注：
  		
***/
int freeFileInfo(void)
{
	if(NULL != FileSend_Info.pfileList)
	{
		free(FileSend_Info.pfileList);
		FileSend_Info.pfileList = NULL;
	}

	FileSend_Info.fileReady = 0;
	return 0;
}


/*得到文件发送开关状态*/
int getsendFileSwitch(void)
{
	return FileSend_Info.sendFileSwitch;
}

/*设置文件发送开关状态*/
void setsendFileSwitch(int sendswitch)
{
	FileSend_Info.sendFileSwitch = sendswitch;
}

/*得到是否已连接到PC*/
int getconnectToPC(void)
{
	return FileSend_Info.connectToPc;
}


/*得到已发送文件数*/
int getsendCount(void)
{
	return FileSend_Info.sendCount;
}

/*获得发送文件总数*/
int getfileCount(void)
{
	return FileSend_Info.fileCount;
}

/*设置回调函数(调用时传入的参数为当前正在发送文件已发送的大小)*/
void setcallback(SENDFILECALLBACK1 callback)
{
	FileSend_Info.callbackfunc = callback; 
}

/*设置发送完成后的回调函数(调用时传入的参数为成功传送个数)*/
void setcompletecallback(SENDFILECALLBACK callback)
{
	FileSend_Info.callbackcomplete = callback; 
}

/*设置发送完成后的回调函数(调用时传入的参数为成功传送个数)*/
void setdisconnectcallback(SENDFILECALLBACK callback)
{
	FileSend_Info.callbackdisconnect = callback; 
}

/*获得正在传送文件信息*/
SELECT_FILE_INFO_SINGLE getsendfile(void)
{
	return sendingFile;
}


/***
  * 功能：
        设置需发送的文件信息
  * 参数：
        1:char **path:文件路径列表
        2:int count:文件个数
  * 返回：
        成功返回0，失败返回-1，路径列表中包含目录返回1
  * 备注：
  		
***/
int setFileInfo(char **path, int count)
{
	int loop = 0;
	int iErr = 0;
	
	if(path == NULL || count == 0)
		return -1;

	freeFileInfo();

	FileSend_Info.pfileList = (SELECT_FILE_INFO_SINGLE *)malloc(count * sizeof(SELECT_FILE_INFO_SINGLE));
	
	if(FileSend_Info.pfileList == NULL)
		return -1;
	
	memset(FileSend_Info.pfileList, 0, count * sizeof(SELECT_FILE_INFO_SINGLE));
	FileSend_Info.fileCount = count;

	for(;loop<count;loop++)
	{
		char *tempname;
		char *temppath = path[loop];
		struct stat fileInfo;
		
		if(temppath == NULL)
			continue;
		if(access(temppath,F_OK) == -1)
			continue;
		
		memcpy(FileSend_Info.pfileList[loop].pfilepath, temppath, strlen(temppath));

		tempname = strrchr(temppath, '/');

		if(tempname != NULL)
		{
			memcpy(FileSend_Info.pfileList[loop].pfilename, tempname+1, strlen(tempname) - 1);
		}

		if(-1 == stat(temppath, &fileInfo))
		{
			perror("file stat:");
			continue;
		}

		FileSend_Info.pfileList[loop].size = fileInfo.st_size;

		if(S_ISREG(fileInfo.st_mode))				//文件
		{
			FileSend_Info.pfileList[loop].type = 0;
		}
		else if(S_ISDIR(fileInfo.st_mode))			//目录
		{
			FileSend_Info.pfileList[loop].type = 1;
			iErr = 1;
		}
		else										//其他
		{
			FileSend_Info.pfileList[loop].type = 2;
		}

		LOG(LOG_INFO, "path = %s    name = %s   size = %ld   type = %d\n", FileSend_Info.pfileList[loop].pfilepath,
			FileSend_Info.pfileList[loop].pfilename,
			FileSend_Info.pfileList[loop].size,
			FileSend_Info.pfileList[loop].type);
	}
	
	if(iErr != 0)
	{
		freeFileInfo();
		return 1;
	}
	
	FileSend_Info.sendCount = 0;
	FileSend_Info.fileReady = 1;
	FileSend_Info.sendFileSwitch = 1;
	return 0;
}




/***
  * 功能：
        发送一个数据包，并等待客户端反馈
   * 参数：
        1.char *dataBuffer:		数据缓存区指针
        2.int dataLength:		数据长度
        3.int sockfd:			套接字描述符
        4.int needResponse     是否等待回应
  * 返回：
          成功返回1，失败返回0，错误返回-1,重发返回2
  * 备注：
  		  失败为未收到服务器端响应，或反馈错误
***/
int sendData(char *dataBuffer, int dataLength, int sockfd, int needResponse)
{
	int ret=0;
	fd_set rdfds; /* 先申明一个 fd_set 集合来保存我们要检测的 socket描述符 */
	struct timeval tv; /* 申明一个时间变量来保存时间 */
	
	FD_ZERO(&rdfds); /* 用select函数之前先把集合清零 */
	FD_SET(sockfd, &rdfds); /* 把要检测的描述符socket加入到集合里 */
	
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	//
	if(write(sockfd, dataBuffer, dataLength) == -1)         
	{             
		fprintf(stderr, "Write Error:%s\n", strerror(errno));             
		return -1;
	}	
	if(needResponse == 1)
    {
        FRAME_RESPONSE response;
        ret = select(sockfd + 1, &rdfds, NULL, NULL, &tv); /* 检测我们上面设置到集合rdfds里的描述符是否有可读信息 */
        if(ret < 0)
        {
            perror("select");/* 这说明select函数出错 */
        }
        else if(ret == 0)
        {
			LOG(LOG_ERROR, "time out\n");
			if(connectState(sockfd))
            {
                shutdown(sockfd, SHUT_RDWR);
				return -1;
            }
        }
        else
        {
            if(FD_ISSET(sockfd, &rdfds))
            {
                ret = read(sockfd, &response, sizeof(response));
                if(ret == -1)
                {
                    perror("send file read:");
                }
                if((response.head.identifyBegin == IDENTIFY) && (response.head.identifyEnd == IDENTIFY))
                {
                    switch(response.head.reason)
                    {
                    case COT_CONFIRM:
                        return 1;
                    case COT_NEGATIVE:
                        return 2;
					default:
						break;
                    }
                }
            }
        }
    }
	
	return 1;
}

/***
  * 功能：
        构造报文头
   * 参数：
        1.FRAME_HEAD *pFrameHead:		报文头指针
        2.char func:                    功能码
        3.char reason:                  传送原因
        4.char sectionNum:              段序号
        5.char partNum:                 节序号
        6.short contentLen:             内容长度
  * 返回：
         
  * 备注：
  		
***/
void BuildFrameHead(FRAME_HEAD *pFrameHead, char func, char reason,char sectionNum,char partNum,short contentLen)
{
    pFrameHead->identifyBegin = IDENTIFY;
    pFrameHead->function = func;
    pFrameHead->reason = reason;
	pFrameHead->sectionNum = sectionNum;
	pFrameHead->partNum = partNum;
	pFrameHead->highDataLen = (char)((contentLen >> 8)& 0xff); 
	pFrameHead->lowDataLen = (char)(contentLen & 0xff);
	pFrameHead->identifyEnd = IDENTIFY;
}

/***
  * 功能：
        发送文件数量
   * 参数：
        1.short fileCount:		需要发送的文件数
        2.int sockfd:           套接字描述符
  * 返回：
        成功返回1，失败返回0，错误返回-1
  * 备注：
  		
***/
int BuildFileCountFrame(short fileCount,int sockfd)
{
	FRAME_CONTENT frame;
	memset(&frame, 0, sizeof(frame));
	BuildFrameHead(&frame.head, FUN_FILECOUNT, COT_ACT, 0, 0, sizeof(fileCount));
	memcpy(frame.data, &fileCount, sizeof(fileCount));
	return sendData((char*)&frame, HEAD_LEN + sizeof(fileCount), sockfd, 1);
}

/***
  * 功能：
        发送文件数量
  * 参数：
        1.char* fileName:		     发送文件名
        2.int fileNameLen:           文件名长度
        3.int fileContentLen:        文件内容长度
        4.int sockfd:                套接字描述符
* 返回：
        成功返回1，失败返回0，错误返回-1
* 备注：

***/
int BuildFileNameFrame(char* fileName,int fileNameLen,long fileContentLen,int sockfd)
{
    FRAME_CONTENT frame;
    memset(&frame, 0, sizeof(frame));
    BuildFrameHead(&frame.head, FUN_FILEINFO, COT_ACT, 0, 0, (short)(sizeof(fileContentLen) + fileNameLen));
    memcpy(frame.data, &fileContentLen, sizeof(fileContentLen));
    memcpy(frame.data + sizeof(fileContentLen), fileName, fileNameLen);
    return sendData((char*)&frame, HEAD_LEN + sizeof(fileContentLen) + fileNameLen, sockfd, 1);
}

/***
  * 功能：
    构造命令报文
  * 参数：
    1.char func:                    功能码
    2.char reason:                  发送原因
    3.char sectionNum:              段序号
    4.char partNum:                 节序号
    5.int sockfd:                   套接字描述符
  * 返回：
        成功返回1，失败返回0，错误返回-1，重发返回2
  * 备注：

***/
int BuildCmdFrame(char func, char reason, char sectionNum, char partNum,int sockfd)
{
    FRAME_CMD  frame;
    memset(&frame, 0, sizeof(frame));
    BuildFrameHead(&frame.head, func, reason, sectionNum, partNum, 0);
    return sendData((char*)&frame, HEAD_LEN, sockfd, 1);
}

/***
  * 功能：
    构造命令报文
  * 参数：
    1.char* content:              文件内容
    2.contentLen:                 文件内容长度
    3.char sectionNum:            段序号
    4.char partNum:               节序号
    5.int sockfd:                 套接字描述符
  * 返回：
    成功返回1，失败返回0，错误返回-1
  * 备注：
***/
int BuildFileContentFrame(char* content, short contentLen, char sectionNum, char partNum, int sockfd)
{
    FRAME_CONTENT  frame;
    memset(&frame, 0, sizeof(frame));
    BuildFrameHead(&frame.head, FUN_FILECONTENT, COT_ACT, sectionNum, partNum, contentLen);
    memcpy(frame.data, content, contentLen);
    return sendData((char*)&frame, HEAD_LEN + contentLen, sockfd, 0);
}

/***
  * 功能：
        发送文件函数
   * 参数：
        1.int sockfd:		套接字描述符
  * 返回：

  * 备注：
  		
***/

void SendFileToPC(int sockfd)
{
	int ret = 0;
	static char sendBuffer[102400];
	

	if(sockfd == -1)
		return;
    
	memset(sendBuffer, 0, sizeof(sendBuffer));

	//发送文件数量帧
	ret = BuildFileCountFrame(FileSend_Info.fileCount, sockfd);
	if(ret == 0 || ret == -1)
	{
		goto __exit;
	}

	int i = 0;
	for(; i < FileSend_Info.fileCount; i++)
	{
		FILE *stream;
		long long sendsize = 0;
		int size = 0;
        char sectionNum = 0;
        ret = BuildFileNameFrame(FileSend_Info.pfileList[i].pfilename, strlen(FileSend_Info.pfileList[i].pfilename),
                                    FileSend_Info.pfileList[i].size, sockfd);
        //发送文件名和文件大小
		if(ret == 0 || ret == -1)
		{
			goto __exit;
		}
		
		stream = fopen(FileSend_Info.pfileList[i].pfilepath, "r");
		if(stream == NULL)
		{
			perror("fopen:");
			continue;
		}
		
		sendingFile = FileSend_Info.pfileList[i];
		while(FileSend_Info.sendFileSwitch)
		{
            //读取文件内容 若读取到的为0 发送文件发送完毕帧
			if(0 == (size = fread(&sendBuffer, (size_t)sizeof(char), (size_t)(DATA_LENGTH * 10), stream)))
			{
                ret = BuildCmdFrame(FUN_FILECONTENT, COT_FILE, 0, 0, sockfd);
				if(ret == 0 || ret == -1)
				{
					fclose(stream);
					goto __exit;
				}
                else if(ret == 2)
                {

                    fclose(stream);
                    i = i - 1;
                }
				break;
			}
            ++sectionNum;
            //发送文件内容
            char frameCount = size / DATA_LENGTH + (size % DATA_LENGTH ? 1 : 0); //计算当前段需要发送的节数
            char partNum = 0;
			
__retransmission://重发这一段内容
            for (partNum = 0; partNum < frameCount; partNum++)
            {
                short contentLen = (size - partNum * DATA_LENGTH) > DATA_LENGTH ? DATA_LENGTH : size - partNum * DATA_LENGTH;
                ret = BuildFileContentFrame(sendBuffer + DATA_LENGTH * partNum, contentLen, sectionNum, partNum, sockfd);
                if (ret == 0 || ret == -1)
                {
                    fclose(stream);
                    goto __exit;
                }
            }

			//最后一节最后发完，发送段确认帧
            ret = BuildCmdFrame(FUN_FILECONTENT, COT_SECTION, sectionNum, partNum, sockfd);
			if (!connectState(sockfd))
			{
				goto __exit;
			}
            if (ret == 0 || ret == -1)
            {
                fclose(stream);
                goto __exit;
            }
            else if (ret == 2)
            {
                goto __retransmission;
            }

            sendsize += size;
			LOG(LOG_INFO, "read %lld\n", sendsize);

			if(FileSend_Info.callbackfunc && FileSend_Info.sendFileSwitch)
			{
				FileSend_Info.callbackfunc(sendsize);
			}
		}
		fclose(stream);
		if(!FileSend_Info.sendFileSwitch)
		{
			break;
		}
		
		FileSend_Info.sendCount++;
	}
	
	if(FileSend_Info.callbackcomplete && (FileSend_Info.sendCount == FileSend_Info.fileCount))
	{
		FileSend_Info.callbackcomplete(FileSend_Info.sendCount);
	}
	
__exit:
	LOG(LOG_ERROR, "Exit  !!!!!!!\n");
}


/***
  * 功能：
        判断和上位机的连接状态
  * 参数：
        1.int sock:套接字
  * 返回：
        处于连接状态返回1，否则返回0；
  * 备注：
***/
int connectState(int sock)
{
	struct tcp_info info; 
	int len = sizeof(info); 
	
	getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len); 
	if((info.tcpi_state == TCP_ESTABLISHED)) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/***
  * 功能：
        创建套接字并监听端口线程
  * 参数：
        1.void *pThreadArg:		创建新线程时传递的参数
  * 返回：
          成功返回GUI线程地址，失败返回NULL
  * 备注：
***/
void* ConnectToPC(void *pThreadArg)
{
	struct sockaddr_in server_addr; //描述服务器地址    
	struct sockaddr_in client_addr; //描述客户端地址    
	socklen_t sin_size;     

	char *connectVeri_server = "#INNO_DEVICE#";
	char *connectVeri_client = "#INNO_COMPUTER#";
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	/* 服务器端开始建立sockfd描述符 */     
	if((inno_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) // AF_INET:IPV4;SOCK_STREAM:TCP    
	{         
		fprintf(stderr,"Socket error:%s\n",strerror(errno));         
		return NULL;     
	}     
		
	/* 服务器端填充 sockaddr结构 */     
	bzero(&server_addr, sizeof(struct sockaddr_in)); // 初始化,置0    
	server_addr.sin_family = AF_INET; // Internet    
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // (将本机器上的long数据转化为网络上的long数据)和任何主机通信 //INADDR_ANY 表示可以接收任意IP地址的数据，即绑定到所有的IP    
	server_addr.sin_port = htons(PORTNUMBER); // (将本机器上的short数据转化为网络上的short数据)端口号        
	
	int opt = 1; 
	setsockopt(inno_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 

	/* 捆绑sockfd描述符到IP地址 */     
	if(bind(inno_sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))==-1)     
	{         
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));         
		return NULL;    
	}     

	/* 设置允许连接的最大客户端数 */     
	if(listen(inno_sockfd, 1) == -1)     
	{         
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));         
		return NULL;     
	} 

	memset(&FileSend_Info, 0, sizeof(SEND_INFO));
	
	while(1)     
	{         
		/* 服务器阻塞,直到客户程序建立连接 */         
		sin_size=sizeof(struct sockaddr_in);
		LOG(LOG_INFO, "accept \n");
		if((pc_sockfd = accept(inno_sockfd,(struct sockaddr *)(&client_addr), &sin_size))==-1)         
		{             
			fprintf(stderr, "Accept error:%s\n\a", strerror(errno));             
			continue;          	
		}         
		fprintf(stderr, "Server get connection from %s\n", inet_ntoa(client_addr.sin_addr)); // 将网络地址转换成.字符串，并打印到输出终端


		if(write(pc_sockfd, connectVeri_server, strlen(connectVeri_server))==-1)		   //发送身份验证信息
		{			  
			fprintf(stderr, "Write Error:%s\n", strerror(errno)); 			
			close(pc_sockfd);
			continue; 
		}		

		int rece = 0;				
		char buffer[1024];				
		rece = read(pc_sockfd, buffer, 1024);			//接收身份验证		
		if(rece == 0)				
		{
			LOG(LOG_INFO, "server receive nothing");
			FileSend_Info.connectToPc = 0;
			close(pc_sockfd);
			continue;
		} 				
		else				
		{		
			if(0 == strcmp(buffer, connectVeri_client))
			{
				LOG(LOG_INFO, "Connect to INNO upper computer !!!\n");
				FileSend_Info.connectToPc = 1;
			}
			else
			{
				LOG(LOG_ERROR, "not connect INNO upper computer !!!\n");
				FileSend_Info.connectToPc = 0;
				close(pc_sockfd);
				continue;
			}
		}  

		while(connectState(pc_sockfd))
		{
			if(0 == FileSend_Info.fileReady)
			{
				LOG(LOG_INFO, "Heart0\n");
				BuildCmdFrame(FUN_HEARTBEAT, COT_ACT, 0, 0, pc_sockfd);
				LOG(LOG_INFO, "Heart1\n");
				sleep(1);
				continue;
			}
			
			
			SendFileToPC(pc_sockfd);

			if(connectState(pc_sockfd))
			{
                BuildCmdFrame(FUN_HEARTBEAT, COT_OVER, 0, 0, pc_sockfd);
				LOG(LOG_INFO, "Over\n");
			}
			else if(FileSend_Info.callbackdisconnect != NULL)
			{
				FileSend_Info.callbackdisconnect(0);
			}
			
			freeFileInfo();
		}
		LOG(LOG_INFO, "disconnect  \n");
		FileSend_Info.connectToPc = 0;
		close(pc_sockfd); 
	}     
	/* 结束通讯 */     
	close(inno_sockfd);
	LOG(LOG_INFO, "end  \n");
	pthread_exit(NULL);
}

/*取消文件发送线程*/
int CancelSendFile(void)
{
	if(FileSend_Info.fileReady && !FileSend_Info.connectToPc)
	{
		freeFileInfo();
	}
	
	FileSend_Info.sendFileSwitch = 0;
	
	return 1;
}


/*初始化文件发送功能*/
int Sendfile_Init(void)
{
	int err = 0;
 
	err = pthread_create(&sendfile_main, NULL, ConnectToPC, NULL);

	if(err != 0)
	{
		perror("create thread sendFileToPC:");
		return -1;
	}

	return 0;
}


/*退出文件发送功能*/
int Sendfile_Quit(void)
{
	CancelSendFile();
	if(connectState(pc_sockfd))
	{
		shutdown(pc_sockfd, SHUT_RDWR);
	}
	
	pthread_cancel(sendfile_main);

	return 0;
}



