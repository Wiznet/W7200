/**********************************************
(c)COPYRIGHT WIZnet Inc. ALL RIGHT RESERVED
 W7100 driver
**********************************************/
/*
*
@file		types.h
@brief	define variable type for C code 
*
*/

#ifndef _TYPE_H_
#define _TYPE_H_

/***************************************************
 * attribute for mcu ( types, ... ) 
 ***************************************************/

#ifndef NULL
#define NULL		((void *) 0)
#endif

typedef enum { false, true } bool;

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

/**
 * The 8-bit signed data type.
 */
typedef char int8;
/**
 * The volatile 8-bit signed data type.
 */
typedef volatile char vint8;
/**
 * The 8-bit unsigned data type.
 */
typedef unsigned char uint8;
typedef unsigned char	SOCKET;
/**
 * The volatile 8-bit unsigned data type.
 */
typedef volatile unsigned char vuint8;
/**
 * The 16-bit signed data type.
 */
typedef int int16;
/**
 * The volatile 16-bit signed data type.
 */
typedef volatile int vint16;
/**
 * The 16-bit unsigned data type.
 */
typedef unsigned int uint16;
/**
 * The volatile 16-bit unsigned data type.
 */
typedef volatile unsigned int vuint16;
/**
 * The 32-bit signed data type.
 */
typedef long int32;
/**
 * The volatile 32-bit signed data type.
 */
typedef volatile long vint32;
/**
 * The 32-bit unsigned data type.
 */
typedef unsigned long uint32;
/**
 * The volatile 32-bit unsigned data type.
 */
typedef volatile unsigned long vuint32;

#endif		/* _TYPE_H_ */

