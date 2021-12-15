/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_stack.h
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

#ifndef _WND_STACK_H
#define _WND_STACK_H

#include "guiglobal.h"
#include "app_global.h"

typedef void (*Item)(GUIWINDOW **);
typedef struct node * pNode;

typedef struct node
{
	Item data;
	pNode down;
}NODE;

typedef struct stack
{
	pNode top;
	int size;
}Stack;

//初始化栈
Stack *InitStack(void);

//判断栈是否为空
int IsEmpty(Stack *ps);

//获得栈顶元素
pNode GetTop(Stack *ps, Item *pItem);

//入栈
pNode Push(Stack *ps, Item item);

//入栈前判断是否元素大小是否超出MAXSTACKSIZE(10)
pNode WndPush(Stack *ps, Item item);

//出栈
pNode Pop(Stack *ps, Item *pItem);
//出栈调用窗口
pNode WndPop(Stack *ps, Item *pItem, GUIWINDOW *pCurrWnd);

//清空栈内元素
void ClearStack(Stack *ps);

//销毁栈
void DestoryStack(Stack *ps);

//栈遍历
void StackTraverse(Stack *ps, void (*visitfunc)());

//得到窗口栈
Stack *GetCurrWndStack(void);

//设置当前窗口栈
int SetCurrWndStack(Stack *ps);

#endif

