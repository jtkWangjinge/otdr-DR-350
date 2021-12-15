/******************************************************************************
 *  名称：include/guitype.h
 *  描述：GUI的基本头文件，定义GUI的类型常量
 *  历史：
 *  备注：适用于EP9307，ARM-Linux
 ******************************************************************************/

#ifndef _GUI_TYPE_H_
#define _GUI_TYPE_H_


/*
  定义类型常量
  +----------------------------------------------------------+
  |                       类型常量                           |
  |---------------+--------------+-------------+-------------|
  | bit31 ~ bit12 | bit11 ~ bit8 | bit7 ~ bit4 | bit3 ~ bit0 |
  |   类型标志    |   符号标志   |  指针标志   |  对象标志   |
  +---------------+--------------+-------------+-------------+
*/
//定义类型常量中对象标志的相关信息
#define OBJ_FLG                 1
#define OBJ_FLG_SIZE            4
#define OBJ_FLG_SHIFT           0
#define OBJ_FLG_MASK            (((1 << OBJ_FLG_SIZE) - 1) << OBJ_FLG_SHIFT)
#define GET_TYP_OBJFLG(type)    (((type) & OBJ_FLG_MASK) >> OBJ_FLG_SHIFT)

//定义类型常量中指针标志的相关信息
#define PTR_FLG                 1
#define PTR_FLG_SIZE            4
#define PTR_FLG_SHIFT           4
#define PTR_FLG_MASK            (((1 << PTR_FLG_SIZE) - 1) << PTR_FLG_SHIFT)
#define GET_TYP_PTRFLG(type)    (((type) & PTR_FLG_MASK) >> PTR_FLG_SHIFT)

//定义类型常量中符号标志的相关信息
#define SGN_FLG                 1
#define SGN_FLG_SIZE            4
#define SGN_FLG_SHIFT           8
#define SGN_FLG_MASK            (((1 << SGN_FLG_SIZE) - 1) << SGN_FLG_SHIFT)
#define GET_TYP_SGNFLG(type)    (((type) & SGN_FLG_MASK) >> SGN_FLG_SHIFT)

//定义类型常量中类型标志的相关信息
#define TYP_FLG_SIZE            20
#define TYP_FLG_SHIFT           12
#define TYP_FLG_MASK            (((1 << TYP_FLG_SIZE) - 1) << TYP_FLG_SHIFT)
#define GET_TYP_TYPFLG(type)    (((type) & TYP_FLG_MASK) >> TYP_FLG_SHIFT)

//定义类型标志值
#define TYP_FLG_VOID            0x00000001      //void
#define TYP_FLG_CHAR            0x00000002      //char
#define TYP_FLG_SHORT           0x00000003      //short
#define TYP_FLG_INT             0x00000004      //int
#define TYP_FLG_LONG            0x00000005      //long
#define TYP_FLG_FLOAT           0x00000006      //float
#define TYP_FLG_DOUBLE          0x00000007      //double

#define TYP_FLG_REG             0x00000008      //CGListRegInfo
#define TYP_FLG_LIST            0x00000009      //CGList
#define TYP_FLG_OBJ             0x0000000A      //CGObject

#define TYP_FLG_PEN             0x0000000B      //CGPen

//定义类型常量值
#define BLD_TYP_CONST(typ, sgn, ptr, obj)       (((typ) << TYP_FLG_SHIFT)   \
                                                |((sgn) << SGN_FLG_SHIFT)   \
                                                |((ptr) << PTR_FLG_SHIFT)   \
                                                |((obj) << OBJ_FLG_SHIFT))

#define TYP_VOID_PTR            BLD_TYP_CONST(TYP_FLG_VOID, !SGN_FLG, PTR_FLG, !OBJ_FLG)        //void *

#define TYP_CHAR_PTR            BLD_TYP_CONST(TYP_FLG_CHAR, SGN_FLG, PTR_FLG, !OBJ_FLG)         //char *
#define TYP_CHAR_VAL            BLD_TYP_CONST(TYP_FLG_CHAR, SGN_FLG, !PTR_FLG, !OBJ_FLG)        //char
#define TYP_UCHAR_PTR           BLD_TYP_CONST(TYP_FLG_CHAR, !SGN_FLG, PTR_FLG, !OBJ_FLG)        //unsigned char *
#define TYP_UCHAR_VAL           BLD_TYP_CONST(TYP_FLG_CHAR, !SGN_FLG, !PTR_FLG, !OBJ_FLG)       //unsigned char

