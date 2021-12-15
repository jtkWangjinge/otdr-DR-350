#ifndef __LIST_H__
#define __LIST_H__

#include "FIncludes.h"

#ifdef __cplusplus
extern "C" {
#endif

struct f_list_private;

typedef struct f_list {
	struct f_list_private *d;
} FList;

typedef struct f_list_iterator
{
	int		i;
	void* 	p;
} FListIterator;

typedef BOOL (*FListCompare)(const void*, const void*);

#define f_container_of(ptr, type, member) \
({\
	const typeof(((type *)0)->member) *__mptr = (ptr);\
	(type *)((char *)__mptr - ((size_t)&(type *)0)->member));\
})

void 	FList_ctor				(FList* me);
void 	FList_dtor				(FList* me);

BOOL 	FList_isEmpty			(FList* me);
U32 	FList_count				(FList* me);

BOOL 	FList_insertAtIndex		(FList* me, U32 index, const void* data);
void* 	FList_takeAtIndex		(FList* me, U32 index);
void* 	FList_atIndex			(FList* me, U32 index);

BOOL 	FList_insertBeforeIter	(FList* me, FListIterator* iter, const void* data);
void*	FList_takeAtIter   		(FList* me, FListIterator* iter);
void*	FList_atIter			(FList* me, FListIterator* iter);

void* 	FList_popBack			(FList* me);
BOOL 	FList_pushBack			(FList* me, const void* data);
void 	FList_clear				(FList* me);

void 	FList_output			(FList* me, FILE* stream);
void	FList_debug				(FList* me);

FListIterator FList_begin		(FList* me);
FListIterator FList_end			(FList* me);

//Algorithm
BOOL	FList_quickSort 		(FList* me, FListCompare compare);
int		FList_findForward   	(FList* me, const void* data, FListCompare compare);
int		FList_findBackward  	(FList* me, const void* data, FListCompare compare);

//Iterator
BOOL 	FListIterator_isVaild	(FListIterator* me);
void* 	FListIterator_dataOf	(FListIterator* me);
void 	FListIterator_inc		(FListIterator* me);
void 	FListIterator_dec		(FListIterator* me);
void 	FListIterator_addSelf	(FListIterator* me, U32 addend);
void 	FListIterator_subSelf	(FListIterator* me, U32 subtractor);
BOOL 	FListIterator_lessThan	(FListIterator* me, FListIterator* other);
BOOL 	FListIterator_largeThan	(FListIterator* me, FListIterator* other);
BOOL 	FListIterator_equal		(FListIterator* me, FListIterator* other);
BOOL 	FListIterator_notEqual	(FListIterator* me, FListIterator* other);


#ifdef __cplusplus
}
#endif
#endif //__LIST_H__
