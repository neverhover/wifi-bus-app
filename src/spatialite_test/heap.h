#ifndef _HEAP_H_
#define _HEAP_H_

#include <sys/types.h>
#define DEBUG 0
#if DEBUG
	#define DG(x,...) printf(x,##__VA_ARGS__);
#else
 	#define DG(x,...)	
#endif

#ifdef DOUBLE_TYPE
typedef int *PQ_PTR;
typedef int PQ_TYPE;		//索引的ID可能会非常大。
typedef double  DIS_TYPE;
typedef double  *DIS_PTR;
#else
typedef int *PQ_PTR;
typedef int PQ_TYPE;
typedef int DIS_TYPE;
typedef int *DIS_PTR;
#endif


#endif	//#define _HEAP_H_
