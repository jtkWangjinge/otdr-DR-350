#include "app_qep.h"


/*
 * Fsm 状态机的保留事件
 **/
static const QEvent QEP_reservedEvt_[] = {
	{(QState)0, 			(unsigned int)0},
	{(QState)Q_ENTRY_SIG, 	(unsigned int)0},
	{(QState)Q_EXIT_SIG, 	(unsigned int)0},
	{(QState)Q_INIT_SIG, 	(unsigned int)0},			
};

/*
 * Fsm 的初始化状态机
 **/
void QFsm_init(QFsm *me, const QEvent *e)
{
	//执行状态机的初始化转换
	me->state(me, e);
	//处理新状态的ENTRY事件
	me->state(me, &QEP_reservedEvt_[Q_ENTRY_SIG]);
}

/*
 * Fsm 的事件派发函数
 **/
void QFsm_dispatch(QFsm *me, const QEvent *e)
{	
	//保存当前状态
	QStateHandler s = me->state;
	//执行当前状态事件处理
	QState r = s(me, e);

	//如果发生了状态转换
	if (r == Q_RET_TRAN)
	{
		//执行源状态的EXIT事件
		s(me, &QEP_reservedEvt_[Q_EXIT_SIG]);
		//执行目标状态(新的当前状态)的ENTRY事件
		me->state(me, &QEP_reservedEvt_[Q_ENTRY_SIG]);
	}
}