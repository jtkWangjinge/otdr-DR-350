/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  hangulime.h
** Latest Version:             V1.0.0
** Latest modified Date:       2018/06/13
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 SHI
** Created date:               2018/06/13
** Descriptions:               
** 
*********************************************************************************************************/
#ifndef HANGULIME_H
#define HANGULIME_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* 
* @param[in] 判断这个字符是否是韩国字母
* @return int
*/
int isJamo(const unsigned short keycode);


/**
* 
* @param[in] in：输入字符。必须定义数组大小为2
* @param[in] out：输出字符。必须定义数组大小为2
* @return int：输出字符的字符个数。0，1，2三种情况。
*/
int hangulIME(unsigned short *in, unsigned short *out);

#ifdef __cplusplus
}
#endif

#endif // HANGULIME_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