#define TYP_SHORT_PTR           BLD_TYP_CONST(TYP_FLG_SHORT, SGN_FLG, PTR_FLG, !OBJ_FLG)        //short *
#define TYP_SHORT_VAL           BLD_TYP_CONST(TYP_FLG_SHORT, SGN_FLG, !PTR_FLG, !OBJ_FLG)       //short
#define TYP_USHORT_PTR          BLD_TYP_CONST(TYP_FLG_SHORT, !SGN_FLG, PTR_FLG, !OBJ_FLG)       //unsigned short *
#define TYP_USHORT_VAL          BLD_TYP_CONST(TYP_FLG_SHORT, !SGN_FLG, !PTR_FLG, !OBJ_FLG)      //unsigned short

#define TYP_INT_PTR             BLD_TYP_CONST(TYP_FLG_INT, SGN_FLG, PTR_FLG, !OBJ_FLG)          //int *
#define TYP_INT_VAL             BLD_TYP_CONST(TYP_FLG_INT, SGN_FLG, !PTR_FLG, !OBJ_FLG)         //int
#define TYP_UINT_PTR            BLD_TYP_CONST(TYP_FLG_INT, !SGN_FLG, PTR_FLG, !OBJ_FLG)         //unsigned int *
#define TYP_UINT_VAL            BLD_TYP_CONST(TYP_FLG_INT, !SGN_FLG, !PTR_FLG, !OBJ_FLG)        //unsigned int

#define TYP_LONG_PTR            BLD_TYP_CONST(TYP_FLG_LONG, SGN_FLG, PTR_FLG, !OBJ_FLG)         //long *
#define TYP_LONG_VAL            BLD_TYP_CONST(TYP_FLG_LONG, SGN_FLG, !PTR_FLG, !OBJ_FLG)        //long
#define TYP_ULONG_PTR           BLD_TYP_CONST(TYP_FLG_LONG, !SGN_FLG, PTR_FLG, !OBJ_FLG)        //unsigned long *
#define TYP_ULONG_VAL           BLD_TYP_CONST(TYP_FLG_LONG, !SGN_FLG, !PTR_FLG, !OBJ_FLG)       //unsigned long

#define TYP_FLOAT_PTR           BLD_TYP_CONST(TYP_FLG_FLOAT, SGN_FLG, PTR_FLG, !OBJ_FLG)        //float *
#define TYP_FLOAT_VAL           BLD_TYP_CONST(TYP_FLG_FLOAT, SGN_FLG, !PTR_FLG, !OBJ_FLG)       //float

#define TYP_DOUBLE_PTR          BLD_TYP_CONST(TYP_FLG_DOUBLE, SGN_FLG, PTR_FLG, !OBJ_FLG)       //double *
#define TYP_DOUBLE_VAL          BLD_TYP_CONST(TYP_FLG_DOUBLE, SGN_FLG, !PTR_FLG, !OBJ_FLG)      //double

#define TYP_REG_PTR             BLD_TYP_CONST(TYP_FLG_REG, !SGN_FLG, PTR_FLG, OBJ_FLG)          //CGListRegInfo *
#define TYP_REG_VAL             BLD_TYP_CONST(TYP_FLG_REG, !SGN_FLG, !PTR_FLG, OBJ_FLG)         //CGListRegInfo

#define TYP_LIST_PTR            BLD_TYP_CONST(TYP_FLG_LIST, !SGN_FLG, PTR_FLG, OBJ_FLG)         //CGList *
#define TYP_LIST_VAL            BLD_TYP_CONST(TYP_FLG_LIST, !SGN_FLG, !PTR_FLG, OBJ_FLG)        //CGList

#define TYP_OBJ_PTR             BLD_TYP_CONST(TYP_FLG_OBJ, !SGN_FLG, PTR_FLG, OBJ_FLG)          //CGObject *
#define TYP_OBJ_VAL             BLD_TYP_CONST(TYP_FLG_OBJ, !SGN_FLG, !PTR_FLG, OBJ_FLG)         //CGObject

#define TYP_PEN_PTR             BLD_TYP_CONST(TYP_FLG_PEN, !SGN_FLG, PTR_FLG, OBJ_FLG)          //CGPen *
#define TYP_PEN_VAL             BLD_TYP_CONST(TYP_FLG_PEN, !SGN_FLG, !PTR_FLG, OBJ_FLG)         //CGPen

#endif  //_GUI_TYPE_H_

