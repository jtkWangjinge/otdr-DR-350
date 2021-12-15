
#ifndef __QEP_H__
#define __QEP_H__


/* QSignal define*/
typedef unsigned int QSignal;

typedef struct QEventTag 
{
	QSignal sig;                //信号
    unsigned int dynamic;       //标志是否为malloc分配的事件
} QEvent;

typedef unsigned int QState;
//状态处理句柄
typedef QState (*QStateHandler) (void *me, const QEvent *e);

//Fsm定义(理解为抽象对象)，包含一个状态处理的句柄
//用户必须通过继承(c语言的单一继承)该对象来实例化一个状态机
typedef struct QFsmTag 
{
	QStateHandler state;
} QFsm;

//FSM的构造函数(构造函数仅执行基本的初始化)
//由实例化的状态机在自己的构造函数中调用该函数来构造父类FSM
#define QFsm_ctor(me_, initial_) ((me_)->state = (initial_))

//实例化的状态机构造完成之后，由用户代码调用该函数执行初始化转换
//状态机在此阶段进入初始状态
void QFsm_init 		(QFsm *me, const QEvent *e);
//状态机的事件派发函数
void QFsm_dispatch	(QFsm *me, const QEvent *e);

//处理结果相关定义
#define Q_RET_HANDLED		((QState)0)
#define Q_RET_IGNORED		((QState)1)
#define Q_RET_TRAN			((QState)2)

#define Q_HANDLED()			(Q_RET_HANDLED)
#define Q_IGNORED()			(Q_RET_IGNORED)
//执行状态转换 将目标状态赋予状态机的状态处理指针
#define Q_TRAN(target_)		\
	(((QFsm *)me)->state = (QStateHandler)(target_), Q_RET_TRAN)


//FSM处理器保留的事件信号
enum QReservedSignals 
{
	Q_ENTRY_SIG = 1,   	//ENTRY动作信号
	Q_EXIT_SIG,			//EXIT动作信号
	Q_INIT_SIG,			//初始转换信号
	Q_USER_SIG,			//用户可以使用的第一个信号
};

#endif	//__QEP_H__
