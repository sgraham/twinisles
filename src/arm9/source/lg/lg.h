#ifndef LG_INCLUDED_lg_lg_H
#define LG_INCLUDED_lg_lg_H

#define LG_PLATFORM LG_PLATFORM_DS

#include "lg/memtrack.h"

#include <cstdio>
#include <nds.h>

#define ARRAYSIZE(a) ((int)(sizeof(a)/sizeof((a)[0])))

#include <nds/arm9/console.h>
#define assert(e)																\
do																				\
{																				\
	if (!(e))																	\
	{																			\
		consoleDemoInit();														\
		consoleClear();															\
		BG_PALETTE_SUB[0] = RGB15(0,0,0);										\
		BG_PALETTE_SUB[255] = RGB15(31,31,31);									\
		iprintf("Assertion failed:\n\n%s(%d)\n\n%s\n", __FILE__, __LINE__, #e);	\
		for (;;) {}																\
	}																			\
} while (0)

#define assertmsg(e, s)															\
do																				\
{																				\
	if (!(e))																	\
	{																			\
		consoleDemoInit();														\
		consoleClear();															\
		BG_PALETTE_SUB[0] = RGB15(0,0,0);										\
		BG_PALETTE_SUB[255] = RGB15(31,31,31);									\
		iprintf("Assertion failed:\n\n%s(%d)\n\n%s\n%s\n", __FILE__, __LINE__, #e, s);	\
		for (;;) {}																\
	}																			\
} while (0)

template <class T> T Abs(const T& t)
{
	return t < 0 ? (-t) : t;
}

template <class T> T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template <class T> T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

#endif
