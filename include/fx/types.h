#ifndef __FXTYPES_H__
#define __FXTYPES_H__

#include <stdint.h>
#include <uchar.h>

typedef unsigned char byte;
typedef signed long int intsize;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long int int64;

typedef unsigned long int uintsize;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long int uint64;

typedef float float32;
typedef double float64;

#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#define __bool_true_false_are_defined
typedef byte bool;
#define true ((bool)1)
#define false ((bool)0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif   // __FXTYPES_H__
