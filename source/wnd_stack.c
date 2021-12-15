/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_stack.c
* 摘    要：  分配栈空间，存储上一次销毁的窗口
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-1-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_stack.h"

#define MAXSTACKSIZE 10

static Stack *pWndStack = NULL;

/***
  * 功能：
        初始化栈
  * 参数：无
  * 返回：
        返回初始化栈的指针
  * 备注：
***/
Stack *InitStack(void)
{
	Stack * ps = (Stack *)malloc(sizeof(Stack));
	if(ps != NULL)
	{
		ps->top = NULL;
		ps->size = 0;
	}
	CODER_LOG(CoderGu,"wnd stack current init \n");

	return ps;
}

/***
  * 功能：
        判断栈是否为空
  * 参数：栈指针
  * 返回：
        为空返回1，不为空返回0
  * 备注：
***/
int IsEmpty(Stack *ps)
{
	if( (ps->top == NULL) && (ps->size == 0) )
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
        获得栈顶元素
  * 参数：
  		ps:传入的栈指针
  		pItem:需要返回返回栈顶元素节点的数据
  * 返回：
        返回栈顶元素节点
  * 备注：
***/
pNode GetTop(Stack *ps, Item *pItem)
{
	if( (IsEmpty(ps) != 1) && (pItem != NULL) )
	{
		*pItem = ps->top->data;
	}
	
	return ps->top;
}

/***
  * 功能：
        入栈
  * 参数：
  		ps:传入的栈指针
  		pItem:入栈元素节点的数据
  * 返回：
        返回入栈元素节点
  * 备注：
***/
pNode Push(Stack *ps, Item item)
{
	pNode pn = (pNode)malloc(sizeof(NODE));
	if(pn != NULL)
	{
		pn->data = item;
		pn->down = GetTop(ps, NULL);
		ps->top = pn;
		ps->size++;
		CODER_LOG(CoderGu,"wnd stack current size : %d\n", ps->size);
	}

	return pn;
}

/***
  * 功能：
        入栈前判断是否元素大小是否超出MAXSTACKSIZE(10)
  * 参数：
  		ps:传入的栈指针
  		pItem:入栈元素节点的数据
  * 返回：
        返回入栈元素节点
  * 备注：
***/
pNode WndPush(Stack *ps, Item item)
{
	pNode pn = NULL;
	if(ps->size >= MAXSTACKSIZE)
	{
		CODER_LOG(CoderGu,"wnd stack current size : %d\n", ps->size);
		ClearStack(ps);
	}
	pn = Push(ps, item);

	return pn;
}

/***
  * 功能：
        出栈
  * 参数：
  		ps:传入的栈指针
  		pItem:需要返回的出栈元素节点的数据
  * 返回：
        返回出栈元素节点
  * 备注：
***/
pNode Pop(Stack *ps, Item *pItem)
{
	pNode pn = ps->top;
	
	if( (IsEmpty(ps) != 1) && (pn != NULL) )
	{
		if(pItem != NULL)
		{
			*pItem = pn->data;
		}
		ps->size--;
		ps->top = ps->top->down;
		free(pn);
		pn = NULL;
		CODER_LOG(CoderGu,"wnd stack current size : %d\n", ps->size);
	}
	return ps->top;
}

/***
  * 功能：
        出栈
  * 参数：
  		ps:传入的栈指针
  		pItem:需要返回的出栈元素节点的数据
  * 返回：
        返回出栈元素节点
  * 备注：
***/
pNode WndPop(Stack *ps, Item *pItem, GUIWINDOW *pCurrWnd)
{
	pNode pn = NULL;

    GUIWINDOW *pWnd = NULL;

	pn = Pop(ps, pItem);
	if(NULL != pItem)
	{
	    SendWndMsg_WindowExit(pCurrWnd);      //发送消息以便退出当前窗体
	    (*pItem)(&pWnd);
	   	SendSysMsg_ThreadCreate(pWnd);       //发送消息以便调用新的窗体
	}

	return pn;
}

/***
  * 功能：
        清空栈内元素
  * 参数：
  		ps:传入的栈指针
  * 返回：
        无
  * 备注：
***/
void ClearStack(Stack *ps)
{
	while( IsEmpty(ps) != 1 )
	{
		Pop(ps, NULL);
	}
}

/***
  * 功能：
        销毁栈
  * 参数：
  		ps:传入的栈指针
  * 返回：
        无
  * 备注：
***/
void DestoryStack(Stack *ps)
{
	if( IsEmpty(ps) != 1)
	{
		ClearStack(ps);
	}
	free(ps);
	ps = NULL;
}

/***
  * 功能：
        栈遍历
  * 参数：
  		ps:传入的栈指针
  		visitfunc:对栈内每个元素的数据处理函数
  * 返回：
        无
  * 备注：
***/
void StackTraverse(Stack *ps, void (*visitfunc)())
{
	pNode p = ps->top;
	int i = ps->size;
	while(i--)
	{
		visitfunc(p->data);
		p = p->down;
	}
}

/***
  * 功能：
        得到窗口栈
  * 参数：无
  * 返回：
        无
  * 备注：
***/
Stack *GetCurrWndStack(void)
{
	return pWndStack;
}

/***
  * 功能：
        设置当前窗口栈
  * 参数：
  		ps:传入的栈
  * 返回：
        失败返回1，成功返回0
  * 备注：
***/
int SetCurrWndStack(Stack *ps)
{
	if(NULL == ps)
	{
		return 1;
	}
	pWndStack = ps;
	
	return 0;
}


