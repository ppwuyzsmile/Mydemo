#ifndef _MY_DEBUG_H_
#define _MY_DEBUG_H_
#include "windows.h"
	static void xprintf(char *fmt, ...)
	{
		va_list ap;
		char sz[4096]={0};
		char* offsetpt = sz+strlen("BigStorageReaderSystem");
		strcpy(sz,"BigStorageReaderSystem");
		va_start(ap, fmt);
		vsprintf(offsetpt, fmt, ap);
		OutputDebugStringA(sz);
		va_end(ap);
	}
#endif
